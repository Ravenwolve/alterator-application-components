#include "mainwindow.h"
#include "controller.h"
#include "mainwindowsettings.h"
#include "model/item.h"
#include "ui_mainwindow.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>
#include <QDialog>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QDomText>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QShortcut>
#include <QStandardItemModel>
#include <QString>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QWidget>
#include <QtGlobal>
#include <qglobal.h>

#include <memory>

namespace amc
{
class MainWindowPrivate
{
public:
    std::unique_ptr<Ui::MainWindow> ui           = nullptr;
    std::unique_ptr<MainWindowSettings> settings = nullptr;
    model::Model *model                          = nullptr;
    ControllerInterface *controller              = nullptr;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d(std::make_unique<MainWindowPrivate>())
{
    d->ui = std::make_unique<Ui::MainWindow>();
    d->ui->setupUi(this);

    d->settings = std::make_unique<MainWindowSettings>(this, d->ui.get());
    d->settings->restoreSettings();

    setWindowIcon(QIcon(":logo.png"));
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent *event)
{
    d->settings->saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::setController(ControllerInterface *newController)
{
    d->controller = newController;
    d->ui->componentsWidget->setController(newController);
}

void MainWindow::setModel(model::Model *model)
{
    if (!model)
    {
        return;
    }

    d->model = model;
    d->ui->componentsWidget->setComponentsModel(d->model);

    connect(d->model, &model::Model::itemChanged, this, &MainWindow::onItemChanged);
}

void MainWindow::setDescription(const QString &description)
{
    d->ui->componentsWidget->setDescription(description);
}

void MainWindow::setEnabledApplyButton(bool isEnabled)
{
    d->ui->applyPushButton->setEnabled(isEnabled);
}

void MainWindow::setEnabledResetButton(bool isEnabled)
{
    d->ui->resetPushButton->setEnabled(isEnabled);
}

void MainWindow::on_okPushButton_clicked()
{
    d->controller->ok();
}

void MainWindow::on_exitPushButton_clicked()
{
    d->controller->exit();
}

void MainWindow::on_resetPushButton_clicked()
{
    d->controller->reset();
}

void MainWindow::on_applyPushButton_clicked()
{
    d->controller->apply();
}

void MainWindow::onItemChanged(QStandardItem *item)
{
    d->controller->itemChanged(dynamic_cast<model::ModelItem *>(item));
}

} // namespace amc
