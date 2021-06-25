#pragma once

#include "Caffe2NetManager.h"
#include "human_pose.hpp"

#include <opencv2/core/core.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace human_pose_estimation {

class HumanPoseEstimatorMRCNN {
public:
    HumanPoseEstimatorMRCNN
    (
        Caffe2NetManager & detectorNetManager,
        Caffe2NetManager & keypointsNetManager,
        float threshold = 0.7f,
        float kptThreshold = 0.02f,
        const std::string & heatmapsBlobName = "heatmap"
    );

    std::vector<HumanPose> estimate(cv::Mat image);

private:
    std::unordered_map<std::string, caffe2::Tensor> genInputs(const std::vector<Detection> & detections) const;

    void applySoftmax(float * data, int channels, int spatialSize) const;

    Caffe2NetManager & m_detectorNetManager;
    Caffe2NetManager & m_keypointsNetManager;
    float m_threshold;
    float m_kptThreshold;
    std::string m_heatmapsBlobName;
};

}  // namespace human_pose_estimation
