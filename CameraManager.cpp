#include "CameraManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

#define CAMERA_FILE "cameras.json"

CameraManager::CameraManager(QObject* parent)
    : QObject(parent)
{
}

void CameraManager::add(CameraInstance* cam) {
    cameras.append(cam);
    saveToFile();
}

void CameraManager::remove(CameraInstance* cam) {
    cameras.removeOne(cam);

    saveToFile();
}

void CameraManager::saveToFile() {
    QJsonArray arr;

    for (auto* cam : cameras)
        arr.append(cam->toJson());

    QFile file(CAMERA_FILE);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(arr).toJson());
    }
}

void CameraManager::loadFromFile() {
    QFile file(CAMERA_FILE);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonArray arr = QJsonDocument::fromJson(file.readAll()).array();

    for (auto v : arr) {
        cameras.append(CameraInstance::fromJson(v.toObject()));
    }
}
