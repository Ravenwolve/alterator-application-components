#ifndef AB_CONTROLLER_H
#define AB_CONTROLLER_H

#include "controllerinterface.h"
#include "mainwindow.h"
#include "model/item.h"

#include <memory>
#include <QObject>
#include <QPushButton>

namespace amc
{
class ControllerPrivate;
class MainWindow;

class Controller : public QObject, public ControllerInterface
{
public:
    Q_OBJECT
public:
    Controller(MainWindow *window);
    ~Controller() override;

    void setModel(amc::model::Model *model) override;
    void ComponentSelected(QModelIndex index) override;
    void itemChanged(model::ModelItem *item) override;

    void exit() override;
    void apply() override;
    void reset() override;
    void ok() override;

private:
    void translateModel();
    bool isSatusEquivalent(ComponentsState state);
    void setStatusApply(bool status);
    void showWaitingDialog();

private slots:
    void onBeginApply(const int installPackagesCount, const int removePackagesCount);
    void onEndApply();
    void onBeginInstallPackage(QString package);
    void onEndInstallPackage(QString package, int result);
    void onBeginRemovePackage(QString package);
    void onEndRemovePackage(QString package, int result);

private:
    std::unique_ptr<ControllerPrivate> d;

public:
    Controller(const Controller &)            = delete;
    Controller(Controller &&)                 = delete;
    Controller &operator=(const Controller &) = delete;
    Controller &operator=(Controller &&)      = delete;
};

} // namespace amc

#endif // AB_CONTROLLER_H
