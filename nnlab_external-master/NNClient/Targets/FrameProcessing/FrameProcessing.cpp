#include <NNClientDemo.h>
#include <NMResource.h>

#include <QCoreApplication>
#include <QTimer>

#include <iostream>

int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: FrameProcessing.exe <task_config> <demo_config>" << std::endl;
        return 0;
    }

    const auto readConfigFromFile = [](const char *fileName, nnlab::json& config) -> bool
    {
        try
        {
            std::ifstream f(fileName);
            if (!f.is_open())
            {
                std::cerr << "Could not open config file " << fileName << std::endl;
                return false;
            }

            f >> config;
        }
        catch (const nnlab::json::exception& e)
        {
            std::cerr << "Exception during json parsing: " << e.what() << std::endl;
            return false;
        }
        return true;
    };

    const char* taskConfigFile = argv[1];
    const char* demoConfigFile = argv[2];

    nnlab::json taskConfig;
    nnlab::json demoConfig;

    if (!readConfigFromFile(taskConfigFile, taskConfig))
    {
        return 0;
    }

    if (!readConfigFromFile(demoConfigFile, demoConfig))
    {
        return 0;
    }

    std::cout << "Task Config: " << taskConfig.dump(4) << std::endl;
    std::cout << "Demo Config: " << demoConfig.dump(4) << std::endl;

    QCoreApplication app(argc, argv);
    nnlab::NNClientDemo demo(taskConfig, demoConfig, &app);
    QObject::connect(&demo, SIGNAL(finished()), &app, SLOT(quit()));
    QTimer::singleShot(0, &demo, SLOT(run()));

    return app.exec();
}
