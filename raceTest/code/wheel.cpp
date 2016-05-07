#include "wheel.h"
#include "widget.h"
#include <QMessageBox>
#include <QPainter>
#include <QTimerEvent>

Wheel::Wheel(QObject *parent, bool wManual, float wAngle, float wX, float wY, float wW, float wH)
{
    // Set Widget
    w = dynamic_cast<Widget *>(parent);

    // Create
    for(int f = 0; f<trackCoords.size(); f++)
    {
        Track *track = new Track(this, -100, -100, 0);
        trackCoords[f] = track;
    }

    // Coords
    x = wX;
    y = wY;

    // Size
    width = wW;
    height = wH;

    // Set manual / Angle
    manual = wManual;
    angle = wAngle;

    timerTrackId = startTimer(80);
}

Wheel::Wheel(QObject *parent, bool wManual, float wX, float wY, float wW, float wH)
{
    Wheel(parent, false, 0, wX, wY, wW, wH);
}

void Wheel::loadImage(QImage &image,const QString &imageName)
{
    // Set Path
    QString path = w->baseDir+"\\images\\"+imageName;

    // Load
    image.load(path);
}

void Wheel::setWidget(Widget *widget)
{
    // Set Widget
    w = widget;
}

//void Wheel::addTrack(Track *newTrack)
//{
//    skipTrackCoords();

//    trackCoords[memoryTrack-1] = newTrack;
//}

void Wheel::addTrack(float x, float y, float angle)
{
//    // Skip
//    skipTrackCoords();

//    // Set Last Value
//    Track *newTrack = new Track(this, x, y, angle);
//    trackCoords[trackCoords.size()-1] = newTrack;
    trackCoords.push_back(new Track(this, x, y, angle));
}

void Wheel::skipTrackCoords()
{
    // Skip
    for(int f = 0; f<trackCoords.size()-1; f++)
    {
        std::swap(trackCoords[f], trackCoords[f+1]);
    }
}

void Wheel::draw(QPainter &p)
{
    // Rotate
    p.translate(x, y);
    if(manual)
        p.rotate(angle);

        // Draw
        QBrush brush;
        brush.setColor(Qt::gray);
        p.setBrush(brush);
        p.drawRoundRect(QRect(x-width/2, y-height/2, width, height), 50, 50);

    // UnRotate
    if(manual)
        p.rotate(-angle);
    p.translate(-x, -y);
}

void Wheel::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timerTrackId)
    {
        trackCoords.pop_front();
    }
}

