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

//    static const int memoryTrack = 50;

    // Coords
    float x, y;
    QVector<QPointF> trackCoords;

    // Angles
    float angle, whellsAngle;

    // Speed
    float speed;
    float maxSpeed;
    float maxBackSpeed;
    float speedScale();

    // Wheels
    float wheelsWidth;
    float wheelsHeight;
    QVector<Wheel*> staticWheels, manualWheels;

    // Sensitive
    int sensitive;

    // Boost
    float boost;
    float backBoost;

    // Image
    QImage image;
    int carWidth();
    int carHeight();
    int biggestSide();

    // Timer Move Coords
    int timerMoveId;

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
    void drawTrack(QPainter &p);
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
    void addWheelStatic(int x,int y, int width, int height);
    void addWheelManual(int x,int y, int width, int height);
};

#endif // CAR_H
