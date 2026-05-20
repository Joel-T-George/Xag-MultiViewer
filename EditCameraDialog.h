#pragma once

#include <QDialog>

class QLineEdit;
class QComboBox;
class QPushButton;
class CameraInstance;

class EditCameraDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditCameraDialog(CameraInstance *camera, QWidget *parent = nullptr);

signals:
    void cameraUpdated(CameraInstance *camera);

private slots:
    void onSaveClicked();

private:
    CameraInstance *m_camera;

    QLineEdit  *nameEdit;
    QComboBox  *typeCombo;
    QLineEdit  *modelEdit;
    QLineEdit  *urlEdit;

    QPushButton *saveBtn;
    QPushButton *cancelBtn;
};
