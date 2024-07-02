#include "amcapp.h"
#include "controller.h"
#include "controllerinterface.h"
#include "mainwindow.h"
#include "model/model.h"

#include <memory>
#include <QApplication>
#include <QTranslator>

namespace amc
{
class AMCAppPrivate
{
public:
    AMCAppPrivate(std::unique_ptr<QApplication> application)
        : m_application(std::move(application))
        , m_model(new amc::model::Model())
    {}

public:
    std::unique_ptr<QApplication> m_application{nullptr};
    std::unique_ptr<amc::model::Model> m_model{nullptr};
    std::unique_ptr<amc::ControllerInterface> m_controller{nullptr};
    std::unique_ptr<amc::MainWindow> m_mainWindow{nullptr};
};

AMCApp::AMCApp(std::unique_ptr<QApplication> application)
    : d(new AMCAppPrivate(std::move(application)))
{
    QTranslator translator{};
    translator.load(QLocale{}.system().name(), ":/");
    d->m_application->installTranslator(&translator);

    d->m_mainWindow = std::make_unique<amc::MainWindow>();
    d->m_controller = std::make_unique<amc::Controller>(d->m_mainWindow.get());
    d->m_mainWindow->setController(d->m_controller.get());
}

AMCApp::~AMCApp() = default;

int AMCApp::run()
{
    d->m_model->rebuildModel();
    d->m_controller->setModel(d->m_model.get());
    d->m_mainWindow->show();

    return d->m_application->exec();
}
} // namespace amc
