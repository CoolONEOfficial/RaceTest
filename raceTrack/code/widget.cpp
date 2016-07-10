#include "widget.h"
#include "ui_widget.h"
#include <time.h>

#include <QPainter>
#include <QKeyEvent>
#include <math.h>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTextStream>
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
    // QMaps

    // States
    statesMap["passive"] = 0;
    statesMap["move"] = 1;
    statesMap["click"] = 2;

    // Scenes
    scenesMap["main"] = 0;
    scenesMap["game"] = 1;
    scenesMap["maps"] = 2;

    scene = scenesMap["main"];

    // Vectors

    // Butons
    QVector<Button *> newVector;
    for(int f = 0; f<scenesMap.size(); f++)
    {
        buttons.push_back(newVector);
    }

    // Map Buttons
    QMap<QString, int> newMap;
    for(int f = 0; f<scenesMap.size(); f++)
    {
        buttonsMap.push_back(newMap);
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
    loadImage(mainTexture, ":/maintexture");
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

                        // Add Poly
                        addCMask->setPoly(addPoly);
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

    int w,h,x,y;
    Button *mainButton;

    // ----------------------------------------- Main Buttons -----------------------------------------

    w = width() / 5;
    h = w/2;

    x = width() / 3;
    y = height() - h;

    // Button Play
    mainButton = new Button(this, "Play", QPointF(-w/2,y),
                                    QPointF(x,y), w, h);

    addButton(mainButton, scenesMap["main"]);

    // Button Exit
    x = width() / 3 * 2;
    y = height() - h;

    mainButton = new Button(this, "Exit", QPointF(width()+w/2,y),
                                        QPointF(x,y), w, h);

    addButton(mainButton, scenesMap["main"]);

    // ----------------------------------------- Maps Buttons -----------------------------------------
    for(int f = 0; f<gMaps.size(); f++)
    {
        // Set Width / Height
        w = width() - width()/5;
        h = ((height() - height()/5) / gMaps.size()) / 3 * 2;

        // Max height
        if(h > w/4)
            h = w/4;

        x = width() / 2;
        y = height()/5 + f*(h + h/2) + h/2;

        mainButton = new Button(this, gMaps[f]->name, QPointF(x,-h),
                                            QPointF(x,y), w, h);

        addButton(mainButton, scenesMap["maps"]);
    }

    // Set Widget
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
    buttonsMap[sceneId][newButton->text] = buttons[sceneId].size()-1;
}

void Widget::syncKeyVals()
{
    #ifdef __ANDROID_API__
        if(!keyDownPressed)
            keyUpPressed = true;
    #endif

    player->keyUp = keyUpPressed;
    player->keyDown = keyDownPressed;
    player->keyLeft = keyLeftPressed;
    player->keyRight = keyRightPressed;
}

void Widget::drawMain(QPainter &p)
{
    // Draw Logo Text

    // Set font
    magneto.setPixelSize(width()/7);
    p.setFont(magneto);

    // Set Text Rect
    QRect textRect(0,height()/2, width(), height()/3*2);

    // Draw
    drawShadowText(p, textRect, "RaceTrack", 10, Qt::black, Qt::blue);
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

    #ifdef __ANDROID_API__
        // Android Buttons
        if(keyDownPressed)
        {
            p.drawRect(width()/3, height()/3, width()/3, height()/3);
        }

        if(keyLeftPressed)
        {
            p.drawRect(0, height()/3, width()/3, height()/3);
        }

        if(keyRightPressed)
        {
            p.drawRect(width()/3*2, height()/3, width()/3, height()/3);
        }
    #endif

    // Debug Texts
    #ifdef QT_DEBUG
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
    #endif
}

void Widget::drawRedactor(QPainter &p)
{

}

void Widget::drawMaps(QPainter &p)
{
    // Draw text label

    // Set font
    magneto.setPixelSize(width()/7);
    p.setFont(magneto);

    // Set Text Rect
    QRect textRect(0, 0, width(), height()/5);

    // Draw
    drawShadowText(p, textRect, "Maps", 10, Qt::black, Qt::blue);
}

void Widget::drawButtons(QPainter &p)
{
    for(int f = 0; f<buttons[scene].size(); f++)
    {
        buttons[scene][f]->draw(p);
    }
}

void Widget::drawButtons(QPainter &p, int sceneId)
{
    for(int f = 0; f<buttons[sceneId].size(); f++)
    {
        buttons[sceneId][f]->draw(p);
    }
}

void Widget::drawShadowText(QPainter &p, QPoint coords, QString str, int size, QColor colorText, QColor colorShadow)
{
    QPen oldPen = p.pen(),pen;

    // Draw shadow
    pen.setColor(colorShadow);
    p.setPen(pen);

    p.drawText(coords.x()-size, coords.y()-size, str);
    p.drawText(coords.x()+size, coords.y()+size, str);
    p.drawText(coords.x()+size, coords.y()-size, str);
    p.drawText(coords.x()-size, coords.y()+size, str);
    p.drawText(coords.x(), coords.y()-size, str);
    p.drawText(coords.x(), coords.y()+size, str);
    p.drawText(coords.x()+size, coords.y(), str);
    p.drawText(coords.x()-size, coords.y(), str);

    // Draw text
    pen.setColor(colorText);
    p.setPen(pen);

    p.drawText(coords.x(), coords.y(), str);

    // Set old pen
    p.setPen(oldPen);
}

void Widget::drawShadowText(QPainter &p, QRect rect, QString str, int size, QColor colorText, QColor colorShadow)
{
    QPen oldPen = p.pen(), pen;

    // Draw Shadow
    pen.setColor(colorShadow);
    p.setPen(pen);

    p.drawText(QRect(rect.x()-size, rect.y()-size, rect.width(), rect.height()), Qt::AlignHCenter, str);
    p.drawText(QRect(rect.x()+size, rect.y()+size, rect.width(), rect.height()), Qt::AlignHCenter, str);
    p.drawText(QRect(rect.x()+size, rect.y()-size, rect.width(), rect.height()), Qt::AlignHCenter, str);
    p.drawText(QRect(rect.x()-size, rect.y()+size, rect.width(), rect.height()), Qt::AlignHCenter, str);
    p.drawText(QRect(rect.x()-size, rect.y(), rect.width(), rect.height()), Qt::AlignHCenter, str);
    p.drawText(QRect(rect.x()+size, rect.y(), rect.width(), rect.height()), Qt::AlignHCenter, str);
    p.drawText(QRect(rect.x(), rect.y()-size, rect.width(), rect.height()), Qt::AlignHCenter, str);
    p.drawText(QRect(rect.x(), rect.y()+size, rect.width(), rect.height()), Qt::AlignHCenter, str);

    // Draw text
    pen.setColor(colorText);
    p.setPen(pen);

    p.drawText(QRect(rect.x(), rect.y(), rect.width(), rect.height()), Qt::AlignHCenter, str);

    // Set Old pen
    p.setPen(oldPen);
}

void Widget::drawBackground(QPainter &p)
{
    // Draw Background

    // Background
    p.setBrush(Qt::blue);
    p.drawRect(0,0,width(),height());

    // Texture

    // Parallax
    #ifndef ANDROID
        // Translate
        p.translate(move.x()/4, move.y()/4);
    #endif

    // Set Brush (Texture)
    QBrush mainBrush;
    mainBrush.setTextureImage(mainTexture);
    p.setBrush(mainBrush);

    // Draw
    p.drawRect(-width(),-height(), width()*2, height()*2);

    // Parallax
    #ifndef ANDROID
        // Untranslate
        p.translate(-move.x()/4, -move.y()/4);
    #endif
}

void Widget::setScene(int sceneId, bool startAnimations)
{
    scene = sceneId;

    if(startAnimations)
    {
        for(int f = 0; f<buttons[scene].size(); f++)
        {
            buttons[scene][f]->animation.start();
        }
    }
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

    // Draw Background
    drawBackground(p);

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

    // Maps
    else if(scene == scenesMap["maps"])
    {
        // Draw maps
        drawMaps(p);
    }

    // Draw buttons
    drawButtons(p);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
    {
        if(scene == scenesMap["game"])
        {
            // Controls car

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
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat())
    {
        if(scene == scenesMap["game"])
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
    clicked = true;

    click.setX(event->x());
    click.setY(event->y());

    if(scene == scenesMap["game"])
    {
        #ifdef __ANDROID_API__
            if(event->y() > height()/3)
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
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    move.setX(event->x());
    move.setY(event->y());

    if(scene == scenesMap["game"])
    {
        #ifdef __ANDROID_API__
            mousePressEvent(event);
        #endif
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    clicked = false;

    release.setX(event->x());
    release.setY(event->y());

    // Check Buttons
    for(int f = 0; f<buttons[scene].size(); f++)
    {
        if(buttons[scene][f]->inside(event->x(), event->y()))
        {
            buttonClick(f);
        }
    }

    if(scene == scenesMap["game"])
    {
        #ifdef __ANDROID_API__
            // Off all values
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
