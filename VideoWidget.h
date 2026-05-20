#pragma once

#include <QWidget>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include "CameraInstance.h"
#include <QComboBox>
#include <QLabel>

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
        void resetPipeline();
        void updatePaletteControls();

        void showEvent(QShowEvent* event) override;
        void hideEvent(QHideEvent* event) override;
        void exitFullScreen();
        bool isFullscreen = false;
        bool isTracking = false;
        void setLRFText(const QString& text);
        QLabel* lrfLabel;

    
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
        QWidget* createRightControlPanel();
        int originalIndex = -1;
        QString lrfText = "LRF 000";


    protected:
        void paintEvent(QPaintEvent *event) override;
        void mouseDoubleClickEvent(QMouseEvent* event) override;
        void mousePressEvent(QMouseEvent* e) override;


        // void mousePressEvent(QMouseEvent* ) override;
        // void mouseMoveEvent(QMouseEvent*) override;
        // void mouseReleaseEvent(QMouseEvent*) override;
};
