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
        speedForwardMax = 10;
        speedBackMax = 2;

        // Set Boost
        boostForward = 0.5;
        boostBack = 0.2;

    // Set bools
    keyLeft = false;
    keyRight = false;
    keyUp = false;
    keyDown = false;

    // Set X and Y
    x = 100;
    y = 100;

    // Start Move Timer
    timerMoveId = startTimer(15);
    timerRotateId = startTimer(100);
}

void Car::draw(QPainter &p)
{
    // Draw Wheels Angle
    float carAngle = angle * 180 / M_PI;

    // Wheels
    drawWheels(p, carAngle);

    // Car
    drawCar(p, carAngle);
}

void Car::drawWheels(QPainter &p, float carAngle)
{
    // Draw Wheels Angle
    float wheelsCarAngle = whellsAngle * 180 / M_PI * 6;

    if(speed < 0)
        wheelsCarAngle = -wheelsCarAngle;

    // Draw Tracks

    // Manual
    for(int f = 0; f<wheelsManual.size(); f++)
    {
        wheelsManual[f]->drawTracks(p);
    }

    // Static
    for(int f = 0; f<wheelsStatic.size(); f++)
    {
        wheelsStatic[f]->drawTracks(p);
    }

    // Draw Wheels

    // Rotate
    if(w != 0)
    {
        p.translate(x, y);
        p.rotate(carAngle);

        // Manual
        for(int f = 0; f<wheelsManual.size(); f++)
        {
            wheelsManual[f]->draw(p, wheelsCarAngle, true);
        }

        // Static
        for(int f = 0; f<wheelsStatic.size(); f++)
        {
            wheelsStatic[f]->draw(p, wheelsCarAngle, false);
        }

        // UnRotate
        p.rotate(-carAngle);
        p.translate(-(x), -(y));
    }
}

void Car::drawCar(QPainter &p, float carAngle)
{
    // Rotate
    if(w != 0)
    {
        p.translate(x, y);
        p.rotate(carAngle);

            // Random
            int random = 0;
            if(!keyUp && !keyDown)
                random = (rand()%3)-1;

            // Draw
            p.drawImage(QRect(-(carWidth()/2 + random), -(carHeight()/2 + random),
                              carWidth(), carHeight()), image);

        // UnRotate
        p.rotate(-carAngle);
        p.translate(-(x), -(y));
    }
}

void Car::move()
{
    // FPS
    w->timerFps++;

    // Keys
    keysEvent();

    // Move
    float vX = speed * cos(angle);
    float vY = speed * sin(angle);

    float oldX = x, oldY = y, oldAngle = angle;

    x += vX;
    y += vY;

    // Rotate car
    rotate(speedScale() * whellsAngle);

    // Drift
    if(keyDown)
        rotate(speedScale()/2 * whellsAngle);

    if( w != 0 )
    {
        // cMasks
        if(w->gMaps.size() > 0 && w->gMap >= 0 && w->gMap<w->gMaps.size())
        {
            for(int f = 0; f<w->gMaps[w->gMap]->cMasks.size(); f++)
            {
                QPointF touchPoint;
                if( touchCMask(w->gMaps[w->gMap]->cMasks[f], touchPoint) )
                {
                    // Back
                    x = oldX;
                    y = oldY;
                    angle = oldAngle;

                    speed *= -0.5;

                    // Drift
                    addTrackBranches();
                }
            }
        }

        if(!keyUp && !keyDown)
        {
            // Center
            w->screenCar.setEndY(w->height()/2);
            w->screenCar.start(true, false);
        }
        else if(keyUp)
        {
            // Car down
            w->screenCar.setEndY(w->height()/4*3);
            w->screenCar.start(true, false);
        }
        else if(speed < 0 && !keyUp)
        {
            // Car up
            w->screenCar.setEndY(w->height()/4);
            w->screenCar.start(true, false);
        }

        if(!keyLeft && !keyRight)
        {
            // Center
            w->screenCar.setEndX(w->width()/2);
            w->screenCar.start(true, false);
        }
        else if(keyLeft)
        {
            // Car right
            if(!drift())
            {
                w->screenCar.setDuration(1000);
            }
            w->screenCar.setEndX(w->width()/4*3);
            w->screenCar.start(true, false);
            w->screenCar.setDuration( w->screenCar.dDuration );
        }
        else if(keyRight)
        {
            // Car left
            if(!drift())
            {
                w->screenCar.setDuration(1000);
            }
            w->screenCar.setEndX(w->width()/4);
            w->screenCar.start(true, false);
            w->screenCar.setDuration( w->screenCar.dDuration );
        }
    }

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

    // Move Camera
    if(w != 0)
    {
        w->cam.setX(x - w->screenCar.x);
        w->cam.setY(y - w->screenCar.y);
    }
}

void Car::keyRightEvent()
{
    // Key Right

    // Rotate Wheels Right
    float rotate = M_PI / 600;

    if(speed>0)
        rotateWheels(rotate);
    else
        rotateWheels(-rotate);
}

void Car::keyLeftEvent()
{
    // Key Left

    // Rotate Wheels Left
    float rotate = M_PI / 600;

    if(speed>0)
        rotateWheels(-rotate);
    else
        rotateWheels(rotate);
}

void Car::keyUpEvent()
{
    // Key Up

    // Up Boost

    // Limit
    if(speed < speedForwardMax)
    {
        if(speed < 0)
        {
            // Add tracks
            addTracks();
        }

        speed += boostForward*speedScale()+0.01;
    }
}

void Car::keyDownEvent()
{
    // Key Down

    // Down boost

    // Limit
    if(speed > -fabs(speedBackMax))
    {

        // Drift
        if(speed > 0)
        {
            // Add Tracks
            addTracks();
        }

        // Back Boost
        speed -= boostBack*speedScale()+0.01;
    }
}

void Car::rotate(float rotateAngle)
{
    // Rotate
    angle += rotateAngle;

    // Correct
    correctAngle(angle);
}

void Car::rotateWheels(float rotateAngle)
{
    // Correct
//    correctAngle(rotateAngle);

    // Rotate
    whellsAngle += rotateAngle;

    // Correct
    correctAngle(whellsAngle);

    // Limit
    float limitWheelsAngle = M_PI / 4.0 / 15.0;

    float limitScale = 0.6;

    limitVal(-limitWheelsAngle * ( 1-speedScale()* limitScale ),
             limitWheelsAngle * ( 1-speedScale()* limitScale ),
             whellsAngle);
}

void Car::setAngle(float newAngle)
{
    Q_ASSERT("function doesn't work!" && 0);

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

    // Down
    if(keyDown)
        keyDownEvent();

    // Up
    if(keyUp && !keyDown)
        keyUpEvent();

    // Left
    if(keyLeft)
        keyLeftEvent();

    // Right
    if(keyRight)
        keyRightEvent();
}

void Car::loadImage(QString name)
{
    // Load image
    if(w != 0)
    {
        w->loadImage(image, name);
    }
}

void Car::timerEvent(QTimerEvent *event)
{
    // Move Timer
    if(event->timerId() == timerMoveId)
    {
        // Move Player Car
        move();
    }

    // Rotate Timer
    if(event->timerId() == timerRotateId)
    {
        // Rotate wheel to default angle
        if(angle > 0)
        {
            angle -= 0.01;
        }
        else if(angle < 0)
        {
            angle += 0.01;
        }
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

bool Car::touchPoly(QPolygonF poly, QPointF &touchPoint)
{
    // Touch polygon

    int touch = false;

    if(w != 0)
    {
        for(int f = 0; f<poly.size()-1; f++)
        {
            // Set Touch if true
            if(touchLine( QLineF( poly[f].x(), poly[f].y(),
                       poly[f+1].x(), poly[f+1].y() ), touchPoint ))
                touch = true;
        }
    }

    return touch;
}

bool Car::touchCMask(CMask *cMask, QPointF &touchPoint)
{
    // Touch CMask
    QPolygon gPoly;

    if(!cMask->solid)
        return false;

    for(int f = 0; f<cMask->poly.size(); f++)
    {
        gPoly.push_back(QPoint(cMask->poly[f].x() + cMask->x,
                               cMask->poly[f].y() + cMask->y));
    }

    return touchPoly(gPoly, touchPoint);
}

bool Car::drift()
{
    if(speed > 0 && keyDown)
        return true;
    else
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
    // Speed Scale
    float speedScaleOut = fabs(speed) / speedForwardMax;

    limitVal(0, 1.0, speedScaleOut);

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

void Car::addWheelManual(Wheel *wheelManual)
{
    // Add Manual Wheels
    wheelsManual.push_back(wheelManual);
    wheelsManual.push_back(new Wheel(wheelManual->getWidget(), wheelManual->x, -wheelManual->y, 20, 10));
}

void Car::addWheelStatic(Wheel *wheelStatic)
{
    // Add Static Wheels
    wheelsStatic.push_back(wheelStatic);
    wheelsStatic.push_back(new Wheel(wheelStatic->getWidget(), wheelStatic->x, -wheelStatic->y, 20, 10));
}

void Car::addTrackBranches()
{
    // Add Branches

    // Static
    for(int f = 0; f<wheelsStatic.size(); f++)
    {
        wheelsStatic[f]->addTrackBranch();
    }

    // Manual
    for(int f = 0; f<wheelsManual.size(); f++)
    {
        wheelsManual[f]->addTrackBranch();
    }
}

void Car::addTracks()
{
    // Add Tracks

    // Manual
    for(int f = 0; f<wheelsManual.size(); f++)
    {
        wheelsManual[f]->addTrack( toGlobalCoords( QPointF(wheelsManual[f]->x*2, wheelsManual[f]->y*2) ) );
    }

    // Static
    for(int f = 0; f<wheelsStatic.size(); f++)
    {
        wheelsStatic[f]->addTrack( toGlobalCoords( QPointF(wheelsStatic[f]->x*2, wheelsStatic[f]->y*2) ) );
    }
}
