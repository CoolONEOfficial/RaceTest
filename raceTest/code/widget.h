#ifndef WIDGET_H
#define WIDGET_H

#include <car.h>
#include <cmask.h>
#include <animationcoords.h>
#include <QWidget>
#include <QVector>
#include <QPropertyAnimation>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    // Fps
    static const int fps = 60;

    // Camera coords
    QPointF cam;

    // Player
    Car *player;

    // Screen coords
    AnimationCoords screenCar;

    // Images
    QImage backgroundTexture;

    // Collision Masks
    QVector< CMask* > cMasks;

    // Timer Update
    int timerUpdateId;

    int timerFps, debugFps;

    int timerFpsId;

    // Key Bools
    bool keyRightPressed;
    bool keyLeftPressed;
    bool keyUpPressed;
    bool keyDownPressed;
    #ifdef __ANDROID_API__
    bool clicked;
    QPoint click, move, release;
    #endif

public slots:
    void loadImage(QImage &image, const QString &imageName );

private slots:
    void paintEvent(QPaintEvent *);
    void drawTexture(QPainter &p, QImage texture, QRectF rect);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *);
    void setValues();
    void loadAll();
    void loadMaps();
    void createAll();
    void addAll();
    void addFigure(CMask *cMask);
    void syncKeyVals();
#ifdef __ANDROID_API__
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
#endif

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
