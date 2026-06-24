#include "VideoEngine.h"
#include <QMetaObject>
#include <QDebug>

VideoEngine::VideoEngine(QObject* parent)
    : QThread(parent)
{
}

VideoEngine::~VideoEngine()
{
    shutdown();
}
void VideoEngine::startEngine()
{
    if (!isRunning())
        start();
}

void VideoEngine::run()
{
    // qDebug() << "[VideoEngine] Thread started";

    exec();   // Qt event loop (critical)

    // qDebug() << "[VideoEngine] Thread exiting";
}

void VideoEngine::shutdown()
{
    if (!isRunning())
        return;

    // qDebug() << "[VideoEngine] Shutting down";

    quit();        // stop event loop
    wait();        // block until finished
}

void VideoEngine::invoke(std::function<void()> fn)
{
    if (!isRunning())
        return;

    QMetaObject::invokeMethod(
        this,
        [fn]() { fn(); },
        Qt::QueuedConnection
    );
}
