#include "car.h"
#include <QPainter>
#include <math.h>
#include <QTimerEvent>
#include <QApplication>
Car::Car(QWidget *parent) : QWidget(parent)
{
    angle = 0;
    whellsAngle = 0;
    speed = 0;
    maxSpeed = 20;
    maxBackSpeed = 2;
    boost = 1.5;
    backBoost = 0.5;

    x = width()/2;
    y = height()/2;

    timerMoveId = startTimer(15);
}

float Car::speedScale()
{
    float speedScaleOut = fabs(speed) / maxSpeed * 4;

    return speedScaleOut;
}

void Car::draw(QPainter &p)
{
    p.resetTransform();

    float carAngle = angle * 180 / M_PI;

    p.translate(x, y);
    p.rotate(carAngle);

    p.drawImage(QRect(-image.width()/8, -image.height()/8, image.width()/4, image.height()/4), image);

    p.rotate(-carAngle);
    p.translate(-x, -y);

    p.resetTransform();
}

void Car::loadImage(QString name)
{
    image.load(QApplication::applicationDirPath()+"\\images\\"+name);
}

void Car::move()
{
    // Move
    float vX = speed * cos(angle);
    float vY = speed * sin(angle);

    x += vX;
    y += vY;

    rotate(speedScale() * whellsAngle);

    // Move whellsAngle to 0
    whellsAngle *= 0.995;

    if(whellsAngle < 1e-3 &&
       whellsAngle > -1e-3)
        whellsAngle = 0;

    // Move speed to 0
    speed *= 0.995;

    if(speed < 1e-3 &&
       speed > -1e-3)
        speed = 0;
}

void Car::rotate(float rotateAngle)
{
    // Correct
    correctAngle(rotateAngle);

    // Rotate
    angle += rotateAngle;

    // Correct
    correctAngle(angle);
}

void Car::rotateWheels(float rotateAngle)
{
    // Correct
    correctAngle(rotateAngle);

    if((rotateAngle > 0 && whellsAngle < 0) ||
       (rotateAngle < 0 && whellsAngle > 0))
        whellsAngle = 0;

    // Rotate
    whellsAngle += rotateAngle;

    // Correct
    correctAngle(whellsAngle);

    //
    float maxWheelsAngle = M_PI/4/15;
    float minWheelsAngle = -(M_PI/4/15);

    if(whellsAngle > maxWheelsAngle)
    {
        whellsAngle = maxWheelsAngle;
    }
    else if(whellsAngle < minWheelsAngle)
    {
        whellsAngle = minWheelsAngle;
    }
}

void Car::setAngle(float newAngle)
{
    // Correct
    correctAngle(newAngle);

    // Set
    angle = newAngle;
}

void Car::setWheelsAngle(float newAngle)
{
    // Correct
    correctAngle(newAngle);

    // Set
    whellsAngle = newAngle;
}

void Car::correctAngle(float &val)
{
    // Correct Angle ( 0 to 2PI )
    float maxmin = 2*M_PI;
    correctVal(-maxmin, maxmin, val);
}

void Car::correctVal(float min, float max, float &val)
{
    // Correct

    float delta = max-min;

    while (val < min)
    {
        val += delta;
    }

    while (val > max)
    {
        val -= delta;
    }
}

void Car::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timerMoveId)
    {
        // Move Player Car
        move();
    }
}

