#ifndef AOB_BASEOBJECTPARSER_H
#define AOB_BASEOBJECTPARSER_H

#include "objectparserinterface.h"

namespace amc::model
{
class BaseObjectParser : public ObjectParserInterface
{
public:
    BaseObjectParser() = default;
    ~BaseObjectParser() override;

public:
    Sections getSections() override;
    bool parse(QString data) override;

    QString getKeyLocale(QString keyName) override;
    QString getKeyNameWithoutLocale(QString keyName) override;
    QString getDefaultValue(QList<IniField> iniFileKey) override;
    QString getValue(QString section, QString key) override;

private:
    Sections m_sections{};
};

} // namespace amc::model

#endif // BASEOBJECTPARSER_H
