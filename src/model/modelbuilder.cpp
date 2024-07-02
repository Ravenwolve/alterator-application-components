#include "modelbuilder.h"
#include "constants.h"
#include "model/componentcategory.h"
#include "model/componentobject.h"
#include "model/item.h"

#include <memory>
#include <vector>
#include <QChar>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QVariant>
#include <QWidget>
#include <QtGlobal>
#include <qbase/logger/prelude.h>

namespace amc::model
{
ModelBuilder::ModelBuilder()
    : m_connection(QDBusConnection::systemBus())
{}

void ModelBuilder::build(Model *model)
{
    std::vector<std::unique_ptr<ComponentCategory>> categories = buildCategories();
    std::vector<std::unique_ptr<ComponentObject>> components   = buildObjects();

    for (auto &category : categories)
    {
        QString categoryId = category->m_id;
        auto categoryItem  = std::make_unique<ModelItem>(std::move(category));

        for (int i = 0; i < components.size(); ++i)
        {
            if (components[i]->m_category != categoryId)
            {
                continue;
            }

            auto componentItem = std::make_unique<ModelItem>(std::move(components[i]));
            categoryItem->insertRow(categoryItem->rowCount(), componentItem.release());
            components.erase(components.begin() + i);
        }

        // NOTE(cherniginma): don't add empty categories
        if (categoryItem->rowCount() != 0)
        {
            model->insertRow(model->rowCount(), categoryItem.release());
        }
    }

    if (components.empty())
    {
        return;
    }

    // NOTE(cherniginma): put the rest of components in default category
    auto defaultCategory = std::make_unique<ModelItem>(std::move(buildDefaultCategory()));
    for (auto &component : components)
    {
        auto componentItem = std::make_unique<ModelItem>(std::move(component));
        defaultCategory->insertRow(defaultCategory->rowCount(), componentItem.release());
    }
    model->insertRow(model->rowCount(), defaultCategory.release());

    model->correctCheckItemStates();
}

std::unique_ptr<ComponentCategory> ModelBuilder::buildDefaultCategory()
{
    auto defaultCategory           = std::make_unique<ComponentCategory>();
    defaultCategory->m_id          = DEFAULT_CATEGORY_ID;
    defaultCategory->m_displayName = DEFAULT_CATEGORY_DISPLAY_NAME;

    return defaultCategory;
}

std::vector<std::unique_ptr<ComponentObject>> ModelBuilder::buildObjects()
{
    std::vector<std::unique_ptr<ComponentObject>> components{};

    QStringList objects = getObjectPaths();

    for (QString &object : objects)
    {
        QString objectInfo        = getComponentInfo(object);
        QString objectDescription = getComponentDescription(object);
        int objectStatus          = getComponentStatus(object);

        if (objectInfo.isEmpty() || objectStatus < 0)
        {
            continue;
        }

        BaseObjectParser parser{};
        parser.parse(objectInfo);
        std::unique_ptr<ComponentObject> newComponent = buildObject(&parser, objectDescription, objectStatus);

        if (newComponent)
        {
            components.push_back(std::move(newComponent));
        }
    }

    return components;
}

std::vector<std::unique_ptr<ComponentCategory>> ModelBuilder::buildCategories()
{
    std::vector<std::unique_ptr<ComponentCategory>> categories{};

    QStringList categoryNames = getCategoriesList();
    for (auto &categoryName : categoryNames)
    {
        QString categoryInfo = getCategoryInfo(categoryName);
        if (categoryInfo.isEmpty())
        {
            qWarning() << "Empty categoryInfo";
            continue;
        }

        BaseObjectParser parser{};
        parser.parse(categoryInfo);
        std::unique_ptr<ComponentCategory> newCategory = buildCategory(&parser);

        if (newCategory)
        {
            categories.push_back(std::move(newCategory));
        }
    }

    return categories;
}

std::unique_ptr<ComponentCategory> ModelBuilder::buildCategory(ObjectParserInterface *parser)
{
    auto object = std::make_unique<ComponentObject>();

    if (!buildBase(parser, ALTERATOR_SECTION_NAME, object.get()))
    {
        qWarning() << "Failed to build base";
        return {};
    }

    if (!buildFieldWithLocale(parser,
                              ALTERATOR_SECTION_NAME,
                              COMPONENT_DISPLAY_NAME_KEY_NAME,
                              object->m_displayName,
                              object->m_displayNameLocaleStorage))
    {
        object->m_displayName = object->m_id;
        qWarning() << "Failed to build field" << COMPONENT_DISPLAY_NAME_KEY_NAME << "of" << object->m_id;
    }

    if (!buildFieldWithLocale(parser,
                              ALTERATOR_SECTION_NAME,
                              COMPONENT_COMMENT_KEY_NAME,
                              object->m_comment,
                              object->m_commentLocaleStorage))
    {
        qWarning() << "Failed to build field" << COMPONENT_COMMENT_KEY_NAME << "of" << object->m_id;
    }

    object->m_packages = parseValuesFromKey(parser, ALTERATOR_SECTION_NAME, COMPONENT_PACKAGES_KEY_NAME, " ");

    auto category = std::make_unique<ComponentCategory>();

    category->m_id          = object->m_id;
    category->m_type        = object->m_type;
    category->m_displayName = object->m_displayName;
    category->m_comment     = object->m_comment;

    category->m_displayNameLocaleStorage = object->m_displayNameLocaleStorage;
    category->m_commentLocaleStorage     = object->m_commentLocaleStorage;

    return category;
}

QStringList ModelBuilder::getCategoriesList()
{
    QDBusInterface iface{QString(ALTERATOR_MANAGER_SERVICE_NAME),
                         QString(GLOBAL_PATH),
                         QString(COMPONENT_CATEGORIES_INTERFACE_NAME),
                         m_connection};

    if (!iface.isValid())
    {
        qWarning() << "Invalid interface";
        return {};
    }

    const QDBusReply<QStringList> reply = iface.call(COMPONENT_CATEGORIES_LIST_METHOD_NAME);

    if (!reply.isValid())
    {
        qWarning() << "Invalid DBus reply:" << reply.error().message();
        return {};
    }

    return reply.value();
}

QString ModelBuilder::getCategoryInfo(const QString &categoryName)
{
    QDBusInterface iface(ALTERATOR_MANAGER_SERVICE_NAME, GLOBAL_PATH, COMPONENT_CATEGORIES_INTERFACE_NAME, m_connection);

    if (!iface.isValid())
    {
        qWarning() << "Invalid interface";
        return {};
    }

    QDBusReply<QByteArray> reply = iface.call(COMPONENT_CATEGORIES_INFO_METHOD_NAME, categoryName);

    if (!reply.isValid())
    {
        qWarning() << "Invalid DBus reply:" << reply.error().message();
        return {};
    }

    return {reply.value()};
}

QStringList ModelBuilder::getObjectPaths()
{
    QDBusInterface iface{QString(ALTERATOR_MANAGER_SERVICE_NAME),
                         QString(ALTERATOR_MANAGER_PATH),
                         QString(ALTERATOR_MANAGER_INTERFACE_NAME),
                         m_connection};

    if (!iface.isValid())
    {
        qWarning() << "Invalid interface";
        return {};
    }

    const QDBusReply<QList<QDBusObjectPath>> reply = iface.call(ALTERATOR_MANAGER_GET_OBJECTS_METHOD_NAME,
                                                                COMPONENT1_INTERFACE_NAME);

    if (!reply.isValid())
    {
        qWarning() << "Invalid DBus reply:" << reply.error().message();
        return {};
    }

    QStringList result;
    for (const QDBusObjectPath &path : reply.value())
    {
        result.append(path.path());
    }

    return result;
}

QString ModelBuilder::getComponentInfo(const QString &path)
{
    QDBusInterface iface(ALTERATOR_MANAGER_SERVICE_NAME, path, COMPONENT1_INTERFACE_NAME, m_connection);

    if (!iface.isValid())
    {
        qWarning() << "Invalid interface";
        return {};
    }

    QDBusReply<QStringList> reply = iface.call(COMPONENT_INFO_METHOD_NAME);

    if (!reply.isValid())
    {
        qWarning() << "Invalid DBus reply:" << reply.error().message();
        return {};
    }

    return reply.value().join("\n");
}

QString ModelBuilder::getComponentDescription(const QString &path)
{
    QDBusInterface iface(ALTERATOR_MANAGER_SERVICE_NAME, path, COMPONENT1_INTERFACE_NAME, m_connection);

    if (!iface.isValid())
    {
        qWarning() << "Invalid interface";
        return {};
    }

    QDBusReply<QStringList> reply = iface.call(COMPONENT_DESCRIPTION_METHOD_NAME);

    if (!reply.isValid())
    {
        qWarning() << "Invalid DBus reply:" << reply.error().message();
        return {};
    }

    return reply.value().join("\n");
}

int ModelBuilder::getComponentStatus(const QString &path)
{
    QDBusInterface iface(ALTERATOR_MANAGER_SERVICE_NAME, path, COMPONENT1_INTERFACE_NAME, m_connection);

    if (!iface.isValid())
    {
        qWarning() << "Invalid interface";
        return -1;
    }

    QDBusMessage reply = iface.call(COMPONENT_STATUS_METHOD_NAME);

    if (reply.type() == QDBusMessage::InvalidMessage || reply.arguments().size() != 2)
    {
        qWarning() << "Invalid reply";
        return -1;
    }

    bool ok = false;

    int response = reply.arguments().at(1).toInt(&ok);

    if (!ok)
    {
        qWarning() << "Invalid response";
        return -1;
    }

    if (response)
    {
        qWarning() << "Error! Can't get status";
    }

    QStringList statusList = reply.arguments().at(0).toStringList();

    if (statusList.isEmpty() || statusList.size() > 1)
    {
        qWarning() << "Invalid answer";
        return -1;
    }

    ok = false;

    QString statusStr = statusList.at(0);

    int status = statusStr.toInt(&ok);

    if (!ok)
    {
        qWarning() << "Invalid status, can't convert to int";
        return -1;
    }

    return status;
}

bool ModelBuilder::buildBase(ObjectParserInterface *parser, QString sectionName, ComponentObject *object)
{
    QString type = parser->getValue(sectionName, COMPONENT_OBJECT_TYPE_KEY_NAME);
    if (type.isEmpty())
    {
        return false;
    }
    object->m_type = type;

    QString name = parser->getValue(sectionName, COMPONENT_NAME_KEY_NAME);
    if (name.isEmpty())
    {
        return false;
    }
    object->m_id = name;

    QString category = parser->getValue(sectionName, COMPONENT_CATEGORY_KEY_NAME);
    if (!category.isEmpty())
    {
        object->m_category = category;
    }

    return true;
}

bool ModelBuilder::buildFieldWithLocale(ObjectParserInterface *parser,
                                        QString sectionName,
                                        QString entryName,
                                        QString &field,
                                        QMap<QString, QString> &localeStorage)
{
    auto sections = parser->getSections();

    auto alteratorEntrySectionIt = sections.find(sectionName);
    if (alteratorEntrySectionIt == sections.end())
    {
        return false;
    }

    auto section = *alteratorEntrySectionIt;

    QList<ObjectParserInterface::IniField> listOfKeys = section.values(entryName);

    QString defaultName = parser->getDefaultValue(listOfKeys);

    if (defaultName.isEmpty())
    {
        return false;
    }

    field = defaultName;

    for (const ObjectParserInterface::IniField &currentIniFileKey : listOfKeys)
    {
        localeStorage.insert(currentIniFileKey.keyLocale, currentIniFileKey.value.toString());
    }

    return true;
}

std::vector<QString> ModelBuilder::parseValuesFromKey(ObjectParserInterface *parser,
                                                      QString section,
                                                      QString key,
                                                      QString delimiter)
{
    QString values = parser->getValue(section, key);
    if (values.isEmpty())
    {
        return {};
    }

    if (values.back() == delimiter)
    {
        values.truncate(values.size() - 1);
    }

    QStringList valuesList = values.split(delimiter);

    std::vector<QString> result;
    for (QString &currentValue : valuesList)
    {
        if (!currentValue.isEmpty())
        {
            result.push_back(currentValue);
        }
    }

    return result;
}

std::unique_ptr<ComponentObject> ModelBuilder::buildObject(BaseObjectParser *parser, QString &description, int status)
{
    auto object = std::make_unique<ComponentObject>();

    if (!buildBase(parser, ALTERATOR_SECTION_NAME, object.get()))
    {
        return {};
    }

    if (!buildFieldWithLocale(parser,
                              ALTERATOR_SECTION_NAME,
                              COMPONENT_DISPLAY_NAME_KEY_NAME,
                              object->m_displayName,
                              object->m_displayNameLocaleStorage))
    {
        object->m_displayName = object->m_id;
        qWarning() << "Failed to build field" << COMPONENT_DISPLAY_NAME_KEY_NAME << "of" << object->m_id;
    }

    if (buildFieldWithLocale(parser,
                             ALTERATOR_SECTION_NAME,
                             COMPONENT_COMMENT_KEY_NAME,
                             object->m_comment,
                             object->m_commentLocaleStorage))
    {
        qWarning() << "Failed to build field" << COMPONENT_COMMENT_KEY_NAME << "of" << object->m_id;
    }

    object->m_packages = parseValuesFromKey(parser, ALTERATOR_SECTION_NAME, COMPONENT_PACKAGES_KEY_NAME, " ");

    object->m_description = description;
    object->m_descriptionLocaleStorage.insert("", description);

    object->m_state = static_cast<ComponentState>(status);

    return object;
}
} // namespace amc::model
