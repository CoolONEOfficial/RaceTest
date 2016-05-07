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
    player->loadImage("sprites\\car\\main\\car.png");

    // Background Texture
    loadImage(backgroundTexture, "textures\\background\\texture.jpg");
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
    player->addWheelManual(player->carWidth()/6, player->carHeight()/5, 20, 10);
    player->addWheelStatic(-player->carWidth()/6, player->carHeight()/5, 20, 10);
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

//    QLineF line(1000, 0, 1000, height());
//    p.drawLine(line);

//    // Draw Debug Texts
//    p.drawText(5, 15*1, "x: "+QString::number(player->x));
//    p.drawText(5, 15*2, "y: "+QString::number(player->y));
//    p.drawText(5, 15*3, "Speed: "+QString::number(player->speed));
//    p.drawText(5, 15*4, "Angle: "+QString::number(player->angle));
//    p.drawText(5, 15*5, "WheelsAngle: "+QString::number(player->whellsAngle));

//    for(int f = 0; f<player->staticWheels[0]->trackCoords.size(); f++)
//    {
//        p.drawText(20, 20+15*f, "x:"+QString::number(player->staticWheels[1]->trackCoords[f]->x)+" y:"+QString::number(player->staticWheels[1]->trackCoords[f]->y) );
//    }
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
    if( !image.load(baseDir+"\\images\\"+imageName))
    {
        QMessageBox::information(this,"Error load image",baseDir+"\\images\\"+imageName);
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
