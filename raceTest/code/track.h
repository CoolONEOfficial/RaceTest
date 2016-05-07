#ifndef TRACK_H
#define TRACK_H

#include <QObject>
#include <QPointF>

class Track : public QObject
{
    Q_OBJECT
public:
    explicit Track ();
               Track(QObject *parent = 0, float tX = -100, float tY = -100, float tAngle = 0);
               Track(QObject *parent = 0, QPointF coords = QPointF(-100, -100), float tAngle = 0);

    // Coords
    float x,y;

    // Angle
    float angle;

signals:

public slots:
};

#endif // TRACK_H
