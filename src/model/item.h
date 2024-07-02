#ifndef AB_MODEL_OBJECT_ITEM_H
#define AB_MODEL_OBJECT_ITEM_H

#include "componentobject.h"
#include "model/componentcategory.h"

#include <memory>
#include <QStandardItem>

namespace amc::model
{
class ModelItem : public QStandardItem
{
public:
    enum Type
    {
        Category,
        Component
    };

public:
    explicit ModelItem(std::unique_ptr<ComponentObject> component);
    explicit ModelItem(std::unique_ptr<ComponentCategory> category);
    ~ModelItem() override = default;

    void translateItem(QString locale);

public:
    Type itemType;

public:
    ModelItem(ModelItem &)                  = delete;
    ModelItem(ModelItem &&)                 = delete;
    ModelItem &operator=(const ModelItem &) = delete;
    ModelItem &operator=(ModelItem &&)      = delete;
};
} // namespace amc::model

#endif // AB_MODEL_OBJECT_ITEM_H
