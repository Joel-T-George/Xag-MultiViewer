#pragma once
#include <QObject>
#include <QUdpSocket>

class SkydroidDriver : public QObject
{
    Q_OBJECT
public:
    explicit SkydroidDriver(QString ip, int port = 5000, QObject* parent = nullptr);

    void setGimbalSpeed(int yaw, int pitch);
    void centerGimbal();
    void lookDown();
    void lookForward();
    void takephoto();
    void Startrecord();
    void Stoprecord();
    void OSDEnable();
    void OSDDisable();
    void setTrackingState(bool enabled);
    void setTrackingTarget(int x , int y);
    void sendRawHex(const QString& hex);
    void setPalette(int index);

    void zoomIn();
    void zoomOut();
    quint8 seq = 0;
    quint16 calculateCrc16Xmodem(const QByteArray& data);
    void sendTrackingBinary(int x1, int y1, int x2, int y2);

    void startTracking(int x1, int y1, int x2, int y2);
    void stopTracking();

private:
    QUdpSocket socket;
    QString ip;
    int port;

    QString buildCommand(QString addr, QString ctrl, QString id, QString data);
    QString checksum(QString payload);
};
