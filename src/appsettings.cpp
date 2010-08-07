#include "appsettings.h"

AppSettings::AppSettings(QObject *parent)
    :QSettings ("Pashazz", "Winegame", parent)
{}
void AppSettings::createConfigValue (const QString &key, const QVariant &value)
{
    if (this->value(key).isNull ())
	setValue (key, value);
}

AppSettings::~AppSettings ()
{
    sync ();
}

QByteArray AppSettings::readGeometry (const QString &object)
{
    return value (QString ("Geometry/%1").arg (object)).toByteArray ();
}

void AppSettings::writeGeometry (const QByteArray &data, const QString &object)
{
    setValue (QString("Geometry/%1").arg (object), data);
}

bool AppSettings::feedBack ()
{
    return value ("Settings/AskReports", true).toBool ();
}

QByteArray AppSettings::readState (const QString &object)
{
    return value (QString ("State/%1").arg (object)).toByteArray ();
}

void AppSettings::writeState (const QByteArray &data, const QString &object)
{
    setValue (QString("State/%1").arg (object), data);
}
