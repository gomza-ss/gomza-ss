#include <DeviceDemo.h>
#include <NMResource.h>

#include <QCoreApplication>
#include <QTimer>

#include <iostream>

int main(int argc, char ** argv)
{
    QCoreApplication app(argc, argv);

    const std::string resourcePath = NNLab::NMResourcePath();

    nnlab::json taskConfig;

    if (argc == 3)
    {
        taskConfig["module"] = std::string(argv[1]);
        taskConfig["task"] = std::string(argv[2]);
    }
    else
    {
        taskConfig["module"] = std::string("Auto");
        taskConfig["task"] = std::string("LPDet");
    }

    std::cout << "Task Config: " << taskConfig.dump(4) << std::endl;

    nnlab::DeviceDemo demo(taskConfig, &app);
    QObject::connect(&demo, SIGNAL(finished()), &app, SLOT(quit()));
    QTimer::singleShot(0, &demo, SLOT(run()));

    return app.exec();
}
