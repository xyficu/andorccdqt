#ifndef ANDORTCP_H
#define ANDORTCP_H

#include <QObject>

class AndorTcp : public QObject
{
    Q_OBJECT
public:
    explicit AndorTcp(QObject *parent = 0);
    ~AndorTcp();

signals:

public slots:
};

#endif // ANDORTCP_H
