#include "boatsetting.h"
#include "ui_boatsetting.h"
#include <QComboBox>
#include <QMessageBox>

BoatSetting::BoatSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BoatSetting)
{
    ui->setupUi(this);
    ui->BoatTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->BoatTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->BoatlineEdit, &QLineEdit::editingFinished, this, &BoatSetting::onBoatNameChange);
    connect(ui->BoatTableView, &QTableView::doubleClicked, this, &BoatSetting::onBoatDoubleClicked);
    connect(ui->PIPlineEdit, &QLineEdit::editingFinished, this, &BoatSetting::onChangeIP);
    connect(ui->SIPlineEdit, &QLineEdit::editingFinished, this, &BoatSetting::onChangeIP);

    settings = new QSettings("Ezosirius", "GPlayer_v1", this);

    //setup boat tableview
    boatItemModel = new QStandardItemModel();
    QStringList label = {"name", "Primary", "Secondary"};
    boatItemModel->setHorizontalHeaderLabels(label);
    ui->BoatTableView->setModel(boatItemModel);
    ui->BoatTableView->setColumnWidth(0,60);
    ui->BoatTableView->setColumnWidth(1,100);
    ui->BoatTableView->setColumnWidth(2,100);

    //insertBoat("Charlie", "192.168.0.1", "100.109.143.71");


    connect(ui->addBoatPushButton, &QPushButton::clicked, this, &BoatSetting::onAddBoat);
    connect(ui->deleteBoatPushButton, &QPushButton::clicked, this, &BoatSetting::onDeleteBoat);
    connect(ui->BoatcomboBox, (void (QComboBox::*)(int))&QComboBox::currentIndexChanged, this, &BoatSetting::onBoatSelected);


}

QStringList BoatSetting::initSettings()
{
    QStringList boatList;
    settings->beginGroup(QString("%1").arg(config));
    int size = settings->beginReadArray("boat");

    if(size!=0){
        for(int i = 0; i < size; i++){

            settings->setArrayIndex(i);
            QString boatname = settings->value("boatname").toString();
            QString boatPIP = settings->value("/PIP").toString();
            QString boatSIP = settings->value("/SIP").toString();
            boatList.push_back(boatname);
            qDebug()<<"initBoatSettings: "<<boatname;
            insertBoat(boatname,boatPIP,boatSIP);
            (*boatMap)[boatname] = boatPIP+" "+boatSIP;

            emit AddBoat(boatname, boatPIP, boatSIP);
        }
    }

    settings->endArray();
    settings->endGroup();
    return boatList;
}

void BoatSetting::insertBoat(QString boatname, QString PIP, QString SIP)
{

    int current = boatItemModel->rowCount();
    QStandardItem* item1 = new QStandardItem(boatname);
    QStandardItem* item2 = new QStandardItem(QString("SB"));
    item2->setData(PIP);
    item2->setBackground(QBrush(QColor(120,0,0)));
    QStandardItem* item3 = new QStandardItem(QString("SB"));
    item3->setData(SIP);
    item3->setBackground(QBrush(QColor(120,0,0)));
    boatItemModel->setItem(current,0,item1);
    boatItemModel->setItem(current,1,item2);
    boatItemModel->setItem(current,2,item3);
    ui->BoatcomboBox->addItem(boatname,boatname);
    ui->BoatcomboBox->setCurrentIndex(ui->BoatcomboBox->count()-1);
    ui->BoatlineEdit->setText(boatname);
    ui->PIPlineEdit->setText(PIP);
    ui->SIPlineEdit->setText(SIP);
}

void BoatSetting::setconfig(QString conf)
{
    config = conf;
}

BoatSetting::~BoatSetting()
{
    delete ui;
}

void BoatSetting::setBoatMap(QMap<QString,QString>* map)
{
    boatMap = map;
}

void BoatSetting::onConnected(QString boatname, bool isprimary)
{
    for(int i = 0; i < boatItemModel->rowCount();i++){
        //qDebug()<<boatItemModel->item(i,0)->text();
        if(boatItemModel->item(i,0)->text() == boatname){
            if(isprimary){
                boatItemModel->item(i,1)->setText("Active");
                boatItemModel->item(i,1)->setBackground(QBrush(QColor(0,120,0)));
            }else{
                boatItemModel->item(i,2)->setText("Active");
                boatItemModel->item(i,2)->setBackground(QBrush(QColor(0,120,0)));
            }
        }
    }

}

void BoatSetting::onDisonnected(QString boatname, bool isprimary)
{
    for(int i = 0; i < boatItemModel->rowCount();i++){
        //qDebug()<<boatItemModel->item(i,0)->text();
        if(boatItemModel->item(i,0)->text() == boatname){
            if(isprimary){
                boatItemModel->item(i,1)->setText("SB");
                boatItemModel->item(i,1)->setBackground(QBrush(QColor(120,0,0)));
            }else{
                boatItemModel->item(i,2)->setText("SB");
                boatItemModel->item(i,2)->setBackground(QBrush(QColor(120,0,0)));
            }
        }
    }
}

QString BoatSetting::getIP(QString boatname){
    for(int i = 0; i < boatItemModel->rowCount();i++){
        if(boatItemModel->item(i,0)->text() == boatname){
            if(boatItemModel->item(i,1)->text() == QString("Active")){
                return boatItemModel->item(i,1)->data().toString();
                qDebug()<<"Using Primary IP";
            }else{
                return boatItemModel->item(i,2)->data().toString();
                qDebug()<<"Using Secondary IP";
            }

        }
    }
    return "192.168.0.1";

}

bool BoatSetting::isPrimary(QString boatname){
    for(int i = 0; i < boatItemModel->rowCount();i++){
        if(boatItemModel->item(i,0)->text() == boatname){
            if(boatItemModel->item(i,1)->text() == QString("Active")){
                return true;
                qDebug()<<"Using Primary IP";
            }else{
                return false;
                qDebug()<<"Using Secondary IP";
            }

        }
    }
    return "192.168.0.1";

}

void BoatSetting::onBoatNameChange()
{
    QString oldname = ui->BoatcomboBox->currentText();
    QString newname = ui->BoatlineEdit->text();
    for(int i = 0; i < boatItemModel->rowCount();i++){
        if(boatItemModel->item(i,0)->text() == oldname){
            boatItemModel->item(i,0)->setText(newname);
        }
    }
    emit changeBoatName(oldname, newname);
    ui->BoatcomboBox->setItemText(ui->BoatcomboBox->currentIndex(),ui->BoatlineEdit->text());
    settings->beginGroup(QString("%1").arg(config));
    int size = settings->beginReadArray("boat");
    int index = 0;
    for(int i = 0; i<size; i++){
        settings->setArrayIndex(i);
        ;
        if(settings->value("boatname").toString() == oldname){
            settings->setValue("boatname",newname);
            index = i;
            break;
        }
    }
    //settings->endArray();
    //settings->beginWriteArray("boat");
    //settings->setArrayIndex(index);
    //settings->setValue("boatname", newname);
    settings->endArray();
    settings->endGroup();
    //settings->remove(oldname)
    //settings->setValue(QString("%1/PIP").arg(newname), "192.168.0.1");
    //settings->setValue(QString("%1/SIP").arg(newname), "100.100.100.100");

}

void BoatSetting::onAddBoat()
{
    int count = 0;
    for(int i = 0; i < boatItemModel->rowCount();i++){
        if(boatItemModel->item(i,0)->text().contains("unknown")){
            count++;
        }
    }
    QString newboatname = "unknown"+QString::number(count);

    insertBoat(newboatname,"192.168.0.1", "100.100.100.100");

    settings->beginGroup(QString("%1").arg(config));
    int size = settings->beginReadArray("boat");

    settings->endArray();
    settings->beginWriteArray("boat");
    settings->setArrayIndex(size);
    settings->setValue(QString("boatname"), newboatname);;
    settings->setValue(QString("PIP"), "192.168.0.1");
    settings->setValue(QString("SIP"), "100.100.100.100");
    settings->endArray();
    settings->endGroup();

    ui->BoatcomboBox->setCurrentIndex(ui->BoatcomboBox->count()-1);
    emit AddBoat(newboatname, "192.168.0.1", "100.100.100.100");
}

void BoatSetting::onDeleteBoat()
{
    int index = ui->BoatcomboBox->currentIndex();
    qDebug()<<"BoatSetting delete: " <<index;
    QMessageBox msgBox;
    msgBox.setText(QString("Are you sure you want to delete ")+ui->BoatcomboBox->currentText());
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Ok){
        emit deleteBoat(ui->BoatcomboBox->currentText());
        ui->BoatcomboBox->removeItem(index);
        boatItemModel->removeRows(index,1);
    }
    settings->beginGroup(QString("%1").arg(config));
    int size = settings->beginReadArray("boat");
    QString lastname;
    QString lastPIP;
    QString lastSIP;
    for(int i = index; i<size-1; i++){
        settings->setArrayIndex(i+1);
        lastname = settings->value("boatname").toString();
        qDebug()<<"lastname: "<<lastname;
        lastPIP = settings->value("PIP").toString();
        lastSIP = settings->value("SIP").toString();

        settings->setArrayIndex(i);
        settings->setValue("boatname",lastname);
        settings->setValue("PIP", lastPIP);
        settings->setValue("SIP", lastSIP);
    }
    settings->endArray();
    settings->setValue("boat/size", size-1);
    settings->remove(QString("boat/%1/boatname").arg(size));
    settings->remove(QString("boat/%1/PIP").arg(size));
    settings->remove(QString("boat/%1/SIP").arg(size));
    settings->remove(QString("boat/%1").arg(size));
    settings->endGroup();
}

void BoatSetting::onBoatSelected(int index)
{
    if(index>-1){
        ui->BoatlineEdit->setText(ui->BoatcomboBox->currentText());
        ui->PIPlineEdit->setText(boatItemModel->item(index,1)->data().toString());
        ui->SIPlineEdit->setText(boatItemModel->item(index,2)->data().toString());
    }
}

void BoatSetting::onBoatDoubleClicked(QModelIndex index)
{
    ui->BoatlineEdit->setText(boatItemModel->item(index.row(),0)->text());
    ui->PIPlineEdit->setText(boatItemModel->item(index.row(),1)->data().toString());
    ui->SIPlineEdit->setText(boatItemModel->item(index.row(),2)->data().toString());
    ui->BoatcomboBox->setCurrentIndex(index.row());
}

void BoatSetting::onChangeIP()
{
    emit ChangeIP(ui->BoatcomboBox->currentText(), ui->PIPlineEdit->text(), ui->SIPlineEdit->text());
    settings->beginGroup(QString("%1").arg(config));
    int size = settings->beginReadArray("boat");
    int index = 0;
    for(int i = 0; i<size; i++){
        settings->setArrayIndex(i);
        if(settings->value("boatname").toString() == ui->BoatcomboBox->currentText()){
            settings->setValue("PIP",ui->PIPlineEdit->text());
            settings->setValue("SIP",ui->SIPlineEdit->text());
        }
    }
    boatItemModel->item(ui->BoatcomboBox->currentIndex(),1)->setData(ui->PIPlineEdit->text());
    boatItemModel->item(ui->BoatcomboBox->currentIndex(),2)->setData(ui->SIPlineEdit->text());
    settings->endArray();
    settings->endGroup();
}
