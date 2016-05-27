#include "cmask.h"
#include <QPainter>

CMask::CMask(QObject *parent)
{
    // Set Polygon
    poly = QPolygonF( QRectF( -10, -10, 20, 20 ) );

    // Set coords
    x = 0;
    y = 0;
}

CMask::CMask(QObject *parent, int cX, int cY, QPolygonF cPoly, QImage cTexture)
{
    // Set Polygon
    poly = cPoly;

    // Set Texture
    texture = cTexture;

    // Set coords
    x = cX;
    y = cY;
}

CMask::CMask(QObject *parent, int cX, int cY, QRectF cRect, QImage cTexture) : QObject(parent)
{
    // Set Polygon
    poly = QPolygonF( cRect );

    // Set Texture
    texture = cTexture;

    // Set coords
    x = cX;
    y = cY;
}

void CMask::draw(QPainter &p)
{
    // Translate
    p.translate(x, y);

    // Draw poly
    p.drawPolygon(poly);

    // Untranslate
    p.translate(-(x), -(y));
}
