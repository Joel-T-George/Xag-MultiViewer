#include "HomeTab.h"
#include "VideoWidget.h"
#include "CameraInstance.h"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>


HomeTab::HomeTab(QVector<CameraInstance*>* cams, QWidget* parent)
    : QWidget(parent), cameras(cams)
{
    auto* root = new QHBoxLayout(this);
    QWidget * gridContainer = new QWidget;
    gridContainer->setObjectName("videoPanel");
    grid = new QGridLayout(gridContainer);
    grid->setSpacing(6);
    // setStyleSheet("background-color: #111;");
    auto* btnPrev = new QPushButton("<", this);
    auto* btnNext = new QPushButton(">", this);

    // btnPrev->setStyleSheet("background:rgba(0,0,0,80);color:white;");
    // btnNext->setStyleSheet("background:rgba(0,0,0,80);color:white;");

    grid->addWidget(btnPrev, 2, 0);
    grid->addWidget(btnNext, 2, 1);

    // RIGHT: Sidebar
    QWidget* sidebar = new QWidget;
    sidebar->setObjectName("sidebar");

    sidebar->setFixedWidth(260);
    // sidebar->setStyleSheet("background:#1b1b1b;color:white;");
    QVBoxLayout* sideLayout = new QVBoxLayout(sidebar);

    selectedLabel = new QLabel("No Camera Selected");
    // selectedLabel->setStyleSheet("padding:6px;background:#222;");

    selectAllBtn = new QPushButton("Select All");
    unselectAllBtn =  new QPushButton("Un Select All");
    controlBtn = new QPushButton("Move Selected");
    QWidget* controlpanel = createControlPanel();
    QWidget* cameraContainer = new QWidget;

    cameraList = new QListWidget();
    cameraList->setWordWrap(false);
    cameraList->setWrapping(false);
    cameraList->setSelectionMode(QAbstractItemView::MultiSelection);
    cameraList->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    cameraList->setTextElideMode(Qt::ElideRight); // adds "..." if too long

    cameraContainer->setMaximumHeight(180);   // adjust: 180–300 typical
    cameraContainer->setFixedWidth(250);   // adjust: 180–300 typical
    cameraContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);


    sideLayout->setAlignment(Qt::AlignTop);

    sideLayout->addWidget(selectAllBtn);
    sideLayout->addWidget(selectedLabel);
    // sideLayout->addWidget(unselectAllBtn);
    sideLayout->addSpacing(10);
    sideLayout->addWidget(cameraContainer, 3);
    sideLayout->addStretch();
    sideLayout->addWidget(controlpanel,1, Qt::AlignHCenter);
    sideLayout->addStretch();

    root->addWidget(gridContainer, 3);


    root->addWidget(sidebar, 1);
    root->setStretch(0,3);
    root->setStretch(1,1);


    connect(selectAllBtn, &QPushButton::clicked, this, &HomeTab::selectAllCameras);
    connect(cameraList, &QListWidget::itemClicked, this,
             [this](QListWidgetItem* item)
             {
                CameraInstance* cam = itemToCamera.value(item);

                if (selectedCameras.contains(cam)) {
                   // REMOVE
                    selectedCameras.remove(cam);
                    item->setSelected(false);
                 } else {
                    // ADD
                  selectedCameras.insert(cam);
                    item->setSelected(true);
                }

                updateSelectionUI();
           });

    connect(btnPrev, &QPushButton::clicked, this, &HomeTab::prevPage);
    connect(btnNext, &QPushButton::clicked, this, &HomeTab::nextPage);


}

void HomeTab::selectAllCameras()
{
    if (!cameras) return;

    bool allSelected = (selectedCameras.size() == cameras->size());

    selectedCameras.clear();

    for (CameraInstance* cam : *cameras)
    {
        QListWidgetItem* item = cameraToItem.value(cam, nullptr);

        if (!allSelected)
        {
            selectedCameras.insert(cam);
            if (item) item->setSelected(true);
        }
        else
        {
            if (item) item->setSelected(false);
        }
    }

    updateSelectionUI();

    // Optional: update button text
    selectAllBtn->setText(allSelected ? "Select All" : "Clear All");
}
void HomeTab::updateSelectionUI()
{
    for (auto it = cameraContainers.begin(); it != cameraContainers.end(); ++it)
    {
        CameraInstance* cam = it.key();
        QWidget* container = it.value();

        if (selectedCameras.contains(cam)) {
            container->setStyleSheet("border: 1px solid blue;");
        } else {
            container->setStyleSheet("");
        }
    }

    // Update label
    if (selectedCameras.isEmpty()) {
        selectedLabel->setText("No Camera Selected");
    } else {
        QStringList names;
        for (auto* cam : selectedCameras)
            names << cam->name;

        selectedLabel->setText(names.join(", "));
    }
}
QWidget* HomeTab::createControlPanel()
{
    QWidget* panel = new QWidget;
    panel->setFixedWidth(160);
    panel->setStyleSheet("background:#1b1b1b;color:white;");

    QVBoxLayout* v = new QVBoxLayout(panel);
    v->setContentsMargins(10,10,10,10);

    // ===== JOYSTICK =====
    QPushButton* up = new QPushButton("↑");
    QPushButton* down = new QPushButton("↓");
    QPushButton* left = new QPushButton("←");
    QPushButton* right = new QPushButton("→");
    QPushButton* center = new QPushButton("●");

    QList<QPushButton*> btns = {up, down, left, right, center};

    for (auto* b : btns) {
        b->setFixedSize(40,40);
        b->setStyleSheet("background:#2c2c2c;color:white;border-radius:20px;");
    }

    QGridLayout* grid = new QGridLayout;
    grid->addWidget(up,    0,1);
    grid->addWidget(left,  1,0);
    grid->addWidget(center,1,1);
    grid->addWidget(right, 1,2);
    grid->addWidget(down,  2,1);

    v->addLayout(grid);

    // ===== PRESETS =====
    QGridLayout * grid2 = new QGridLayout;
    QPushButton* btnZin = new QPushButton("Zoom In");
    QPushButton* btnZot = new QPushButton("Zoom Out°");
    QPushButton* btnRec = new QPushButton("Rec Start");
    QPushButton* btnRecstop = new QPushButton("Rec Stop");
    QPushButton* btnOsdOn = new QPushButton("OSD On");
    QPushButton* btnOsdOff = new QPushButton("OSD Off");
    QPushButton* btnPhot = new QPushButton("Photo");
    QPushButton* btnMap = new QPushButton("Mapping");
    QPushButton* btnFPV = new QPushButton("FPV");

    // QPushButton* btnTrackEnable = new QPushButton("Track Enable");

    QList<QPushButton*> presets = {btnZin, btnZot, btnRec,btnRecstop,btnOsdOn,btnOsdOff, btnPhot, btnMap,btnFPV};

    for (auto* b : presets) {
        b->setStyleSheet("background:#3a3a3a;color:white;");

    }
    grid2->addWidget(btnZin,0,0);
    grid2->addWidget(btnZot,0,1);
    grid2->addWidget(btnRec,1,0);
    grid2->addWidget(btnRecstop,1,1);
    grid2->addWidget(btnOsdOn,2,0);
    grid2->addWidget(btnOsdOff,2,1);
    grid2->addWidget(btnPhot,3,0);
    grid2->addWidget(btnMap,3,1);
    grid2->addWidget(btnFPV,4,0);
    // grid2->addWidget(btnTrackEnable,4,1);
    v->addLayout(grid2);

    v->addStretch();

    connect(up, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::MOVE_UP,0,0);
    });
    connect(down, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::MOVE_DOWN,0,0);
    });
    connect(left, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::MOVE_LEFT,0,0);
    });
    connect(right, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::MOVE_RIGHT,0,0);
    });
    connect(center, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::CENTER,0,0);
    });
    connect(btnZin, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::ZOOM_IN,0,0);
    });
    connect(btnZot, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::ZOOM_OUT,0,0);
    });
    connect(btnRec, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
        {
            cam->control(ControlCommand::REC,0,0);
            onRecordClicked(cam->ip);
        }
    });
    connect(btnPhot, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::PHOTO,0,0);
    });
    connect(btnFPV, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::LOOK_FORWARD,0,0);
    });
    connect(btnMap, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::LOOK_DOWN,0,0);




    });
    // connect(btnTrackEnable, &QPushButton::pressed, this, [=]() {
    //     for (auto* cam : selectedCameras)
    //         cam->control(ControlCommand::TRACK_MODE,0,0);
    // });
    connect(btnRecstop, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::RECStop,0,0);
    });
    connect(btnOsdOn, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::OSDON,0,0);
    });
    connect(btnOsdOff, &QPushButton::pressed, this, [=]() {
        for (auto* cam : selectedCameras)
            cam->control(ControlCommand::OSDOFF,0,0);
    });
    return panel;
}

void HomeTab::onRecordClicked(QString camip)
{
    QString filePath =
        QCoreApplication::applicationDirPath()
        + "/record_log.txt";

    QFile file(filePath);

    if (file.open(QIODevice::Append |
                  QIODevice::Text))
    {
        QTextStream out(&file);

        QString timestamp =
            QDateTime::currentDateTime()
                .toString(
                    "yyyy-MM-dd hh:mm:ss.zzz"
                    );

        out << "REC START : "
            << timestamp
            << "  "
            << camip
            << "\n";

        file.close();
    }
}
void HomeTab::setCamera(QVector<CameraInstance*>*cams)
{
    cameras = cams;

    // create persistent panels for all cameras
    for (auto* cam : *cameras) {
        if (  cameraContainers.contains(cam))
            continue;

        QWidget* container = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(container);
        v->setContentsMargins(2,2,2,2);
        QLabel* label = new QLabel(cam->name);
        // label->setStyleSheet("color:white;background:#222;padding:3px;");
        VideoWidget* video = new VideoWidget(container);
        QString pipelineStarter = "videotestsrc ! d3d11videosink sync=false";

        video->setMinimumSize(320,240);
        QListWidgetItem* item = new QListWidgetItem(cam->name);
        cameraList->addItem(item);
        v->addWidget(label);
        v->addWidget(video,1);
    

        itemToCamera[item] = cam;
        cameraToItem[cam] = item;

        cameraContainers[cam] = container;
        cameraWidgets[cam] = video;
        cam->initDriver(cam->ip,cam->port);
        cam->widget = video;
        video->cam= cam;
        connect(video, &VideoWidget::controlRequested, this,
            [cam](ControlCommand cmd, int x, int y)
            {
                if (cam)
                    cam->control(cmd,x,y);
            });
        connect(video, &VideoWidget::clicked, this, [this, cam]() {
            toggleCameraSelection(cam);
        });


        container->hide(); // hide until page shows
        grid->addWidget(container, 0,0); // temporary, will re-layout in layoutPage()

        emit  video->startRequested(pipelineStarter, video->nativeHandle());
    }

    layoutPage();
}

void HomeTab::refresh()
{
    layoutPage();
}
void HomeTab::layoutPage()
{
    if (!cameras) return;

    int start = pageIndex * maxPerPage;
    int end = qMin(start + maxPerPage, cameras->size());

    // hide all containers first
    for (auto it = cameraContainers.cbegin(); it != cameraContainers.cend(); ++it)
    {
        if (it.value())
            it.value()->hide();


    }

    // show only current page
    int idx = 0;
    for (int i=start; i<end; ++i) {
        CameraInstance* cam = cameras->at(i);
        QWidget* container = cameraContainers.value(cam,nullptr);
        if (!container) continue;

        int r = idx / 2;
        int c = idx % 2;

        grid->addWidget(container, r, c);
        container->show();

        VideoWidget* video = cameraWidgets.value(cam);
        if (cam->connected && video)
            video->m_connected = true;
            video->start(cam->pipeline);

        idx++;
    }
}

void HomeTab::addCameraPanel(CameraInstance* camera)
{
    if (!camera || cameraContainers.contains(camera))
        return;

    QWidget* container = new QWidget;
    QVBoxLayout* v = new QVBoxLayout(container);
    QLabel* label = new QLabel(camera->name);
    label->setStyleSheet("color:white;background:#222;padding:3px;");
    VideoWidget* video = new VideoWidget(container);
    video->setMinimumSize(320,240);
    video->m_connected = false;

    QListWidgetItem* item = new QListWidgetItem(camera->name);
    cameraList->addItem(item);

    itemToCamera[item] = camera;
    cameraToItem[camera] = item;
    video->cam = camera;
    camera->widget = video;

    
    v->addWidget(label);
    v->addWidget(video,1);

    cameraContainers[camera] = container;
    cameraWidgets[camera] = video;
    connect(video, &VideoWidget::controlRequested, this,
        [camera](ControlCommand cmd, int x, int y)
        {
            if (camera)
                camera->control(cmd,x,y);
        });
    connect(video, &VideoWidget::clicked, this, [this, camera]() {
        toggleCameraSelection(camera);
    });

    container->hide();
    layoutPage();
}
void HomeTab::removeCameraPanel(CameraInstance* camera)
{
    if (!camera || !cameraContainers.contains(camera)) return;

    QWidget* container = cameraContainers.take(camera);
    VideoWidget* video = cameraWidgets.take(camera);

    if (video) {
        video->m_connected = false;
        video->stop();
        delete video;
    }

    if (container) delete container;

    layoutPage();
}
void HomeTab::toggleCameraSelection(CameraInstance* cam)
{
    if (selectedCameras.contains(cam))
        selectedCameras.remove(cam);
    else
        selectedCameras.insert(cam);

    // sync list UI
    QListWidgetItem* item = cameraToItem.value(cam);
    if (item)
        item->setSelected(selectedCameras.contains(cam));

    updateSelectionUI();
}
void HomeTab::updateCameraState(CameraInstance* camera)
{
    if (!camera) return;

    VideoWidget* video = cameraWidgets.value(camera,nullptr);
    video->m_connected=false;
    if (!video) return;



    if (camera->connected)
    {
        video->m_connected = true;
        video->start(camera->pipeline);
    }
    else
    {
        video->m_connected=false;
        video->stop();
    }

    }


void HomeTab::nextPage()
{
    if (!cameras) return;
    if ((pageIndex+1)*maxPerPage < cameras->size()) {
        pageIndex++;
        layoutPage();
    }
}
void HomeTab::prevPage()
{
    if (!cameras) return;
    if (pageIndex>0) {
        pageIndex--;
        layoutPage();
    }
}
