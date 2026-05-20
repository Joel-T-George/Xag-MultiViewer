#include "ConnectTab.h"
// #include "VideoWidget.h"
#include "HomeTab.h"
#include "CameraInstance.h"
#include "CameraCardWidget.h"
#include "CreateCameraDialog.h"
#include <QMessageBox>
#include "EditCameraDialog.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>

static QLineEdit* g_pipelineEdit = nullptr;

ConnectTab::ConnectTab(CameraManager* manager,QVector<CameraInstance*>* cams,
                       HomeTab*homeTab,
                       QWidget* parent)
    : QWidget(parent), cameras(cams),cammanager(manager)
{
    auto * root = new QVBoxLayout(this);
    root->setContentsMargins(5, 5, 5, 5);
    root->setSpacing(5);

    QPushButton *createBtn = new QPushButton("Create Camera");
    createBtn->setFixedHeight(32);

    root->addWidget(createBtn, 0,Qt::AlignCenter );

    cardsContainer = new QWidget(this);

    listLayout = new QGridLayout(cardsContainer);
    listLayout->setSpacing(1);
    listLayout->setContentsMargins(2, 2, 2, 2);

    cardsContainer->setLayout(listLayout);

    root->addWidget(cardsContainer);

    connect(createBtn, &QPushButton::clicked,
        this, &ConnectTab::onCreateCamera);


    // g_pipelineEdit = new QLineEdit(this);
    // g_pipelineEdit->setPlaceholderText(
    //     "videotestsrc ! d3d11videosink sync=false");

    // auto* btnAdd = new QPushButton("Connect Camera", this);

    // layout->addWidget(g_pipelineEdit);
    // layout->addWidget(btnAdd);

    // connect(btnAdd, &QPushButton::clicked,
    //         this, &ConnectTab::addCamera);
    for (auto* cam : *cameras)
        addCameraCard(cam);
}

void ConnectTab::editCamera(CameraInstance *camera){
    if (!camera) return;

    EditCameraDialog dlg(camera, this);

    connect(&dlg, &EditCameraDialog::cameraUpdated, this, &ConnectTab::onCameraUpdated);



    dlg.exec();
    // Optional: stop before edit
    if (camera->connected)
        stopCamera(camera);

    // ---- OPEN CreateCameraDialog IN EDIT MODE ----
    // CreateCameraDialog dlg(camera, this);
    // if (dlg.exec() == QDialog::Accepted) {
    //     saveConfig();
    //     cardMap[camera]->updateState();
    // }

    qDebug() << "Editing camera:" << camera->name;
}

void ConnectTab::onCameraUpdated(CameraInstance *camera)
{
    if(cardMap.contains(camera)) cardMap[camera]->refresh();
    
}


void ConnectTab::onCreateCamera()
{
    CreateCameraDialog dig(this);
    // const QString uniqueName = generateNextCameraName(*cameras);
    dig.setDefaultName(generateNextCameraName(*cameras));
    if(dig.exec() != QDialog::Accepted) return;

    const QString newName = dig.cameraName();
    
    for (auto *cam : *cameras) {
        if (cam && cam->name == newName) {
            QMessageBox::warning(
                this,
                "Duplicate Name",
                "Camera name already exists.\nPlease choose a unique name."
            );
            return;
        }
    }

    CameraInstance *cam = dig.createdCamera();
    cameras->append(cam);
    addCameraCard(cam);
    homeTab->addCameraPanel(cam);
    cammanager->add(cam);
    emit camerasChanged();

    // cameras -> append(new CameraInstance(dlg.result()));
    
}
QString ConnectTab::generateNextCameraName(QVector<CameraInstance*> const &cameras)
{
    int index = 1;

    while (true) {
        QString name = QString("Camera %1").arg(index);
        bool found = false;

        for (CameraInstance *cam : cameras) {
            if (cam && cam->name == name) {
                found = true;
                break;
            }
        }

        if (!found)
            return name;

        ++index;
    }
}

void ConnectTab::rebuildGrid()
{
    // Clear grid
    QLayoutItem *item;
    while ((item = listLayout->takeAt(0)) != nullptr) {
        if (item->widget())
            item->widget()->setParent(nullptr);
        delete item;
    }

    // Re-add cards
    int colCount = 4;  // cards per row
    int row = 0, col = 0;

    for (auto it = cardMap.begin(); it != cardMap.end(); ++it) {
        CameraCardWidget *card = it.value();
        // card->setStyleSheet("border: 1px solid #3a3a3a;border-radius: 10px;");
        listLayout->addWidget(card, row, col);

        col++;
        if (col >= colCount) {
            col = 0;
            row++;
        }
    }
}


void ConnectTab::addCameraCard(CameraInstance *cam)
{
    

    auto *card = new CameraCardWidget(cam, this);


    
    

    cardMap[cam] = card;
    rebuildGrid();

    connect(card, &CameraCardWidget::connectRequested,
            this, &ConnectTab::startCamera);

    connect(card, &CameraCardWidget::disconnectRequested,
            this, &ConnectTab::stopCamera);

    connect(card, &CameraCardWidget::editRequested,
            this, &ConnectTab::editCamera);

    connect(card, &CameraCardWidget::deleteRequested,
            this, &ConnectTab::deleteCamera);
}

void ConnectTab::startCamera(CameraInstance *camera)
{
    if (!camera || camera->connected)
        return;


    if (camera->connected) {
        qDebug() << "Camera already connected:" << camera->name;
        return;
    }



    // ---- CREATE GST PIPELINE HERE ----
    // Example:
    // camera->pipeline = GstPipelineManager::instance()->create(camera);

    camera->connected = true;
    

    homeTab->updateCameraState(camera);
    if (cardMap.contains(camera))
        cardMap[camera]->updateState();


    // Notify HomeTab (signal if you have it)
    // emit cameraConnected(camera);





    qDebug() << "Started camera:" << camera->name;
}
void ConnectTab::stopCamera(CameraInstance *camera)
{
    if (!camera || !camera->connected)
        return;

    // ---- STOP GST PIPELINE HERE ----
    // GstPipelineManager::instance()->stop(camera);

    camera->connected = false;
    homeTab->updateCameraState(camera);

    // emit cameraDisconnected(camera);

    if (cardMap.contains(camera))
        cardMap[camera]->updateState();

    // emit camerasChanged();

    qDebug() << "Stopped camera:" << camera->name;
}

void ConnectTab::removeCameraCard(CameraInstance *camera)
{
    if (!cardMap.contains(camera))
        return;

    CameraCardWidget *card = cardMap.take(camera);
    card->deleteLater();
    rebuildGrid();

    emit camerasChanged();
}

void ConnectTab::deleteCamera(CameraInstance *camera)
{
    if (!camera)
        return;




    auto reply = QMessageBox::question(
        this,
        "Delete Camera",
        "Delete camera '" + camera->name + "' ?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes)
        return;

    if (camera->connected)
        stopCamera(camera);

    homeTab->removeCameraPanel(camera);


    if (cardMap.contains(camera))
        cardMap[camera]->updateState();


    removeCameraCard(camera);
    cammanager->remove(camera);
    cameras->removeOne(camera);
    delete camera;

    // saveConfig();

    qDebug() << "Deleted camera";
}




// void ConnectTab::addCamera()
// {
//     if (!g_pipelineEdit) return;

//     auto* cam = new CameraInstance;
//     cam->pipeline = g_pipelineEdit->text();
//     cam->widget = new VideoWidget;
//     cam->connected = true;

//     cam->widget->start(cam->pipeline);

//     cameras->append(cam);
//     homeTab->updatePage();
// }
