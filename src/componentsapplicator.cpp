#include "componentsapplicator.h"
#include "constants.h"

#include <memory>
#include <utility>
#include <QApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QHash>
#include <QLocale>
#include <QtGlobal>
#include <qglobal.h>

namespace amc
{
class ComponentsApplicatorPrivate
{
public:
    ComponentsApplicatorPrivate(
        QString serviceName, QString path, QString interface, QString installMethod, QString removeMethod)
        : m_serviceName(std::move(serviceName))
        , m_path(std::move(path))
        , m_interfaceName(std::move(interface))
        , m_installMethodName(std::move(installMethod))
        , m_removeMethodName(std::move(removeMethod))
        , m_connection(QDBusConnection::systemBus())
    {}
    ~ComponentsApplicatorPrivate() = default;

    QDBusConnection m_connection;
    QString m_serviceName{};
    QString m_path{};
    QString m_interfaceName{};
    QString m_installMethodName{};
    QString m_removeMethodName{};

    QHash<QString, int> m_installedPackages{};

    QMap<QString, QString> packagesToInstall{};
    QMap<QString, QString> packagesToRemove{};
};

ComponentsApplicator::ComponentsApplicator(
    QString serviceName, QString path, QString interface, QString installMethod, QString removeMethod)
    : d(std::make_unique<ComponentsApplicatorPrivate>(serviceName, path, interface, installMethod, removeMethod))
{
    this->setManagerLocale();
}

ComponentsApplicator::~ComponentsApplicator() = default;

void ComponentsApplicator::setManagerLocale()
{
    QDBusInterface iface(ALTERATOR_MANAGER_SERVICE_NAME,
                         ALTERATOR_MANAGER_PATH,
                         ALTERATOR_MANAGER_INTERFACE_NAME,
                         d->m_connection);
    if (!iface.isValid())
    {
        qWarning() << "Incorrect interface" << ALTERATOR_MANAGER_SERVICE_NAME << ALTERATOR_MANAGER_PATH
                   << ALTERATOR_MANAGER_INTERFACE_NAME;
        return;
    }

    QDBusMessage reply = iface.call(ALTERATOR_MANAGER_SET_ENV_METHOD_NAME, "LC_ALL", QLocale{}.system().name());
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning() << "Error reply from manager:" << reply.errorMessage();
        return;
    }
}

void ComponentsApplicator::setTask(ComponentsState currentState,
                                   ComponentsState applyingState,
                                   QMap<QString, model::ComponentObject> components)
{
    d->m_installedPackages.clear();
    d->packagesToInstall.clear();
    d->packagesToRemove.clear();

    readInstalledPackages();

    auto componentsToRemove  = getComponentsToRemove(currentState, applyingState, components);
    auto componentsToInstall = getComponentsToInstall(currentState, applyingState, components);
    auto intactComponents    = getIntactComponents(componentsToInstall, componentsToRemove, components);

    d->packagesToInstall = getPackagesToInstall(componentsToInstall);
    d->packagesToRemove  = getPackagesToRemove(componentsToRemove);

    //Remove packages from the vector of packages to be deleted that are in unaffected components
    removePackagesFromIntactComponents(d->packagesToRemove, intactComponents);

    //Remove packages from the vector of packages to be removed that are part of the components to be installed
    removePackagesFromInstallComponents(d->packagesToRemove, componentsToInstall);

    //Remove duplicates from the vector of installing packages
    removeInstalledPackages(d->packagesToInstall);
}

QMap<QString, QString> ComponentsApplicator::getPackagesToInstall()
{
    return d->packagesToInstall;
}

QMap<QString, QString> ComponentsApplicator::getPackagesToRemove()
{
    return d->packagesToRemove;
}

int ComponentsApplicator::apply()
{
    int result = 0;

    emit beginApply(d->packagesToInstall.size(), d->packagesToRemove.size());

    //TO DO refactor returned value
    int removeResult  = removePackages(d->packagesToRemove);
    int installResult = installPackages(d->packagesToInstall);

    if (removeResult || installResult)
    {
        result = -1;
    }

    this->moveToThread(QApplication::instance()->thread());

    emit endApply();

    return result;
}

QMap<QString, model::ComponentObject> ComponentsApplicator::getComponentsToInstall(
    const ComponentsState currentState,
    const ComponentsState applyingState,
    const QMap<QString, model::ComponentObject> &components)
{
    if (currentState.size() != applyingState.size())
    {
        return {};
    }

    QMap<QString, model::ComponentObject> installComponents;
    for (QString &component : currentState.keys())
    {
        auto comp = applyingState.find(component);
        if (comp == applyingState.end())
        {
            return {};
        }

        ComponentState currentComponent = currentState[component];
        ComponentState applyComponent   = comp.value();

        if ((currentComponent == ComponentState::not_installed
             || currentComponent == ComponentState::partially_installed)
            && applyComponent == ComponentState::installed)
        {
            auto componentsIt = components.find(component);
            if (componentsIt == components.end())
            {
                return {};
            }
            installComponents.insert(component, componentsIt.value());
        }
    }

    return installComponents;
}

QMap<QString, model::ComponentObject> ComponentsApplicator::getComponentsToRemove(
    const ComponentsState currentState,
    const ComponentsState applyingState,
    const QMap<QString, model::ComponentObject> &components)
{
    if (currentState.size() != applyingState.size())
    {
        return {};
    }

    QMap<QString, model::ComponentObject> removeComponents;
    for (QString &component : currentState.keys())
    {
        auto comp = applyingState.find(component);
        if (comp == applyingState.end())
        {
            return {};
        }

        ComponentState currentComponent = currentState[component];
        ComponentState applyComponent   = comp.value();

        if ((currentComponent == ComponentState::installed || currentComponent == ComponentState::partially_installed)
            && applyComponent == ComponentState::not_installed)
        {
            auto componentsIt = components.find(component);
            if (componentsIt == components.end())
            {
                return {};
            }
            removeComponents.insert(component, componentsIt.value());
        }
    }

    return removeComponents;
}

QMap<QString, QString> ComponentsApplicator::getPackagesToInstall(QMap<QString, model::ComponentObject> installComponents)
{
    return getPackagesFromComponents(installComponents);
}

QMap<QString, QString> ComponentsApplicator::getPackagesToRemove(QMap<QString, model::ComponentObject> removeComponents)
{
    return getPackagesFromComponents(removeComponents);
}

QMap<QString, QString> ComponentsApplicator::getPackagesFromComponents(QMap<QString, model::ComponentObject> components)
{
    QMap<QString, QString> packages;
    for (QString currentComponent : components.keys())
    {
        for (QString &package : components[currentComponent].m_packages)
        {
            packages.insert(package, currentComponent);
        }
    }

    return packages;
}

void ComponentsApplicator::removePackagesFromIntactComponents(QMap<QString, QString> &removePackages,
                                                              QMap<QString, model::ComponentObject> intactComponents)
{
    for (QString &component : intactComponents.keys())
    {
        auto packages = intactComponents[component].m_packages;
        for (QString package : packages)
        {
            auto packagesIt = removePackages.find(package);
            if (packagesIt == removePackages.end())
            {
                continue;
            }
            else
            {
                removePackages.erase(packagesIt);
            }
        }
    }
}

void ComponentsApplicator::removePackagesFromInstallComponents(QMap<QString, QString> &removePackages,
                                                               QMap<QString, model::ComponentObject> installComponents)
{
    for (QString &component : installComponents.keys())
    {
        auto packages = installComponents[component].m_packages;
        for (QString package : packages)
        {
            auto packagesIt = removePackages.find(package);

            if (packagesIt == removePackages.end())
            {
                continue;
            }
            else
            {
                removePackages.erase(packagesIt);
            }
        }
    }
}

void ComponentsApplicator::removeInstalledPackages(QMap<QString, QString> &packagesToInstall)
{
    for (QString packageToInstall : packagesToInstall.keys())
    {
        auto installedPackagesIt = d->m_installedPackages.find(packageToInstall);

        if (installedPackagesIt == d->m_installedPackages.end())
        {
            continue;
        }
        else
        {
            packagesToInstall.remove(packageToInstall);
        }
    }
}

int ComponentsApplicator::installPackages(QMap<QString, QString> packages)
{
    if (packages.isEmpty())
    {
        return 0;
    }

    QDBusInterface iface(d->m_serviceName, d->m_path, d->m_interfaceName, d->m_connection);
    if (!iface.isValid())
    {
        return -1;
    }

    int result = 0;

    const auto packagesStr = packages.keys().join(" ");

    emit beginInstallPackage(packagesStr);

    QDBusMessage reply = iface.call(d->m_installMethodName, packagesStr);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        result = -2;
        emit endInstallPackage(packagesStr, result);
    }

    auto args = reply.arguments();
    result    = args.at(0).toInt();

    emit endInstallPackage(packagesStr, result);

    return result;
}

int ComponentsApplicator::removePackages(QMap<QString, QString> packages)
{
    if (packages.isEmpty())
    {
        return 0;
    }

    QDBusInterface iface(d->m_serviceName, d->m_path, d->m_interfaceName, d->m_connection);
    if (!iface.isValid())
    {
        return -1;
    }

    int result = 0;

    const auto packagesStr = packages.keys().join(" ");

    emit beginRemovePackage(packagesStr);

    QDBusMessage reply = iface.call(d->m_removeMethodName, packagesStr);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        result = -2;
        emit endRemovePackage(packagesStr, result);
    }

    auto args = reply.arguments();
    result    = args.at(0).toInt();

    emit endRemovePackage(packagesStr, result);

    return result;
}

void ComponentsApplicator::readInstalledPackages()
{
    //TO DO refactor
    QDBusInterface iface(d->m_serviceName, RPM1_PATH, RPM1_INTERFACE_NAME, d->m_connection);
    if (!iface.isValid())
    {
        return;
    }

    QDBusReply<QStringList> reply = iface.call(RPM1_INTERFACE_LIST_METHOD_NAME);

    if (!reply.isValid())
    {
        return;
    }

    for (const QString &package : reply.value())
    {
        int z          = package.size() - 1;
        int firstDot   = package.lastIndexOf(".");
        int lastDash   = package.lastIndexOf("-");
        int secondDash = package.lastIndexOf("-", lastDash);
        int firstDash  = package.lastIndexOf("-", secondDash - 1);

        //^(.+)-(\d+\.\d+\.\d+)-alt(\d+)\.(x86_64|noarch)$

        QString arch    = package.mid(firstDot + 1, z - 1);
        QString release = package.mid(lastDash + 1, firstDot - lastDash - 1);
        QString version = package.mid(firstDash + 1, secondDash - firstDash - 1);
        QString name    = package.mid(0, firstDash);

        d->m_installedPackages.insert(name, 0);
    }
}

QMap<QString, model::ComponentObject> ComponentsApplicator::getIntactComponents(
    const QMap<QString, model::ComponentObject> &componentsToInstall,
    const QMap<QString, model::ComponentObject> &componentsToRemove,
    const QMap<QString, model::ComponentObject> &components)
{
    QMap<QString, model::ComponentObject> result;

    for (QString component : components.keys())
    {
        bool find = false;

        auto finder = [&find, &component](const model::ComponentObject &comp) {
            if (comp.m_id == component)
            {
                find = true;
            }
        };

        std::for_each(componentsToInstall.cbegin(), componentsToInstall.cend(), finder);

        if (find)
        {
            continue;
        }

        find = false;

        std::for_each(componentsToRemove.cbegin(), componentsToRemove.cend(), finder);

        if (!find)
        {
            result.insert(component, components[component]);
        }
    }

    return result;
}
} // namespace amc
