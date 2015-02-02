#include "andortcp.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>

AndorTcp::AndorTcp(QObject *parent) : QObject(parent)
{
    m_tcpSocket=new QTcpSocket(this);
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(ReadMessage()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(DispalyError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(OnConnected()));
    connect(m_tcpSocket, SIGNAL(aboutToClose()), this, SLOT(OnAboutToClose()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(OnDisconnected()));

    connect(&m_tmrHouseKp, SIGNAL(timeout()), this, SLOT(HouseKeeping()));
    m_tmrHouseKp.start(10000);
}

AndorTcp::~AndorTcp()
{
    OnAboutToClose();
    m_tcpSocket->close();
}

void AndorTcp::ResolveMessage(QString msg)
{
    try
    {
        QString deviceType, cmd, value, fileName, shutter, expTime, amount, lt;
        qint32 temp, gain, binx, biny, curNumb, imgAmt;
        float acqProc;
        bool coolerSwitch, isAcq;
        QString imgSavPath, reply;
        //split message to cmd string
        QStringList cmdList = m_message.split(",");
        deviceType = cmdList[0];

        if(cmdList.count()<3)
            return;
        if(deviceType != "C")
            return;
        cmd = cmdList[1];
        lt = cmdList.last();

        if(cmd == "GETIMG")
        {
            fileName = cmdList[2];
            shutter = cmdList[3];
            expTime = cmdList[4];
            amount = cmdList[5];
            lt = cmdList[6];
            //emit signal set filter wheel position
            emit TGetImage(fileName,
                           shutter.toInt()==1?true:false,
                           expTime.toFloat(),
                           amount.toInt());
            //send reply message
            cmdList.insert(cmdList.length()-1, "0");
            reply = "R"+cmdList.join(',');
            m_tcpSocket->write(reply.toLatin1());
            m_tcpSocket->waitForBytesWritten();
            qDebug()<<"send:"<<reply;


        }
        else if(cmd == "ABORTACQ")
        {
            emit TAbortAcq();
            //send reply
            cmdList.insert(cmdList.length()-1, "0");
            reply = "R"+cmdList.join(',');
            m_tcpSocket->write(reply.toLatin1());
            m_tcpSocket->waitForBytesWritten();
            qDebug()<<"send:"<<reply;

        }
        else if(cmd == "PATH")
        {
            imgSavPath = cmdList[2];
            //send  signal
            emit TSetImageSavPath(imgSavPath);
            //send reply
            cmdList.insert(cmdList.length()-1, "0");
            reply = "R"+cmdList.join(',');
            m_tcpSocket->write(reply.toLatin1());
            m_tcpSocket->waitForBytesWritten();
            qDebug()<<"send:"<<reply;

        }
        else if(cmd == "SETGAIN")
        {
            gain = cmdList[2].toInt();
            //send  signal
            emit TSetGain(gain);
            //send reply
            cmdList.insert(cmdList.length()-1, "0");
            reply = "R"+cmdList.join(',');
            m_tcpSocket->write(reply.toLatin1());
            m_tcpSocket->waitForBytesWritten();
            qDebug()<<"send:"<<reply;
        }
        else if(cmd == "SETBIN")
        {

            binx = cmdList[2].toInt();
            biny = cmdList[3].toInt();
            //send  signal
            emit TSetBin(binx, biny);
            //send reply
            cmdList.insert(cmdList.length()-1, "0");
            reply = "R"+cmdList.join(',');
            m_tcpSocket->write(reply.toLatin1());
            m_tcpSocket->waitForBytesWritten();
            qDebug()<<"send:"<<reply;
        }
        else if(cmd == "SETCOOLERTEMP")
        {
            temp = cmdList[2].toInt();
            //send  signal
            emit TSetTemp(temp);
            //send reply
            cmdList.insert(cmdList.length()-1, "0");
            reply = "R"+cmdList.join(',');
            m_tcpSocket->write(reply.toLatin1());
            m_tcpSocket->waitForBytesWritten();
            qDebug()<<"send:"<<reply;
        }
        else if (cmd == "SETCOOLERSWITCH")
        {
            coolerSwitch = cmdList[2]=="1"?true:false;
            //send  signal
            emit TSetCoolerSwitch(coolerSwitch);
            //send reply
            cmdList.insert(cmdList.length()-1, "0");
            reply = "R"+cmdList.join(',');
            m_tcpSocket->write(reply.toLatin1());
            m_tcpSocket->waitForBytesWritten();
            qDebug()<<"send:"<<reply;
        }
        else if(cmd == "STATUS")
        {
            value = "";
            lt = cmdList[2];
            //emit signal get device status
            emit TGetAllStat(&temp,&coolerSwitch, &isAcq, &gain, &binx, &biny, &imgSavPath, &acqProc, &curNumb, &imgAmt);
            //send replay message
            cmdList.insert(cmdList.length()-1, QString::number(temp));
            cmdList.insert(cmdList.length()-1, coolerSwitch==true?"1":"0");
            cmdList.insert(cmdList.length()-1, isAcq==true?"1":"0");
            cmdList.insert(cmdList.length()-1, QString::number(gain));
            cmdList.insert(cmdList.length()-1, QString::number(binx));
            cmdList.insert(cmdList.length()-1, QString::number(biny));
            cmdList.insert(cmdList.length()-1, imgSavPath);
            cmdList.insert(cmdList.length()-1, QString::number(acqProc));
            cmdList.insert(cmdList.length()-1, QString::number(curNumb));
            cmdList.insert(cmdList.length()-1, QString::number(imgAmt));
            qDebug()<<"binx"<<binx;
            qDebug()<<"biny"<<biny;
            reply = "R"+cmdList.join(',');
            m_tcpSocket->write(reply.toLatin1());
            m_tcpSocket->waitForBytesWritten();
            qDebug()<<"send:"<<reply;

        }else
            return;
    }
    catch(QString exception)
    {
        qDebug()<<"Resolve message error: "<<exception;
    }
}

void AndorTcp::ReConnToHost()
{
    //if socket is down try to reconnect to the host
    m_tcpCon = false;
    while(!m_tcpCon)
    {
        qDebug()<<"try to connect to host...";
        m_tcpSocket->connectToHost("190.168.1.115", 30001);
        m_tcpSocket->waitForConnected(5000);
        QThread::sleep(5);
    }
}

void AndorTcp::RegisterDevice()
{
    //send register message
    QByteArray deviceRegMsg = QByteArray("CCD");
    m_tcpSocket->write(deviceRegMsg);
    m_tcpSocket->waitForBytesWritten();
}

void AndorTcp::DeregisterDevice()
{
    //send deregister message
    QByteArray deviceDeregMsg = QByteArray("RCCD");
    m_tcpSocket->write(deviceDeregMsg);
    m_tcpSocket->waitForBytesWritten();
}

void AndorTcp::HouseKeeping()
{
    if(m_tcpCon == true)
    {
        //get local time YYYYMMDDTHHMMSS.SSS
        m_dateTime=QDateTime::currentDateTime();
        QString lt = m_dateTime.toString("yyyyMMddThhmmss.zzz");

        //send HouseKeeping message
        QByteArray msg = QByteArray("RC,HOUSEKEEPING,"+lt.toLatin1());
        m_tcpSocket->write(msg);
        m_tcpSocket->waitForBytesWritten();
    }
}

void AndorTcp::NewConnect()
{
    m_blockSize=0;
    m_tcpSocket->abort();
    ReConnToHost();
}

void AndorTcp::ReadMessage()
{
    //receive tcp message
    QByteArray in = m_tcpSocket->readAll();

    m_message = QString(in);

    qDebug()<<"message: "<<m_message;

    //handle message: set filterwheel position, get filterwheel position

    ResolveMessage(m_message);
}

void AndorTcp::DispalyError(QAbstractSocket::SocketError)
{
    qDebug()<<"socket error:"<<m_tcpSocket->errorString();
}

void AndorTcp::OnConnected()
{
    m_tcpCon=true;
    qDebug()<<"connect to host"<<m_tcpSocket->peerAddress().toString()<<":"
           <<m_tcpSocket->peerPort()<<"successfully!";
    RegisterDevice();
}

void AndorTcp::OnDisconnected()
{
    ReConnToHost();
}

void AndorTcp::OnAboutToClose()
{
    DeregisterDevice();
}

void AndorTcp::StopConnect()
{
    m_tcpCon=true;
}

