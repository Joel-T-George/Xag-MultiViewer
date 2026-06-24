#pragma once
#include "CameraInstance.h"
#include <QVector>
#include <QObject>

class CameraManager: public QObject {

    Q_OBJECT

public:

    explicit CameraManager(QObject* parent = nullptr);
    QVector<CameraInstance*> cameras;

    void add(CameraInstance* cam);
    void remove(CameraInstance* cam);


    void saveToFile();
    void loadFromFile();
};
