#ifndef BUTTON_H
#define BUTTON_H

#include <QObject>
#include "animation.h"
#include "animationcoords.h"

#include <QPropertyAnimation>
#include <QPainter>
#include <QMap>

class Widget;

class Button : public QObject
{
    Q_OBJECT
public:
    explicit Button(QObject *parent = 0);
    explicit Button(QObject *parent = 0, QColor bColor = Qt::blue, QString bText = "", int bWidth = 200, int bHeight = 100,
                    QPointF sCoords = QPointF(0,0), QPointF eCoords = QPointF(0,0), bool bVisible = true);

    // Widget
    Widget *widget;

    QColor color;

    // Shadow
    const int shadowScale = 20;

    // Animation
    AnimationCoords animation;
    int x(bool left = false);
    int y(bool up = false);
    int shadowX(bool left = false);
    int shadowY(bool up = false);

    // Buton Text
    QString text;

    // Images for states
    int width;
    int height;

    // Font
    QFont font;

    // States
    int state();

    // Visible
    bool visible;

    void setWidget(Widget *w);
    bool inside(int cursorX,int cursorY);
    void setText(QString newText);
    void setCoords(QPointF newCoords);
    void draw(QPainter &p);
    void setEndX(float endX);
    void setEndY(float endY);

signals:

public slots:
};

#endif // BUTTON_H
