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
    for (size_t i = 0; i < rowCount(); ++i)
    {
        auto currentItem = dynamic_cast<ModelItem *>(item(i));
        currentItem->translateItem(locale);
        
        auto object = currentItem->itemType == ModelItem::Component
                    ? static_cast<AMCObject *>(currentItem->data().value<ComponentObject   *>())
                    : static_cast<AMCObject *>(currentItem->data().value<ComponentCategory *>());

        currentItem->setData(QVariant(object->m_displayName), Qt::DisplayRole);
    }
}

void Model::resetCurrentState(ComponentsState state)
{
    for (size_t i = 0; i < rowCount(); ++i)
    {
        auto categoryItem = dynamic_cast<ModelItem *>(invisibleRootItem()->child(i));
        auto checkStateOfCategory = resetCurrentStateInner(categoryItem, state);
        categoryItem->setData(checkStateOfCategory, Qt::CheckStateRole);
    }
}

Qt::CheckState Model::resetCurrentStateInner(ModelItem *parent, ComponentsState &state)
{
    std::map<Qt::CheckState, int> hasVariant = {{Qt::Unchecked, 0}, {Qt::PartiallyChecked, 0}, {Qt::Checked, 0}};
    parent->setBackground(QColorConstants::White);
    const size_t numberOfChildren = parent->rowCount();
    for (size_t i = 0; i < numberOfChildren; ++i)
    {
        auto childItem = dynamic_cast<ModelItem *>(parent->child(i));
        if (childItem->itemType == ModelItem::Type::Category)
        {
            auto checkStateOfCategory = resetCurrentStateInner(childItem, state);
            childItem->setData(checkStateOfCategory, Qt::CheckStateRole);
            ++hasVariant[checkStateOfCategory];
        }
        else
        {
            auto component = childItem->data().value<ComponentObject *>();
            if (component)
            {
                auto checkStateOfComponent = static_cast<Qt::CheckState>(state[component->m_id]); 
                childItem->setData(checkStateOfComponent, Qt::CheckStateRole);
                ++hasVariant[checkStateOfComponent];
            }
        }
    }

    if (hasVariant[Qt::Unchecked] == numberOfChildren)
    {
        return Qt::Unchecked;
    }
    else if (hasVariant[Qt::Checked] == numberOfChildren)
    {
        return Qt::Checked;
    }
    return Qt::PartiallyChecked;
}

ComponentsState Model::getCurrentState()
{
    ComponentsState state{};
    for (size_t i = 0; i < rowCount(); ++i)
    {
        auto categoryItem = dynamic_cast<ModelItem *>(invisibleRootItem()->child(i));
        getCurrentStateInner(categoryItem, state);
    }
    
    return state;
}

void Model::getCurrentStateInner(ModelItem *parent, ComponentsState &state)
{
    const size_t numberOfChildren = parent->rowCount();
    for (size_t i = 0; i < numberOfChildren; ++i)
    {
        auto childItem = dynamic_cast<ModelItem *>(parent->child(i));
        if (childItem->itemType == ModelItem::Type::Category)
        {
            getCurrentStateInner(childItem, state);
        }
        else
        {
            auto component = childItem->data().value<ComponentObject *>();
            if (component)
            {
                state.insert(component->m_id, component->m_state);
            }
        }
    }
}

QMap<QString, ComponentObject> Model::getComponents()
{
    QMap<QString, ComponentObject> components;
    for (size_t i = 0; i < rowCount(); ++i)
    {
        auto categoryItem = dynamic_cast<ModelItem *>(invisibleRootItem()->child(i));
        getComponentsInner(categoryItem, components);
    }

    return components;
}

void Model::getComponentsInner(ModelItem *parent, QMap<QString, ComponentObject> &components)
{
    const size_t numberOfChildren = parent->rowCount();
    for (size_t i = 0; i < numberOfChildren; ++i)
    {
        auto childItem = dynamic_cast<ModelItem *>(parent->child(i));
        if (childItem->itemType == ModelItem::Type::Category)
        {
            getComponentsInner(childItem, components);
        }
        else
        {
            auto component = childItem->data().value<ComponentObject *>();
            if (component)
            {
                components.insert(component->m_id, *component);
            }
        }
    }
}

Qt::CheckState Model::getItemCheckStateBasedOnChildren(const QStandardItem *parent)
{
    std::map<Qt::CheckState, int> hasVariant = {{Qt::Unchecked, 0}, {Qt::PartiallyChecked, 0}, {Qt::Checked, 0}};
    
    const size_t numberOfChildren = parent->rowCount();
    for (size_t row = 0; row < numberOfChildren; ++row)
    {
        auto child      = parent->child(row);
        auto checkState = dynamic_cast<ModelItem *>(child)->itemType == ModelItem::Type::Category
                        ? getItemCheckStateBasedOnChildren(child)
                        : child->checkState();
        QStandardItemModel::setData(child->index(), checkState, Qt::CheckStateRole);
        ++hasVariant[checkState];
    }

    if (hasVariant[Qt::Unchecked] == numberOfChildren)
    {
        return Qt::Unchecked;
    }
    else if (hasVariant[Qt::Checked] == numberOfChildren)
    {
        return Qt::Checked;
    }
    return Qt::PartiallyChecked;
}

QColor Model::getItemColorBasedOnChildren(const QStandardItem *parent)
{
    for (size_t row = 0; row < parent->rowCount(); ++row)
    {
        auto child = parent->child(row);
        if (child->background().color() == QColorConstants::Svg::lightgray)
        {
            return QColorConstants::Svg::lightgray;
        }
    }

    return QColorConstants::White;
}

void Model::correctCheckItemStates()
{
    for (size_t i = 0; i < rowCount(); ++i)
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
        for (size_t i = 0; i < item->rowCount(); ++i)
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
