#ifndef MODELBUILDER_H
#define MODELBUILDER_H

#include "model/componentcategory.h"
#include "model/componentobject.h"
#include "model/model.h"
#include "model/parsers/baseobjectparser.h"
#include "model/item.h"

#include <memory>
#include <vector>
#include <QDBusConnection>

namespace amc::model
{
class ModelBuilder
{
public:
    ModelBuilder();

    void build(Model *model);

private:
    void buildInner(ModelItem *item,
                    std::unordered_map<QString, std::vector<std::unique_ptr<ComponentCategory>>> &categories,
                    std::unordered_map<QString, std::vector<std::unique_ptr<ComponentObject>>> &components);
    
    QStringList getObjectPaths();
    std::unordered_map<QString, std::vector<std::unique_ptr<ComponentCategory>>> buildCategories();
    std::unordered_map<QString, std::vector<std::unique_ptr<ComponentObject>>> buildObjects();
    QStringList getCategoriesList();
    QString getCategoryInfo(const QString &path);
    QString getComponentInfo(const QString &path);
    QString getComponentDescription(const QString &path);
    int getComponentStatus(const QString &path);

    std::unique_ptr<ComponentCategory> buildCategory(ObjectParserInterface *parser);
    std::unique_ptr<ComponentObject> buildObject(BaseObjectParser *parser, QString &description, int status);
    std::unique_ptr<ComponentCategory> buildDefaultCategory();

    bool buildBase(ObjectParserInterface *parser, QString sectionName, ComponentObject *object);

    bool buildFieldWithLocale(ObjectParserInterface *parser,
                              QString sectionName,
                              QString entryName,
                              QString &field,
                              QMap<QString, QString> &localeStorage);

    std::vector<QString> parseValuesFromKey(ObjectParserInterface *parser,
                                            QString section,
                                            QString key,
                                            QString delimiter);

private:
    QDBusConnection m_connection;
};
} // namespace amc::model

#endif // MODELBUILDER_H
