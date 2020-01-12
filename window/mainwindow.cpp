#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "executor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // ui->Input->setPlaceholderText("请输入SQL语句");

    setWindowTitle(QString::fromLocal8Bit("simpleDB"));

    QPushButton *allSelect = new QPushButton;
    allSelect->setStyleSheet("QPushButton{border-image: url(:/res/appicon/wx.png) 0 0 0 0;border:none;color:rgb(255, 255, 255);}"
                             "QPushButton:hover{background-color: rgb(20, 62, 134);border:none;color:rgb(255, 255, 255);}"
                             "QPushButton:checked{background-color: rgb(20, 62, 134);border:none;color:rgb(255, 255, 255);}");
    char sql[10240];
    execute_sql(sql, strlen(sql));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Run_clicked()  //按钮
{
    QString str = ui->Input->toPlainText();  //输入的SQL语句
    ui->Output->insertPlainText(str);  //输出的内容
}
