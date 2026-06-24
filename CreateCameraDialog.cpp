#include "CreateCameraDialog.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>



CreateCameraDialog::CreateCameraDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Create Camera");
    setModal(true);
    resize(420, 360);

    buildUi();
}

void CreateCameraDialog::buildUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *form = new QFormLayout;

    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("Camera name");

    typeCombo = new QComboBox;
    typeCombo->addItems({"None","RTSP", "UDP", "USB" });

    modelCombo = new QComboBox;
    modelCombo->addItems({ "Generic", "Skydroid C13", "View Pro" });

    usbCombo = new QComboBox;

    ipEdit = new QLineEdit;
    ipEdit->setText("192.168.1.100");
    ipEdit->setPlaceholderText("192.168.1.10");

    primaryUrlEdit = new QLineEdit;
    primaryUrlEdit->setPlaceholderText("rtsp://ip:port/stream");

    secondaryEnableCheck = new QCheckBox("Enable Secondary Video");
    secondaryEnableCheck->setChecked(false);

    secondaryUrlEdit = new QLineEdit;
    secondaryUrlEdit->setPlaceholderText("Optional backup stream");

    portSpin = new QSpinBox;
    portSpin->setRange(1, 65535);
    portSpin->setValue(554);

    transportCombo = new QComboBox;
    transportCombo->addItems({ "udp", "tcp" });

    autoReconnectCheck = new QCheckBox("Auto reconnect on failure");
    autoReconnectCheck->setChecked(true);

    primaryPipelineLabel = new QLabel;
    primaryPipelineLabel->setWordWrap(true);
    primaryPipelineLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    secondaryPipelineLabel = new QLabel;
    secondaryPipelineLabel->setWordWrap(true);
    secondaryPipelineLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);



    form->addRow("Name", nameEdit);
    form->addRow("Type", typeCombo);
    form->addRow("Model", modelCombo);
    form->addRow("Usb Selector", usbCombo);
    form->addRow("IP Address", ipEdit);
    form->addRow("Primary URL", primaryUrlEdit);
    form->addRow("Pip",secondaryEnableCheck);
    form->addRow("Secondary URL", secondaryUrlEdit);
    form->addRow("Port", portSpin);
    form->addRow("Transport", transportCombo);
    form->addRow("Primary pipeline", primaryPipelineLabel);
    form->addRow("Secondary pipeline", secondaryPipelineLabel);

    mainLayout->addLayout(form);
    mainLayout->addWidget(autoReconnectCheck);

    // buttons
    createBtn = new QPushButton("Create");
    cancelBtn = new QPushButton("Cancel");

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(createBtn);

    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    primaryUrlEdit->hide();
    secondaryEnableCheck->hide();
    ipEdit->hide();
    secondaryUrlEdit->hide();
    portSpin->hide();
    transportCombo->hide();
    modelCombo->hide();
    usbCombo->hide();
    primaryPipelineLabel->hide();
    secondaryPipelineLabel->hide();
    updateRtspEdit();

     auto rebuild = [this](bool force) {
        rebuildDraft(force);
    };


    connect(typeCombo, &QComboBox::currentTextChanged, this, [rebuild](){ rebuild(true); });
     connect(modelCombo,&QComboBox::currentTextChanged,this, [rebuild](){ rebuild(true); });
    connect(ipEdit, &QLineEdit::textChanged, this, [rebuild](){ rebuild(false); });
    connect(usbCombo,  &QComboBox::currentIndexChanged,this,  [rebuild](){ rebuild(false); });
    connect(secondaryEnableCheck, &QCheckBox::toggled, this,  [rebuild](){ rebuild(false); });
    connect(primaryUrlEdit, &QLineEdit::textChanged,this, [this](){updateRtspEdit();});
    connect(secondaryUrlEdit, &QLineEdit::textChanged,this, [this](){updateRtspEdit();});






    // connect(typeCombo, &QComboBox::currentTextChanged,
    //     this, &CreateCameraDialog::onTypeChanged);

    // connect(secondaryEnableCheck, &QCheckBox::toggled,
    //     secondaryUrlEdit, &QWidget::setEnabled);

    // connect(modelCombo, &QComboBox::currentTextChanged,
    //         this, &CreateCameraDialog::onModelChanged);

    // connect(ipEdit, &QLineEdit::textChanged,
    //         this, &CreateCameraDialog::onIpChanged);

    connect(createBtn, &QPushButton::clicked,
            this, &CreateCameraDialog::onCreateClicked);
    connect(cancelBtn, &QPushButton::clicked,
            this, &CreateCameraDialog::onCancelClicked);
     // Initial render

}
// void CreateCameraDialog::onTypeChanged(const QString &type)
// {
//     if(type == "USB"){
//         modelCombo->hide();
//         ipEdit->hide();
//         primaryUrlEdit->hide();
//         secondaryUrlEdit->hide();
//         portSpin->hide();
//         transportCombo->hide();

//         // TODO later: USB device dropdown
//         return;
//     }

//     if(type == "RTSP")
//     {
//         ipEdit->show();
//         modelCombo->show();
//         modelCombo->setEnabled(true);

//     }


// }
void CreateCameraDialog::populateUsbDevices()
{
    usbCombo->clear();

    const auto cameras = QMediaDevices::videoInputs();

    if (cameras.isEmpty()) {
        usbCombo->addItem("No camera found", -1);
        usbCombo->setEnabled(false);
        return;
    }

    usbCombo->setEnabled(true);

    int index = 0;
    for (const QCameraDevice &cam : cameras) {
        usbCombo->addItem(
            QString("%1 (%2)")
                .arg(cam.description())
                .arg(cam.id()),
            index
        );
        index++;
    }
}

// void CreateCameraDialog::onIpChanged(const QString &ip)
// {
//     if (ip.isEmpty())
//         return;

//     if (modelCombo->currentText() == "Skydroid C13")
//         applyC13Rules(ip);
//     else if (modelCombo->currentText() == "Generic")
//         applyGenericRules(ip);
// }

// void CreateCameraDialog::onModelChanged(const QString &model)
// {
//     QString ip = ipEdit->text().trimmed();
//     if (ip.isEmpty())
//         return;

//     if (model == "Skydroid C13")
//         applyC13Rules(ip);
//     else
//         applyGenericRules(ip);
// }
void CreateCameraDialog::rebuildDraft(bool force)
{
    if (force){

        // ---- RESET VISIBILITY ----
        usbCombo->hide();
        modelCombo->hide();
        ipEdit->hide();
        primaryUrlEdit->hide();
        secondaryUrlEdit->hide();
        portSpin->hide();
        transportCombo->hide();
        secondaryEnableCheck->hide();
        primaryPipelineLabel->hide();
        secondaryPipelineLabel->hide();

        draft = CameraDraft{}; // reset safely

    }

    const QString type = typeCombo->currentText();
    if (type == "USB") {
        draft.type = CameraDraft::Type::USB;
        transportCombo->setVisible(false);


        usbCombo->show();

        if (usbCombo->count() == 0)
            populateUsbDevices();

        int idx = usbCombo->currentData().toInt();
        draft.usbIndex = idx;

        if (idx >= 0) {
            draft.primaryUrl =
                QString("mfvideosrc device-index=%1 ! videoconvert ! autovideosink")
                .arg(idx);
        }
    }


    else if (type == "RTSP") {
        draft.type = CameraDraft::Type::RTSP;

        ipEdit->show();
        modelCombo->show();
        transportCombo->setVisible(true);


        const QString ip = ipEdit->text().trimmed();
        const QString model = modelCombo->currentText();

        draft.ip = ip;

        if (ip.isEmpty())
            return; // stop early, no pipeline

        

        // ---- MODEL RULES ----
        if (model == "Skydroid C13") {
            draft.model = CameraDraft::Model::C13;


            draft.primaryUrl =
                QString("rtsp://%1:554/stream=1").arg(ip);
            draft.secondaryUrl =
                QString("rtsp://%1:555/stream=2").arg(ip);
            draft.udpPort = 5000;
            draft.transport="udp";
        }
        else {
            draft.model = CameraDraft::Model::Generic;

            draft.primaryUrl =
                QString("rtsp://%1:8554/live").arg(ip);
            draft.secondaryUrl =
                QString("rtsp://%1:8555/live").arg(ip);
            draft.udpPort = 2000;
            draft.transport="udp";
        }
        secondaryEnableCheck->show();
        if (secondaryEnableCheck->isEnabled())
            secondaryUrlEdit->setDisabled(false);

        else secondaryUrlEdit->setDisabled(true);




        primaryUrlEdit->show();
        primaryUrlEdit->setText(draft.primaryUrl);
        secondaryUrlEdit->show();
        transportCombo->show();
        portSpin->show();
        secondaryUrlEdit->setText(draft.secondaryUrl);
        portSpin->setValue(draft.udpPort);
        transportCombo->setCurrentText(draft.transport);





    }

    updatePipelinePreview();
}
void CreateCameraDialog::updateRtspEdit()
{
    const QString primary_url = primaryUrlEdit->text().trimmed();
    const QString sec_url = secondaryUrlEdit->text().trimmed();
    draft.primaryUrl = primary_url;
    draft.secondaryUrl= sec_url;
    updatePipelinePreview();
}
void CreateCameraDialog::updatePipelinePreview()
{
    // Primary
    if (draft.type == CameraDraft::Type::RTSP && !draft.primaryUrl.isEmpty()) {
        draft.primaryPipeline =
            QString("rtspsrc location=%1 latency=50  drop-on-latency=true buffer-mode=0 ! decodebin ! queue leaky=downstream max-size-buffers=1 max-size-time=0 max-size-bytes=0 ! videoconvert ! glimagesink sync=false")
            .arg(draft.primaryUrl);
    } 
    else if(draft.type == CameraDraft::Type::USB)
    {
         draft.primaryPipeline =
            QString("mfvideosrc device-index=%1 ! videoconvert ! glimagesink sync=false")
            .arg(draft.usbIndex);

    }
    
    else {

        draft.primaryPipeline =
            QString("videotestsrc ! videoconvert ! glimagesink sync=false");
    }

    primaryPipelineLabel->setText(draft.primaryPipeline);
    primaryPipelineLabel->show();

    // Secondary
    if (draft.secondaryEnabled && !draft.secondaryUrl.isEmpty()) {
        draft.secondaryPipeline =
            QString("rtspsrc location=%1 latency=50  drop-on-latency=true buffer-mode=0 ! decodebin ! queue leaky=downstream max-size-buffers=1 max-size-time=0 max-size-bytes=0 ! videoconvert ! glimagesink sync=false")
            .arg(draft.secondaryUrl);
        secondaryPipelineLabel->setText(draft.secondaryPipeline);
        secondaryPipelineLabel->show();
    } else {
        draft.secondaryPipeline.clear();
        secondaryPipelineLabel->setText("— disabled —");
    }
     // 6️⃣ enable/disable Create button
    QString err;
    createBtn->setEnabled(validateDraft(&err));
}
bool CreateCameraDialog::validateDraft(QString *error) const
{
    if (!draft.primaryPipeline.isEmpty()){
        return true;
    }
    if (nameEdit->text().trimmed().isEmpty()) {
        if (error) *error = "Camera name is required";
        return false;
    }

    if (draft.type == CameraDraft::Type::RTSP) {
        if (draft.ip.isEmpty()) {
            if (error) *error = "IP address is required for RTSP";
            return false;
        }
    }

    if (draft.primaryPipeline.isEmpty()) {
        if (error) *error = "Primary pipeline is empty";
        return false;
    }

    return true;
}



// void CreateCameraDialog::applyC13Rules(const QString &ip)
// {
//     primaryUrlEdit->setText(
//         QString("rtsp://%1:554/stream=0").arg(ip)
//     );
//     secondaryUrlEdit->setText(
//         QString("rtsp://%1:555/stream=1").arg(ip)
//     );

//     transportCombo->setCurrentText("udp");
//     portSpin->setValue(5000);

//     showRtspFields();
// }
// void CreateCameraDialog::applyGenericRules(const QString &ip)
// {
//     primaryUrlEdit->setText(
//         QString("rtsp://%1:8554/live").arg(ip)
//     );
//     secondaryUrlEdit->setText(
//         QString("rtsp://%1:8555/live").arg(ip)
//     );

//     transportCombo->setCurrentText("udp");
//     portSpin->setValue(2000);

//     showRtspFields();
// }
QString CreateCameraDialog::cameraName() const
{
    return nameEdit->text().trimmed();
}
void CreateCameraDialog::setDefaultName(const QString &name)
{
    nameEdit->setText(name);
    nameEdit->selectAll(); // allow quick rename
}
// void CreateCameraDialog::showRtspFields()
// {
//     primaryUrlEdit->show();
//     secondaryUrlEdit->show();
//     portSpin->show();
//     transportCombo->show();
// }

bool CreateCameraDialog::validate() const
{
    if (nameEdit->text().trimmed().isEmpty())
        return false;

    if (typeCombo->currentText() == "RTSP") {
         if (ipEdit->text().trimmed().isEmpty())
            return false;
        if (primaryUrlEdit->text().trimmed().isEmpty())
            return false;

        if (secondaryEnableCheck->isChecked() &&
            secondaryUrlEdit->text().trimmed().isEmpty())
            return false;
    }

    return true;
}

void CreateCameraDialog::onCreateClicked()
{
    if (!validate()) {
        QMessageBox::warning(
            this,
            "Invalid Input",
            "Camera name and primary URL are required."
        );
        return;
    }

    camera = new CameraInstance;
    camera->name = nameEdit->text().trimmed();
    camera->cameraType = typeCombo->currentText();
    camera->model =cameraModelFromString( modelCombo->currentText());



    camera->primaryUrl = primaryUrlEdit->text().trimmed();
    camera->ip = ipEdit->text().trimmed();
    
    camera->port = portSpin->value();
    camera->transport = transportCombo->currentText();
    camera->secondaryEnabled = secondaryEnableCheck->isChecked();
    camera->secondaryUrl = secondaryEnableCheck->isChecked()
                        ? secondaryUrlEdit->text().trimmed()
                        : QString();

    camera ->pipeline =  draft.primaryPipeline;
    camera ->secondaryPipeline = camera ->secondaryEnabled ? draft.secondaryPipeline:QString();


    // camera->autoReconnect = autoReconnectCheck->isChecked();
    camera->connected = false;
    camera->initDriver(camera->ip,camera->port);
    // camera->pipelineRunning = false;
    // camera->video = nullptr;

    accept();
}

void CreateCameraDialog::onCancelClicked()
{
    reject();
}

CameraInstance *CreateCameraDialog::createdCamera() const
{
    return camera;
}
