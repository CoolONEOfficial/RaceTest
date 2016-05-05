#ifndef CAR_H
#define CAR_H

#include <QWidget>
#include <QTimer>

class Car : public QWidget
{
    Q_OBJECT
public:
    explicit Car(QWidget *parent = 0);

    // Coords
    float x, y;

    // Angles
    float angle, whellsAngle;

    // Speed
    float speed;
    float maxSpeed;
    float maxBackSpeed;
    float speedScale();

    // Boost
    float boost;
    float backBoost;

    // Image
    QImage image;

    // Timer Move Coords
    int timerMoveId;

    // Bools
    bool keyRight;
    bool keyLeft;
    bool keyUp;
    bool keyDown;

signals:

public slots:
    void draw(QPainter &p);
    void loadImage(QString name);
    void move();
    void rotate(float rotateAngle);
    void rotateWheels(float rotateAngle);
    void setAngle(float newAngle);
    void setWheelsAngle(float newAngle);
    void correctAngle(float &val);
    void correctVal(float min, float max, float &val);
    void limitVal(float min, float max, float &val);
    void keysEvent();
    void keyRightEvent();
    void keyLeftEvent();
    void keyUpEvent();
    void keyDownEvent();
    void timerEvent(QTimerEvent *);
};

#endif // CAR_H
