#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "options.h"
#include "qdebug.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //set window tilte
    setWindowTitle("Andor EMCCD Controller");

    ui->lineEdit_ImagePath->setText("./");
    ui->lineEdit_ExpTime->setText("0.1");
    ui->lineEdit_ImageAmount->setText("3");
    ui->lineEdit_ImageName->setText("test");
    ui->label_Temp->setText("unknown..");
//    ui->checkBox_shutterOpen->setChecked(true);
    m_connect=false;

    m_andorUser.moveToThread(&m_andorThread);
    m_andorThread.start();

    connect(this, SIGNAL(MInitCamera()), &m_andorUser, SLOT(InitCamera()), Qt::QueuedConnection);
    connect(this, SIGNAL(MGetImage()), &m_andorUser, SLOT(UserGetImage()), Qt::QueuedConnection);
    connect(this, SIGNAL(MSetImageSavPath(QString)), &m_andorUser, SLOT(UserSetImageSavPath(QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(MSetExpTime(float)), &m_andorUser, SLOT(UserSetExpTime(float)), Qt::QueuedConnection);
    connect(this, SIGNAL(MSetFileName(QString)), &m_andorUser, SLOT(UserSetFileName(QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(MSetFileAmount(qint32)), &m_andorUser, SLOT(UserSetFileAmount(qint32)), Qt::QueuedConnection);
    connect(this, SIGNAL(MSetTemp(qint32)), &m_andorUser, SLOT(UserSetTemp(qint32)), Qt::QueuedConnection);
    connect(this, SIGNAL(MSetCoolerSwitch(bool)), &m_andorUser, SLOT(UserSetCoolerSwitch(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(MGetTemp(qint32*)), &m_andorUser, SLOT(UserGetTemp(qint32*)), Qt::DirectConnection);
    connect(this, SIGNAL(MGetCoolerSwitch(bool*)), &m_andorUser, SLOT(UserGetCoolerSwitch(bool*)), Qt::DirectConnection);
    connect(this, SIGNAL(MGetStat(bool*)), &m_andorUser, SLOT(UserGetStat(bool*)), Qt::DirectConnection);
    connect(this, SIGNAL(MGetAmountImage()), &m_andorUser, SLOT(UserGetAmountImage()), Qt::QueuedConnection);
    connect(this, SIGNAL(MGetImage(QString,bool,float,qint32)),
            &m_andorUser, SLOT(UserGetImage(QString,bool,float,qint32)), Qt::QueuedConnection);
    connect(this, SIGNAL(MGetConnect(bool*)), &m_andorUser, SLOT(UserGetConnect(bool*)), Qt::DirectConnection);

    emit MInitCamera();

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(UpdateStatus()));
    m_timer.start(100);


}

MainWindow::~MainWindow()
{
    delete ui;
    m_timer.stop();
    m_timer.deleteLater();

    m_andorThread.exit();
    m_andorThread.wait();
    m_andorThread.deleteLater();
}

void MainWindow::UpdateStatus()
{
    m_dateTime = QDateTime::currentDateTime();
//    qDebug()<<m_dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz");

    //get cooler status
    emit MGetCoolerSwitch(&m_coolerStatus);
    switch(m_coolerStatus)
    {
    case true:
        ui->label_CoolerStat->setText("ON");
        break;
    case false:
        ui->label_CoolerStat->setText("OFF");
        break;
    default:
        ui->label_CoolerStat->setText("Unknown");
        break;
    }

    //get cooler temperature
    emit MGetTemp(&m_temp);
    ui->label_Temp->setText(QString::number(m_temp));

    //get acq status
    emit MGetStat(&m_acqStat);
    switch (m_acqStat) {
    case true:
        ui->label_AcqStat->setText("<font color=red>acquiring...</font>");
        break;
    case false:
        ui->label_AcqStat->setText("stopped.");
        break;
    default:
        ui->label_AcqStat->setText("unknown.");
        break;
    }

    //get connect status
    emit MGetConnect(&m_connect);
    switch (m_connect) {
    case true:
        ui->label_conStat->setText("<font color=green>CCD connected.</font>");
        break;
    case false:
        ui->label_conStat->setText("<font color=red>CCD not connected.</font>");
        break;
    default:
        break;
    }

}

void MainWindow::on_pushButton_AcqImage_clicked()
{
    emit MSetExpTime(ui->lineEdit_ExpTime->text().toFloat());
    emit MGetImage();
}

void MainWindow::on_action_Parametres_triggered()
{
    Options w(this);
    w.exec();
}

void MainWindow::on_pushButton_AcqAmountImg_clicked()
{

    emit MSetImageSavPath(ui->lineEdit_ImagePath->text());


    emit MGetImage(ui->lineEdit_ImageName->text(),
                   true, //ui->checkBox_shutterOpen->checkState()==Qt::Checked?true:false,
                   ui->lineEdit_ExpTime->text().toFloat(),
                   ui->lineEdit_ImageAmount->text().toInt());
}

void MainWindow::on_pushButton_CreatDir_clicked()
{
    emit MSetImageSavPath(ui->lineEdit_ImagePath->text());
}
