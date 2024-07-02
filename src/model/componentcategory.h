#ifndef COMPONENT_CATEGORY_H
#define COMPONENT_CATEGORY_H

#include "model/amcobject.h"

#include <QObject>

namespace amc::model
{
class ComponentCategory : public AMCObject
{
public:
    ~ComponentCategory() override = default;
};
} // namespace amc::model

Q_DECLARE_METATYPE(amc::model::ComponentCategory *);

#endif // COMPONENT_CATEGORY_H
