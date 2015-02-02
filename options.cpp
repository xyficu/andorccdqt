#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);
    m_Parent = (MainWindow*)parent;
    ui->lineEdit_Temp->setText("-80");
    //set window tilte
    setWindowTitle("Options");

}

Options::~Options()
{
    delete ui;

}

void Options::on_pushButton_clicked()
{

}

void Options::on_pushButton_StartCooler_clicked()
{
    if(NULL != m_Parent)
    {
        qint32 temp = ui->lineEdit_Temp->text().toInt();
        emit m_Parent->MSetTemp(temp);
        emit m_Parent->MSetCoolerSwitch(true);
    }
}

void Options::on_pushButton_StopCooler_clicked()
{
    if(NULL != m_Parent)
    {

        emit m_Parent->MSetCoolerSwitch(false);
    }
}
