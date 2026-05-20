#pragma once
#include <QString>
struct CameraDraft
{
    enum class Type {
        USB, RTSP, UDP
    };

    enum Model {
        C13,
        Generic,
        ViewPro,
        None

    };

    QString name;
    Type type = Type::RTSP;
    Model model = Model::None;

    int usbIndex =-1;
    // QString usbIndex;

      // ---------- Network ----------
    QString ip;                // user input
    QString transport = "udp"; // udp / tcp

    // ---------- Auto-generated ----------
    QString primaryUrl;
    bool secondaryEnabled = true;
    QString secondaryUrl;
    int udpPort = -1;

    // ---------- User override flags ----------
    bool primaryUrlEdited   = false;
    bool secondaryUrlEdited = false;
    bool portEdited         = false;

    QString primaryPipeline;
    QString secondaryPipeline;

    // ---------- Validation ----------
    bool valid = false;

};
