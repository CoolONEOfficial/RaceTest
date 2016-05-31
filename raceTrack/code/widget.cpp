#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <math.h>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTextStream>
#include <time.h>
#include <QFontDatabase>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    // Srand
    srand(time(0));

    // Set and Load Default values
    createAll();
    setValues();
    loadAll();
    addAll();

    // Open Window
    ui->setupUi(this);

    setMouseTracking(true);

    // Fullscreen
    showFullScreen();

    // Start Timers
    timerUpdateId = startTimer(1000 / fps);
    timerFpsId = startTimer(1000);

    for(int f = 0; f<buttons[scenes["main"]].size(); f++)
    {
        buttons[scenes["main"]][f]->animation.start();
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setValues()
{    
    // States
    states["passive"] = 0;
    states["move"] = 1;
    states["click"] = 2;

    // Scene
    scenes["main"] = 0;
    scenes["game"] = 1;
    scenes["redactor"] = 3;

    scene = scenes["main"];

    // Vectors
    QVector<Button *> newVector;
    for(int f = 0; f<scenes.size(); f++)
    {
        buttons.push_back(newVector);
    }

    // Bools

    // Keys
    // Android auto press
    keyUpPressed = false;
    keyDownPressed = false;
    keyLeftPressed = false;
    keyRightPressed = false;

    #ifdef __ANDROID_API__
        keyUpPressed = true;
    #endif

    // Sync
    syncKeyVals();

    // Click bool
    clicked = false;

    // Main Buttons
    Button *mainButton = new Button(this, Qt::blue, "Play", 300, 150, QPointF(-150,height()*2-150-20),
                                    QPointF(width()*2/8,height()*2-150-20));
    loadFont(mainButton->font, "grobold");

    addButton(mainButton, scenes["main"]);

    mainButton = new Button(this, Qt::blue, "Exit", 300, 150, QPointF(width()*2+150,height()*2-150-20),
                                    QPointF(width()*2-width()*2/3,height()*2-150-20));
    loadFont(mainButton->font, "grobold");

    addButton(mainButton, scenes["main"]);

    for(int f1 = 0; f1<scenes.size(); f1++)
    {
        for(int f2 = 0; f2<buttons[f1].size(); f2++)
        {
            buttons[f1][f2]->setWidget(this);
        }
    }
}

void Widget::loadAll()
{
    // Load Maps
    loadMaps();

    // Load Fonts
    loadFont(magneto, "magneto");
    loadFont(grobold, "grobold");

    // Car Image
    player->loadImage(":/car");

    // Background Texture
    loadImage(backgroundTexture, ":/texture");
}

void Widget::loadMaps()
{
    // Load maps
    QString path = QApplication::applicationDirPath()+"/maps.txt";

    QFile fMaps(path);

    if( fMaps.open(QIODevice::ReadOnly) )
    {
        QTextStream sMaps(&fMaps);

        // Load vals
        bool loadCMask = false;
        CMask *addCMask = 0;

        do
        {
            // Read command
            QString command;
            sMaps>>command;

            if(command == "cmask")
            {
                loadCMask = true;

                // Create cmask
                addCMask = new CMask(this, true, 0, 0, QPolygonF(), backgroundTexture);
            }

            else if(command == "/cmask")
            {
                loadCMask = false;

                // Add cmask
                addFigure(addCMask);
            }

            else if(loadCMask)
            {
                // Solid
                if(command == "solid")
                {
                    // Read solidId
                    int solidId;
                    sMaps>>solidId;

                    // Set solid
                    if(solidId == 0)
                        addCMask->solid = false;
                    else
                        addCMask->solid = true;
                }

                // Rect
                else if(command == "rect")
                {
                    // Read rect
                    int x,y,w,h;

                    // Load rect
                    sMaps>>x;
                    sMaps>>y;
                    sMaps>>w;
                    sMaps>>h;

                    // Set rect
                    addCMask->setRect(QRectF(x, y, w, h));
                }

                // Coords
                else if(command == "coords")
                {
                    // Read rect
                    int x,y;

                    // Load rect
                    sMaps>>x;
                    sMaps>>y;

                    // Set rect
                    addCMask->setCoords(QPointF(x, y));
                }
            }
        }while(!sMaps.atEnd());
    }

    fMaps.close();
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

    screenCar.create();
    screenCar.setDuration(300);
    screenCar.setX(width()/2);
    screenCar.setY(height()/2);
}

void Widget::addFigure(CMask *cMask)
{
    // Add figure cMask
    cMasks.push_back(cMask);
}

void Widget::addButton(Button *newButton, int sceneId)
{
    // Add button
    buttons[sceneId].push_back(newButton);

    // Add id
    buttonsId[newButton->text] = buttons[sceneId].size()-1;
}

void Widget::syncKeyVals()
{
    player->keyUp = keyUpPressed;
    player->keyDown = keyDownPressed;
    player->keyLeft = keyLeftPressed;
    player->keyRight = keyRightPressed;

    #ifdef __ANDROID_API__
        keyUpPressed = true;
    #endif
}

void Widget::drawMain(QPainter &p)
{
    magneto.setPixelSize(height()/2/3);
    p.setFont(magneto);
    p.drawText(QRect(0,height()/2,width(), height()/3*2), Qt::AlignHCenter, "RaceTrack");

    for(int f = 0; f<buttons[scenes["main"]].size(); f++)
    {
        buttons[scenes["main"]][f]->draw(p);
    }
}

void Widget::drawGame(QPainter &p)
{
    // Translate
    p.translate(-cam.x(), -cam.y());

    p.translate(player->x, player->y);
    p.rotate(-player->angle / M_PI * 180.0 - 90);
    p.translate(-player->x, -player->y);

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


#ifdef QT_DEBUG
    #ifndef __ANDROID_API__

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
        p.drawText(5, 15*5, "scrX: "+QString::number(screenCar.x));
        p.drawText(5, 15*6, "scrY: "+QString::number(screenCar.y));
        p.drawText(5, 15*7, "Speed: "+QString::number(player->speed));
        p.drawText(5, 15*8, "Angle: "+QString::number(player->angle));
        p.drawText(5, 15*9, "WheelsAngle: "+QString::number(player->whellsAngle));
        p.drawText(5, 15*10, "SpeedScale: "+QString::number(player->speedScale()));
        p.drawText(5, 15*11, "Fps: "+QString::number(debugFps));

        // Keys debug
        if(player->keyUp)
        {
            p.drawRect(width()-80, 10, 30, 30);
            p.drawLine(width()-80+15, 15, width()-80+15, 35);
            p.drawLine(width()-80+15, 15, width()-80+10, 25);
            p.drawLine(width()-80+15, 15, width()-80+20, 25);
        }

        if(player->keyDown)
        {
            p.drawRect(width()-80, 50, 30, 30);
            p.drawLine(width()-80+15, 55, width()-80+15, 50+25);
            p.drawLine(width()-80+15, 50+25, width()-80+15-5, 50+25-10);
            p.drawLine(width()-80+15, 50+25, width()-80+15+5, 50+25-10);
        }

        if(player->keyRight)
            p.drawRect(width()-40, 50, 30, 30);

        if(player->keyLeft)
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

    #else
        // Android Buttons
        if(keyDownPressed)
        {
            p.drawRect(width()/3, height()/2, width()/3, height()/2);
        }

        if(keyLeftPressed)
        {
            p.drawRect(0, height()/2, width()/3, height()/2);
        }

        if(keyRightPressed)
        {
            p.drawRect(width()/3*2, height()/2, width()/3, height()/2);
        }
    #endif
#endif
}

void Widget::drawRedactor(QPainter &p)
{

}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    // Set pen width
    QPen pen;
    pen.setWidth(dPenWidth);
    p.setPen(pen);

    // Antialiasing
    p.setRenderHints(QPainter::Antialiasing);

    // Game
    if(scene == scenes["game"])
    {
        // Draw Game
        drawGame(p);
    }

    // Main
    else if(scene == scenes["main"])
    {
        // Draw main
        drawMain(p);
    }

    // Redactor
    else if(scene == scenes["redactor"])
    {
        // Draw main
        drawRedactor(p);
    }

//    p.drawText(10,15,"moveX:"+QString::number(move.x()));
//    p.drawText(10,15*2,"moveY:"+QString::number(move.y()));
//    p.drawText(10,15*3,"clickX:"+QString::number(click.x()));
//    p.drawText(10,15*4,"clickY:"+QString::number(click.y()));
//    p.drawText(10,15*5,"releaseX:"+QString::number(release.x()));
//    p.drawText(10,15*6,"releaseY:"+QString::number(release.y()));
//    if(clicked)
//        p.drawText(10,15*7,"clicked:true");
//    else
//        p.drawText(10,15*7,"clicked:false");
//    p.drawText(10,15*8,"state:"+QString::number(buttonMainPlay.state));
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
    {
        // W / Up / Gas
        if(event->key() == Qt::Key_W ||
           event->key() == Qt::Key_Up)
        {
                keyUpPressed = true;
        }

        // S / Down / Brake
        if(event->key() == Qt::Key_S ||
                event->key() == Qt::Key_Down)
        {
            keyDownPressed = true;
        }

        // A / Left
        if(event->key() == Qt::Key_A ||
           event->key() == Qt::Key_Left)
        {
            keyLeftPressed = true;
        }

        // D / Right
        if(event->key() == Qt::Key_D ||
                event->key() == Qt::Key_Right)
        {
            keyRightPressed = true;
        }

        syncKeyVals();

        // R / Restart
        if(event->key() == Qt::Key_R)
        {
            player->x = 0;
            player->y = 0;
            player->speed = 0;
            player->angle = 0;
        }
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
                player->addTrackBranches();
            }

            // S / Down / Brake
            if(event->key() == Qt::Key_S ||
                    event->key() == Qt::Key_Down)
            {
                keyDownPressed = false;
                player->addTrackBranches();
            }

            // A / Left
            if(event->key() == Qt::Key_A ||
               event->key() == Qt::Key_Left)
            {
                keyLeftPressed = false;
            }

            // D / Right
            if(event->key() == Qt::Key_D ||
                    event->key() == Qt::Key_Right)
            {
                keyRightPressed = false;
            }

            // Sync
            syncKeyVals();
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

    image.load(path);
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

void Widget::mousePressEvent(QMouseEvent *event)
{
    click.setX(event->x());
    click.setY(event->y());

    #ifdef __ANDROID_API__
        if(event->y() > height()/2)
        {
            if(event->x() < width() / 3)
            {
                keyLeftPressed = true;
            }
            else if(event->x() > width() / 3*2)
            {
                keyRightPressed = true;
            }
            else if(event->x() > width() / 3 && event->x() < width() / 3*2)
            {
                keyDownPressed = true;
            }
        }

        // Sync
        syncKeyVals();
    #endif

    clicked = true;
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    move.setX(event->x());
    move.setY(event->y());

//    buttons[scenes["main"]][0]->setCoords(QPointF(event->x(), event->y()));
}

void Widget::buttonClicked(int id)
{
    if(id == buttonsId["play"])
    {
        scene = scenes["game"];
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    clicked = false;

    release.setX(event->x());
    release.setY(event->y());

    for(int f = 0; f<buttons[scene].size(); f++)
    {
        if(buttons[scene][f]->inside(event->x(), event->y()))
        {
            buttonClicked(f);
        }
    }

#ifdef __ANDROID_API__
    if(scene == scenes["game"])
    {
        keyUpPressed = false;
        keyDownPressed = false;
        keyLeftPressed = false;
        keyRightPressed = false;

        // Sync
        syncKeyVals();

        player->addTrackBranches();
    }
#endif
}

void Widget::loadFont(QFont &font, const QString &name)
{
    int fontId;
    QString path = ":/fonts/"+name;

    // Set path
    fontId = QFontDatabase::addApplicationFont(path);

    if (fontId != -1)  // No load Fail?
    {
        font.setFamily(QFontDatabase::applicationFontFamilies(fontId).first());
    }
}
