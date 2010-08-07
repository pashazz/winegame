#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QtCore>

class AppSettings : protected QSettings
{
    Q_OBJECT
public:
    explicit AppSettings(QObject *parent = 0);
    bool feedBack ();
    void createConfigValue (const QString &key, const QVariant &value);
    void writeState (const QByteArray &data, const QString &object);
    QByteArray readState (const QString& object);

    void writeGeometry (const QByteArray &data, const QString &object);
    QByteArray readGeometry (const QString& object);
    ~AppSettings ();
};

#endif // APPSETTINGS_H
