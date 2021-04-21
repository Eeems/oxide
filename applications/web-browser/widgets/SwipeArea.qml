/* This code was written by Sergejs Kovrovs and has been placed in the public domain. */
// https://gist.github.com/kovrov/1742405#file-swipearea-qml

import QtQuick 2.0


MouseArea {
    property point origin
    property bool ready: false
    signal move(int x, int y)
    signal swipe(string direction)

    onPressed: {
        drag.axis = Drag.XAndYAxis;
        origin = Qt.point(mouse.x, mouse.y);
        mouse.accepted = true;
    }

    onPositionChanged: {
        switch (drag.axis) {
            case Drag.XAndYAxis:
                if (Math.abs(mouse.x - origin.x) > 16) {
                    drag.axis = Drag.XAxis;
                }
                else if (Math.abs(mouse.y - origin.y) > 16) {
                    drag.axis = Drag.YAxis;
                }
                break;
            case Drag.XAxis:
                move(mouse.x - origin.x, 0);
                break;
            case Drag.YAxis:
                move(0, mouse.y - origin.y);
                break;
        }
        mouse.accepted = true;
    }

    onReleased: {
        switch (drag.axis) {
            case Drag.XAndYAxis:
                canceled(mouse);
                break;
            case Drag.XAxis:
                swipe(mouse.x - origin.x < 0 ? "left" : "right");
                break;
            case Drag.YAxis:
                swipe(mouse.y - origin.y < 0 ? "up" : "down");
                break;
        }
        mouse.accepted = true;
    }
}
