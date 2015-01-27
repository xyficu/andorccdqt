#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_StartCooler_clicked();

    void on_pushButton_StopCooler_clicked();

private:
    Ui::Options *ui;
    MainWindow *m_Parent;
};

#endif // OPTIONS_H
