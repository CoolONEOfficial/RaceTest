#ifndef CAR_H
#define CAR_H

#include "wheel.h"
#include <QWidget>
#include <QTimer>
#include <QVector>

class Widget;

class Car : public QWidget
{
    Q_OBJECT
public:
    explicit Car(QWidget *parent = 0);

    // Widget
    Widget *w;

    // Coords
    float x, y;

    // Angles
    float angle, whellsAngle;

    // Speed
    float speed;
    float speedForwardMax;
    float speedBackMax;
    float speedScale();

    // Wheels
    QVector<Wheel*> wheelsStatic, wheelsManual;

    // Boost
    float boostForward;
    float boostBack;

    // Image
    QImage image;
    int carWidth();
    int carHeight();
    int biggestSide();

    // Timer Move Coords
    int timerMoveId;
    int timerRotateId;

    // Bools
    bool keyRight;
    bool keyLeft;
    bool keyUp;
    bool keyDown;

    // From Local to Global Coords
    QPointF toGlobalCoords(QPointF localCoords);

    // Touch line to car
    bool touchLine(QLineF line, QPointF &touchPoint);

signals:

public slots:
    void setWidget(Widget *widget);
    void draw(QPainter &p);
    void drawWheels(QPainter &p, float carAngle);
    void drawCar(QPainter &p, float carAngle);
    void loadImage(QString name);
    void move();
    void rotate(float rotateAngle);
    void rotateWheels(float rotateAngle);
    void setAngle(float newAngle);
    void setWheelsAngle(float newAngle);
    void correctAngle(float &val);
    void correctCoords(float &val);
    void correctVal(float min, float max, float &val);
    void limitVal(float min, float max, float &val);
    void keysEvent();
    void keyRightEvent();
    void keyLeftEvent();
    void keyUpEvent();
    void keyDownEvent();
    void timerEvent(QTimerEvent *);
    void addWheelStatic(Wheel *wheelStatic);
    void addWheelManual(Wheel *wheelManual);
    void startDrift();
};

#endif // CAR_H
