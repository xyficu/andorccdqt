#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <andoruser.h>
#include <QThread>
#include <QTimer>
#include <QTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    AndorUser m_andorUser;
private:
    Ui::MainWindow *ui;
    QThread m_andorThread;
    QTimer m_timer;
    QDateTime m_dateTime;

        bool m_coolerStatus;
        qint32 m_temp;
            bool m_acqStat;


private slots:

    void on_pushButton_AcqImage_clicked();
    void on_action_Parametres_triggered();
    void UpdateStatus();

    void on_pushButton_AcqAmountImg_clicked();

    void on_pushButton_CreatDir_clicked();

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
    void MGetImage(QString fileName, bool shutterOpen, float expTime, qint32 amount=1);


};

#endif // MAINWINDOW_H
