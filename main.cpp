#include <QApplication>
#include <QCoreApplication>
#include <gst/gst.h>
#include "MainWindow.h"
#include <QFile>


static void initGStreamer(const QString& appDir)
{
        QString gstRoot = "C:/gstreamer/1.0/msvc_x86_64";


        qputenv("PATH", (gstRoot + "/bin;" + qgetenv("PATH")).toLocal8Bit());
        qputenv("GST_PLUGIN_PATH", (gstRoot + "/lib/gstreamer-1.0").toLocal8Bit());
        qputenv("GST_PLUGIN_SYSTEM_PATH", (gstRoot + "/lib/gstreamer-1.0").toLocal8Bit());
        qputenv("GST_REGISTRY",
        ( appDir + "/gst-registry.bin").toLocal8Bit());
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Xag Multiviewer");


        app.setStyleSheet(R"(

/* ================= GLOBAL ================= */
QWidget, QMainWindow  {
    background-color: #181818;
    color: #e0e0e0;
    font-family: "Segoe UI", "Arial";
    font-size: 12px;
}

/* ================= LABEL ================= */
QLabel {
    background-color: transparent;
    color: #cccccc;
}

/* Optional: title labels */
QLabel#titleLabel {
    font-size: 14px;
    font-weight: bold;
    color: #ffffff;
}

/* ================= BUTTON ================= */
QPushButton {
    background-color: #2c2c2c;
    border: 1px solid #444;
    border-radius: 4px;
    padding: 6px;
    color: #e0e0e0;
}

QPushButton:hover {
    background-color: #3a3a3a;
    border: 1px solid #666;
}

QPushButton:pressed {
    background-color: #505050;
}

QPushButton:disabled {
    background-color: #1a1a1a;
    color: #666;
    border: 1px solid #333;
}

/* ================= INPUT ================= */
QLineEdit, QTextEdit {
    background-color: #202020;
    border: 1px solid #444;
    padding: 4px;
    border-radius: 3px;
}

QLineEdit:focus, QTextEdit:focus {
    border: 1px solid #3d6df2;
}

/* ================= LIST ================= */
QListWidget {
    background-color: #202020;
    border: 1px solid #333;
}

QListWidget::item {
    padding: 4px;
}

QListWidget::item:selected {
    background-color: #3d6df2;
    color: white;
}

/* ================= SCROLLBAR ================= */
QScrollBar:vertical {
    background: #1c1c1c;
    width: 10px;
    margin: 0;
}

QScrollBar::handle:vertical {
    background: #444;
    min-height: 20px;
    border-radius: 4px;
}

QScrollBar::handle:vertical:hover {
    background: #666;
}

QScrollBar::add-line,
QScrollBar::sub-line {
    height: 0;
}

/* ================= COMBOBOX ================= */
QComboBox {
    background-color: #2c2c2c;
    border: 1px solid #444;
    padding: 4px;
}

QComboBox:hover {
    border: 1px solid #666;
}

/* ================= GROUP BOX ================= */
QGroupBox {
    border: 1px solid #333;
    margin-top: 10px;
}

QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 5px;
}

/* ================= TOOLTIP ================= */
QToolTip {
    background-color: #2a2a2a;
    color: #ffffff;
    border: 1px solid #444;
}

/* ================= SELECTED CAMERA (YOUR USE CASE) ================= */
QWidget[selected="true"] {
    border: 2px solid #00ffcc;
    background-color: #202020;
}

/* ================= SIDEBAR ================= */
QWidget#sidebar {
    background-color: #1b1b1b;
    border-left: 1px solid #333;
}

/* ================= VIDEO PANEL ================= */
QWidget#videoPanel {
    background-color: black;
    border: 1px solid #333;
}

/* ===== TAB WIDGET PANE ===== */
QTabWidget::pane {
    border: 1px solid #333;
    background: #1c1c1c;
    top: -1px;
}

/* ===== TAB BAR BASE ===== */
QTabBar {
    background: #181818;
}

/* ===== NORMAL TAB ===== */
QTabBar::tab {
    background: #2c2c2c;
    color: #cccccc;
    padding: 6px 12px;
    border: 1px solid #333;
    border-bottom: none;
    margin-right: 2px;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
}

/* ===== HOVER TAB ===== */
QTabBar::tab:hover {
    background: #3a3a3a;
    color: #ffffff;
}

/* ===== SELECTED TAB ===== */
QTabBar::tab:selected {
    background: #1c1c1c;
    color: #ffffff;
    border: 1px solid #555;
    border-bottom: 1px solid #1c1c1c; /* merge with pane */
}

/* ===== INACTIVE TAB ===== */
QTabBar::tab:!selected {
    margin-top: 2px; /* gives depth effect */
}

/* ===== DISABLED TAB ===== */
QTabBar::tab:disabled {
    color: #666;
    background: #1a1a1a;
}

)");

    const QString appDir = QCoreApplication::applicationDirPath();
        initGStreamer(appDir);
        gst_init(&argc, &argv);

        QIcon appIcon(":/resources/icons/xag.ico");
        // QApplication app(argc, argv);
        MainWindow w;

        w.setWindowTitle("Xag Multiviewer v1.0");
        w.setWindowIcon(appIcon);
        w.show();
        return app.exec();
}
