#ifndef ANDORUSER_H
#define ANDORUSER_H
#include "atmcdLXd.h"

#include <QObject>

class AndorUser : public QObject
{
    Q_OBJECT
public:
    explicit AndorUser(QObject *parent = 0);
    ~AndorUser();

signals:

public slots:
};

#endif // ANDORUSER_H
