#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <math.h>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    player.loadImage("car.png");

    ui->setupUi(this);

    timerUpdateId = startTimer(1000 / 60);

    showFullScreen();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.setRenderHints(QPainter::Antialiasing);

    player.draw(p);

    p.drawText(5, 15, "Speed: "+QString::number(player.speed));
    p.drawText(5, 15*2, "Angle: "+QString::number(player.angle));
    p.drawText(5, 15*3, "WheelsAngle: "+QString::number(player.whellsAngle));
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W ||
       event->key() == Qt::Key_Up)
    {
        if(player.speed < player.maxSpeed)
            player.speed += player.boost;
    }
    else if(event->key() == Qt::Key_S ||
            event->key() == Qt::Key_Down)
    {
        if(player.speed > -fabs(player.maxBackSpeed))
            player.speed -= player.backBoost;
    }

    if(event->key() == Qt::Key_A ||
       event->key() == Qt::Key_Left)
    {
        player.rotateWheels(-(M_PI / 200));
    }
    else if(event->key() == Qt::Key_D ||
            event->key() == Qt::Key_Right)
    {
        player.rotateWheels(M_PI / 200);
    }

    if(event->key() == Qt::Key_R)
    {
        player.x = width()/2;
        player.y = height()/2;
        player.speed = 0;
    }
}

void Widget::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timerUpdateId)
    {
        update();
    }
}
