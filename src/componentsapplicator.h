#ifndef COMPONENTSAPPLICATOR_H
#define COMPONENTSAPPLICATOR_H

#include <memory>
#include <QDBusConnection>
#include <QString>

#include "model/componentobject.h"

namespace amc
{
class ComponentsApplicatorPrivate;

class ComponentsApplicator : public QObject
{
    Q_OBJECT
public:
    ComponentsApplicator(
        QString serviceName, QString path, QString interface, QString installMethod, QString removeMethod);
    ~ComponentsApplicator() override;

    void setTask(ComponentsState currentState,
                 ComponentsState applyingState,
                 QMap<QString, model::ComponentObject> components);

    QMap<QString, QString> getPackagesToInstall();
    QMap<QString, QString> getPackagesToRemove();

    int apply();

signals:
    void beginApply(const int installPackagesCount, const int removePackagesCount);
    void endApply();
    void beginInstallPackage(QString package);
    void endInstallPackage(QString package, int result);
    void beginRemovePackage(QString package);
    void endRemovePackage(QString package, int result);

private:
    QMap<QString, model::ComponentObject> getComponentsToInstall(const ComponentsState currentState,
                                                                 const ComponentsState applyingState,
                                                                 const QMap<QString, model::ComponentObject> &components);
    QMap<QString, model::ComponentObject> getComponentsToRemove(const ComponentsState currentState,
                                                                const ComponentsState applyingState,
                                                                const QMap<QString, model::ComponentObject> &components);
    QMap<QString, model::ComponentObject> getIntactComponents(
        const QMap<QString, model::ComponentObject> &componentsToInstall,
        const QMap<QString, model::ComponentObject> &componentsToRemove,
        const QMap<QString, model::ComponentObject> &components);

    QMap<QString, QString> getPackagesToInstall(QMap<QString, model::ComponentObject> installComponents);
    QMap<QString, QString> getPackagesToRemove(QMap<QString, model::ComponentObject> removeComponents);
    QMap<QString, QString> getPackagesFromComponents(QMap<QString, model::ComponentObject> components);

    void removePackagesFromIntactComponents(QMap<QString, QString> &removePackages,
                                            QMap<QString, model::ComponentObject> intactComponents);
    void removePackagesFromInstallComponents(QMap<QString, QString> &removePackages,
                                             QMap<QString, model::ComponentObject> installComponents);
    void removeInstalledPackages(QMap<QString, QString> &packagesToInstall);

    int installPackages(QMap<QString, QString> packages);
    int removePackages(QMap<QString, QString> packages);

    void readInstalledPackages();

    void setManagerLocale();

private:
    std::unique_ptr<ComponentsApplicatorPrivate> d;
};
} // namespace amc
#endif // COMPONENTSAPPLICATOR_H
