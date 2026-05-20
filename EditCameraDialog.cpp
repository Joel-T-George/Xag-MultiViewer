#include "EditCameraDialog.h"
#include "CameraInstance.h"

#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

EditCameraDialog::EditCameraDialog(CameraInstance *camera, QWidget *parent)
    : QDialog(parent), m_camera(camera)
{
    setWindowTitle("Edit Camera");
    setModal(true);
    resize(360, 220);

    nameEdit  = new QLineEdit(camera->name);
    modelEdit = new QLineEdit(cameraModelToString(camera->model));
    urlEdit   = new QLineEdit(camera->primaryUrl);

    typeCombo = new QComboBox;
    typeCombo->addItems({"RTSP", "USB", "IP"});
    typeCombo->setCurrentText(camera->cameraType);

    saveBtn   = new QPushButton("Save");
    cancelBtn = new QPushButton("Cancel");

    auto *form = new QFormLayout;
    form->addRow("Name", nameEdit);
    form->addRow("Type", typeCombo);
    form->addRow("Model", modelEdit);
    form->addRow("RTSP URL", urlEdit);

    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch();
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(saveBtn);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addStretch();
    mainLayout->addLayout(btnRow);

    connect(saveBtn, &QPushButton::clicked,
            this, &EditCameraDialog::onSaveClicked);
    connect(cancelBtn, &QPushButton::clicked,
            this, &QDialog::reject);
}

void EditCameraDialog::onSaveClicked()
{
    m_camera->name    = nameEdit->text().trimmed();
    m_camera->cameraType    = typeCombo->currentText();
    m_camera->model   = cameraModelFromString(modelEdit->text().trimmed());

    m_camera->primaryUrl = urlEdit->text().trimmed();
    m_camera->initDriver(m_camera->ip,m_camera->port);

    emit cameraUpdated(m_camera);
    accept();
}
