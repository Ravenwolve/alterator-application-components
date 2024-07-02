#include "amcapp.h"

#include <QApplication>
#include <QDebug>
#include <qbase/logger/prelude.h>
#include <qtranslator.h>

int main(int argc, char *argv[])
{
    auto loggerManager = qbase::logger::LoggerManager::globalInstance();

    loggerManager->addLogger<qbase::logger::ConsoleLogger>(QtDebugMsg);
    loggerManager->addLogger<qbase::logger::FileLogger>(QtWarningMsg,
                                                        "alterator-application-components",
                                                        "alterator-application-components.log");
    loggerManager->addLogger<qbase::logger::SyslogLogger>(LOG_LEVEL_DISABLED, "alterator-application-components");

    auto app = std::make_unique<QApplication>(argc, argv);

    amc::AMCApp amcApp(std::move(app));

    return amcApp.run();
}
