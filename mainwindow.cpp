#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "videowindow.h"
#include "createwindowdialog.h"
#include "configdialog.h"

MainWindow::MainWindow(QWidget *parent, QString config)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _config(config)
{
    ui->setupUi(this);


    this->setWindowTitle(QString("GPlayer"));

    ConfigDialog* configDialog = new ConfigDialog(this);
    connect(configDialog,&ConfigDialog::setconfig,this,&MainWindow::setConfig);
    configDialog->exec();

    // initialize settings
    settings = new QSettings("Ezosirius", "GPlayer_v1",this);

    //initialize UDP socket
    serverSocket = new QUdpSocket(this);
    clientSocket = new QUdpSocket(this);
    clientSocket->bind(50008,QUdpSocket::ShareAddress);
    connect(clientSocket,&QUdpSocket::readyRead,this, &MainWindow::onUDPMsg);

    initNetworkSettings();
    initBoatSettings();
    //initialize videowindows
    initVideoWindows();





    QDockWidget* dockwidget = new QDockWidget("Sensor",this);
    sensor_widget = new sensorWidget(dockwidget);
    dockwidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockwidget->setWidget(sensor_widget);
    dockwidget->setMinimumWidth(300);
    addDockWidget(Qt::RightDockWidgetArea, dockwidget);
    connect(this, &MainWindow::onSensorMsg, sensor_widget, &sensorWidget::onMsg);
    connect(sensor_widget, &sensorWidget::sendMsg, this, &MainWindow::sendUDPCommand);

    //connect(ui->addWindowButton, &QPushButton::clicked, this, &MainWindow::openCreateWindowDialog);
}

void MainWindow::initBoatSettings()
{
    QDockWidget* dockwidget = new QDockWidget("Boat setting",this);
    boatSetting = new BoatSetting(dockwidget);
    boatSetting->setBoatMap(&boatMap);
    connect(boatSetting, &BoatSetting::AddBoat, this, &MainWindow::onNewBoat);
    boatSetting->setconfig(_config);
    boatList = boatSetting->initSettings();
    QMapIterator<QString, QString> i(boatMap);
    while(i.hasNext()){
        i.next();
        qDebug()<<"MainWindow initBoatSettings: "<<i.key()<<" "<<i.value();
    }

    //dockwidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockwidget->setWidget(boatSetting);
    dockwidget->setMinimumWidth(400);
    addDockWidget(Qt::RightDockWidgetArea, dockwidget);
    /*
    settings->beginGroup(QString("%1").arg(_config));
    int size = settings->beginReadArray("boat");

    if(size!=0){
        for(int i = 0; i < size; i++){

            settings->setArrayIndex(i);
            QString boatname = settings->value("boatname").toString();
            QString boatPIP = settings->value("/PIP").toString();
            QString boatSIP = settings->value("/SIP").toString();
            boatList.push_back(boatname);
            qDebug()<<"initBoatSettings: "<<boatname;
            boatSetting->insertBoat(boatname,boatPIP,boatSIP);


            primaryHeartBeat = new HeartBeat(boatname, PCPIP,boatPIP, 50007,true, this);
            primaryHeartBeat->HeartBeatLoop();
            secondaryHeartBeat = new HeartBeat(boatname, PCSIP,boatSIP, 50007,false, this);
            secondaryHeartBeat->HeartBeatLoop();
            connect(this, &MainWindow::AliveResponse, primaryHeartBeat, &HeartBeat::alive);
            connect(primaryHeartBeat, &HeartBeat::sendHeartBeat, this, &MainWindow::sendHB);
            connect(primaryHeartBeat, &HeartBeat::connected, boatSetting, &BoatSetting::onConnected);
            connect(primaryHeartBeat, &HeartBeat::disconnected, boatSetting, &BoatSetting::onDisonnected);
            connect(boatSetting, &BoatSetting::changeBoatName, primaryHeartBeat, &HeartBeat::resetBoatName);
            connect(boatSetting, &BoatSetting::ChangeIP, primaryHeartBeat, &HeartBeat::onChangeIP);
            connect(boatSetting, &BoatSetting::deleteBoat, primaryHeartBeat, &HeartBeat::onDeleteBoat);

            connect(this, &MainWindow::AliveResponse, secondaryHeartBeat, &HeartBeat::alive);
            connect(secondaryHeartBeat, &HeartBeat::sendHeartBeat, this, &MainWindow::sendHB);
            connect(secondaryHeartBeat, &HeartBeat::connected, boatSetting, &BoatSetting::onConnected);
            connect(secondaryHeartBeat, &HeartBeat::disconnected, boatSetting, &BoatSetting::onDisonnected);
            connect(boatSetting, &BoatSetting::changeBoatName, secondaryHeartBeat, &HeartBeat::resetBoatName);
            connect(boatSetting, &BoatSetting::ChangeIP, secondaryHeartBeat, &HeartBeat::onChangeIP);
            connect(boatSetting, &BoatSetting::deleteBoat, secondaryHeartBeat, &HeartBeat::onDeleteBoat);

            onNewBoat(boatname, boatPIP,boatSIP);
        }
    }

    settings->endArray();
    settings->endGroup();
    */

}

void MainWindow::initVideoWindows()
{

    int window_count = 4;
    Qt::DockWidgetArea area = Qt::LeftDockWidgetArea;
    //settings->endGroup();
    for(int i = 1; i <= window_count; i++){
        //create settings if first time opened
        if(settings->value(QString("%1/w%2/in_port").arg(_config,QString::number(i))) == QVariant()){
            QList<QString> formatlist = {"video0", "YUYV", "640-480-15", "nan", "80", "192.168.0.100", "5200"};
            settings->setValue(QString("%1/w%2/boat_name").arg(_config,QString::number(i)),QString("unknown"));
            settings->setValue(QString("%1/w%2/in_port").arg(_config,QString::number(i)),5200+i);
            settings->setValue(QString("%1/w%2/title").arg(_config,QString::number(i)),QString("window%1").arg(i));
            settings->setValue(QString("%1/w%2/videoinfo").arg(_config,QString::number(i)), 1);
            settings->setValue(QString("%1/w%2/formatno").arg(_config,QString::number(i)), 0);
        }

        if(i == 1){ //set to central widget
            addVideoWindow( i, true);
        }else{ // set to dock widget
            addVideoWindow( i, false);
        }
    }


}


void MainWindow::addVideoWindow(int index, bool central_widget)
{
    Qt::DockWidgetArea area = Qt::LeftDockWidgetArea;


    VideoWindow* vwindow = new VideoWindow(this,_config);

    vwindow->setPCPort(settings->value(QString("%1/w%2/in_port").arg(_config,QString::number(index))).toInt());
    vwindow->setTitle(settings->value(QString("%1/w%2/title").arg(_config,QString::number(index))).toString());
    vwindow->setVideoNo(settings->value(QString("%1/w%2/videono").arg(_config,QString::number(index))).toInt());
    vwindow->setFormatNo(settings->value(QString("%1/w%2/formatno").arg(_config,QString::number(index))).toInt());
    vwindow->setFormat();
    vwindow->setIndex(index);
    vwindow->setBoatList(boatList);


    if(settings->value(QString("%1/w%2/videoinfo").arg(_config,QString::number(index))) == 1){
        vwindow->setVideoInfo(true);
    }else{
        vwindow->setVideoInfo(false);
    }
    vwindow->init();

    if(central_widget){
        setCentralWidget(vwindow);
    }else{
        QDockWidget* dockwidget = new QDockWidget(settings->value(QString("%1/w%2/title").arg(_config,QString::number(index))).toString(),this);
        addDockWidget(area, dockwidget);
        //dockwidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
        dockwidget->setWidget(vwindow);
        dockwidget->setMinimumHeight(300);
    }

    connect(vwindow,&VideoWindow::sendCommand,this,&MainWindow::sendUDPCommand);
    connect(this, &MainWindow::setFormat, vwindow, &VideoWindow::setVideoFormat);
    connect(boatSetting, &BoatSetting::AddBoat, vwindow, &VideoWindow::AddBoat);
    connect(boatSetting, &BoatSetting::changeBoatName, vwindow, &VideoWindow::onBoatNameChange);
    connect(boatSetting, &BoatSetting::deleteBoat, vwindow, &VideoWindow::onDeleteBoat);

}

void MainWindow::initNetworkSettings()
{

    QDockWidget* dockwidget = new QDockWidget("Network",this);
    networkSettings = new NetworkSettings(dockwidget);
    networkSettings->setConfig(_config);
    connect(networkSettings, &NetworkSettings::PIPChanged, this, &MainWindow::onPCPIPChanged);
    connect(networkSettings, &NetworkSettings::SIPChanged, this, &MainWindow::onPCSIPChanged);

    //dockwidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockwidget->setWidget(networkSettings);
    dockwidget->setMinimumWidth(300);
    addDockWidget(Qt::RightDockWidgetArea, dockwidget);

    PCPIP = settings->value(QString("%1/PC/PIP").arg(_config)).toString();
    PCSIP = settings->value(QString("%1/PC/SIP").arg(_config)).toString();

    networkSettings->setIP(PCPIP, PCSIP);
}


void MainWindow::openCreateWindowDialog()
{
    CreateWindowDialog* dialog = new CreateWindowDialog(this);
    dialog->exec();
}

void MainWindow::sendUDPCommand(QString boatname, QString command, int PCPort)
{
    QHostAddress addr = QHostAddress(boatSetting->getIP(boatname));
    QString PC_addr;
    if(boatSetting->isPrimary(boatname)){
        PC_addr = PCPIP;
    }else{
        PC_addr = PCSIP;
    }
    if(PCPort != 0){
        command = QString("cmd ")+command+" "+PC_addr+" "+QString::number(PCPort);
    }
    serverSocket->writeDatagram(command.toUtf8().data(),command.toUtf8().size(), addr, 50007);
    qDebug()<<"MainWindow "<<",ip: "<<addr.toString()<<"send command: "<<command;

}

void MainWindow::sendHB(QString ip, QString command)
{
    QHostAddress addr = QHostAddress(ip);

    serverSocket->writeDatagram(command.toUtf8().data(),command.toUtf8().size(), addr, 50007);

}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onUDPMsg()
{
    while(clientSocket->hasPendingDatagrams()){
        QByteArray data;
        QHostAddress addr;
        QString ip;
        data.resize(clientSocket->pendingDatagramSize());
        clientSocket->readDatagram(data.data(),data.size(),&addr);

        ip = QHostAddress(addr.toIPv4Address()).toString();
        if(ip == BoatPIP){
            //qDebug()<<QString("recv from primary: %1").arg(ip);
        }else if(ip == BoatSIP){
            //qDebug()<<QString("recv from secondary: %1").arg(ip);
        }
        QStringList dataList = QString(data).split(' ');
        QString topic = dataList[0];

        QString message;
        if(data.split(' ').size() >1){
            message = data.split(' ')[1];
        }
        if(topic == QString("alive")){
            emit AliveResponse(ip);
        }else if(topic == QString("format")){
            QString boatname = dataList[1].split('\n')[0];
            QStringList format = QString(data).split('\n');
            format.pop_front();
            emit setFormat(boatname, format);
            qDebug()<<format;

        }
        const QString content = QLatin1String(" Received Topic: ")
                    + topic
                    + QLatin1String(" Message: ")
                    + message
                    + QLatin1Char('\n');
        emit onSensorMsg(topic, message);
        //qDebug() << content;
    }
}

void MainWindow::setConfig(QString config)
{
    _config = QString("config")+config;
    qDebug()<<config;
}

void MainWindow::onPCPIPChanged(QString PCIP)
{
    PCPIP = PCIP;
    emit changePCPIP(PCIP);
    settings->setValue(QString("%1/PC/PIP").arg(_config), PCIP);

}

void MainWindow::onPCSIPChanged(QString PCIP)
{
    PCSIP = PCIP;
    emit changePCSIP(PCIP);
    settings->setValue(QString("%1/PC/SIP").arg(_config), PCIP);
}

void MainWindow::onNewBoat(QString boatname, QString PIP, QString SIP)
{
    primaryHeartBeat = new HeartBeat(boatname, PCPIP,PIP, 50007,true, this);
    primaryHeartBeat->HeartBeatLoop();
    secondaryHeartBeat = new HeartBeat(boatname, PCSIP,SIP, 50007,false, this);
    secondaryHeartBeat->HeartBeatLoop();
    connect(this, &MainWindow::AliveResponse, primaryHeartBeat, &HeartBeat::alive);
    connect(primaryHeartBeat, &HeartBeat::sendHeartBeat, this, &MainWindow::sendHB);
    connect(primaryHeartBeat, &HeartBeat::connected, boatSetting, &BoatSetting::onConnected);
    connect(primaryHeartBeat, &HeartBeat::disconnected, boatSetting, &BoatSetting::onDisonnected);
    connect(boatSetting, &BoatSetting::changeBoatName, primaryHeartBeat, &HeartBeat::resetBoatName);
    connect(boatSetting, &BoatSetting::ChangeIP, primaryHeartBeat, &HeartBeat::onChangeIP);
    connect(boatSetting, &BoatSetting::deleteBoat, primaryHeartBeat, &HeartBeat::onDeleteBoat);
    connect(this, &MainWindow::changePCPIP, primaryHeartBeat, &HeartBeat::onPCPIPChanged);

    connect(this, &MainWindow::AliveResponse, secondaryHeartBeat, &HeartBeat::alive);
    connect(secondaryHeartBeat, &HeartBeat::sendHeartBeat, this, &MainWindow::sendHB);
    connect(secondaryHeartBeat, &HeartBeat::connected, boatSetting, &BoatSetting::onConnected);
    connect(secondaryHeartBeat, &HeartBeat::disconnected, boatSetting, &BoatSetting::onDisonnected);
    connect(boatSetting, &BoatSetting::changeBoatName, secondaryHeartBeat, &HeartBeat::resetBoatName);
    connect(boatSetting, &BoatSetting::ChangeIP, secondaryHeartBeat, &HeartBeat::onChangeIP);
    connect(boatSetting, &BoatSetting::deleteBoat, secondaryHeartBeat, &HeartBeat::onDeleteBoat);
    connect(this, &MainWindow::changePCSIP, secondaryHeartBeat, &HeartBeat::onPCSIPChanged);


    qDebug()<<"mainwindow: "<<"new boat";
}



HeartBeat::HeartBeat(QObject *parent): QObject(parent)
{

}

HeartBeat::HeartBeat(QString boatname, QString PC_ip, QString Boat_ip, int port, bool isPrimary, QObject *parent): QObject(parent)
{
    boatName = boatname;
    boatIP = Boat_ip;
    PCIP = PC_ip;
    boatPort = port;
    isAlive = false;
    isHearBeatLoop = false;
    primary = isPrimary;
    heartBeatTimer = new QTimer(this);
    checkAliveTimer = new QTimer(this);
    connect(heartBeatTimer, &QTimer::timeout, this, &HeartBeat::beat);
    connect(checkAliveTimer,&QTimer::timeout, this, &HeartBeat::checkAlive);
}

HeartBeat::~HeartBeat()
{
    run = false;
}

void HeartBeat::resetBoatName(QString boatname, QString newname)
{
    qDebug()<<"HeartBeat: "<<boatname+" >> "+boatName+" >> "+newname;
    if(boatname == boatName){
        boatName = newname;
        if(!isHearBeatLoop){
            checkAliveTimer->stop();
            HeartBeatLoop();
        }
    }
}

void HeartBeat::onChangeIP(QString boatname, QString PIP, QString SIP)
{
    if(boatname == boatName){
        if(primary){
            boatIP = PIP;
        }else{
            boatIP = SIP;
        }
        if(!isHearBeatLoop){
            checkAliveTimer->stop();
            HeartBeatLoop();
        }
    }
}

void HeartBeat::onPCPIPChanged(QString IP)
{
        if(primary){
            PCIP = IP;
            if(!isHearBeatLoop){
                checkAliveTimer->stop();
                HeartBeatLoop();
            }
        }

}

void HeartBeat::onPCSIPChanged(QString IP)
{
        if(!primary){
            PCIP = IP;
            if(!isHearBeatLoop){
                checkAliveTimer->stop();
                HeartBeatLoop();
            }
        }

}

void HeartBeat::onDeleteBoat(QString boatname)
{

    if(boatName == boatname){
        this->deleteLater();
    }

}

void HeartBeat::HeartBeatLoop()
{
    qDebug()<<"HeartBeat "<<", boatname:"<<boatName<<", boat ip:"<<boatIP<<", HB loop";
    QString cmd;
    if(primary){
        cmd =  QString("HB ")+PCIP+QString(" ")+boatName+QString(" P");
    }else{
        cmd =  QString("HB ")+PCIP+QString(" ")+boatName+QString(" S");
    }
    emit sendHeartBeat(boatIP, cmd);
    heartBeatTimer->start(1000);
    isHearBeatLoop = true;
    isAlive = false;
    emit disconnected(boatName,primary);
}


void HeartBeat::alive(QString ip)
{
    if(ip == boatIP){
        if(isHearBeatLoop == false){
            //qDebug()<<"HeartBeat: alive from "<<ip;
            isAlive = true;
        }else{
            // Enter alive loop
            qDebug()<<"HeartBeat "<<", boatname:"<<boatName<<", boat ip:"<<ip<<", alive loop";
            heartBeatTimer->stop();
            checkAliveTimer->start(5000);
            isHearBeatLoop = false;
            qDebug()<<"Start alive loop";
            emit connected(boatName,primary);
            emit sendHeartBeat(boatIP, QString("qformat"));
        }
    }else{
        //Deposit other boat's message;
    }
}

void HeartBeat::beat()
{
    QString cmd;
    if(primary){
        cmd =  QString("HB ")+PCIP+QString(" ")+boatName+QString(" P");
    }else{
        cmd =  QString("HB ")+PCIP+QString(" ")+boatName+QString(" S");
    }
    emit sendHeartBeat(boatIP, cmd);
}

void HeartBeat::checkAlive()
{
    if(isAlive == false){
        checkAliveTimer->stop();
        HeartBeatLoop();
    }
    isAlive = false;
}

void HeartBeat::setPCIP(QString PC_ip)
{
    PCIP = PC_ip;
    if(checkAliveTimer->isActive()){
        checkAliveTimer->stop();
        isAlive = false;
        HeartBeatLoop();
    }

}
