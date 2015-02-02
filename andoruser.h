#ifndef ANDORUSER_H
#define ANDORUSER_H
#include "atmcdLXd.h"
#include "ccdparams.h"
#include <QTimer>
#include <QObject>
#include "fitsio.h"
#include "QProcess"
#include "xpa.h"
#include <QTime>
#include <QThread>

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
    QProcess m_procDs9;
    QProcess m_procXpa;
    QStringList m_argsDs9;
    QString m_argsXpa;
    QString m_ds9Id;
    qint32 m_curNumb;
    QTime m_acqTmCul;
    QThread m_selfUpdtThd;


    void WriteFitsKeys(QString fileName);
    void StartDs9();
    void DisplayImage(QString fileName, QString ds9Id);

private slots:
    void UserSetImageSavPath(QString path);
    void UserSetExpTime(float time);
    void UserSetShutter(qint32 type, qint32 mode, qint32 openingTime, qint32 closingTime);
    void UserSetFileName(QString fileName);
    void UserSetFileAmount(qint32 fileAmount);
    void UserSetGain(qint32 gain);
    void UserSetGainSwitch(bool gainSwitch);
    void UserSetBinning(qint32 binx, qint32 biny);
    void UserSetReadMode(qint32 readMode);
    void UserSetTemp(qint32 temp);
    void UserSetCoolerSwitch(bool coolerSwitch);

    void UserGetTemp(qint32 *temp);
    void UserGetCoolerSwitch(bool *coolerSwitch);
    void UserGetStat(bool *isAcquiring);
    void UserGetGain(qint32 *gain);
    void UserGetBinning(quint32 bin[2]);
    void UserGetImageSavPath(QString *path);
    void UserGetConnect(bool *connect);
    void ResetAcqTime();

    void UserCreateDir(QString path);

    void InitCamera();
    void UserGetImage();
    void UserGetAmountImage();
    void UserGetImage(QString fileName, bool shutterOpen, float expTime, qint32 amount=1);
    void SelfUpdateStat();
    void UserGetAllStat(qint32 *temp, bool *coolerSwitch, bool *isAcq, qint32 *gain, qint32 *binx, qint32 *biny, QString *imgSavPath, float *acqProc, qint32 *curNumb, qint32 *imgAmt);
    void UserAbortAcq();
    void UserGetCurNumb(qint32 *curNumb);
    void UserGetAcqProc(float *acqProc);

signals:


public slots:

};

#endif // ANDORUSER_H
