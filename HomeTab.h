#pragma once
#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QMap>
#include <QGridLayout>
#include "CameraInstance.h"
#include "OverlayWidget.h"

class CameraInstance;
class VideoWidget;

class HomeTab : public QWidget
{
    Q_OBJECT
public:
    explicit HomeTab(QVector<CameraInstance*>* cams, QWidget* parent = nullptr);

    void setCamera(QVector<CameraInstance*>* cams);
    void updateSelectionUI();
    void toggleCameraSelection(CameraInstance* cam);
    void refresh();
    QLabel* selectedLabel;
    QPushButton* selectAllBtn;
    QPushButton* unselectAllBtn;
    QPushButton* controlBtn;
    QListWidget* cameraList;
    QWidget* createControlPanel();
    void selectAllCameras();
    void unselectAllCameras();
    OverlayWidget *overlay;



    QSet<CameraInstance*> selectedCameras;
    QMap<QListWidgetItem*, CameraInstance*> itemToCamera;
    QMap<CameraInstance*, QListWidgetItem*> cameraToItem;




public slots:

    void addCameraPanel(CameraInstance* cam);
    void removeCameraPanel(CameraInstance* camera);
    void updateCameraState(CameraInstance* camera);


    void nextPage();
    void prevPage();

private:
    QVector<CameraInstance*>* cameras;
    QGridLayout* grid;
    QMap<CameraInstance*, QWidget*> cameraContainers;
    QMap<CameraInstance*, VideoWidget*> cameraWidgets; // panel container per camera
    int pageIndex = 0;
    const int maxPerPage = 4;

    void layoutPage();

};
