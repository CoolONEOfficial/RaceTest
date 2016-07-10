#include <widget.h>

#include <QApplication>

void Widget::buttonClick(int id)
{
    if(scene == scenesMap["main"])
    {
        // Play Button
        if(id == buttonsMap[scenesMap["main"]]["Play"])
        {
            setScene(scenesMap["maps"]);
        }

        //  Exit Button
        else if(id == buttonsMap[scenesMap["main"]]["Exit"])
        {
            QApplication::quit();
        }
    }

    else if(scene == scenesMap["maps"])
    {
        // Set gMap
        gMap = id;

        // Set scene
        setScene(scenesMap["game"]);
    }
}
