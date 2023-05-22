#include "sensorwidget.h"
#include "ui_sensorwidget.h"

sensorWidget::sensorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sensorWidget),
    connection_counter(0)
{
    ui->setupUi(this);
    connect(ui->info_Button, &QPushButton::clicked, this, &sensorWidget::info_button_pushed);

}

sensorWidget::~sensorWidget()
{
    delete ui;
}

void sensorWidget::onMsg(QString topic, QString value)
{
   if(value.split(" ")[0] == QString("alive")){
       ui->connection_label->setStyleSheet("*{ background-color:green;}");

       QTimer::singleShot(4900,[=]{
           qDebug()<<"shot";
           ui->connection_label->setStyleSheet("*{ background-color:#444444;}");

       });
   }else{
       ui->system_msg->setText(value);
   }
}

void sensorWidget::info_button_pushed()
{
    //emit sendMsg(QString("usv1"),QString("qformat"));
}
