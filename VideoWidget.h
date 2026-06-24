#pragma once

#include <QWidget>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include "CameraInstance.h"
#include "LrfWorker.h"
#include <QComboBox>
#include <QLabel>
#include <QThread>
#include <QPushButton>

class LrfWorker;


class VideoWidget : public QWidget
{
    Q_OBJECT
    public:
        explicit VideoWidget(QWidget *parent = nullptr);
        ~VideoWidget();
        WId nativeHandle() const;

        QString m_pipelineString ="None";
        bool m_connected = false;

        void start(const QString &pipelineStr);
        void stop();
        bool event(QEvent* event) override;
        CameraInstance *cam=nullptr;
       
        void updatePaletteControls();

        void showEvent(QShowEvent* event) override;
        void hideEvent(QHideEvent* event) override;
        void exitFullScreen();
        bool isFullscreen = false;
        bool isTracking = false;
        void stopLrf();
        void resetPipeline();



        bool lrfEnabled = false;

    private slots:
        void toggleLrf();
        void onDistanceUpdated(double meters);
        

    
    signals:
        void startRequested(const QString &pipelineStr, WId windId);
        void stopRequested();
        void visible();
        void hidden();
        void clicked();
        void controlRequested(ControlCommand cmd,int x,int y);


        void moveCommand(QString dir);
        void movePreset(int angle);
        void trackObject();

    private:
        GstElement *pipeline = nullptr;
        QWidget* fullScreenWindow = nullptr;
        QWidget* originalParent = nullptr;
        QLayout* originalLayout = nullptr;
        QComboBox*paletteDropdown = nullptr;
        QLabel *lrfLabel = nullptr;
        QPushButton * lrfButton = nullptr;
        QThread *lrfThread = nullptr;
        LrfWorker *lrfWorker = nullptr;
        QWidget* createRightControlPanel();
        int originalIndex = -1;
       

    protected:
        void mouseDoubleClickEvent(QMouseEvent* event) override;
        void mousePressEvent(QMouseEvent* e) override;


        // void mousePressEvent(QMouseEvent* ) override;
        // void mouseMoveEvent(QMouseEvent*) override;
        // void mouseReleaseEvent(QMouseEvent*) override;
};
