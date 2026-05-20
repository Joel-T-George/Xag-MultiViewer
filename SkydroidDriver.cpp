#include "SkydroidDriver.h"
#include "QDebug"

SkydroidDriver::SkydroidDriver(QString ip, int port, QObject* parent)
    : QObject(parent), ip(ip), port(port)
{

}

QString SkydroidDriver::checksum(QString payload)
{
    QByteArray bytes = payload.toUtf8();
    int sum = 0;
    for (auto b : bytes)
        sum += (unsigned char)b;

    int lsb = sum & 0xFF;
    return QString("%1").arg(lsb, 2, 16, QLatin1Char('0')).toUpper();
}

QString SkydroidDriver::buildCommand(QString addr, QString ctrl, QString id, QString data)
{
    QString len = QString::number(data.length(), 16).toUpper();
    QString payload = "#TP" + addr + len + ctrl + id + data;
    return payload + checksum(payload);
}

void SkydroidDriver::lookForward()
{
    int angle = 0;
    int speed = 50;

    QString angleStr = QString("%1").arg(angle, 4, 16, QChar('0')).toUpper();
    QString speedStr = QString("%1").arg(speed, 2, 16, QChar('0')).toUpper();
    QString cmd = buildCommand("UG","w","GAP",angleStr+speedStr);
    socket.writeDatagram(cmd.toUtf8(), QHostAddress(ip), port);
}
void SkydroidDriver::setTrackingTarget(int x, int y)
{
    x = qBound(0, x, 1280);
    y = qBound(0, y, 720);

    QString xStr = QString("%1").arg(x, 4, 16, QChar('0')).toUpper();
    QString yStr = QString("%1").arg(y, 4, 16, QChar('0')).toUpper();

    QString cmd =buildCommand("UG", "w", "GOT", xStr + yStr);
}
void SkydroidDriver::sendTrackingBinary(int x1, int y1, int x2, int y2)
{
    qDebug() << "[Skydroiddriver] show Event" << x1 << y1 << x2 << y2;
    QByteArray cmd;

    // Header
    cmd.append(char(0xAA));
    cmd.append(char(0xA5));
    cmd.append(char(0x09));

    cmd.append(char(0x00));           // fixed
    cmd.append(char(seq & 0xFF));     // sequence
    cmd.append(char(0x00));
    cmd.append(char(0x01));
    cmd.append(char(0x00));

    // ---- Coordinates (LITTLE ENDIAN) ----
    auto appendLE16 = [&](int value)
    {
        cmd.append(char(value & 0xFF));        // low byte
        cmd.append(char((value >> 8) & 0xFF)); // high byte
    };

    appendLE16(x1);
    appendLE16(y1);
    appendLE16(x2);
    appendLE16(y2);

    // ---- CRC ----
    quint16 crc = calculateCrc16Xmodem(cmd);

    cmd.append(char(crc & 0xFF));         // CRC low
    cmd.append(char((crc >> 8) & 0xFF));  // CRC high

    // Tail
    cmd.append(char(0xCD));
    // ---- Send ----
    socket.writeDatagram(cmd, QHostAddress(ip), 1030);
    // ---- Increment sequence ----
    seq = (seq + 1) & 0xFF;
}

quint16 SkydroidDriver::calculateCrc16Xmodem(const QByteArray& data)
{
    quint16 crc = 0x0000;
    for (auto byte : data)
    {
        crc ^= (quint8(byte) << 8);
        for (int i = 0; i < 8; i++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc = (crc << 1);
            crc &= 0xFFFF;
        }
    }
    return crc;
}
void SkydroidDriver::OSDEnable()
{
    QString cmd = buildCommand("UG", "w", "EXT", "0010");
    socket.writeDatagram(cmd.toUtf8(), QHostAddress(ip), port);
}
void SkydroidDriver::OSDDisable()
{
    QString cmd = buildCommand("UG", "w", "EXT", "0000");
    socket.writeDatagram(cmd.toUtf8(), QHostAddress(ip), port);
}
void SkydroidDriver::setTrackingState(bool enabled)
{
    QString val = enabled ? "01" : "00";
    QString cmd =buildCommand("UG", "w", "SUM", val);
}
void SkydroidDriver::sendRawHex(const QString& hex)
{
    QByteArray data = QByteArray::fromHex(hex.toUtf8());
    socket.writeDatagram(data, QHostAddress(ip), 1030);
}

void SkydroidDriver::stopTracking()
{
    setTrackingState(false);
    QString cmd = "aaa50a000000020000000000000000000062c6cd";
    sendRawHex(cmd);

}
void SkydroidDriver::setPalette(int palette_idx)
{
    QString val_str =
        QString("%1").arg(palette_idx, 2,16, QLatin1Char('0')).toUpper();

    QString cmd =buildCommand("UD","w", "IMG",val_str);
    socket.writeDatagram(cmd.toUtf8(), QHostAddress(ip), port);
}

void SkydroidDriver::setGimbalSpeed(int yaw, int pitch)
{
    yaw = qBound(-127, yaw, 127);
    pitch = qBound(-127, pitch, 127);

    auto toHex = [](int v) {
        if (v < 0) v += 256;
        return QString("%1").arg(v, 2, 16, QLatin1Char('0')).toUpper();
    };

    QString data = toHex(yaw) + toHex(pitch);
    QString cmd = buildCommand("UG", "w", "GSM", data);

    socket.writeDatagram(cmd.toUtf8(), QHostAddress(ip), port);
}

void SkydroidDriver::centerGimbal()
{
    QString cmd = buildCommand("UG", "w", "PTZ", "05");
    socket.writeDatagram(cmd.toUtf8(), QHostAddress(ip), port);
}
void SkydroidDriver::takephoto()
{
    QString cmd = buildCommand("UD","w","CAP","01");
    socket.writeDatagram(cmd.toUtf8(),QHostAddress(ip),port);
}
void SkydroidDriver::Startrecord()
{
    QString cmd = buildCommand("UD","w","REC","01");
    socket.writeDatagram(cmd.toUtf8(),QHostAddress(ip),port);
}

void SkydroidDriver::Stoprecord()
{
    QString cmd = buildCommand("UD","w","REC","00");
    socket.writeDatagram(cmd.toUtf8(),QHostAddress(ip),port);
}

void SkydroidDriver::lookDown()
{
    int angle = -9000;
    if (angle < 0) angle += 65536;

    QString data = QString("%1").arg(angle, 4, 16, QLatin1Char('0')).toUpper() + "32";
    QString cmd = buildCommand("UG", "w", "GAP", data);

    socket.writeDatagram(cmd.toUtf8(), QHostAddress(ip), port);
}

void SkydroidDriver::zoomIn()
{
    QString cmd = buildCommand("UD", "w", "DZM", "0A");
    socket.writeDatagram(cmd.toUtf8(), QHostAddress(ip), port);
}

void SkydroidDriver::zoomOut()
{
    QString cmd = buildCommand("UD", "w", "DZM", "0B");
    socket.writeDatagram(cmd.toUtf8(), QHostAddress(ip), port);
}
