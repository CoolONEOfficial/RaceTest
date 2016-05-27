#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <math.h>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    // Srand
    srand(0);

    // Start Timer Update
    timerUpdateId = startTimer(1000 / fps);

    // Set and Load Default values
    createAll();
    setValues();
    loadAll();
    addAll();

    // Open Window
    ui->setupUi(this);

    // Fullscreen
    showFullScreen();

    timerFpsId = startTimer(1000);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setValues()
{
    // Bools

    // Keys
    keyLeftPressed = false;
    keyRightPressed = false;
    keyUpPressed = false;
    keyDownPressed = false;
}

void Widget::loadAll()
{
    // Car Image
    player->loadImage(":/car");

    // Background Texture
    loadImage(backgroundTexture, ":/texture");
}

void Widget::createAll()
{
    // Create all

    // Cars
    player = new Car(this);
}

void Widget::addAll()
{
    // Wheels
    player->addWheelManual(new Wheel(this, player->width()/3, player->height()/2, 20, 10));

    player->addWheelStatic(new Wheel(this, -player->width()/3, player->height()/2, 20, 10));

    CMask *mask = new CMask(this, 200,200, QRect(-10,-10,20,20), backgroundTexture);

    addFigure(mask);
}

void Widget::addFigure(CMask *cMask)
{
    // Add figure cMask
    cMasks.push_back(cMask);
}

void Widget::
paintEvent(QPaintEvent *)
{
    QPainter p(this);

    // Antialiasing
    p.setRenderHints(QPainter::Antialiasing);

    // Translate
    p.translate(-cam.x(), -cam.y());

    p.translate(player->x, player->y);
    p.rotate(-player->angle / M_PI * 180.0 - 90);
    p.translate(-player->x, -player->y);

    // Draw Background Texture
    drawTexture(p, backgroundTexture, QRectF(-width()*2, -height()*2, width()*4, height()*4));

    // Draw collision masks
    for(int f = 0; f<cMasks.size(); f++)
    {
        cMasks[f]->draw(p);
    }

    // Draw Player
    player->draw(p);

    // Untranslate
    p.translate(player->x, player->y);
    p.rotate((player->angle / M_PI * 180.0 + 90));
    p.translate(-player->x, -player->y);

    p.translate(cam.x(), cam.y());

    // Debug

    // Rect
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(255, 255, 255, 100));
    p.setBrush(brush);
    p.drawRect(0,0, width(), 150);

    // Draw Debug Texts
    p.drawText(5, 15*1, "x: "+QString::number(player->x));
    p.drawText(5, 15*2, "y: "+QString::number(player->y));
    p.drawText(5, 15*3, "camX: "+QString::number(cam.x()));
    p.drawText(5, 15*4, "camY: "+QString::number(cam.y()));
    p.drawText(5, 15*5, "Speed: "+QString::number(player->speed));
    p.drawText(5, 15*6, "Angle: "+QString::number(player->angle));
    p.drawText(5, 15*7, "WheelsAngle: "+QString::number(player->whellsAngle));
    p.drawText(5, 15*8, "SpeedScale: "+QString::number(player->speedScale()));
    p.drawText(5, 15*9, "Fps: "+QString::number(debugFps));

    // Keys debug
    if(keyUpPressed)
    {
        p.drawRect(width()-80, 10, 30, 30);
        p.drawLine(width()-80+15, 15, width()-80+15, 35);
        p.drawLine(width()-80+15, 15, width()-80+10, 25);
        p.drawLine(width()-80+15, 15, width()-80+20, 25);
    }

    if(keyDownPressed)
    {
        p.drawRect(width()-80, 50, 30, 30);
        p.drawLine(width()-80+15, 55, width()-80+15, 50+25);
        p.drawLine(width()-80+15, 50+25, width()-80+15-5, 50+25-10);
        p.drawLine(width()-80+15, 50+25, width()-80+15+5, 50+25-10);
    }

    if(keyRightPressed)
        p.drawRect(width()-40, 50, 30, 30);

    if(keyLeftPressed)
        p.drawRect(width()-120, 50, 30, 30);

    // Wheels
    for(int f = 0; f<player->wheelsManual.size(); f++)
    {
        p.drawText(f*100+200, 15*1, "Wheel "+QString::number(f));
        p.drawText(f*100+200, 15*2, "X: "+QString::number(player->wheelsManual[f]->x)+
                                                      " Y: "+QString::number(player->wheelsManual[f]->y));
        p.drawText(f*100+200, 15*3, "Branches: "+QString::number(player->wheelsManual[f]->tracks.size()));

        for(int f2 = 0; f2<player->wheelsManual[f]->tracks.size(); f2++)
        {
            p.drawText(f*100+200, 15*(4+f2), "size: "+QString::number(player->wheelsManual[f]->tracks[f2].size()));
        }
    }

    for(int f = 0; f<player->wheelsStatic.size(); f++)
    {
        p.drawText(f*100+500, 15*1, "Wheel "+QString::number(f));
        p.drawText(f*100+500, 15*2, "X: "+QString::number(player->wheelsStatic[f]->x)+
                                                      " Y: "+QString::number(player->wheelsStatic[f]->y));
        p.drawText(f*100+500, 15*3, "Branches: "+QString::number(player->wheelsStatic[f]->tracks.size()));

        for(int f2 = 0; f2<player->wheelsStatic[f]->tracks.size(); f2++)
        {
            p.drawText(f*100+500, 15*(4+f2), "size: "+QString::number(player->wheelsStatic[f]->tracks[f2].size()));
        }
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    // W / Up / Gas
    if(event->key() == Qt::Key_W ||
       event->key() == Qt::Key_Up)
    {
        keyUpPressed = true;
        player->keyUp = true;
    }

    // S / Down / Brake
    else if(event->key() == Qt::Key_S ||
            event->key() == Qt::Key_Down)
    {
        keyDownPressed = true;
        player->keyDown = true;
    }

    // A / Left
    if(event->key() == Qt::Key_A ||
       event->key() == Qt::Key_Left)
    {
        keyLeftPressed = true;
        player->keyLeft = true;
    }

    // D / Right
    else if(event->key() == Qt::Key_D ||
            event->key() == Qt::Key_Right)
    {
        keyRightPressed = true;
        player->keyRight = true;
    }

    // R / Restart
    if(event->key() == Qt::Key_R)
    {
        player->x = width()/2;
        player->y = height()/2;
        player->speed = 0;
        player->angle = 0;
    }
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
    {
        // W / Up / Gas
        if(event->key() == Qt::Key_W ||
           event->key() == Qt::Key_Up)
        {
            keyUpPressed = false;
            player->keyUp = false;
            player->addTrackBranches();
        }

        // S / Down / Brake
        else if(event->key() == Qt::Key_S ||
                event->key() == Qt::Key_Down)
        {
            keyDownPressed = false;
            player->keyDown = false;
            player->addTrackBranches();
        }

        // A / Left
        if(event->key() == Qt::Key_A ||
           event->key() == Qt::Key_Left)
        {
            keyLeftPressed = false;
            player->keyLeft = false;
        }

        // D / Right
        else if(event->key() == Qt::Key_D ||
                event->key() == Qt::Key_Right)
        {
            keyRightPressed = false;
            player->keyRight = false;
        }
    }
}

void Widget::timerEvent(QTimerEvent *event)
{
    // Update Timer
    if(event->timerId() == timerUpdateId)
    {
        // Update
        update();
    }

    // FPS Timer
    else if(event->timerId() == timerFpsId)
    {
        debugFps = timerFps;
        timerFps = 0;
    }
}

void Widget::loadImage(QImage &image, const QString &imageName)
{
    QString path = imageName;

    if( !image.load(path))
    {
        QMessageBox::information(this,"Error load image", path);
    }
}

void Widget::drawTexture(QPainter &p, QImage texture, QRectF rect)
{
    // Draw Texture

    // Create Brush/Pen
    QBrush oldBrush = p.brush(), brush;

    // Set Texture
    brush.setTextureImage(texture);

    // Set Brush
    p.setBrush(brush);

    // Set Coords
    rect.setCoords( 0,  0,
                    rect.width(),  rect.height());

    // Translate
    p.translate(rect.x(), rect.y());

    // Draw texture
    p.drawRect(rect);

    // Untranslate
    p.translate(-(rect.x()), -(rect.y()));

    // Reset brush/Pen
    p.setBrush(oldBrush);
}
