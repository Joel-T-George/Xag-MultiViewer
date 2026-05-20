#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>


struct CameraInstance;

class CameraCardWidget:  public QWidget{

    Q_OBJECT
    public:
        explicit CameraCardWidget(CameraInstance *cam, QWidget *parent = nullptr);
        CameraInstance *camera() const;
        void updateState();
        void refresh();

    signals:
        void stateChanged();

        void connectRequested(CameraInstance *camera);
        void disconnectRequested(CameraInstance *camera);
        void editRequested(CameraInstance *camera);
        void deleteRequested(CameraInstance *camera);


    private slots:
        // CameraInstance *camera;
        void onConnectClicked();
        void onEditClicked();
        void onDeleteClicked();

    private:
        void buildUi();

    private:
        CameraInstance *m_camera;

        QLabel *nameLabel;
        QLabel *typeLabel;
        QLabel *modelLabel;

        QPushButton *connectBtn;
        QPushButton *editBtn;
        QPushButton *deleteBtn;

};
