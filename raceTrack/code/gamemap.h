#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <cmask.h>
#include <QObject>

class GameMap : public QObject
{
    Q_OBJECT
public:
    explicit GameMap(QObject *parent = 0);
    explicit GameMap(QObject *parent = 0, QString mName = "No Name", QPoint startCoords = QPoint(0,0));

    // Name map
    QString name;

    // Start coords
    int playerX, playerY;

    // Masks
    QVector< CMask* > cMasks;

signals:

public slots:
    void addFigure(CMask *cMask);
};

#endif // GAMEMAP_H
