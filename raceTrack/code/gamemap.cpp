#include "gamemap.h"

GameMap::GameMap(QObject *parent) : QObject(parent)
{
    playerX = 0;
    playerY = 0;
}

GameMap::GameMap(QObject *parent, QString mName, QPoint startCoords)
{
    name = mName;

    playerX = startCoords.x();
    playerY = startCoords.y();
}

void GameMap::addFigure(CMask *cMask)
{
    // Add figure cMask
    cMasks.push_back(cMask);
}
