import QtQuick 2.10
import QtQuick.Controls 2.4

Label {
    property string source
    color: "white"
    leftPadding: icon.width + 5
    Image {
        id: icon
        source: parent.source
        height: parent.height - 10
        width: height
        fillMode: Image.PreserveAspectFit
    }
}
