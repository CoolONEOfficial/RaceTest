#include "track.h"
#include <QPointF>

Track::Track(QObject *parent, QPointF coords, float tAngle) : QObject(parent)
{
    x = coords.x();
    y = coords.y();

    angle = tAngle;
}

Track::Track()
{
    x = -100;
    y = -100;
    angle = 0;
}

Track::Track(QObject *parent, float tX, float tY, float tAngle) : QObject(parent)
{
    x = tX;
    y = tY;

    angle = tAngle;
}

