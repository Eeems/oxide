import QtQuick 2.10
import QtQuick.Controls 2.4

Menu {
    delegate: MenuItem {
        id: menuItem
        implicitWidth: 250
        implicitHeight: 60
        arrow: Canvas {
            x: parent.width - width
            implicitWidth: 40
            implicitHeight: 40
            visible: menuItem.subMenu
            onPaint: {
                var ctx = getContext("2d")
                ctx.fillStyle = "black"
                ctx.moveTo(15, 15)
                ctx.lineTo(width - 15, height / 2)
                ctx.lineTo(15, height - 15)
                ctx.closePath()
                ctx.fill()
            }
        }
        indicator: Item {
            implicitWidth: 40
            implicitHeight: 40
            Rectangle {
                width: 26
                height: 26
                anchors.centerIn: parent
                visible: menuItem.checkable
                border.color: "black"
                radius: 3
                Rectangle {
                    width: 14
                    height: 14
                    anchors.centerIn: parent
                    visible: menuItem.checked
                    color: "black"
                    radius: 2
                }
            }
        }
        contentItem: Text {
            leftPadding: menuItem.checkable ? menuItem.indicator.width : 0
            rightPadding: menuItem.subMenu ? menuItem.arrow.width : 0
            text: menuItem.text
            font: menuItem.font
            opacity: enabled ? 1.0 : 0.3
            color: "black"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        background: Rectangle {
            implicitWidth: menuItem.implicitWidth
            implicitHeight: menuItem.implicitHeight
            opacity: enabled ? 1 : 0.3
            color: "transparent"
        }
    }
}
