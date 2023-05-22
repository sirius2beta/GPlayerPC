#ifndef BOATSETTING_H
#define BOATSETTING_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDebug>
#include <QMap>
#include <QSettings>


namespace Ui {
class BoatSetting;
}

class BoatSetting : public QWidget
{
    Q_OBJECT

public:
    explicit BoatSetting(QWidget *parent = nullptr);
    ~BoatSetting();
    QStringList initSettings();
    QString getIP(QString boatname);
    bool isPrimary(QString boatname);
    void setconfig(QString conf);
    void setBoatMap(QMap<QString,QString>* map);
signals:
    void changeBoatName(QString boatname, QString newboatname);
    void AddBoat(QString newname, QString PIP, QString SIP);
    void ChangeIP(QString boatname, QString PIP, QString SIP);
    void deleteBoat(QString boatname);

public slots:
    void onConnected(QString boatname, bool isprimary);
    void onDisonnected(QString boatname, bool isprimary);
    void insertBoat(QString boatname, QString PIP, QString SIP);
    void onBoatNameChange();
    void onAddBoat();
    void onDeleteBoat();
    void onBoatSelected(int index);
    void onBoatDoubleClicked(QModelIndex index);
    void onChangeIP();

private:
    Ui::BoatSetting *ui;
    QStandardItemModel* boatItemModel;
    QSettings *settings;
    QString config;
    QMap<QString, QString>* boatMap;

};

#endif // BOATSETTING_H
