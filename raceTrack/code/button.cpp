#include "button.h"
#include "widget.h"

#include <QMessageBox>

Button::Button(QObject *parent)
{
    widget = 0;

    color = Qt::blue;

    text = "";

    width = 300;
    height = 150;

    animation.create();
    animation.setX(0);
    animation.setY(0);
    animation.setEndX(0);
    animation.setEndY(0);
    animation.setDuration(2000);
    animation.setEasingCurve(QEasingCurve::OutBack);

    visible = true;
}

Button::Button(QObject *parent, QString bText, QPointF sCoords, QPointF eCoords, int bWidth, int bHeight, QColor bColor, bool bVisible) : QObject(parent)
{
    widget = 0;

    color = bColor;

    text = bText;

    width = bWidth;
    height = bHeight;

    animation.create();
    animation.setX(sCoords.x()-width/2);
    animation.setY(sCoords.y()-height/2);
    animation.setEndX(eCoords.x()-width/2);
    animation.setEndY(eCoords.y()-height/2);
    animation.setDuration(2000);
    animation.setEasingCurve(QEasingCurve::OutBack);

    visible = bVisible;
}

int Button::x(bool left)
{
    if(left)
        return animation.x;

    return animation.x + width / 2;
}

int Button::y(bool up)
{
    if(up)
        return animation.y;

    return animation.y + height / 2;
}

int Button::shadowX(bool left)
{
    return x(left) - ((x() - widget->width() / 2) / shadowScale);
}

int Button::shadowY(bool up)
{
    return y(up) - ((y() - widget->height() / 2) / shadowScale);
}

int Button::state()
{
    // Set state
    if(widget != 0)
    {
        if(inside(widget->move.x(),widget->move.y()))
        {
            if(widget->clicked)
            {
                return widget->statesMap["click"];
            }
            else
            {
                return widget->statesMap["move"];
            }
        }
        else
        {
            return widget->statesMap["passive"];
        }
    }
    else
    {
        return 0;
    }
}

bool Button::inside(int cursorX, int cursorY)
{
    if( cursorX > x(true) &&
       cursorX < x(true) + width &&
       cursorY > y(true) &&
       cursorY < y(true) + height &&
       visible)
       return true;
    else
       return false;
}

void Button::setWidget(Widget *w)
{
    widget = w;
}

void Button::setCoords(QPointF newCoords)
{
    animation.setX(newCoords.x() - width / 2);
    animation.setY(newCoords.y() - height / 2);
}

void Button::draw(QPainter &p)
{
    if(visible)
    {
        // Set state
        if(widget != 0)
        {
            // Draw
            QRect rect;

            if(state() == widget->statesMap["passive"])
            {
                // Shadow
                p.setBrush(color);
                p.drawRect(shadowX(true),
                                        shadowY(true), width, height);

                p.setBrush(QColor(0,0,0,100));
                p.drawRect(shadowX(true),
                                        shadowY(true), width, height);

                // Button
                p.setBrush(color);
                p.drawRect(x(true),
                                        y(true), width, height);
            }
            else if(state() == widget->statesMap["move"])
            {
                // Shadow
                p.setBrush(color);
                p.drawRect(shadowX(true),
                                        shadowY(true), width, height);

                p.setBrush(QColor(255, 255, 255, 100));
                p.drawRect(shadowX(true),
                                        shadowY(true), width, height);

                // Button
                p.setBrush(color);
                p.drawRect(x(true),
                                        y(true), width, height);

                p.setBrush(QColor(255, 255, 255, 100));
                p.drawRect(x(true),
                                        y(true), width, height);
            }
            else if(state() == widget->statesMap["click"])
            {
                // Button on coords shadow
                p.setBrush(color);
                p.drawRect(shadowX(true),
                                        shadowY(true), width, height);

                p.setBrush(QColor(255, 255, 255, 100));
                p.drawRect(shadowX(true),
                                        shadowY(true), width, height);
            }

            // Set font
            widget->grobold.setPixelSize(height/2);
            p.setFont(widget->grobold);

            // Draw button text
            if(state() == widget->statesMap["click"])
            {
                // Click pos text
                rect.setRect(shadowX(true), shadowY(true), width, height);
            }
            else
            {
                // Default pos text
                rect.setRect(x(true), y(true), width, height);
            }
            p.drawText(rect, Qt::AlignCenter, text);
        }
    }
}

void Button::setEndX(float endX)
{
    animation.setEndX(endX - width / 2);
}

void Button::setEndY(float endY)
{
    animation.setEndY(endY - height / 2);
}

void Button::setText(QString newText)
{
    text = newText;
}
