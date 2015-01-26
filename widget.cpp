#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_andorUser.InitCamera();


}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
//    m_andorUser.SetExpTime( ui->lineEdit_ExpTime->text().toFloat());
    m_andorUser.GetImage();
}
