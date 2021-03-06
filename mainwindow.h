#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <andoruser.h>
#include <QThread>
#include <QTimer>
#include <QTime>
#include "andortcp.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    AndorUser m_andorUser;
    QThread m_andorThread;
    QTimer m_timer;
    QDateTime m_dateTime;
    AndorTcp m_andorTcp;
    QThread m_andorTcpThread;

    bool m_coolerStatus;
    qint32 m_temp;
    bool m_acqStat;
    bool m_connect;
    qint32 m_curNumb;
    float m_acqProc;


private slots:

    void on_pushButton_AcqImage_clicked();
    void on_action_Parametres_triggered();
    void UpdateStatus();

    void on_pushButton_AcqAmountImg_clicked();

    void on_pushButton_CreatDir_clicked();

    void on_pushButton_AbortAcq_clicked();

signals:
    void MInitCamera();
    void MGetImage();
    void MSetImageSavPath(QString path);
    void MSetExpTime(float time);
    void MSetFileName(QString fileName);
    void MSetFileAmount(qint32 fileAmount);
    void MSetTemp(qint32 temp);
    void MSetCoolerSwitch(bool coolerSwitch);
    void MGetTemp(qint32 *temp);
    void MGetCoolerSwitch(bool *coolerSwitch);
    void MGetStat(bool *isAcquiring);
    void MGetAmountImage();
    void MGetImage(QString fileName, bool shutterOpen, float expTime, qint32 amount,
                   QString ra, QString dec, QString ut, QString st, QString futId,
                   QString imgType, QString filtColor, QString lt);
    void MGetConnect(bool *connect);
    void MAbortAcq();
    void MGetAcqProc(float *acqProc);
    void MGetCurNumb(qint32 *curNumb);
    void MCreateImgPath(QString imgPath, bool *res);

    void MStartConToHost();
    void MStopConToHost();

};

#endif // MAINWINDOW_H
