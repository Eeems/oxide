import QtQuick 2.9

Item {
    id: root
    clip: true
    property string text: ""

    property string color: "black"
    property string backgroundColor: "transparent"
    property string borderColor: "black"
    property string selectedColor: "transparent"
    property string selectedBackgroundColor: "black"
    property string selectedBorderColor: "transparent"
    property int transitionTime: 300
    property int fontsize: 8
    property int borderwidth: 5
    property bool toggle: false
    signal click()
    signal hold()
    signal release()


    property bool isSelected: false
    property bool isHeld: false

    Rectangle {
        id: background
        anchors.fill: root
        color: root.backgroundColor
        border.color: root.borderColor
        border.width: borderwidth
        Text {
            id: label
            color: root.color
            text: root.text
            anchors.centerIn: parent
            font.pointSize: fontsize
        }
    }
    MouseArea {
        anchors.fill: root
        hoverEnabled: true
        onClicked: {
            if(root.toggle){
                root.isSelected = !root.isSelected;
            }else{
                root.isSelected = true;
                timer.start();
            }
            console.log("click (" + this + ") " +  root.isSelected);
            root.click()
        }
        onPressAndHold: {
            root.isSelected = true;
            root.isHeld = true;
            console.log("hold " + this);
            root.hold()
        }
        onReleased: {
            if(root.isSelected){
                if(root.isHeld){
                    root.isSelected = false;
                }
                console.log("release (" + this + ")");
                root.release()
            }
        }
    }
    state: "resting"
    states: [
        State { name: "resting"; when: !root.isSelected },
        State { name: "pressed"; when: root.isSelected }
    ]
    transitions: [
        Transition {
            from: "resting"
            to: "pressed"
            SequentialAnimation {
                PropertyAction { target: label; property: "color"; value: root.color }
                PropertyAction { target: background; property: "color"; value: root.backgroundColor }
                PropertyAction { target: background; property: "border.color"; value: root.borderColor }
                PauseAnimation { duration: root.transitionTime }
                PropertyAction { target: label; property: "color"; value: root.selectedColor }
                PropertyAction { target: background; property: "color"; value: root.selectedBackgroundColor }
                PropertyAction { target: background; property: "border.color"; value: root.selectedBorderColor }
            }
        },
        Transition {
            from: "pressed"
            to: "resting"
            SequentialAnimation {
                PropertyAction { target: label; property: "color"; value: root.selectedColor }
                PropertyAction { target: background; property: "color"; value: root.selectedBackgroundColor }
                PropertyAction { target: background; property: "border.color"; value: root.selectedBorderColor }
                PauseAnimation { duration: root.transitionTime }
                PropertyAction { target: label; property: "color"; value: root.color }
                PropertyAction { target: background; property: "color"; value: root.backgroundColor }
                PropertyAction { target: background; property: "border.color"; value: root.borderColor }
            }
        }
    ]
    Timer {
        id: timer
        interval: 1
        repeat: false
        onTriggered: {
            root.isSelected = false;
            console.log("click clear (" + this + ") " +  root.isSelected);
        }
    }
}
