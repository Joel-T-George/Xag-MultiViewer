#include "VideoWidget.h"
// #include "OverlayWidget.h"
#include <QMouseEvent>
#include <QEvent>
#include <QLayout>
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QPushButton>
#include <QComboBox>
#include <QPaintEvent>
#include <QPainter>
VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NativeWindow);  // required to get a native window handle
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    setMinimumSize(200, 150);

    m_connected = false;

}

VideoWidget::~VideoWidget()
{
    stop();
    emit stopRequested();
}
WId VideoWidget::nativeHandle() const
{
    return winId(); // HWND on Windows
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (isTracking && fullScreenWindow && isFullscreen)
            return;

        if (!fullScreenWindow)
        {
            // ENTER FULLSCREEN

            originalParent = parentWidget();
            originalLayout = originalParent->layout();

            fullScreenWindow = new QWidget();
            fullScreenWindow->setWindowFlags(Qt::Window);
            // fullScreenWindow->setWindowState(Qt::WindowFullScreen);
            fullScreenWindow->showFullScreen();

            QHBoxLayout* layout = new QHBoxLayout(fullScreenWindow);
            layout->setContentsMargins(0,0,0,0);
            layout->setSpacing(0);
            this->setParent(fullScreenWindow);
            this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->resize(fullScreenWindow->size());
            this->updateGeometry();

            layout->addWidget(this, 1);

            QWidget* controlPanel = createRightControlPanel();
            controlPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            controlPanel->setMinimumWidth(160);
            controlPanel->setMaximumWidth(200);
            layout->addWidget(controlPanel,0);
            fullScreenWindow->setLayout(layout);
            fullScreenWindow->showFullScreen();
            fullScreenWindow->update();
            isFullscreen = true;


            // fullScreenWindow->show();
        }
        else
        {
            // EXIT FULLSCREEN
            exitFullScreen();
        }
    }
}
void VideoWidget::setLRFText(const QString &text)
{
    lrfLabel->setText(text);

    lrfLabel->raise();
}
void VideoWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing);

    // Black background box
    p.fillRect(
        QRect(20,20,160,40),
        QColor(0,0,0,150)
        );

    // Green text
    p.setPen(Qt::green);

    QFont font;
    font.setPointSize(14);
    font.setBold(true);

    p.setFont(font);

    p.drawText(
        QRect(30,20,140,40),
        Qt::AlignVCenter,
        lrfText
        );
}
void VideoWidget::mousePressEvent(QMouseEvent* e)
{
    if (!fullScreenWindow)
        return;

    if (isFullscreen && isTracking)
    {
        QPoint p = e->pos();

        int widgetW = width();
        int widgetH = height();

        // actual stream resolution
        int videoW = 1280;
        int videoH = 720;

        // keep aspect ratio scaling
        float scale = qMin(
            (float)widgetW / videoW,
            (float)widgetH / videoH
        );

        int displayW = videoW * scale;
        int displayH = videoH * scale;

        // black border offsets
        int offsetX = (widgetW - displayW) / 2;
        int offsetY = (widgetH - displayH) / 2;

        // remove border offset
        int x = p.x() - offsetX;
        int y = p.y() - offsetY;

        // outside video area
        if (x < 0 || y < 0 || x >= displayW || y >= displayH)
            return;

        // convert to real video pixels
        int videoX = x * videoW / displayW;
        int videoY = y * videoH / displayH;

        qDebug() << "Widget:" << p;
        qDebug() << "Video :" << videoX << videoY;

        emit controlRequested(
            ControlCommand::TRACK,
            videoX,
            videoY
        );
    }
}
void VideoWidget::exitFullScreen()
{
    if (!fullScreenWindow)
        return;


    this->setParent(originalParent);
    originalLayout->addWidget(this);

    fullScreenWindow->close();
    delete fullScreenWindow;
    fullScreenWindow = nullptr;
}

QWidget* VideoWidget::createRightControlPanel()
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

    QPushButton* btnTrackEnable = new QPushButton("Track Enable");

    QPushButton* btnSwitchCamera = new QPushButton("Switch Stream");
    paletteDropdown = new QComboBox;
    paletteDropdown->addItem("White Hot",  0x01);
    paletteDropdown->addItem("Sepia",      0x03);
    paletteDropdown->addItem("Ironbow",    0x04);
    paletteDropdown->addItem("Rainbow",    0x05);
    paletteDropdown->addItem("Night",      0x06);
    paletteDropdown->addItem("Aurora",     0x07);
    paletteDropdown->addItem("Red Hot",    0x08);
    paletteDropdown->addItem("Jungle",     0x09);
    paletteDropdown->addItem("Medical",    0x0A);
    paletteDropdown->addItem("Black Hot",  0x0B);
    paletteDropdown->addItem("Glory Hot",  0x0C);


    btnSwitchCamera->setEnabled(cam->secondaryEnabled);

    btnTrackEnable->setCheckable(true);
    isTracking = false;


    QList<QPushButton*> presets = {btnZin, btnZot, btnRec,btnRecstop,btnOsdOn,btnOsdOff, btnPhot, btnMap,btnFPV,btnTrackEnable,btnSwitchCamera};

    for (auto* b : presets) {
        b->setStyleSheet("background:#3a3a3a;color:white;");
        v->addWidget(b);
    }
    paletteDropdown->setStyleSheet("background:#3a3a3a;color:white;");
    v->addWidget(paletteDropdown);
    grid2->addWidget(btnZin,0,0);
    grid2->addWidget(btnZot,0,1);
    grid2->addWidget(btnRec,1,0);
    grid2->addWidget(btnRecstop,1,1);
    grid2->addWidget(btnOsdOn,2,0);
    grid2->addWidget(btnOsdOff,2,1);
    grid2->addWidget(btnPhot,3,0);
    grid2->addWidget(btnMap,3,1);
    grid2->addWidget(btnFPV,4,0);
    grid2->addWidget(btnTrackEnable,4,1);
    grid2->addWidget(btnSwitchCamera,5,0);
    
    grid2->addWidget(paletteDropdown, 5,1);
    

    v->addLayout(grid2);
    v->addStretch();

    // ===== SIGNALS =====
    // connect(btnTrackEnable, &QPushButton::pressed, this, [=]() {
    //     emit controlRequested(ControlCommand::TRACK_MODE,0,0);
    // });
    connect(paletteDropdown,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        [=](int index)
    {
        qDebug() << "Palette Index:" << index;
        int paletteValue =
        paletteDropdown->itemData(index).toInt();

        emit controlRequested(
            ControlCommand::SET_PALETTE,
            index,
            0
        );
    });
    connect(btnTrackEnable, &QPushButton::toggled,
        this,
        [=](bool checked)
        {
            if (checked)
            {
                btnTrackEnable->setText("Track Disable");
                    emit controlRequested(ControlCommand::TRACK_MODE, 0, 0);
                    isTracking = true;
            }
            else
            {
                btnTrackEnable->setText("Track Enable");

                    emit controlRequested(ControlCommand::UNTRACK_MODE, 0, 0);
                    isTracking = false;
            }
        });

    connect(up, &QPushButton::pressed, this, [=]() {
            emit controlRequested(ControlCommand::MOVE_UP,0,0);
    });
    connect(down, &QPushButton::pressed, this, [=]() {
            emit controlRequested(ControlCommand::MOVE_DOWN,0,0);
    });
    connect(left, &QPushButton::pressed, this, [=]() {
            emit controlRequested(ControlCommand::MOVE_LEFT,0,0);
    });
    connect(right, &QPushButton::pressed, this, [=]() {
            emit controlRequested(ControlCommand::MOVE_RIGHT,0,0);
    });
    connect(center, &QPushButton::pressed, this, [=]() {
        emit controlRequested(ControlCommand::CENTER,0,0);
    });
    connect(btnZin, &QPushButton::pressed, this, [=]() {
        emit controlRequested(ControlCommand::ZOOM_IN,0,0);
    });
    connect(btnZot, &QPushButton::pressed, this, [=]() {
        emit controlRequested(ControlCommand::ZOOM_OUT,0,0);
    });
    connect(btnRec, &QPushButton::pressed, this, [=]() {
        emit controlRequested(ControlCommand::REC,0,0);
    });
    connect(btnRecstop, &QPushButton::pressed, this, [=]() {
        emit controlRequested(ControlCommand::RECStop,0,0);
    });
    connect(btnOsdOn, &QPushButton::pressed, this, [=]() {
        emit controlRequested(ControlCommand::OSDON,0,0);
    });
    connect(btnOsdOff, &QPushButton::pressed, this, [=]() {
        emit controlRequested(ControlCommand::OSDOFF,0,0);
    });
    connect(btnPhot, &QPushButton::pressed, this, [=]() {
        emit controlRequested(ControlCommand::PHOTO,0,0);
    });
    connect(btnFPV, &QPushButton::pressed, this, [=]() {
        emit controlRequested(ControlCommand::LOOK_FORWARD,0,0);
    });
    connect(btnMap, &QPushButton::pressed, this, [=]() {
        emit controlRequested(ControlCommand::LOOK_DOWN,0,0);
    });

    connect(btnSwitchCamera,&QPushButton::clicked,this,[=](){

        if (!cam || !cam->secondaryEnabled)
            return;

        cam->swapStreams();

        qDebug() << "Primary :" << cam->primaryUrl;
        qDebug() << "Secondary:" << cam->secondaryUrl;

        // restart playback
        resetPipeline();
    });
    return panel;
}
void VideoWidget::updatePaletteControls()
{
    if (!cam)
        return;

    bool enablePalette =
        cam->secondaryEnabled &&
        cam->primaryUrl.contains(
            "stream=2",
            Qt::CaseInsensitive);

    // enable/disable
    paletteDropdown->setEnabled(enablePalette);

    // visual style
    if (enablePalette)
    {


        paletteDropdown->setStyleSheet(
            "background:#333;"
            "color:white;"
        );
    }
    else
    {


        paletteDropdown->setStyleSheet(
            "background:#222;"
            "color:#777;"
        );
    }
}
void VideoWidget::resetPipeline()
{
    if (!cam)
        return;

    stop();

    update();
    repaint();

    QCoreApplication::processEvents();

    start(cam->pipeline);



    update();
}
bool VideoWidget::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape && fullScreenWindow)
        {
            exitFullScreen();
            return true;
        }
    }
    return QWidget::event(event);
}



void VideoWidget::start(const QString &pipelineStr)
{
    stop(); // in case already running

    GError *error = nullptr;
    if (!m_connected)
    {
        qDebug() << "User not Connected";
        return;
    }

    if (pipelineStr.startsWith("EventAction")) {

        if (m_pipelineString == "None"){
            return;
        }

        pipeline= gst_parse_launch(m_pipelineString.toUtf8().constData(),&error);
    }
    else {
        pipeline = gst_parse_launch(pipelineStr.toUtf8().constData(), &error);
        m_pipelineString = pipelineStr;


    }


    // Build pipeline string, e.g., "videotestsrc ! autovideosink"
    // pipeline = gst_parse_launch(pipelineStr.toUtf8().constData(), &error);
    if (!pipeline && m_connected) {
        qDebug() << "Failed to create pipeline:" << (error ? error->message : "unknown");
        if (error) g_error_free(error);
        return;
    }

    // Get the sink supporting VideoOverlay
    GstElement *sink = gst_bin_get_by_interface(GST_BIN(pipeline), GST_TYPE_VIDEO_OVERLAY);
    if (!sink) {
        qDebug() << "Sink does not support video overlay!";
        gst_object_unref(pipeline);
        pipeline = nullptr;
        return;
    }

// #ifdef Q_OS_WIN
//     WId winId = this->winId();
//     gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), (guintptr)winId);
// #elif defined(Q_OS_LINUX)
//     WId winId = this->winId();
//     gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), (guintptr)winId);
// #endif
#ifdef Q_OS_ANDROID

    sink = gst_element_factory_make("glimagesink", NULL);

#else

    sink = gst_element_factory_make("d3d11videosink", NULL);

    WId winId = this->winId();

    gst_video_overlay_set_window_handle(
        GST_VIDEO_OVERLAY(sink),
        (guintptr)winId);

#endif

    gst_object_unref(sink);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

}


void VideoWidget::stop()
{
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = nullptr;
        
    }
}

void VideoWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    qDebug() << "[VideoWidget] show Event";
    emit visible();
    if(m_connected && !pipeline){

        if(isVisible())
            start("EventAction");


    }
}

void VideoWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    emit hidden();
    qDebug() << "[VideoWidget] hide Event";
    stop();
}
