#include "cmask.h"

#include <QPainter>

CMask::CMask(QObject *parent)
{
    // Set Solid
    solid = true;

    // Set Polygon
    poly = QPolygonF( QRectF( -10, -10, 20, 20 ) );

    // Set coords
    x = 0;
    y = 0;
}

CMask::CMask(QObject *parent, bool cSolid, float cX, float cY, QPolygonF cPoly, QImage cTexture)
{
    // Set Solid
    solid = cSolid;

    // Set Polygon
    poly = cPoly;

    // Set Texture
    texture = cTexture;

    // Set coords
    x = cX;
    y = cY;
}

CMask::CMask(QObject *parent, bool cSolid, float cX, float cY, QRectF cRect, QImage cTexture) : QObject(parent)
{
    // Set Solid
    solid = cSolid;

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

    // Brush
    QBrush brush;
    brush.setTextureImage(texture);

    // Fill polygon
    QPainterPath path;
    path.addPolygon(poly);

    // Pen solid
    QPen pen = p.pen(), oldPen = p.pen();
    pen.setWidth(6);

    if(solid)
        p.setPen(pen);

    // Draw polygon
    p.drawPolygon(poly);

    p.setPen(oldPen);

    // Fill polygon
    p.fillPath(path, brush);

    // Untranslate
    p.translate(-(x), -(y));
}

void CMask::setRect(QRectF cRect)
{
    // Set Rect
    poly = QPolygonF( cRect );
}

void CMask::setPoly(QPolygon cPoly)
{
    poly = cPoly;
}

void CMask::setCoords(QPointF cCoords)
{
    // Set Coords
    x = cCoords.x();
    y = cCoords.y();
}
