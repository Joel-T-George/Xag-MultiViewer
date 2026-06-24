#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

class LrfWorker : public QObject
{
    Q_OBJECT

public:
    explicit LrfWorker(QObject *parent = nullptr);
    bool running = false;

    void setCamera(QString ip, quint16 port);

signals:
    void distanceUpdated(double meters);
    void statusChanged(bool running);

public slots:
    void start();
    void stop();

private slots:
    void sendCommand();
    void readPendingDatagrams();

private:
    QUdpSocket *socket = nullptr;
    QTimer *sendTimer = nullptr;

    QString cameraIp;
    quint16 cameraPort = 0;


};
