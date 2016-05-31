#ifndef WIDGET_H
#define WIDGET_H

#include <car.h>
#include <cmask.h>
#include <button.h>
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

    static const int dPenWidth = 3;

    // States map
    QMap<QString, int> states;

    // Scenes
    int scene;
    QMap< QString, int > scenes;

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

    // Buttons
    QVector < QVector <Button*> > buttons;
    QMap< QString, int > buttonsId;

    // Timer Update
    int timerUpdateId;

    int timerFps, debugFps;

    int timerFpsId;

    // Key Bools
    bool keyRightPressed;
    bool keyLeftPressed;
    bool keyUpPressed;
    bool keyDownPressed;

    // Mouse vals
    bool clicked;
    QPoint click, move, release;

    // Fonts
    QFont magneto, grobold;

public slots:
    void loadImage(QImage &image, const QString &imageName );
    void loadFont(QFont &font, const QString &name);

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
    void addButton(Button *newButton, int sceneId);
    void syncKeyVals();
    void drawMain(QPainter &p);
    void drawGame(QPainter &p);
    void drawRedactor(QPainter &p);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void buttonClicked(int id);

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
