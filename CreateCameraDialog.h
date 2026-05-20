#pragma once

#include <QMediaDevices>
#include <QCameraDevice>


#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>

#include "CameraDraft.h"

#include "CameraInstance.h"

class CreateCameraDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CreateCameraDialog(QWidget *parent = nullptr);

    CameraInstance *createdCamera() const;
    QString cameraName() const;
    void setDefaultName(const QString &name);

private slots:
    void onCreateClicked();
    void onCancelClicked();

private:
    void buildUi();
    void populateUsbDevices();
    bool validate() const;
    void rebuildDraft( bool force);
    bool validateDraft(QString *error) const;
    void updatePipelinePreview();
    void updateRtspEdit();

    // UI
    QLineEdit  *nameEdit;
    QComboBox  *typeCombo;
    QComboBox  *modelCombo;
    QComboBox  *usbCombo;
    QLineEdit *ipEdit;

    QLineEdit  *primaryUrlEdit;
    QCheckBox *secondaryEnableCheck;

    QLineEdit  *secondaryUrlEdit;
    QSpinBox   *portSpin;
    QComboBox  *transportCombo;

    QCheckBox  *autoReconnectCheck;

    QPushButton *createBtn;
    QPushButton *cancelBtn;

    QLabel* primaryPipelineLabel;
    QLabel* secondaryPipelineLabel;

    CameraInstance *camera = nullptr;
    CameraDraft draft;
};
