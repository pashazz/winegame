#ifndef DISCDETECTOR_H
#define DISCDETECTOR_H

#include <QObject>
#include "linux.h"
#include "engine.h"
#include "gamedialog.h"
class DiscDetector : public QObject
{
Q_OBJECT
public:
    explicit DiscDetector(QObject *parent = 0);
 bool  tryDetect (QString path);
 QString getGame () {return gamefolder;}
 void lauchApp (); //call only even tryDetect is TRUE!!!!!!!!
signals:

public slots:

private:
 QString gamefolder;
 QString cdroot;
};

#endif // DISCDETECTOR_H
