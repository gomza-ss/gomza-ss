#include "human_pose_estimator_mrcnn.hpp"
#include "human_pose_estimator_simplepose.hpp"

#include <algorithm>
#include <cmath>

namespace human_pose_estimation {

    HumanPoseEstimatorMRCNN::HumanPoseEstimatorMRCNN
    (
        Caffe2NetManager & detectorNetManager,
        Caffe2NetManager & keypointsNetManager,
        float threshold,
        float kptThreshold,
        const std::string & heatmapsBlobName
    ) :
        m_detectorNetManager(detectorNetManager),
        m_keypointsNetManager(keypointsNetManager),
        m_threshold(threshold),
        m_kptThreshold(kptThreshold),
        m_heatmapsBlobName(heatmapsBlobName)
    {
    }

    std::vector<HumanPose> HumanPoseEstimatorMRCNN::estimate(cv::Mat image)
    {
        CV_Assert(image.type() == CV_8UC3);

        const auto detectorInputSize = m_detectorNetManager.inputSize();
        const float imgScaleX = static_cast<float>(image.cols) / detectorInputSize.width;
        const float imgScaleY = static_cast<float>(image.rows) / detectorInputSize.height;

        cv::resize(image, image, detectorInputSize);

        auto detections = m_detectorNetManager.getDetections(image, m_threshold);

        if (detections.empty())
        {
            return {};
        }

        const auto fpnInputs = genInputs(detections);
        for (const auto & input: fpnInputs)
        {
            m_keypointsNetManager.feedBlob(input.first, input.second);
        }

        m_keypointsNetManager.run();

        auto heatmaps_and_shape = m_keypointsNetManager.getBlob(m_heatmapsBlobName);

        CV_Assert(heatmaps_and_shape.dims.size() == 4 && heatmaps_and_shape.dims[1] == 17);

        const auto shape = heatmaps_and_shape.dims;
        auto & heatmaps = heatmaps_and_shape.data;
        const auto singleImageHeatmapsSize = heatmaps.size() / shape[0];
        const auto heatmapSize = singleImageHeatmapsSize / shape[1];

        std::vector<HumanPose> poses;
        poses.reserve(detections.size());

        for (int i = 0; i < detections.size(); ++i)
        {
            HumanPose pose;
            cv::Point2f absentKeypoint(-1.0f, -1.0f);
            pose.keypoints.resize(18, absentKeypoint);
            auto currentHeatmapsPtr = heatmaps.data() + i * singleImageHeatmapsSize;

            applySoftmax(currentHeatmapsPtr, static_cast<int>(shape[1]), static_cast<int>(shape[2] * shape[3]));

            auto bbox = detections[i].bbox;
            float scaleX = static_cast<float>(bbox.width) / shape[3];
            float scaleY = static_cast<float>(bbox.height) / shape[2];
            for (std::size_t j = 0; j < shape[1]; ++j)
            {
                cv::Mat heatmap
                (
                    static_cast<int>(shape[2]),
                    static_cast<int>(shape[3]),
                    cv::DataType<float>::type,
                    currentHeatmapsPtr + heatmapSize * j
                );
                double min, max;
                cv::Point minLoc, maxLoc;
                cv::minMaxLoc(heatmap, &min, &max, &minLoc, &maxLoc);
                cv::Point2f kpt(-1, -1);
                if (max > m_kptThreshold)
                {
                    kpt =
                        cv::Point
                        (
                            static_cast<int>((maxLoc.x * scaleX + bbox.x) * imgScaleX),
                            static_cast<int>((maxLoc.y * scaleY + bbox.y) * imgScaleY)
                        );
                }
                pose.keypoints[coco2openposeIdx[j]] = kpt;
            }

            if (pose.keypoints[2] != absentKeypoint && pose.keypoints[5] != absentKeypoint)
            {
                auto neckPt = (pose.keypoints[2] + pose.keypoints[5]) / 2.0f;
                pose.keypoints[1] = neckPt;
            }

            poses.push_back(pose);
        }

        return poses;
    }

    std::unordered_map<std::string, caffe2::Tensor> HumanPoseEstimatorMRCNN::genInputs
    (
        const std::vector<Detection> & detections
    ) const
    {
        constexpr float roiCanonicalSize = 224;
        constexpr float roiCanonicalLevel = 4;
        constexpr int minLevel = 2;
        constexpr int maxLevel = 5;
        std::vector<std::vector<float>> inputs_vec(maxLevel - minLevel + 1);
        std::vector<std::vector<std::int32_t>> indices(maxLevel - minLevel + 1);

        for (int i = 0; i < detections.size(); ++i)
        {
            const auto & det = detections[i];

            const auto s = std::sqrt(det.bbox.area());
            auto targetLevel =
                static_cast<int>(std::floor(roiCanonicalLevel + std::log2(s / roiCanonicalSize + 1e-6)));
            targetLevel = std::min(std::max(targetLevel, minLevel), maxLevel);

            const auto idx = targetLevel - minLevel;
            inputs_vec[idx].push_back(0.0f);
            inputs_vec[idx].push_back(static_cast<float>(det.bbox.x));
            inputs_vec[idx].push_back(static_cast<float>(det.bbox.y));
            inputs_vec[idx].push_back(static_cast<float>(det.bbox.x + det.bbox.width));
            inputs_vec[idx].push_back(static_cast<float>(det.bbox.y + det.bbox.height));

            indices[idx].push_back(i);
        }

        const auto indexToBlobName =
            [minLevel] (int lvl) {return std::string("keypoint_rois_fpn") + std::to_string(lvl + minLevel);};

        std::unordered_map<std::string, caffe2::Tensor> inputs;
        std::vector<std::int32_t> indices_flatten(detections.size());
        auto indicesIter = indices_flatten.begin();
        for (int i = 0; i < inputs_vec.size(); ++i)
        {
            const auto & values = inputs_vec[i];
            std::vector<std::int64_t> dims = {static_cast<std::int64_t>(values.size() / 5), 5};
            inputs[indexToBlobName(i)] =
                caffe2::TensorCPUFromValues
                (
                    c10::ArrayRef<std::int64_t>(dims.data(), dims.size()),
                    c10::ArrayRef<float>(values.data(), values.size())
                );
            indicesIter = std::copy(indices[i].begin(), indices[i].end(), indicesIter);
        }
        std::vector<std::int64_t> indices_flatten_dims = {static_cast<std::int64_t>(indices_flatten.size())};
        std::vector<std::int32_t> indices_flatten_permuted(indices_flatten.size());
        for (int i = 0; i < indices_flatten.size(); ++i)
        {
            indices_flatten_permuted[indices_flatten[i]] = i;
        }

        inputs["keypoint_rois_idx_restore_int32"] =
            caffe2::TensorCPUFromValues
            (
                c10::ArrayRef<std::int64_t>(indices_flatten_dims.data(), indices_flatten_dims.size()),
                c10::ArrayRef<std::int32_t>(indices_flatten_permuted.data(), indices_flatten_permuted.size())
            );

        return inputs;
    }

    void HumanPoseEstimatorMRCNN::applySoftmax(float * data, int channels, int spatialSize) const
    {
        cv::Mat mat(channels, spatialSize, cv::DataType<float>::type, data);
        cv::Mat rwiseMax;
        cv::reduce(mat, rwiseMax, 1, cv::REDUCE_MAX);

        for (int i = 0; i < channels; ++i)
        {
            for (int j = 0; j < spatialSize; ++j)
            {
                mat.at<float>(i, j) = std::exp(mat.at<float>(i, j) - rwiseMax.at<float>(i, 0));
            }
        }
        cv::Mat rwiseSum;
        cv::reduce(mat, rwiseSum, 1, cv::REDUCE_SUM);
        for (int i = 0; i < channels; ++i)
        {
            mat.row(i) /= rwiseSum.at<float>(i, 0);
        }
    }

}  // namespace human_pose_estimation
