#pragma once

#include <NNClient/core/Registry.h>

#include <fstream>

#include <opencv2/core.hpp>

namespace nnlab
{

int getImageId(const std::string& outputJson);

class OutputRepresenter
{

public:

    void apply(const cv::Mat& frame, const std::string& json, const std::string& demoJson) const;

public:

    using TVisFunc = std::function<void(const cv::Mat&, const std::string&, const std::string&)>;
    OutputRepresenter& VisFunc(const TVisFunc& visFunc);

    using TLogFunc = std::function<void(std::ofstream& logfile, const std::string&, const std::string&)>;
    OutputRepresenter& LogFunc(const TLogFunc& logFunc);

private:

    TVisFunc m_visFunc;
    TLogFunc m_logFunc;

};

DECLARE_REGISTRY(OutputRepresenter, OutputRepresenter)

#define REGISTER_OUTPUT_REPRESENTER(name)                                                                          \
    void OutputRepresenter##OutputRepresenter##name() {};                                                          \
    static OutputRepresenter* NN_CLIENT_ANONYMOUS_VARIABLE(OutputRepresenterRegistry##name) NN_CLIENT_UNUSED_VAR = \
        &OutputRepresenterRegistry::addOutputRepresenter(#name)

}
