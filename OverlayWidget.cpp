#include "OverlayWidget.h"
#include <QPainter>

OverlayWidget::OverlayWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);

    setAttribute(Qt::WA_NoSystemBackground);

    setAttribute(Qt::WA_TransparentForMouseEvents);

    setStyleSheet("background: transparent;");
}

void OverlayWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing);

    QRect box(20,20,170,45);

    p.fillRect(
        box,
        QColor(0,0,0,140)
    );

    p.setPen(Qt::green);

    QFont f;
    f.setPointSize(16);
    f.setBold(true);

    p.setFont(f);

    p.drawText(
        box.adjusted(10,0,0,0),
        Qt::AlignVCenter,
        lrfText
    );
}

void OverlayWidget::setLRF(const QString &txt)
{
    lrfText = txt;

    update();
}