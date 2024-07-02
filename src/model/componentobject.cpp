#include "componentobject.h"

#include <QRegularExpression>

namespace amc::model
{
ComponentObject::ComponentObject(const ComponentObject &component)
    : m_category(component.m_category)
{
    this->m_id          = component.m_id;
    this->m_displayName = component.m_displayName;
    this->m_description = component.m_description;
    this->m_comment     = component.m_comment;

    this->m_displayNameLocaleStorage = component.m_displayNameLocaleStorage;
    this->m_descriptionLocaleStorage = component.m_descriptionLocaleStorage;
    this->m_commentLocaleStorage     = component.m_commentLocaleStorage;

    std::copy(component.m_packages.begin(), component.m_packages.end(), std::back_inserter(this->m_packages));
}

ComponentObject::~ComponentObject() = default;

void ComponentObject::setState(ComponentState state)
{
    m_state = state;
}
} // namespace amc::model
