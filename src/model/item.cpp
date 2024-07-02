#include "item.h"
#include "model/componentcategory.h"
#include "model/componentobject.h"

#include <memory>
#include <QDebug>
#include <QStandardItemModel>
#include <QVariant>
#include <QtGlobal>

namespace amc::model
{
ModelItem::ModelItem(std::unique_ptr<ComponentObject> component)
    : itemType(Component)
{
    setEditable(false);
    setCheckable(true);

    switch (component->m_state)
    {
    case ComponentState::installed:
        setCheckState(Qt::Checked);
        break;
    case ComponentState::partially_installed:
        setCheckState(Qt::PartiallyChecked);
        break;
    case ComponentState::not_installed:
        setCheckState(Qt::Unchecked);
        break;
    default:
        qWarning() << "Undefined item state";
        setCheckState(Qt::Unchecked);
    }

    setText(component->m_displayName);
    setData(QVariant::fromValue(component.release()));
}

ModelItem::ModelItem(std::unique_ptr<ComponentCategory> category)
    : itemType(Category)
{
    setEditable(false);
    setCheckable(true);

    setText(category->m_displayName);
    setData(QVariant::fromValue(category.release()));
}

void ModelItem::translateItem(QString locale)
{
    if (itemType == Component)
    {
        auto obj = data().value<ComponentObject *>();
        obj->setLocale(locale);
        setText(obj->m_displayName);
    }
    else if (itemType == Category)
    {
        auto obj = data().value<ComponentCategory *>();
        obj->setLocale(locale);
        setText(obj->m_displayName);
    }

    for (int i = 0; i < rowCount(); ++i)
    {
        auto item = dynamic_cast<ModelItem *>(child(i));
        item->translateItem(locale);
    }
}
} // namespace amc::model
