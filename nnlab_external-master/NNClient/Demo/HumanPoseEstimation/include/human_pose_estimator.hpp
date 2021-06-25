// Copyright (C) 2018-2019 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "human_pose.hpp"
#include "peak.hpp"


namespace human_pose_estimation {

class FindPeaksBody : public cv::ParallelLoopBody {
public:
    FindPeaksBody(const std::vector<cv::Mat>& heatMaps, float minPeaksDistance,
        std::vector<std::vector<Peak> >& peaksFromHeatMap)
        : heatMaps(heatMaps),
        minPeaksDistance(minPeaksDistance),
        peaksFromHeatMap(peaksFromHeatMap) {}

    virtual void operator()(const cv::Range& range) const {
        for (int i = range.start; i < range.end; i++) {
            findPeaks(heatMaps, minPeaksDistance, peaksFromHeatMap, i);
        }
    }

private:
    const std::vector<cv::Mat>& heatMaps;
    float minPeaksDistance;
    std::vector<std::vector<Peak> >& peaksFromHeatMap;
};

template <typename NetManager>
class HumanPoseEstimator {
public:
    HumanPoseEstimator(NetManager & netManager, const std::string & pafsBlobName = "Mconv7_stage2_L1", const std::string & heatmapsBlobName = "Mconv7_stage2_L2"):
        m_netManager(netManager),
        minJointsNumber(3),
        stride(8),
        pad(cv::Vec4i::all(0)),
        meanPixel(cv::Vec3f::all(128)),
        minPeaksDistance(3.0f),
        midPointsScoreThreshold(0.05f),
        foundMidPointsRatioThreshold(0.8f),
        minSubsetScore(0.2f),
        inputLayerSize(-1, -1),
        upsampleRatio(4),
        pafsBlobName(pafsBlobName),
        heatmapsBlobName(heatmapsBlobName)
    {
    }

    std::vector<HumanPose> estimate(const cv::Mat& image)
    {
        CV_Assert(image.type() == CV_8UC3);

        cv::Size imageSize = image.size();

        m_netManager.forward(image);


        auto pafsBlob = m_netManager.getBlob(pafsBlobName);
        auto heatMapsBlob = m_netManager.getBlob(heatmapsBlobName);
        const auto & heatMapDims = heatMapsBlob.dims;
        const std::size_t keypointsNumber = static_cast<std::size_t>(heatMapDims[1] - 1);
        CV_Assert(pafsBlob.dims[1] / 2 == keypointsNumber + 1);
        std::vector<HumanPose> poses = postprocess(
            heatMapsBlob.data.data(),
            static_cast<int>(heatMapDims[2] * heatMapDims[3]),
            static_cast<int>(keypointsNumber),
            pafsBlob.data.data(),
            static_cast<int>(heatMapDims[2] * heatMapDims[3]),
            static_cast<int>(pafsBlob.dims[1]),
            static_cast<int>(heatMapDims[3]), static_cast<int>(heatMapDims[2]), imageSize);

        return poses;
    }

private:
    std::vector<HumanPose> postprocess(
        const float* heatMapsData, const int heatMapOffset, const int nHeatMaps,
        const float* pafsData, const int pafOffset, const int nPafs,
        const int featureMapWidth, const int featureMapHeight,
        const cv::Size& imageSize) const
    {
        std::vector<cv::Mat> heatMaps(nHeatMaps);
        for (size_t i = 0; i < heatMaps.size(); i++) {
            heatMaps[i] = cv::Mat(featureMapHeight, featureMapWidth, CV_32FC1,
                reinterpret_cast<void*>(
                    const_cast<float*>(
                        heatMapsData + i * heatMapOffset)));
        }
        resizeFeatureMaps(heatMaps);

        std::vector<cv::Mat> pafs(nPafs);
        for (size_t i = 0; i < pafs.size(); i++) {
            pafs[i] = cv::Mat(featureMapHeight, featureMapWidth, CV_32FC1,
                reinterpret_cast<void*>(
                    const_cast<float*>(
                        pafsData + i * pafOffset)));
        }
        resizeFeatureMaps(pafs);

        std::vector<HumanPose> poses = extractPoses(heatMaps, pafs);
        correctCoordinates(poses, heatMaps[0].size(), imageSize);
        return poses;

    }

    std::vector<HumanPose> extractPoses(const std::vector<cv::Mat>& heatMaps,
        const std::vector<cv::Mat>& pafs) const
    {
        std::vector<std::vector<Peak> > peaksFromHeatMap(heatMaps.size());
        FindPeaksBody findPeaksBody(heatMaps, minPeaksDistance, peaksFromHeatMap);
        cv::parallel_for_(cv::Range(0, static_cast<int>(heatMaps.size())),
            findPeaksBody);
        int peaksBefore = 0;
        for (size_t heatmapId = 1; heatmapId < heatMaps.size(); heatmapId++) {
            peaksBefore += static_cast<int>(peaksFromHeatMap[heatmapId - 1].size());
            for (auto& peak : peaksFromHeatMap[heatmapId]) {
                peak.id += peaksBefore;
            }
        }
        std::vector<HumanPose> poses = groupPeaksToPoses(
            peaksFromHeatMap, pafs, heatMaps.size(), midPointsScoreThreshold,
            foundMidPointsRatioThreshold, minJointsNumber, minSubsetScore);
        return poses;

    }
    void resizeFeatureMaps(std::vector<cv::Mat>& featureMaps) const
    {
        for (auto& featureMap : featureMaps) {
            cv::resize(featureMap, featureMap, cv::Size(),
                upsampleRatio, upsampleRatio, cv::INTER_CUBIC);
        }
    }
    void correctCoordinates(std::vector<HumanPose>& poses,
                            const cv::Size& featureMapsSize,
                            const cv::Size& imageSize) const
    {
        CV_Assert(stride % upsampleRatio == 0);

        cv::Size fullFeatureMapSize = featureMapsSize * stride / upsampleRatio;

        float scaleX = imageSize.width /
            static_cast<float>(fullFeatureMapSize.width - pad(1) - pad(3));
        float scaleY = imageSize.height /
            static_cast<float>(fullFeatureMapSize.height - pad(0) - pad(2));
        for (auto& pose : poses) {
            for (auto& keypoint : pose.keypoints) {
                if (keypoint != cv::Point2f(-1, -1)) {
                    keypoint.x *= stride / upsampleRatio;
                    keypoint.x -= pad(1);
                    keypoint.x *= scaleX;

                    keypoint.y *= stride / upsampleRatio;
                    keypoint.y -= pad(0);
                    keypoint.y *= scaleY;
                }
            }
        }
    }

    NetManager & m_netManager;
    int minJointsNumber;
    int stride;
    cv::Vec4i pad;
    cv::Vec3f meanPixel;
    float minPeaksDistance;
    float midPointsScoreThreshold;
    float foundMidPointsRatioThreshold;
    float minSubsetScore;
    cv::Size inputLayerSize;
    int upsampleRatio;
    std::string pafsBlobName;
    std::string heatmapsBlobName;
};

}  // namespace human_pose_estimation
