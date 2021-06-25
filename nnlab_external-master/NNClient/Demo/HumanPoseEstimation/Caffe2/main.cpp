#include "human_pose_estimator_mrcnn.hpp"
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
    Caffe2NetManager detectorNet(argv[2], argv[3]);
    detectorNet.resizeInput(cv::Size(512, 320));

    Caffe2NetManager keypointNet(argv[4], argv[5], false, false, detectorNet.workspace());
    human_pose_estimation::HumanPoseEstimatorMRCNN estimator(detectorNet, keypointNet);

    cv::VideoCapture cap(argv[1]);

    cv::Mat image;
    while (cap.read(image))
    {
        auto poses = estimator.estimate(image);

        human_pose_estimation::renderHumanPose(poses, image, 18);

        cv::imshow("", image);
        cv::waitKey(1);
    }

    return 0;
}
