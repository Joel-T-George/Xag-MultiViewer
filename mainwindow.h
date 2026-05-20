#pragma once
#include <QMainWindow>
#include <QVector>
#include "CameraInstance.h"
#include "VideoEngine.h"
#include "CameraManager.h"

class HomeTab;
class ConnectTab;


class MainWindow : public QMainWindow
{
Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent = nullptr);
        QVector<CameraInstance*> cameras;
        ~MainWindow();


    private:
        CameraManager* cammanager;
        HomeTab* homeTab;
        ConnectTab* connectTab;
        VideoEngine* videoEngine;
};
