#include "LrfWorker.h"

LrfWorker::LrfWorker(QObject *parent)
    : QObject(parent)
{
}

void LrfWorker::setCamera(QString ip, quint16 port)
{
    cameraIp = ip;
    cameraPort = port;
}

void LrfWorker::start()
{
    if (running)
        return;

    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::AnyIPv4, 0);

    connect(socket,
            &QUdpSocket::readyRead,
            this,
            &LrfWorker::readPendingDatagrams);

    sendTimer = new QTimer(this);

    connect(sendTimer,
            &QTimer::timeout,
            this,
            &LrfWorker::sendCommand);

    sendTimer->start(500);

    running = true;
    emit statusChanged(true);
}

void LrfWorker::stop()
{
    running = false;

    if (sendTimer)
        sendTimer->stop();

    if (socket)
        socket->close();

    emit statusChanged(false);
}

void LrfWorker::sendCommand()
{
    if (!socket || !running)
        return;

    QByteArray cmd("#TPUD2rSLR0055");

    socket->writeDatagram(
        cmd,
        QHostAddress(cameraIp),
        cameraPort
    );
}

void LrfWorker::readPendingDatagrams()
{
    while (socket->hasPendingDatagrams())
    {
        QByteArray data;
        data.resize(socket->pendingDatagramSize());

        socket->readDatagram(data.data(), data.size());

        QString text = QString::fromUtf8(data);



        if (text.contains("#TPDU") &&
            text.contains("SLR"))
        {
            QString hex =
                text.section("SLR", 1, 1)
                    .left(4);

            bool ok = false;

            int raw = hex.toInt(&ok, 16);

            if (ok)
            {
                double meters =
                    (raw * 10.0) / 100.0;

                emit distanceUpdated(meters);
            }

        }
    }
}
