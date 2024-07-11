#ifndef OBJECT_H
#define OBJECT_H

#include <QMap>
#include <QString>

namespace amc::model
{
class AMCObject
{
public:
    virtual ~AMCObject() = 0;
    virtual void setLocale(QString locale);

public:
    QString m_id{};   // Mandatory
    QString m_type{}; // Mandatory
    QString m_category{};
    QString m_displayName{};
    QString m_description{};
    QString m_comment{};

    QMap<QString, QString> m_displayNameLocaleStorage{};
    QMap<QString, QString> m_descriptionLocaleStorage{};
    QMap<QString, QString> m_commentLocaleStorage{};

protected:
    static QString findLocale(QString locale, QMap<QString, QString> &localeStorage);
    static void setFieldLocale(QString locale, QMap<QString, QString> &storage, QString &field);
};
} // namespace amc::model

#endif // OBJECT_H
