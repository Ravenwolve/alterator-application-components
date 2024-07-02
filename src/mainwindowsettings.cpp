#include "mainwindowsettings.h"
#include "mainwindow.h"

#include <QSettings>

namespace amc
{
const QString MAIN_WINDOW_GEOMETRY = "mainwindow/geometry";
const QString MAIN_WINDOW_STATE    = "mainwindow/state";

class MainWindowSettingsPrivate
{
public:
    MainWindowSettingsPrivate(MainWindow *mWindow, Ui::MainWindow *ui)
        : m_mainWindow(mWindow)
        , m_ui(ui)
        , m_settings("BaseALT", "alterator-application-components")
    {}
    ~MainWindowSettingsPrivate() = default;

public:
    MainWindowSettingsPrivate(const MainWindowSettingsPrivate &)            = delete;
    MainWindowSettingsPrivate(MainWindowSettingsPrivate &&)                 = delete;
    MainWindowSettingsPrivate &operator=(const MainWindowSettingsPrivate &) = delete;
    MainWindowSettingsPrivate &operator=(MainWindowSettingsPrivate &&)      = delete;

    MainWindow *m_mainWindow = nullptr;
    Ui::MainWindow *m_ui     = nullptr;
    QSettings m_settings{};
};

MainWindowSettings::MainWindowSettings(MainWindow *mWindow, Ui::MainWindow *ui)
    : d(std::make_unique<MainWindowSettingsPrivate>(mWindow, ui))
{}

MainWindowSettings::~MainWindowSettings() = default;

void MainWindowSettings::restoreSettings()
{
    const QByteArray geometry = d->m_settings.value(MAIN_WINDOW_GEOMETRY).toByteArray();
    d->m_mainWindow->restoreGeometry(geometry);

    const QByteArray state = d->m_settings.value(MAIN_WINDOW_STATE).toByteArray();
    d->m_mainWindow->restoreState(state);
}

void MainWindowSettings::saveSettings()
{
    const QByteArray geometry = d->m_mainWindow->saveGeometry();
    d->m_settings.setValue(MAIN_WINDOW_GEOMETRY, geometry);

    const QByteArray state = d->m_mainWindow->saveState();
    d->m_settings.setValue(MAIN_WINDOW_STATE, state);
}
} // namespace amc
