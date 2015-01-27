#ifndef ANDORUSER_H
#define ANDORUSER_H
#include "atmcdLXd.h"
#include "ccdparams.h"
#include <QTimer>

#include <QObject>

class AndorUser : public QObject
{
    Q_OBJECT
public:
    explicit AndorUser(QObject *parent = 0);
    ~AndorUser();



private:
    unsigned long error;
    bool quit;
    char choice;
    float fChoice;
    int width, height;

    int CameraSelect (int iNumArgs, char* szArgList[]);
    AndorCcDParams *m_andorCcdParams;
    QTimer m_timer;

private slots:
    void UserSetImageSavPath(QString path);
    void UserSetExpTime(float time);
    void UserSetShutter(qint32 type, qint32 mode, qint32 openingTime, qint32 closingTime);
    void UserSetFileName(QString fileName);
    void UserSetFileAmount(qint32 fileAmount);
    void UserSetGain(qint32 gain);
    void UserSetGainSwitch(bool gainSwitch);
    void UserSetBinning(qint32 bin[]);
    void UserSetBinningSwitch(bool binSwitch);
    void UserSetReadMode(qint32 readMode);
    void UserSetTemp(qint32 temp);
    void UserSetCoolerSwitch(bool coolerSwitch);

    void UserGetTemp(qint32 *temp);
    void UserGetCoolerSwitch(bool *coolerSwitch);
    void UserGetStat(bool *isAcquiring);
    void UserGetGain(qint32 *gain);
    void UserGetBinning(quint32 bin[2]);
    void UserGetImageSavPath(QString *path);
    void UserAbortAcq();

    void UserCreateDir(QString path);

    void InitCamera();
    void UserGetImage();
    void UserGetAmountImage();
    void UserGetImage(QString fileName, bool shutterOpen, float expTime, qint32 amount=1);
    void SelfUpdateStat();

signals:

public slots:

};

#endif // ANDORUSER_H
