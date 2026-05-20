#include "MainWindow.h"
#include "HomeTab.h"
#include "ConnectTab.h"
#include <QTabWidget>


MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
{
    auto* tabs = new QTabWidget(this);
    cammanager = new CameraManager(this);

    homeTab = new HomeTab(&cameras);
    connectTab = new ConnectTab(cammanager,&cameras, homeTab);
    connectTab->setHomeTab(homeTab);

    cammanager->loadFromFile();
    for (auto* cam : cammanager->cameras) {
        connectTab->addCameraCard(cam);
        cameras.append(cam);
    }

    tabs->addTab(homeTab, "Home");
    tabs->addTab(connectTab, "Connect");

    homeTab->setCamera(&cameras);

    connect(connectTab, &ConnectTab::camerasChanged, homeTab, &HomeTab::refresh);


    setCentralWidget(tabs);
    resize(1200, 800);

    videoEngine = new VideoEngine(this);
    videoEngine->startEngine();
}
MainWindow::~MainWindow()
{
    videoEngine->shutdown();
}
