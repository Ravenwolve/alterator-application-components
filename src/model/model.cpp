#include "model.h"
#include "model/item.h"
#include "modelbuilder.h"

#include <map>
#include <QAbstractItemModel>
#include <QColor>
#include <QDebug>
#include <QList>
#include <QStandardItem>
#include <QtGlobal>

namespace amc::model
{
Model::Model()  = default;
Model::~Model() = default;

void Model::rebuildModel()
{
    clear();
    amc::model::ModelBuilder{}.build(this);
}

void Model::translateModel(QString locale)
{
    for (int i = 0; i < rowCount(); ++i)
    {
        auto currentItem = dynamic_cast<ModelItem *>(item(i));
        currentItem->translateItem(locale);
        if (currentItem->itemType == ModelItem::Component)
        {
            auto object = currentItem->data().value<ComponentObject *>();
            currentItem->setData(QVariant(object->m_displayName), Qt::DisplayRole);
        }
        else
        {
            auto object = currentItem->data().value<ComponentCategory *>();
            currentItem->setData(QVariant(object->m_displayName), Qt::DisplayRole);
        }
    }
}

void Model::setCurrentState(ComponentsState state)
{
    for (int i = 0; i < rowCount(); i++)
    {
        auto categoryItem = dynamic_cast<ModelItem *>(invisibleRootItem()->child(i));
        for (int j = 0; j < categoryItem->rowCount(); j++)
        {
            auto componentItem = dynamic_cast<ModelItem *>(categoryItem->child(j));

            auto component = componentItem->data().value<ComponentObject *>();
            if (component)
            {
                const auto componentState = state[component->m_id];
                switch (componentState)
                {
                case ComponentState::installed:
                    setData(componentItem->index(), Qt::Checked, Qt::CheckStateRole);
                    break;
                case ComponentState::partially_installed:
                    setData(componentItem->index(), Qt::PartiallyChecked, Qt::CheckStateRole);
                    break;
                case ComponentState::not_installed:
                    setData(componentItem->index(), Qt::Unchecked, Qt::CheckStateRole);
                    break;
                }
            }
        }
    }
}

ComponentsState Model::getCurrentState()
{
    ComponentsState state{};
    for (int i = 0; i < rowCount(); i++)
    {
        auto categoryItem = dynamic_cast<ModelItem *>(invisibleRootItem()->child(i));
        for (int j = 0; j < categoryItem->rowCount(); j++)
        {
            auto componentItem = dynamic_cast<ModelItem *>(categoryItem->child(j));

            auto component = componentItem->data().value<ComponentObject *>();
            if (component)
            {
                state.insert(component->m_id, component->m_state);
            }
        }
    }

    return state;
}

QMap<QString, ComponentObject> Model::getComponents()
{
    QMap<QString, ComponentObject> components;
    for (int i = 0; i < rowCount(); i++)
    {
        auto categoryItem = dynamic_cast<ModelItem *>(invisibleRootItem()->child(i));
        for (int j = 0; j < categoryItem->rowCount(); j++)
        {
            auto componentItem = dynamic_cast<ModelItem *>(categoryItem->child(j));
            auto component     = componentItem->data().value<ComponentObject *>();
            if (component)
            {
                components.insert(component->m_id, *component);
            }
        }
    }

    return components;
}

Qt::CheckState Model::getItemCheckStateBasedOnChildren(const QStandardItem *parent)
{
    std::map<Qt::CheckState, int> hasVariant = {{Qt::Unchecked, 0}, {Qt::PartiallyChecked, 0}, {Qt::Checked, 0}};

    int numberOfChildren = parent->rowCount();
    for (int row = 0; row < numberOfChildren; row++)
    {
        ++hasVariant[parent->child(row)->checkState()];
    }

    for (int variant = 0; variant < hasVariant.size(); ++variant)
    {
        if (hasVariant[static_cast<Qt::CheckState>(variant)] == numberOfChildren)
        {
            return static_cast<Qt::CheckState>(variant);
        };
    }

    return Qt::PartiallyChecked;
}

QColor Model::getItemColorBasedOnChildren(const QStandardItem *parent)
{
    for (int row = 0; row < parent->rowCount(); row++)
    {
        if (parent->child(row)->background().color() == QColorConstants::Svg::lightgray)
        {
            return QColorConstants::Svg::lightgray;
        }
    }

    return QColorConstants::White;
}

void Model::correctCheckItemStates()
{
    for (int i = 0; i < rowCount(); i++)
    {
        QStandardItem *item = invisibleRootItem()->child(i);

        QStandardItemModel::setData(item->index(), getItemCheckStateBasedOnChildren(item), Qt::CheckStateRole);
    }
}

bool Model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
    {
        return false;
    }

    const QStandardItem *item = itemFromIndex(index);
    const bool rv             = QStandardItemModel::setData(index, value, role);

    if (role == Qt::CheckStateRole)
    {
        // (un)check all children on item (un)check
        for (int i = 0; i < item->rowCount(); i++)
        {
            QStandardItem *childItem = item->child(i);
            setData(childItem->index(), value, role);
        }

        // update all parents state
        for (QStandardItem *parent = item->parent(); parent; parent = parent->parent())
        {
            QStandardItemModel::setData(parent->index(), getItemCheckStateBasedOnChildren(parent), role);
            parent->setBackground(getItemColorBasedOnChildren(parent));
        }
    }

    return rv;
}
} // namespace amc::model
