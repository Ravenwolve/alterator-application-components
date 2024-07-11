#include "controller.h"
#include "componentsapplicator.h"
#include "constants.h"
#include "mainwindow.h"
#include "model/componentobject.h"
#include "model/item.h"
#include "packagesdialog.h"
#include "waitdialog.h"

#include <memory>
#include <QColor>
#include <QDebug>
#include <QThread>

namespace amc
{
class ControllerPrivate
{
public:
    ControllerPrivate() = default;

public:
    MainWindow *m_window{nullptr};
    amc::model::Model *m_model{nullptr};
    ComponentsState m_state{};
    bool canApply{false};
    std::unique_ptr<ComponentsApplicator> m_applicator{
        std::make_unique<ComponentsApplicator>(ALTERATOR_MANAGER_SERVICE_NAME,
                                               APT1_PATH,
                                               APT1_INTERFACE_NAME,
                                               APT1_INSTALL_METHOD_NAME,
                                               APT1_REMOVE_METHOD_NAME)};
    QThread *m_workerThread{nullptr};
    WaitDialog *m_waitDialog{new WaitDialog()};
};

Controller::Controller(MainWindow *window)
    : d(std::make_unique<ControllerPrivate>())
{
    d->m_window = window;

    connect(d->m_applicator.get(), &ComponentsApplicator::beginApply, this, &Controller::onBeginApply);
    connect(d->m_applicator.get(), &ComponentsApplicator::endApply, this, &Controller::onEndApply);
    connect(d->m_applicator.get(), &ComponentsApplicator::beginInstallPackage, this, &Controller::onBeginInstallPackage);
    connect(d->m_applicator.get(), &ComponentsApplicator::endInstallPackage, this, &Controller::onEndInstallPackage);
    connect(d->m_applicator.get(), &ComponentsApplicator::beginRemovePackage, this, &Controller::onBeginRemovePackage);
    connect(d->m_applicator.get(), &ComponentsApplicator::endRemovePackage, this, &Controller::onEndRemovePackage);
}

Controller::~Controller() = default;

void Controller::setModel(model::Model *model)
{
    d->m_model = model;
    translateModel();

    d->m_window->setModel(d->m_model);

    setStatusApply(false);

    d->m_state = d->m_model->getCurrentState();
}

void Controller::ComponentSelected(QModelIndex index)
{
    if (!index.isValid())
    {
        return;
    }

    auto *item = static_cast<model::ModelItem *>(index.internalPointer());

    if (!item)
    {
        return;
    }

    auto childItem = dynamic_cast<model::ModelItem *>(item->child(index.row()));

    if (!childItem)
    {
        return;
    }

    auto component = childItem->data().value<model::ComponentObject *>();

    if (component)
    {
        d->m_window->setDescription(component->m_description);
    }
}

void Controller::itemChanged(model::ModelItem *item)
{
    auto component = item->data().value<model::ComponentObject *>();
    if (!component)
    {
        return;
    }

    auto cur_state = d->m_state[item->data().value<model::ComponentObject *>()->m_id];
    bool changed   = false;
    
    auto componentState = static_cast<ComponentState>(item->checkState());
    component->setState(componentState);
    changed = cur_state != componentState;

    item->setBackground(changed ? QColorConstants::Svg::lightgray : QColorConstants::White);

    setStatusApply(!isSatusEquivalent(d->m_model->getCurrentState()));
}

void Controller::exit()
{
    d->m_window->close();
}

void Controller::apply()
{
    d->m_applicator->setTask(d->m_state, d->m_model->getCurrentState(), d->m_model->getComponents());

    PackagesDialog dialog;
    dialog.setInstallPackages(d->m_applicator->getPackagesToInstall());
    dialog.setRemovePackages(d->m_applicator->getPackagesToRemove());
    dialog.exec();
    int result = dialog.result();

    if (result == QDialog::Accepted)
    {
        d->m_workerThread = new QThread();

        connect(d->m_workerThread, &QThread::started, d->m_applicator.get(), &ComponentsApplicator::apply);
        connect(d->m_workerThread, &QThread::finished, d->m_workerThread, &QObject::deleteLater);

        d->m_applicator->moveToThread(d->m_workerThread);

        d->m_waitDialog->clearUi();

        d->m_workerThread->start();

        d->m_waitDialog->exec();

        d->m_model->rebuildModel();

        d->m_state = d->m_model->getCurrentState();

        translateModel();

        d->m_window->setModel(d->m_model);
        setStatusApply(false);
    }
}

void Controller::reset()
{
    d->m_model->resetCurrentState(d->m_state);
}

void Controller::ok()
{
    if (d->canApply == true)
    {
        apply();
    }
    exit();
}

void Controller::translateModel()
{
    QString language = QLocale{}.system().name();
    d->m_model->translateModel(language);
}

bool Controller::isSatusEquivalent(ComponentsState state)
{
    if (d->m_state.size() != state.size())
    {
        return false;
    }

    for (auto it = d->m_state.cbegin(); it != d->m_state.cend(); it++)
    {
        auto it2 = state.find(it.key());
        if (it2 == state.end())
        {
            return false;
        }
        else
        {
            if (it.value() != it2.value())
            {
                return false;
            }
        }
    }

    return true;
}

void Controller::setStatusApply(bool status)
{
    d->m_window->setEnabledApplyButton(status);
    d->m_window->setEnabledResetButton(status);
    d->canApply = status;
}

void Controller::onBeginApply(const int installPackagesCount, const int removePackagesCount)
{
    d->m_waitDialog->beginApply(installPackagesCount, removePackagesCount);
}

void Controller::onEndApply()
{
    d->m_waitDialog->endApply();
}

void Controller::onBeginInstallPackage(QString package)
{
    d->m_waitDialog->beginInstallPackage(package);
}

void Controller::onEndInstallPackage(QString package, int result)
{
    d->m_waitDialog->endInstallPackage(package, result);
}

void Controller::onBeginRemovePackage(QString package)
{
    d->m_waitDialog->beginRemovePackage(package);
}

void Controller::onEndRemovePackage(QString package, int result)
{
    d->m_waitDialog->endRemovePackage(package, result);
}

} // namespace amc
