#pragma once

#include <string>
#include <vector>

#include <opencv2/core/core.hpp>

#include "human_pose.hpp"


namespace human_pose_estimation {

const std::vector<int> coco2openposeIdx = { 0, 14, 15, 16, 17, 2, 5, 3, 6, 4, 7, 8, 11, 9, 12, 10, 13 };

template <typename NetManager>
class HumanPoseEstimatorSimplepose {
public:
    HumanPoseEstimatorSimplepose
    (
        NetManager & netManager,
        float threshold = 0.5f,
        float expandRatio = 1.25f,
        const std::string & heatmapsBlobName = "heatmap"
    ) :
        m_netManager(netManager),
        m_threshold(threshold),
        m_expandRatio(expandRatio),
        m_heatmapsBlobName(heatmapsBlobName)
    {
        CV_Assert(m_expandRatio >= 1.0f);
    }

    std::vector<HumanPose> estimate(const cv::Mat& image, const std::vector<cv::Rect>& bboxes)
    {
        CV_Assert(image.type() == CV_8UC3);

        if (bboxes.empty())
        {
            return {};
        }

        std::vector<cv::Mat> rois;
        std::vector<cv::Rect> expandedBboxes;
        rois.reserve(bboxes.size());
        expandedBboxes.reserve(bboxes.size());
        for (auto bbox: bboxes)
        {
            auto center = (bbox.tl() + bbox.br()) / 2;
            auto w = bbox.width / 2;
            auto h = bbox.height / 2;
            auto x1 = std::max(static_cast<int>(center.x - w * m_expandRatio), 0);
            auto y1 = std::max(static_cast<int>(center.y - h * m_expandRatio), 0);
            auto x2 = std::min(static_cast<int>(center.x + w * m_expandRatio), image.cols);
            auto y2 = std::min(static_cast<int>(center.y + h * m_expandRatio), image.rows);

            cv::Rect expandedBbox(x1, y1, x2 - x1, y2 - y1);
            rois.push_back(image(expandedBbox));
            expandedBboxes.push_back(expandedBbox);
        }

        auto heatmaps_and_shape = m_netManager.forwardBatch(rois, m_heatmapsBlobName);

        CV_Assert(heatmaps_and_shape.dims.size() == 4 && heatmaps_and_shape.dims[1] == 17);

        const auto shape = heatmaps_and_shape.dims;
        auto & heatmaps = heatmaps_and_shape.data;
        const auto singleImageHeatmapsSize = heatmaps.size() / shape[0];
        const auto heatmapSize = singleImageHeatmapsSize / shape[1];

        std::vector<HumanPose> poses;
        poses.reserve(rois.size());

        for (int i = 0; i < rois.size(); ++i)
        {
            HumanPose pose;
            cv::Point2f absentKeypoint(-1.0f, -1.0f);
            pose.keypoints.resize(18, absentKeypoint);
            auto currentHeatmapsPtr = heatmaps.data() + i * singleImageHeatmapsSize;
            auto bbox = expandedBboxes[i];
            float scale_x = static_cast<float>(bbox.width) / shape[3];
            float scale_y = static_cast<float>(bbox.height) / shape[2];
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
                if (max > m_threshold)
                {
                    kpt =
                        cv::Point
                        (
                            static_cast<int>(maxLoc.x * scale_x + bbox.x),
                            static_cast<int>(maxLoc.y * scale_y + bbox.y)
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

private:

    NetManager & m_netManager;
    float m_threshold;
    float m_expandRatio;
    std::string m_heatmapsBlobName;
};

}  // namespace human_pose_estimation
