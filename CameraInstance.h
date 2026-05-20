#pragma once
#include <QString>
#include "SkydroidDriver.h"   // FULL include here
#include "QJsonObject"

struct _GstElement;
typedef struct _GstElement GstElement;


class VideoWidget;

class SkydroidDriver;



// ===== ENUMS (ADD HERE) =====

enum class CameraModel {
        GENERIC,
        SKYDROID,
        VIEWPRO,
};

enum class ControlCommand
{
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    STOP,
    CENTER,
    LOOK_FORWARD,
    LOOK_DOWN,
    ZOOM_IN,
    ZOOM_OUT,
    TRACK_STOP,
    REC,
    RECStop,
    OSDON,
    OSDOFF,
    TRACK,
    PHOTO,
    TRACK_MODE,
    UNTRACK_MODE,
    SET_PALETTE

};

// ===== STRING ↔ ENUM HELPERS =====
inline CameraModel cameraModelFromString(const QString& text)
{
    if (text.contains("Skydroid", Qt::CaseInsensitive))
        return CameraModel::SKYDROID;
    if (text.contains("View Pro", Qt::CaseInsensitive))
        return CameraModel::VIEWPRO;

    return CameraModel::GENERIC;
}

inline QString cameraModelToString(CameraModel model)
{
    switch (model)
    {
    case CameraModel::SKYDROID: return "Skydroid C13";
    case CameraModel::VIEWPRO: return "View Pro";
    default: return "Generic";
    }
}


struct CameraInstance {
    QString name;
    QString cameraType;
    CameraModel  model =  CameraModel::GENERIC;
    
    QString primaryUrl;
    bool secondaryEnabled = false;
    QString secondaryUrl;
    QString ip;
    int port;
    QString transport;
    bool pipelineBackground = false;
    GstElement* GSTpipeline = nullptr;
    GstElement* sink     = nullptr;

    
    
    QString pipeline;
    QString secondaryPipeline; 
    VideoWidget* widget = nullptr;
    bool connected = false;

    SkydroidDriver* skydroid = nullptr;

    void initDriver(QString ip, int port)
    {
        cleanupDriver();

        if (model == CameraModel::SKYDROID) {
            skydroid = new SkydroidDriver(ip, port);
        } else {
            skydroid = nullptr;
        }
    }
    void swapStreams()
    {
        if (!secondaryEnabled)
            return;

        qSwap(primaryUrl, secondaryUrl);
        qSwap(pipeline, secondaryPipeline);
    }
    
    void cleanupDriver()
    {
        if (skydroid) {
            delete skydroid;
            skydroid = nullptr;
        }
    }

    // 🔽 SAVE
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["type"] = cameraType;
        obj["model"] = (int)model;
        obj["primaryUrl"] = primaryUrl;
        obj["ip"] = ip;
        obj["port"] = port;
        obj["transport"] = transport;
        obj["secondaryEnabled"] = secondaryEnabled;
        obj["secondaryUrl"] = secondaryUrl;
        obj["pipeline"] = pipeline;
        obj["secondaryPipeline"] = secondaryPipeline;
        return obj;
    }

    static CameraInstance* fromJson(const QJsonObject& obj) {
        CameraInstance* cam = new CameraInstance;

        cam->name = obj["name"].toString();
        cam->cameraType = obj["type"].toString();
        cam->model = (CameraModel)obj["model"].toInt();
        cam->primaryUrl = obj["primaryUrl"].toString();
        cam->ip = obj["ip"].toString();
        cam->port = obj["port"].toInt();
        cam->transport = obj["transport"].toString();
        cam->secondaryEnabled = obj["secondaryEnabled"].toBool();
        cam->secondaryUrl = obj["secondaryUrl"].toString();
        cam->pipeline = obj["pipeline"].toString();
        cam->secondaryPipeline = obj["secondaryPipeline"].toString();

        return cam;
    }

    // ===== CONTROL FUNCTION =====
    void control(ControlCommand cmd,int x,int y)
    {
        

        if (model != CameraModel::SKYDROID || !skydroid)
            return;

        const int SPEED = 50;

        switch (cmd)
        {
        case ControlCommand::MOVE_UP:
            skydroid->setGimbalSpeed(0, SPEED);
            break;

        case ControlCommand::MOVE_DOWN:
            skydroid->setGimbalSpeed(0, -SPEED);
            break;

        case ControlCommand::MOVE_LEFT:
            skydroid->setGimbalSpeed(-SPEED, 0);
            break;

        case ControlCommand::MOVE_RIGHT:
            skydroid->setGimbalSpeed(SPEED, 0);
            break;

        case ControlCommand::STOP:
            skydroid->setGimbalSpeed(0, 0);
            break;

        case ControlCommand::CENTER:
            skydroid->centerGimbal();
            break;

        case ControlCommand::LOOK_FORWARD:
            skydroid->lookForward();
            break;

        case ControlCommand::LOOK_DOWN:
            skydroid->lookDown();
            break;

        case ControlCommand::ZOOM_IN:
            skydroid->zoomIn();
            break;

        case ControlCommand::ZOOM_OUT:
            skydroid->zoomOut();
            break;

        case ControlCommand::TRACK_STOP:
            skydroid->stopTracking();
            break;
        case ControlCommand::REC:
            skydroid->Startrecord();
            break;
        case ControlCommand::RECStop:
            skydroid->Stoprecord();
            break;
        case ControlCommand::OSDON:
            skydroid->OSDEnable();
            break;
        case ControlCommand::OSDOFF:
            skydroid->OSDDisable();
            break;
        case ControlCommand::PHOTO:
            skydroid->takephoto();
            break;
        case ControlCommand::TRACK:
            skydroid->sendTrackingBinary(x-30,y-30,x+30,y+30);
            break;
        case ControlCommand::TRACK_MODE:
            skydroid->setTrackingState(true);
            break;

        case ControlCommand::UNTRACK_MODE:
            skydroid->stopTracking();
            break;
        case ControlCommand::SET_PALETTE:
            skydroid->setPalette(x); // using x as palette index
            break;

        default:
            break;
        }
    }



};
