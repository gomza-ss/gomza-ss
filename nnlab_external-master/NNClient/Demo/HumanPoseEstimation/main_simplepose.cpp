#include "human_pose_estimator_simplepose.hpp"
#include "render_human_pose.hpp"
#include "Caffe2NetManager.h"

#include <opencv2/core/core.hpp>

int main (int argc, char ** argv)
{
    if (argc != 6)
    {
        std::cout
            << "Usage: "
            << argv[0]
            << " <video_path> <detector_predict_net> <detector_init_net> <keypoint_predict_net> <keypoint_init_net>"
            << std::endl;

        return 0;
    }
    Caffe2NetManager detector(argv[2], argv[3]);
    detector.resizeInput(cv::Size(480, 270));

    Caffe2NetManager keypointNet(argv[4], argv[5]);
    keypointNet.resizeInput(cv::Size(192, 256));
    keypointNet.setBatchSize(4);
    human_pose_estimation::HumanPoseEstimatorSimplepose<Caffe2NetManager> estimator(keypointNet, 0.2f);

    cv::VideoCapture cap(argv[1]);

    std::vector<human_pose_estimation::HumanPose> poses;
    cv::Mat image;
    while (cap.read(image))
    {
        auto detections = detector.getDetections(image, 0.7f);
        std::vector<cv::Rect> bboxes;
        for (auto det : detections)
        {
            bboxes.emplace_back(det.bbox);
        }

        poses = estimator.estimate(image, bboxes);

        human_pose_estimation::renderHumanPose(poses, image, 18);

        cv::imshow("", image);
        cv::waitKey(1);
    }

    return 0;
}
