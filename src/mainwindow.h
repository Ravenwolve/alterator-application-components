#ifndef AB_MAIN_WINDOW_H
#define AB_MAIN_WINDOW_H

#include "mainwindowsettings.h"
#include "model/model.h"
#include <memory>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

namespace amc
{
class MainWindowSettings;
class MainWindowPrivate;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public:
    void closeEvent(QCloseEvent *event) override;

    void setController(ControllerInterface *newController);

    void setModel(model::Model *model);
    void setDescription(const QString &description);

    void setEnabledApplyButton(bool isEnabled);
    void setEnabledResetButton(bool isEnabled);

private slots:
    void on_okPushButton_clicked();
    void on_resetPushButton_clicked();
    void on_exitPushButton_clicked();
    void on_applyPushButton_clicked();

    void onItemChanged(QStandardItem *item);

private:
    std::unique_ptr<MainWindowPrivate> d;

public:
    MainWindow(const MainWindow &)            = delete;
    MainWindow(MainWindow &&)                 = delete;
    MainWindow &operator=(const MainWindow &) = delete;
    MainWindow &operator=(MainWindow &&)      = delete;
};
} // namespace amc

#endif // AB_MAIN_WINDOW_H
