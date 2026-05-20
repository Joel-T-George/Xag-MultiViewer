#pragma once

#include <QWidget>

class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OverlayWidget(QWidget* parent=nullptr);

    void setLRF(const QString& txt);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QString lrfText = "LRF 000";
};