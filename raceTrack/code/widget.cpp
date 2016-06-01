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

    gMap = gMaps.size()-1;

    // Open Window
    ui->setupUi(this);

    setMouseTracking(true);

    // Fullscreen
    showFullScreen();

    addButtons();

    // Start Timers
    timerUpdateId = startTimer(1000 / fps);
    timerFpsId = startTimer(1000);

    for(int f = 0; f<buttons[scenesMap["main"]].size(); f++)
    {
        buttons[scenesMap["main"]][f]->animation.start();
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setValues()
{    
    // Maps

    // States
    statesMap["passive"] = 0;
    statesMap["move"] = 1;
    statesMap["click"] = 2;

    // Scene
    scenesMap["main"] = 0;
    scenesMap["game"] = 1;
    scenesMap["redactor"] = 3;

    scene = scenesMap["main"];

    // Vectors
    QVector<Button *> newVector;
    for(int f = 0; f<scenesMap.size(); f++)
    {
        buttons.push_back(newVector);
    }

    // Bools

    // Keys

    keyUpPressed = false;
    keyDownPressed = false;
    keyLeftPressed = false;
    keyRightPressed = false;

    // Sync
    syncKeyVals();

    // Click bool
    clicked = false;

    // Animations

    // Screen car
    screenCar.create();
    screenCar.setDuration(300);
    screenCar.setX(width()/2);
    screenCar.setY(height()/2);
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
        bool loadMap = false;
        bool loadPlayer = false;
        bool loadPoly = false;
        GameMap *addGMap = 0;
        CMask *addCMask = 0;
        QPolygon addPoly;

        do
        {
            // Read command
            QString command;
            sMaps>>command;

            // Map

            if(command == "map")
            {
                loadMap = true;

                // Read name
                QString nameMap;
                sMaps>>nameMap;

                // Create gmap
                addGMap = new GameMap(this, nameMap);
            }

            else if(command == "/map")
            {
                loadMap = false;

                // Add gmap
                gMaps.push_back(addGMap);
            }

            // Map commands

            else if(loadMap)
            {
                // Player

                if(command == "player")
                {
                    loadPlayer = true;
                }

                else if(command == "/player")
                {
                    loadPlayer = false;
                }

                // Player Commands

                else if(loadPlayer)
                {
                    if(command == "startCoords")
                    {
                        int sX = 0, sY = 0;

                        sMaps>>sX;
                        sMaps>>sY;

                        addGMap->playerX = sX;
                        addGMap->playerY = sY;
                    }

                    else if(command == "startX")
                    {
                        int sX = 0;

                        sMaps>>sX;

                        addGMap->playerX = sX;
                    }

                    else if(command == "startY")
                    {
                        int sY = 0;

                        sMaps>>sY;

                        addGMap->playerY = sY;
                    }
                }

                // CMask

                else if(command == "cmask")
                {
                    loadCMask = true;

                    // Create cmask
                    addCMask = new CMask(this, true, 0, 0, QPolygonF(), backgroundTexture);
                }

                else if(command == "/cmask")
                {
                    loadCMask = false;

                    // Add cmask
                    addGMap->addFigure(addCMask);
                }

                // Cmask commands

                else if(loadCMask)
                {
                    // Coords
                    if(command == "coords")
                    {
                        // Read rect
                        int x,y;

                        // Load rect
                        sMaps>>x;
                        sMaps>>y;

                        // Set rect
                        addCMask->setCoords(QPointF(x, y));
                    }

                    // Solid
                    else if(command == "solid")
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

                    // Poly

                    else if(command == "poly")
                    {
                        loadPoly = true;
                    }

                    else if(command == "/poly")
                    {
                        loadPoly = false;
                    }

                    // Poly commands

                    else if(loadPoly)
                    {
                        if(command == "add")
                        {
                            // Create coords
                            int x,y;

                            // Read coods
                            sMaps>>x;
                            sMaps>>y;

                            // Add point
                            addPoly.push_back(QPoint(x,y));
                        }
                    }
                }
            }
        }while(!sMaps.atEnd());
    }
    else
    {
        QMessageBox::information(0, "Error load maps", path);
    }

    fMaps.close();

    // Add gMaps to QMap
    for(int f = 0; f<gMaps.size(); f++)
    {
        gMapsMap[ gMaps[f]->name ] = f;
    }
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

void Widget::addButtons()
{
    // Add Buttons

    // Main Buttons
    int w,h,x,y;

    w = width() / 5;
    h = w/2;

    x = width() / 3;
    y = height() - h;

    Button *mainButton = new Button(this, "Play", QPointF(-w/2,y),
                                    QPointF(x,y), w, h);

    addButton(mainButton, scenesMap["main"]);

    x = width() / 3 * 2;
    y = height() - h;

    mainButton = new Button(this, "Exit", QPointF(width()+w/2,y),
                                        QPointF(x,y), w, h);

    addButton(mainButton, scenesMap["main"]);

    for(int f1 = 0; f1<scenesMap.size(); f1++)
    {
        for(int f2 = 0; f2<buttons[f1].size(); f2++)
        {
            buttons[f1][f2]->setWidget(this);
        }
    }
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
    QBrush backgroundBrush;
    backgroundBrush.setColor(Qt::blue);
    p.setBrush(backgroundBrush);
    p.drawRect(0, 0, width(), height());

    // Draw Logo Text
    magneto.setPixelSize(width()/7);
    p.setFont(magneto);
    p.drawText(QRect(0,height()/2,width(), height()/3*2), Qt::AlignHCenter, "RaceTrack");

    // Draw buttons
    for(int f = 0; f<buttons[scenesMap["main"]].size(); f++)
    {
        buttons[scenesMap["main"]][f]->draw(p);
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
    if(gMaps.size() > 0 && gMap>=0 && gMap<gMaps.size())
    {
        for(int f = 0; f<gMaps[gMap]->cMasks.size(); f++)
        {
            gMaps[gMap]->cMasks[f]->draw(p);
        }
    }

    // Draw Player
    player->draw(p);

    // Untranslate
    p.translate(player->x, player->y);
    p.rotate((player->angle / M_PI * 180.0 + 90));
    p.translate(-player->x, -player->y);

    p.translate(cam.x(), cam.y());

// Debug Texts
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
    if(scene == scenesMap["game"])
    {
        // Draw Game
        drawGame(p);
    }

    // Main
    else if(scene == scenesMap["main"])
    {
        // Draw main
        drawMain(p);
    }

    // Redactor
    else if(scene == scenesMap["redactor"])
    {
        // Draw main
        drawRedactor(p);
    }
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
    // Play Button
    if(id == buttonsId["Play"])
    {
        scene = scenesMap["game"];
    }

    //  Exit Button
    else if(id == buttonsId["Exit"])
    {
        QApplication::exit();
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
    if(scene == scenesMap["game"])
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
