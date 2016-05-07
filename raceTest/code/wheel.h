#ifndef WHEEL_H
#define WHEEL_H

#include "track.h"
#include <QObject>
#include <QImage>

class Widget;

class Wheel : public QObject
{
    Q_OBJECT
public:
    explicit Wheel(QObject *parent = 0, bool wManual = true, float wAngle = 0, float wX = 0, float wY = 0, float wW = 40, float wH = 20);
    explicit Wheel(QObject *parent = 0, bool wManual = false, float wX = 0, float wY = 0, float wW = 40, float wH = 20);

    // Widget
    Widget *w;

    // Track
//    static const int memoryTrack = 60;
    QVector<Track*> trackCoords;

    // Size
    float width;
    float height;

    // Coords
    float x,y;

    // Type
    bool manual;
    float angle;

    // Timer Track
    int timerTrackId;

signals:

public slots:
    void loadImage(QImage &image, const QString &imageName);
    void setWidget(Widget *widget);
    void addTrack(float x, float y, float angle);
//    void addTrack(Track *newTrack);
    void skipTrackCoords();
    void draw(QPainter &p);
    void timerEvent(QTimerEvent *event);
};

#endif // WHEEL_H
