#ifndef COMPONENTOBJECT_H
#define COMPONENTOBJECT_H

#include "model/amcobject.h"
#include <QMap>
#include <QObject>
#include <QString>

namespace amc
{
enum ComponentState : int
{
    not_installed       = 0,
    partially_installed = 1,
    installed           = 2
};

using ComponentsState = QMap<QString, ComponentState>;

namespace model
{
class ComponentObject : public AMCObject
{
public:
    ComponentObject() = default;
    ComponentObject(const ComponentObject &component);
    ~ComponentObject() override;

public:
    ComponentState m_state{not_installed};

    std::vector<QString> m_packages{};

public:
    void setState(ComponentState state);
};
} // namespace model
} // namespace amc

Q_DECLARE_METATYPE(amc::model::ComponentObject *);

#endif // COMPONENTOBJECT_H
