#include "amcobject.h"

#include <QDebug>
#include <QRegularExpression>

namespace amc::model
{
AMCObject::~AMCObject() = default;

void AMCObject::setLocale(QString locale)
{
    setFieldLocale(locale, m_displayNameLocaleStorage, m_displayName);
    setFieldLocale(locale, m_descriptionLocaleStorage, m_description);
    setFieldLocale(locale, m_commentLocaleStorage, m_comment);
}

QString AMCObject::findLocale(QString locale, QMap<QString, QString> &localeStorage)
{
    QRegularExpression regex(locale);
    for (auto fullLocale : localeStorage.keys())
    {
        QRegularExpressionMatch match = regex.match(fullLocale);
        if (match.hasMatch())
        {
            return localeStorage[fullLocale];
        }
    }
    return "";
}

void AMCObject::setFieldLocale(QString locale, QMap<QString, QString> &storage, QString &field)
{
    for (const auto &locale : QStringList{locale, locale.split('_').first()})
    {
        QString found = findLocale(locale, storage);
        if (!found.isEmpty())
        {
            field = found;
            return;
        }
        found = findLocale(locale + "_[A-Z]{2}", storage);
        if (!found.isEmpty())
        {
            field = found;
            return;
        }
    }
}
} // namespace amc::model
