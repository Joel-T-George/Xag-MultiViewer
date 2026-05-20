#pragma once
#include <QWidget>
#include <QVector>
#include <QVBoxLayout>
#include <QGridLayout >
#include "HomeTab.h"

#include "CameraInstance.h"
#include "CameraCardWidget.h"
#include "CameraManager.h"

// class HomeTab;


class ConnectTab : public QWidget
{
    Q_OBJECT
    public:
        explicit ConnectTab(CameraManager* manager,QVector<CameraInstance*>* cams,HomeTab* homeTab, QWidget* parent=nullptr);

        void setHomeTab(HomeTab* home) { homeTab = home; }
    signals: 
        void camerasChanged();

    public slots:
        void addCameraCard(CameraInstance *cam);

    private slots:
        void onCreateCamera();
        void onCameraUpdated(CameraInstance *camera);

        void startCamera(CameraInstance *camera);
        void stopCamera(CameraInstance *camera);
        void editCamera(CameraInstance *camera);
        void deleteCamera(CameraInstance *camera);
        void rebuildGrid();


        // void addCameraCard(CameraInstance *camera);
        void removeCameraCard(CameraInstance *camera);
        // bool pipelineExists(CameraInstance *camera);


    private:
        HomeTab* homeTab = nullptr;
        CameraManager* cammanager;

        // QVector<CameraInstance*> *cameras = nullptr;
        QString generateNextCameraName(const QList<CameraInstance*> &cameras);
        QVector<CameraInstance*>* cameras = nullptr;
        // QString generateNextCameraName( QList<CameraInstance*>& cameras);
        QGridLayout  *listLayout = nullptr;
        QWidget *cardsContainer = nullptr;
        QMap<CameraInstance*, CameraCardWidget*> cardMap;
};
