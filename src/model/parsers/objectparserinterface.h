#ifndef AOB_OBJECT_PARSER_INTERFACE_H
#define AOB_OBJECT_PARSER_INTERFACE_H

#include <QMap>
#include <QMultiMap>
#include <QString>
#include <QVariant>

namespace amc::model
{
class ObjectParserInterface
{
public:
    struct IniField
    {
        QString key{};
        QString keyLocale{};
        QVariant value{};
    };

    using Section  = QMultiMap<QString, IniField>;
    using Sections = QMap<QString, Section>;

public:
    virtual ~ObjectParserInterface() = default;

    virtual bool parse(QString data) = 0;

    virtual Sections getSections()                              = 0;
    virtual QString getKeyLocale(QString keyName)               = 0;
    virtual QString getKeyNameWithoutLocale(QString keyName)    = 0;
    virtual QString getDefaultValue(QList<IniField> iniFileKey) = 0;
    virtual QString getValue(QString section, QString key)      = 0;
};

} // namespace amc::model

#endif
