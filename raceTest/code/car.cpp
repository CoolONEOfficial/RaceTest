#include "car.h"
#include "widget.h"
#include <QPainter>
#include <math.h>
#include <QTimerEvent>
#include <QApplication>
#include <fstream>
#include <QPoint>
#include <QVector>

Car::Car(QWidget *parent) : QWidget(parent)
{
    // Get Width/Height
    w = dynamic_cast<Widget *>(parent);

    // Set Angles
    angle = 0;
    whellsAngle = 0;

    // Attributes

        // Set Speed
        speed = 0;
        maxSpeed = 10;
        maxBackSpeed = 2;

        // Set Boost
        boost = 0.5;
        backBoost = 0.2;

        sensitive = 0;

    // Set bools keys
    keyLeft = false;
    keyRight = false;
    keyUp = false;
    keyDown = false;

    // Set X and Y
    x = 0;
    y = 0;

//    std::ofstream file( "C:\\User\\Trukhin\\data.dat" );
//    int value = 255;

//    file.write( (char*)(&value), 4 );

//    file.close();

    // Start Move Timer
    timerMoveId = startTimer(15);
}

void Car::draw(QPainter &p)
{
    // Draw Wheels Angle
    float carAngle = angle * 180 / M_PI;

    // ------------------------------------ Track ------------------------------------
    drawTrack(p);

    // ------------------------------------ Wheels ------------------------------------
    drawWheels(p, carAngle);

    // ------------------------------------ Car ------------------------------------
    drawCar(p, carAngle);
}

void Car::drawTrack(QPainter &p)
{
    for(int wheels = 0; wheels<staticWheels.size(); wheels++)
    {
        for(int f = 0; f<staticWheels[wheels]->trackCoords.size(); f++)
        {
            // Rotate
            p.resetTransform();
            p.translate(staticWheels[wheels]->trackCoords[f]->x, staticWheels[wheels]->trackCoords[f]->y);
            p.rotate(staticWheels[wheels]->trackCoords[f]->angle);

                // Draw
                p.setBrush(Qt::SolidPattern);
                p.drawRoundRect(0,
                                 0,
                                 staticWheels[wheels]->height,
                                 staticWheels[wheels]->height,
                                25,25);

            // Unrotate
            p.translate(-(staticWheels[wheels]->trackCoords[f]->x), -(staticWheels[wheels]->trackCoords[f]->y));
            p.rotate(-(staticWheels[wheels]->trackCoords[f]->angle));
            p.resetTransform();
        }
    }
}

void Car::drawWheels(QPainter &p, float carAngle)
{
    // Draw Wheels Angle
    float wheelsCarAngle = whellsAngle * 180 / M_PI * 6;

    if(speed<0)
        wheelsCarAngle = -wheelsCarAngle;

    // Rotate
    p.resetTransform();
    p.translate(x, y);
    p.rotate(carAngle);

        // Manual
        for(int f = 0; f<manualWheels.size(); f++)
        {
            if(manualWheels[f]->x > carWidth()/2)
                manualWheels[f]->angle = -wheelsCarAngle;
            else
                manualWheels[f]->angle = wheelsCarAngle;
            manualWheels[f]->draw(p);
        }

        // Static
        for(int f = 0; f<staticWheels.size(); f++)
        {
            staticWheels[f]->draw(p);
        }

    // UnRotate
    p.rotate(-carAngle);
    p.translate(-x, -y);
    p.resetTransform();
}

void Car::drawCar(QPainter &p, float carAngle)
{
    // Rotate
    p.resetTransform();
    p.translate(x, y);
    p.rotate(carAngle);

        // Random
        int random = 0;
        if(!keyUp && !keyDown)
            random = (rand()%2)-1;

        // Draw
        p.drawImage(QRect(-(carWidth()/2 + random), -(carHeight()/2 + random),
                          carWidth(), carHeight()), image);

    // UnRotate
    p.rotate(-carAngle);
    p.translate(-x, -y);
    p.resetTransform();
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

    int sx = 1280;
    int sy = 1024;

    if( w != 0 )
    {
        sx = w->width();
        sy = w->height();
    }

    // if player lost then teleport Player
    correctVal(-biggestSide(), sx+biggestSide(), x);
    correctVal(-biggestSide(), sy+biggestSide(), y);

    // Rotate car
    rotate(speedScale() * whellsAngle);

    // Move whellsAngle to 0
    whellsAngle *= 0.95;

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
    if(speed>0)
        rotateWheels(M_PI / ((100-sensitive+1)*25));
    else
        rotateWheels(-(M_PI / ((100-sensitive+1)*25)));
}

void Car::keyLeftEvent()
{
    // Rotate Wheels Left
    if(speed>0)
        rotateWheels(-(M_PI / ((100-sensitive+1)*25)));
    else
        rotateWheels(M_PI / ((100-sensitive+1)*25));
}

void Car::keyUpEvent()
{
    // Up Boost
    if(speed < maxSpeed)
        speed += boost*speedScale()+0.01;
}

void Car::keyDownEvent()
{
    // Down Boost
    if(speed > -fabs(maxBackSpeed))
    {
        if(speed > 0)
        {
            for(int f = 0; f<staticWheels.size(); f++)
            {
                staticWheels[f]->addTrack(toGlobalCoords(QPointF(staticWheels[f]->x, staticWheels[f]->y)).x(),
                                          toGlobalCoords(QPointF(staticWheels[f]->x, staticWheels[f]->y)).y(), whellsAngle);
            }
        }

        speed -= backBoost*speedScale()+0.01;
    }
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
    float limitWheelsAngle = M_PI / 4 / 15;

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

void Car::correctCoords(float &val)
{
    // Correct Angle ( 0 to 2PI )
    float correct = biggestSide();
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

void Car::loadImage(QString name)
{
    image.load(QApplication::applicationDirPath()+"\\images\\"+name);
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

bool Car::touchLine(QLineF line, QPointF &pointTouch)
{
    QPointF points[4] = { QPointF(- carWidth()/2,- carHeight()/2),
                          QPointF(  carWidth()/2,- carHeight()/2),
                          QPointF(  carWidth()/2,  carHeight()/2),
                          QPointF(- carWidth()/2,  carHeight()/2) };

    for(int f = 0; f < 4; f++)
    {
        points[f] = toGlobalCoords(points[f]);
    }

    for(int f = 0; f < 4; f++)
    {
        QLineF edge(points[f], points[(f+1) % 4]);
        if(QLineF::BoundedIntersection == line.intersect(edge, &pointTouch))
        {
            return true;
        }
    }

    return false;
}

void Car::setWidget(Widget *widget)
{
    w = widget;
}

QPointF Car::toGlobalCoords(QPointF localCoords)
{
    // Convert coords (local to global)

    QPoint rotateCoords;
    rotateCoords.setX(localCoords.x() * cos(angle) - localCoords.y() * sin(angle));
    rotateCoords.setY(localCoords.x() * sin(angle) + localCoords.y() * cos(angle));

    QPoint globalCoords;
    globalCoords.setX(rotateCoords.x() + x);
    globalCoords.setY(rotateCoords.y() + y);

    return globalCoords;
}

float Car::speedScale()
{
    float speedScaleOut = fabs(speed) / maxSpeed * 1.5;

    return speedScaleOut;
}

int Car::carWidth()
{
    return image.width()/2;
}

int Car::carHeight()
{
    return image.height()/2;
}

int Car::biggestSide()
{
    if(carWidth() > carHeight())
        return carWidth();
    else
        return carHeight();
}

void Car::addWheelManual(int x, int y, int width, int height)
{
    // Add
    Wheel *manualWheel1 = new Wheel(this, true, 0, x, y, width, height);
    Wheel *manualWheel2 = new Wheel(this, true, 0, x, -y, width, height);

    // Push
    manualWheels.push_back(manualWheel1);
    manualWheels.push_back(manualWheel2);

}

void Car::addWheelStatic(int x, int y, int width, int height)
{
    // Add
    Wheel *staticWheel1 = new Wheel(this, false, 0, x, y, width, height);
    Wheel *staticWheel2 = new Wheel(this, false, 0, x, -y, width, height);

    // Push
    staticWheels.push_back(staticWheel1);
    staticWheels.push_back(staticWheel2);
}
