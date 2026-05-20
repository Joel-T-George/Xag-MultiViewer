#include "CameraCardWidget.h"
#include "CameraInstance.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>





CameraCardWidget::CameraCardWidget(CameraInstance *camera, QWidget *parent)
    : QWidget(parent),
      m_camera(camera)
{
    setFixedSize(280, 80);

    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("cameraCard");
    setStyleSheet(R"(
        QWidget#cameraCard  {
            background-color: #ffffff;
            border: 1px solid #3a3a3a;
            border-radius: 10px;
        }
    )");
     // setStyleSheet("border: 1px solid #3a3a3a;border-radius: 10px;");
    buildUi();
    updateState();
}

CameraInstance* CameraCardWidget::camera() const
{
    return m_camera;
}

void CameraCardWidget::buildUi()
{
    nameLabel  = new QLabel(this);
    // nameLabel->setStyleSheet("color:#aaaaaa; font-weight:bold;");
    nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    nameLabel->setWordWrap(false);
    nameLabel->setTextInteractionFlags(Qt::NoTextInteraction);
    // nameLabel->setObjectName("titleLabel");
    typeLabel  = new QLabel(this);
    // typeLabel->setStyleSheet("color:#aaaaaa; font-size:11px;");
    modelLabel = new QLabel(this);
    // modelLabel->setStyleSheet("color:#aaaaaa; font-size:11px;");

    typeLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    modelLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    typeLabel->setAlignment(Qt::AlignCenter);
    modelLabel->setAlignment(Qt::AlignCenter);

    typeLabel->setContentsMargins(2, 2, 2, 2);
    modelLabel->setContentsMargins(2, 2, 2, 2);





    connectBtn = new QPushButton(this);
    editBtn    = new QPushButton("Edit", this);
    deleteBtn  = new QPushButton("Delete", this);
    for (auto *b : {connectBtn, editBtn, deleteBtn}) {
        b->setFixedHeight(26);
        b->setCursor(Qt::PointingHandCursor);
    }
    connectBtn->setStyleSheet(
        "QPushButton{background:#2d7d46;color:white;border-radius:6px;}"
        "QPushButton:hover{background:#369b58;}"
    );

    editBtn->setStyleSheet(
        "QPushButton{background:#3a3a3a;color:white;border-radius:6px;}"
        "QPushButton:hover{background:#4a4a4a;}"
    );

    deleteBtn->setStyleSheet(
        "QPushButton{background:#7d2d2d;color:white;border-radius:6px;}"
        "QPushButton:hover{background:#9b3636;}"
    );


    auto *topRow = new QHBoxLayout;
    topRow->addWidget(nameLabel);
    topRow->addStretch();
    topRow->addWidget(typeLabel);
    topRow->addWidget(modelLabel);

    auto *btnRow = new QHBoxLayout;
    btnRow->addWidget(connectBtn);
    btnRow->addWidget(editBtn);
    btnRow->addWidget(deleteBtn);



    auto *root = new QVBoxLayout(this);

    root->addLayout(topRow);
    root->addLayout(btnRow);

    connect(connectBtn, &QPushButton::clicked,
            this, &CameraCardWidget::onConnectClicked);

    connect(editBtn, &QPushButton::clicked,
            this, &CameraCardWidget::onEditClicked);

    connect(deleteBtn, &QPushButton::clicked,
            this, &CameraCardWidget::onDeleteClicked);
}
void CameraCardWidget::refresh()
{
    if (!m_camera)
        return;

    // Text updates
    nameLabel->setText(m_camera->name);
    typeLabel->setText(m_camera->cameraType);
    modelLabel->setText(cameraModelToString(m_camera->model));
    m_camera->connected = false;


    // Button text
    connectBtn->setText(
        m_camera->connected ? "Disconnect" : "Connect"
        );


    updateGeometry();   // 👈 allow auto-resize
}
void CameraCardWidget::updateState()
{
    nameLabel->setText(m_camera->name);
    typeLabel->setText(m_camera->cameraType.toUpper());
    modelLabel->setText(cameraModelToString(m_camera->model).toUpper());

    connectBtn->setText(
        m_camera->connected ? "Disconnect" : "Connect"
    );
    if(m_camera->connected){


         connectBtn->setStyleSheet(
            "QPushButton{background:#a86c2d;color:white;border-radius:6px;}"
            "QPushButton:hover{background:#c67e35;}"
        );
    }
    else{
         connectBtn->setStyleSheet(
            "QPushButton{background:#2d7d46;color:white;border-radius:6px;}"
            "QPushButton:hover{background:#369b58;}"
        );

    }
}

void CameraCardWidget::onConnectClicked()
{
    if (m_camera->connected)
        emit disconnectRequested(m_camera);
    else
        emit connectRequested(m_camera);
}

void CameraCardWidget::onEditClicked()
{
    emit editRequested(m_camera);
}

void CameraCardWidget::onDeleteClicked()
{
    emit deleteRequested(m_camera);
}
