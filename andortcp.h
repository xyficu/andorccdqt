#ifndef ANDORTCP_H
#define ANDORTCP_H

#include <QObject>
#include <qabstractsocket.h>
#include "andoruser.h"

class QTcpSocket;

class AndorTcp : public QObject
{
    Q_OBJECT
public:
    explicit AndorTcp(QObject *parent = 0);
    ~AndorTcp();

private:
    QTcpSocket *m_tcpSocket;
    QString m_message;
    quint16 m_blockSize;
    bool m_tcpCon;
    AndorCcDParams m_andorCcdParams;

    void ResolveMessage(QString msg);
    void ReConnToHost();
    void RegisterDevice();
    void DeregisterDevice();

private slots:
    void NewConnect();
    void ReadMessage();
    void DispalyError(QAbstractSocket::SocketError);
    void OnConnected();
    void OnDisconnected();
    void OnAboutToClose();
    void StopConnect();


signals:
    void TGetImage(QString fileName, bool shutterOpen, float expTime, qint32 amount=1);
    void TGetAllStat(qint32 *temp, bool *coolerSwitch, bool *isAcq, qint32 *gain, qint32 *binx, qint32 *biny, QString *imgSavPath);
    void TSetImageSavPath(QString path);
    void TSetTemp(qint32 temp);
    void TSetCoolerSwitch(bool coolerSwitch);
    void TSetGain(qint32 gain);
    void TSetBin(qint32 binx, qint32 biny);


public slots:
};

#endif // ANDORTCP_H
