#pragma once

#include <QThread>
#include <QObject>
#include <functional>

class VideoEngine : public QThread
{
    Q_OBJECT
public:
    explicit VideoEngine(QObject* parent = nullptr);
    ~VideoEngine();

    void shutdown();

    void startEngine();

    // Utility to run work safely in video thread
    void invoke(std::function<void()> fn);

protected:
    void run() override;
};
