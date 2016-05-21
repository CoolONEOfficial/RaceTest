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
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setValues()
{
    // BaseDir
    baseDir = QApplication::applicationDirPath();

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
    player->loadImage("sprites/car/main/car.png");

    // Background Texture
    loadImage(backgroundTexture, "textures/background/texture.jpg");
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
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    // Antialiasing
    p.setRenderHints(QPainter::Antialiasing);

    // Draw Background Texture
    drawTexture(p, backgroundTexture, 0, 0);

    // Draw Player
    player->draw(p);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(255, 255, 255, 100));
    p.setBrush(brush);
    p.drawRect(0,0, width(), 150);

    // Draw Debug Texts
    p.drawText(5, 15*1, "x: "+QString::number(player->x));
    p.drawText(5, 15*2, "y: "+QString::number(player->y));
    p.drawText(5, 15*3, "Speed: "+QString::number(player->speed));
    p.drawText(5, 15*4, "Angle: "+QString::number(player->angle));
    p.drawText(5, 15*5, "WheelsAngle: "+QString::number(player->whellsAngle));
    p.drawText(5, 15*6, "SpeedScale: "+QString::number(player->speedScale()));

    // Wheels
    for(int f = 0; f<player->wheelsManual.size(); f++)
    {
        p.drawText(f*100+200, 15*1, "Wheel "+QString::number(f));
        p.drawText(f*100+200, 15*2, "X: "+QString::number(player->wheelsManual[f]->x)+
                                                      " Y: "+QString::number(player->wheelsManual[f]->y));
        p.drawText(f*100+200, 15*3, "Branches: "+QString::number(player->wheelsManual[f]->tracks.size()));

        if(true)
        {
            for(int f2 = 0; f2<player->wheelsManual[f]->tracks.size(); f2++)
            {
                p.drawText(f*100+200, 15*(4+f2), "size: "+QString::number(player->wheelsManual[f]->tracks[f2].size()));
            }
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
        }

        // S / Down / Brake
        else if(event->key() == Qt::Key_S ||
                event->key() == Qt::Key_Down)
        {
            keyDownPressed = false;
            player->keyDown = false;
            player->startDrift();
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
}

void Widget::loadImage(QImage &image, const QString &imageName)
{
    QString path = baseDir+"/images/"+imageName;

    if( !image.load(path))
    {
        QMessageBox::information(this,"Error load image", path);
    }
}

void Widget::drawTexture(QPainter &p, QImage texture, int startX, int startY)
{
    // Draw Texture
    for(int numX = 0; numX < width() / (texture.width()+startX) + 1; numX++)
    {
        for(int numY = 0; numY < height() / (texture.height()+startY) + 1; numY++)
        {
            p.drawImage(startX + numX * texture.width(), startY + numY * texture.height(), texture);
        }
    }
}
