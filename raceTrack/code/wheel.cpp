#include "widget.h"

#include <QMessageBox>
#include <QPainter>
#include <QTimerEvent>

Wheel::Wheel(QObject *parent, float wX, float wY, float wW, float wH)
{
    // Set Widget
    w = dynamic_cast<Widget *>(parent);

    // Add tracks branch
    addTrackBranch();

    // Set Values
    setParams(wX, wY, wW, wH);

    // Start Timers
    timerDeleteId = startTimer(100);
}

void Wheel::setParams(float wX, float wY, float wW, float wH)
{
    // Coords
    x = wX;
    y = wY;

    // Size
    width = wW;
    height = wH;
}

void Wheel::setWidget(Widget *widget)
{
    // Set Widget
    w = widget;
}

Widget *Wheel::getWidget()
{
    return w;
}

void Wheel::addTrack(QPointF wTrack)
{
    // Add track
    if( tracks.size() > 0 )
    {
        // Add
        tracks[ tracks.size()-1 ].push_back(wTrack);
    }
//    else
//    {
//        // Add branch
//        addTrackBranch();

//        // Try again
//        addTrack(point);
    //    }
}

void Wheel::addTrack(int x, int y)
{
    // Add track
    addTrack( QPointF(x, y) );
}

void Wheel::addTrackBranch()
{
    // Add Track Branch
    QVector<QPointF> vector;
    tracks.push_back(vector);
}

void Wheel::draw(QPainter &p, float wAngle, bool rotate = true)
{
    // Rotate
    p.translate(x*2, y*2);
    if(rotate)
        p.rotate(wAngle);

        // Draw
        QBrush brush;
        brush.setColor(Qt::gray);
        p.setBrush(brush);
        p.drawRoundRect(QRect(-width/2, -height/2, width, height), 50, 50);

    // UnRotate
    if(rotate)
        p.rotate(-wAngle);
    p.translate(-x*2, -y*2);
}

void Wheel::drawTracks(QPainter &p)
{
    QPen oldPen = p.pen();
    QPen pen;
    pen.setWidth(5);
    p.setPen(pen);
    for(int f1 = 0; f1<tracks.size(); f1++)
    {
        if(!tracks[f1].empty())
        {
            for(int f2 = 0; f2<tracks[f1].size()-1; f2++)
            {
                // Set pen width
                QPen pen;
                pen.setWidth(height);
                p.setPen(pen);

                // Set opasity
                p.setOpacity(0.2);

                if(w != 0)
                {
                    // Draw Line
                    p.drawLine(tracks[f1][f2].x(), tracks[f1][f2].y(),
                               tracks[f1][f2+1].x(), tracks[f1][f2+1].y());
                }

                // Reset opasity
                p.setOpacity(1);
            }
        }
    }
    p.setPen(oldPen);
}

void Wheel::timerEvent(QTimerEvent *event)
{
    // Delete Timer
    if(event->timerId() == timerDeleteId)
    {
        // Delete Tracks
        QVector< QVector<QPointF> >::iterator iter;

        for( iter = tracks.begin(); iter != tracks.end();  )
        {
            if( iter->size() > 0 )
            {
                // Delete track
                iter->pop_front();
            }

            if( iter->empty() && iter+1 != tracks.end())
            {
                // Delete Branch
                iter = tracks.erase(iter);
            }
            else
                iter++;
        }
    }
}
