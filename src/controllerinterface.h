#ifndef CONTROLLERINTERFACE_H
#define CONTROLLERINTERFACE_H

#include "model/item.h"
#include "model/model.h"

namespace amc
{
class ControllerInterface
{
public:
    virtual ~ControllerInterface() = default;

    virtual void setModel(amc::model::Model *model)   = 0;
    virtual void ComponentSelected(QModelIndex index) = 0;
    virtual void itemChanged(model::ModelItem *item)  = 0;

    virtual void exit()  = 0;
    virtual void apply() = 0;
    virtual void reset() = 0;
    virtual void ok()    = 0;
};
} // namespace amc
#endif // CONTROLLERINTERFACE_H
