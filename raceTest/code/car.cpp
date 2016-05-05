#include "car.h"
#include "widget.h"
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
    boost = 1;
    backBoost = 0.5;

    keyLeft = false;
    keyRight = false;
    keyUp = false;
    keyDown = false;

    x = width()/2;
    y = height()/2;

    timerMoveId = startTimer(15);
}

float Car::speedScale()
{
    float speedScaleOut = fabs(speed) / maxSpeed * 1.5;

    return speedScaleOut;
}

void Car::draw(QPainter &p)
{
    float wheelsCarAngle = whellsAngle * 180 / M_PI * 6;
    if(speed<0)
        wheelsCarAngle = -wheelsCarAngle;

    float carAngle = angle * 180 / M_PI;

    // Wheels

    // Rotate
    p.resetTransform();
    p.translate(x, y);
    p.rotate(carAngle);
    p.translate(image.width()/8-17, image.height()/8-4);
    p.rotate(wheelsCarAngle);

    // Draw
    p.drawRect(-10, -5, 20, 10);

    // UnRotate
    p.rotate(-wheelsCarAngle);
    p.translate(-(image.width()/8-17), -(image.height()/8-4));

    p.translate(image.width()/8-17, -(image.height()/8-6));
    p.rotate(wheelsCarAngle);

    // Draw
    p.drawRect(-10, -5, 20, 10);

    // UnRotate
    p.rotate(-wheelsCarAngle);
    p.translate(-(image.width()/8-17), (image.height()/8-6));
    p.rotate(-carAngle);
    p.translate(-x, -y);
    p.resetTransform();

    // Car

    // Rotate
    p.resetTransform();
    p.translate(x, y);
    p.rotate(carAngle);

    // Random
    int random = 0;
    if(!keyUp && !keyDown)
        random = (rand()%2)-1;

    // Draw
    p.drawImage(QRect(-(image.width()/8 + random), -(image.height()/8 + random), image.width()/4, image.height()/4), image);

    // UnRotate
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
    // Keys
    keysEvent();

    // Move
    float vX = speed * cos(angle);
    float vY = speed * sin(angle);

    x += vX;
    y += vY;

    correctVal(-image.width()/8, 1280+image.width()/8, x);
    correctVal(-image.height()/8, 1024+image.height()/8, y);

    // Rotate car
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

void Car::keyRightEvent()
{
    // Rotate Wheels Right
    rotateWheels(M_PI / 2000.0);
}

void Car::keyLeftEvent()
{
    // Rotate Wheels Left
    rotateWheels(-M_PI / 2000.0);
}

void Car::keyUpEvent()
{
    // Up Boost
    if(speed < maxSpeed)
        speed += boost*speedScale()+0.05;
}

void Car::keyDownEvent()
{
    // Down Boost
    if(speed > -fabs(maxBackSpeed))
        speed -= backBoost;
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

    // Limit
    float limitWheelsAngle = M_PI/4/15;

    limitVal(-limitWheelsAngle, limitWheelsAngle, whellsAngle);
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
    float correct = 2*M_PI;
    correctVal(-correct, correct, val);
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

void Car::limitVal(float min, float max, float &val)
{
    if(val > max)
    {
        val = max;
    }
    else if(val < min)
    {
        val = min;
    }
}

void Car::keysEvent()
{
    // Keys

    // Up
    if(keyUp)
        keyUpEvent();

    // Down
    if(keyDown)
        keyDownEvent();

    // Left
    if(keyLeft)
        keyLeftEvent();

    // Right
    if(keyRight)
        keyRightEvent();
}

void Car::timerEvent(QTimerEvent *event)
{
    // Move Timer
    if(event->timerId() == timerMoveId)
    {
        // Move Player Car
        move();
    }
}

