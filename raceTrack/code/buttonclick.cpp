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

void Widget::setCustomCursor()
{
    if(scene != scenesMap["game"])
    {
        setCursor(cursor);
        for(int f = 0; f<buttons[scene].size(); f++)
        {
            if(buttons[scene][f]->inside(move.x(), move.y()))
            {
                if(clicked)
                {
                    setCursor(cursorCHand);
                }
                else
                {
                    setCursor(cursorHand);
                }
            }
        }
    }
    else
    {
        setCursor(Qt::BlankCursor);
    }
}
