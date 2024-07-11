#ifndef AB_MODEL_MODEL_H
#define AB_MODEL_MODEL_H

#include "componentobject.h"
#include "item.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QTreeWidget>
#include <QVariant>

namespace amc::model
{
class Model final : public QStandardItemModel
{
public:
    Model();
    ~Model() override;

    void rebuildModel();
    void translateModel(QString locale);
    void resetCurrentState(ComponentsState state);
    ComponentsState getCurrentState();
    QMap<QString, ComponentObject> getComponents();
    Qt::CheckState getItemCheckStateBasedOnChildren(const QStandardItem *parent);
    QColor getItemColorBasedOnChildren(const QStandardItem *parent);
    void correctCheckItemStates();

public:
    Model(const Model &)            = delete;
    Model(Model &&)                 = delete;
    Model &operator=(const Model &) = delete;
    Model &operator=(Model &&)      = delete;

private:
    Qt::CheckState resetCurrentStateInner(ModelItem *parent, ComponentsState &state);
    void getCurrentStateInner(ModelItem *parent, ComponentsState &state);
    void getComponentsInner(ModelItem *parent, QMap<QString, ComponentObject> &components);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
} // namespace amc::model

#endif // AB_MODEL_MODEL_H
