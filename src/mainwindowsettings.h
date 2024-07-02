#ifndef AB_MAIN_WINDOW_SETTINGS_H
#define AB_MAIN_WINDOW_SETTINGS_H

#include "ui_mainwindow.h"
#include <memory>

#include <QObject>

namespace amc
{
class MainWindow;
class MainWindowSettingsPrivate;

class MainWindowSettings : public QObject
{
    Q_OBJECT
public:
    MainWindowSettings(MainWindow *mWindow, Ui::MainWindow *ui);
    ~MainWindowSettings() override;

public:
    MainWindowSettings(const MainWindowSettings &)            = delete;
    MainWindowSettings(MainWindowSettings &&)                 = delete;
    MainWindowSettings &operator=(const MainWindowSettings &) = delete;
    MainWindowSettings &operator=(MainWindowSettings &&)      = delete;

    void restoreSettings();

public slots:
    void saveSettings();

private:
    std::unique_ptr<MainWindowSettingsPrivate> d;
};
} // namespace amc

#endif // AB_MAIN_WINDOW_SETTINGS_H
