#ifndef CMASK_H
#define CMASK_H

#include <QObject>
#include <QPolygonF>
#include <QImage>

class CMask : public QObject
{
    Q_OBJECT
public:
    explicit CMask(QObject *parent = 0, int cX = 0, int cY = 0, QPolygonF cPoly = QPolygonF(), QImage cTexture = QImage());
    explicit CMask(QObject *parent = 0, int cX = 0, int cY = 0, QRectF cRect = QRectF(), QImage cTexture = QImage());
    explicit CMask(QObject *parent = 0);

    int x,y;
    QPolygonF poly;
    QImage texture;

signals:

public slots:
    void draw(QPainter &p);
};

#endif // CMASK_H
