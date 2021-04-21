import QtQuick 2.6
import QtQuick.Controls 2.4

MenuBar {
    delegate: MenuBarItem {
        id: menuBarItem
        contentItem: Text {
            text: menuBarItem.text
            font: menuBarItem.font
            opacity: enabled ? 1.0 : 0.3
            color: "white"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        background: Rectangle {
            implicitWidth: 40
            implicitHeight: 40
            opacity: enabled ? 1 : 0.3
            color: "black"
        }
    }
    background: Rectangle {
        implicitWidth: 40
        implicitHeight: 40
        color: "black"
    }
}
