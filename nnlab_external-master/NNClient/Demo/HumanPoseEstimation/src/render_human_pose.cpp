// Copyright (C) 2018-2019 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <utility>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp>

#include "human_pose_estimator.hpp"
#include "render_human_pose.hpp"

namespace human_pose_estimation {

static const std::vector<std::pair<int, int>> limbKeypointsIds_coco =
{
    {1, 2},  {1, 5}, {2, 3}, {3, 4}, {5, 6}, {6, 7}, {1, 8}, {8, 9}, {9, 10}, {1, 11}, {11, 12}, {12, 13}, {1, 0},
    {0, 14}, {14, 16}, {0, 15}, {15, 17}
};

static const std::vector<std::pair<int, int>> limbKeypointsIds_body25 =
{
    {1, 8}, {1, 2}, {1, 5}, {2, 3}, {3, 4}, {5, 6}, {6, 7}, {8, 9}, {9, 10}, {10, 11}, {8, 12}, {12, 13}, {13, 14},
    {1, 0}, {0, 15}, {15, 17}, {0, 16}, {16, 18}, {2, 17}, {5, 18}, {14, 19}, {19, 20}, {14, 21}, {11, 22}, {22, 23},
    {11, 24}
};

static const std::vector<std::pair<int, int>> defaultIds = {};

const std::vector<std::pair<int, int>> & getLimbKeypointsIds (int keypointsNumber)
{
    switch (keypointsNumber)
    {
        case 18:
            return limbKeypointsIds_coco;
        case 25:
            return limbKeypointsIds_body25;
        default:
            return defaultIds;
    }
}


void renderHumanPose(const std::vector<HumanPose>& poses, cv::Mat& image, int keypointsNumber) {
    CV_Assert(image.type() == CV_8UC3);

    static const std::vector<cv::Scalar> colors = {
        cv::Scalar(255, 0, 0), cv::Scalar(255, 85, 0), cv::Scalar(255, 170, 0),
        cv::Scalar(255, 255, 0), cv::Scalar(170, 255, 0), cv::Scalar(85, 255, 0),
        cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 85), cv::Scalar(0, 255, 170),
        cv::Scalar(0, 255, 255), cv::Scalar(0, 170, 255), cv::Scalar(0, 85, 255),
        cv::Scalar(0, 0, 255), cv::Scalar(85, 0, 255), cv::Scalar(170, 0, 255),
        cv::Scalar(255, 0, 255), cv::Scalar(255, 0, 170), cv::Scalar(255, 0, 85),
        cv::Scalar(255, 0, 0), cv::Scalar(255, 85, 0), cv::Scalar(255, 170, 0),
        cv::Scalar(255, 255, 0), cv::Scalar(170, 255, 0), cv::Scalar(85, 255, 0),
        cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 85), cv::Scalar(0, 255, 170)
    };

    const auto & limbKeypointsIds = getLimbKeypointsIds(keypointsNumber);
    CV_Assert(!limbKeypointsIds.empty());

    const int stickWidth = 4;
    const cv::Point2f absentKeypoint(-1.0f, -1.0f);
    for (const auto& pose : poses) {
        CV_Assert(pose.keypoints.size() == keypointsNumber);

        for (size_t keypointIdx = 0; keypointIdx < pose.keypoints.size(); keypointIdx++) {
            if (pose.keypoints[keypointIdx] != absentKeypoint) {
                cv::circle(image, pose.keypoints[keypointIdx], 4, colors[keypointIdx], -1);
            }
        }
    }
    cv::Mat pane = image.clone();
    for (const auto& pose : poses) {
        for (const auto& limbKeypointsId : limbKeypointsIds) {
            std::pair<cv::Point2f, cv::Point2f> limbKeypoints(pose.keypoints[limbKeypointsId.first],
                    pose.keypoints[limbKeypointsId.second]);
            if (limbKeypoints.first == absentKeypoint
                    || limbKeypoints.second == absentKeypoint) {
                continue;
            }

            float meanX = (limbKeypoints.first.x + limbKeypoints.second.x) / 2;
            float meanY = (limbKeypoints.first.y + limbKeypoints.second.y) / 2;
            cv::Point difference = limbKeypoints.first - limbKeypoints.second;
            double length = std::sqrt(difference.x * difference.x + difference.y * difference.y);
            int angle = static_cast<int>(std::atan2(difference.y, difference.x) * 180 / CV_PI);
            std::vector<cv::Point> polygon;
            cv::ellipse2Poly(cv::Point2d(meanX, meanY), cv::Size2d(length / 2, stickWidth),
                             angle, 0, 360, 1, polygon);
            cv::fillConvexPoly(pane, polygon, colors[limbKeypointsId.second]);
        }
    }
    cv::addWeighted(image, 0.4, pane, 0.6, 0, image);
}
}  // namespace human_pose_estimation
