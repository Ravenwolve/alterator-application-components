#ifndef AB_MODEL_MODEL_H
#define AB_MODEL_MODEL_H

#include "componentobject.h"

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
    void setCurrentState(ComponentsState state);
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
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
} // namespace amc::model

#endif // AB_MODEL_MODEL_H
