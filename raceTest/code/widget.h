#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <car.h>

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

    //
    QString baseDir;

    // Player
    Car player;

    // Images
    QImage backgroundTexture;

    // Timer Update
    int timerUpdateId;

    // Key Bools
    bool keyRightPressed;
    bool keyLeftPressed;
    bool keyUpPressed;
    bool keyDownPressed;

private slots:
    void paintEvent(QPaintEvent *);
    void drawTexture(QPainter &p, QImage texture, int startX = 0, int startY = 0);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *);
    void setValues();
    void loadAll();
   void loadImage(QImage &image, const QString &imageName );

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
