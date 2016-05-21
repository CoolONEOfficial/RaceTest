#ifndef WHEEL_H
#define WHEEL_H

#include <QObject>
#include <QImage>

class Widget;

class Wheel : public QObject
{
    Q_OBJECT
public:
    explicit Wheel(QObject *parent, float wX, float wY, float wW, float wH);
    explicit Wheel(QObject *parent);

    // Widget
    Widget *w;

    // Tracks
    QVector< QVector<QPointF> > tracks;

    // Size
    float width;
    float height;

    // Coords
    float x,y;

    // Timers Id
    int timerDeleteId;

signals:

public slots:
    void setParams(float wX, float wY, float wW, float wH);
    void setWidget(Widget *widget);
    void addTrack(QPointF wTrack);
    void addTrack(int x, int y);
    void addTrackBranch();
    void draw(QPainter &p, float wAngle, bool rotate);
    void drawTracks(QPainter &p);
    void timerEvent(QTimerEvent *event);
};

#endif // WHEEL_H
