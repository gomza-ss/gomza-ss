#include "human_pose_estimator.hpp"
#include "render_human_pose.hpp"
#include "OpenVINONetManager.hpp"

#include <opencv2/core/core.hpp>

int main (int argc, char ** argv)
{
    auto image = cv::imread(argv[1]);
    cv::resize(image, image, cv::Size(456, 256));
    OpenVINONetManager manager(argv[2]);
    human_pose_estimation::HumanPoseEstimator<OpenVINONetManager> estimator(manager);
    auto result = estimator.estimate(image);
    if (!result.empty())
    {
        const auto keypointsNumber = static_cast<int>(result.front().keypoints.size());
        human_pose_estimation::renderHumanPose(result, image, keypointsNumber);
    }

    cv::imshow("", image);
    cv::waitKey(0);

    return 0;
}
