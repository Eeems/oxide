import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import "./widgets"
import "page.js" as JS

ApplicationWindow {
    id: window
    objectName: "window"
    visible: stateController.state !== "loading"
    width: screenGeometry.width
    height: screenGeometry.height
    title: page.url
    FontLoader { id: iconFont; source: "/font/icomoon.ttf" }
    Component.onCompleted: {
        controller.startup();
        page.open("https://eink.link");
    }
    header: Rectangle {
        color: "black"
        height: menu.height
        RowLayout {
            id: menu
            anchors.left: parent.left
            anchors.right: parent.right
            Label {
                text: "⬅️"
                color: "white"
                topPadding: 5
                bottomPadding: 5
                leftPadding: 10
                rightPadding: 10
                MouseArea {
                    anchors.fill: parent
                    onClicked: page.back()
                }
            }
            Item { Layout.fillWidth: true }
            TextInput {
                id: input
                color: "white"
                text: "about:blank"
                onFocusChanged: keyboard.visible = focus;
                onAccepted: {
                    page.open(text);
                    focus = false;
                }
            }
            Item { Layout.fillWidth: true }
            CustomMenu {
                visible: stateController.state === "loaded"
                BetterMenu {
                    title: ""
                    font: iconFont.name
                    width: 310
                }
            }
        }
    }
    contentData: [
        Rectangle {
            anchors.fill: parent
            color: "white"
            enabled: stateController.state == "loaded"
            visible: enabled
            Text {
                id: page
                objectName: "page"
                text: ""
                onLinkActivated: open(link)
                focus: true
                Keys.enabled: true
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                property string url: "about:blank"
                property var history: []
                function open(url){
                    history.push(this.url);
                    load(url);
                }
                function back(url){
                    if(!history.length){
                        input.text = url = "about:blank"
                        text = "";
                        return;
                    }
                    load(history.pop());
                }
                function load(url){
                    var req = new XMLHttpRequest();
                    req.onreadystatechange = function(){
                        if(req.readyState === XMLHttpRequest.DONE){
                            input.text = page.url = url;
                            page.y = 100;
                            page.text = "<link type='stylesheet' href='page.css'/>" + JS.replace_all_rel_by_abs(url, req.responseText);
                            console.log("Loaded: " + url);
                        }
                    };
                    req.onerror = function(e){
                        console.log(e);
                        // TODO handle displaying error
                    };
                    req.open("GET", url);
                    req.send();
                }
                anchors { left: parent.left; right: parent.right; }
                anchors.margins: 50;
            }
        },
        Keyboard { id: keyboard; visible: false }
    ]
    StateGroup {
        id: stateController
        objectName: "stateController"
        state: "loading"
        states: [
            State { name: "loaded" },
            State { name: "loading" }
        ]
        transitions: [
            Transition {
                from: "*"; to: "loaded"
                SequentialAnimation {
                    ScriptAction { script: {
                        console.log("Display loaded");
                    } }
                }
            },
            Transition {
                from: "*"; to: "loading"
                SequentialAnimation {
                    ScriptAction { script: {
                        console.log("Loading display");
                        controller.startup();
                    } }
                }
            }
        ]
    }
}
