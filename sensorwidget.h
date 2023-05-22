#ifndef SENSORWIDGET_H
#define SENSORWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDebug>

namespace Ui {
class sensorWidget;
}

class sensorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit sensorWidget(QWidget *parent = nullptr);
    ~sensorWidget();
public slots:
    void onMsg(QString topic, QString value);
    void info_button_pushed();

signals:

    void sendMsg(QString boatname, QString msg, int PCPort);

private:
    Ui::sensorWidget *ui;
    int connection_counter;
};

#endif // SENSORWIDGET_H
