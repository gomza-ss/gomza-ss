#include <NNClient/core/CommandFront.h>

namespace nnlab
{

CommandFront& CommandFront::Name(const std::string& xname)
{
    this->name = xname;

    return *this;
}

CommandFront& CommandFront::TensorRequired()
{
    tensorRequired = true;

    return *this;
}

CommandFront& CommandFront::TensorOutputRequired()
{
    tensorOutputRequired = true;

    return *this;
}

CommandFront& CommandFront::CommandSequence(const std::vector<std::string> & sequence)
{
    std::copy(sequence.begin(), sequence.end(), std::back_inserter(commandSequence));

    return *this;
}

CommandFront& CommandFront::SetPreFn(preFnType xpreFn)
{
    this->preFn = std::move(xpreFn);

    return *this;
}

CommandFront& CommandFront::DefaultSequence()
{
    return CommandSequence({ name });
}

CommandFront& CommandFront::EnableDynamicInputSize()
{
    InputArg<int>("input_image_width");
    InputArg<int>("input_image_height");
    InputArg<bool>("dynamic_input_size", []() { return true; });

    CommandSequence({ "resizeinput", "assignsm" });

    SetPreFn
    (
        []
        (
            CommandProcessorBack& commandProcessorBack,
            Config& inputConfig,
            std::vector<std::string>&,
            std::string&,
            std::string& errorMessage
        )
        {
            if (!commandProcessorBack.setSize(inputConfig))
            {
                errorMessage = "Unable to set input size";
                return false;
            }

            if (!commandProcessorBack.createSM(inputConfig, errorMessage))
            {
                errorMessage = "Failed to fill image SM";
                return false;
            }

            return true;
        }
    );

    return *this;
}

}