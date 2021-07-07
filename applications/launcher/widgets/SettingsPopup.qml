import QtQuick 2.10
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0

Item {
    id: root
    x: (parent.width / 2) - (settings.width / 2)
    y: (parent.height / 2) - (settings.height / 2)
    signal closed
    Popup {
        id: settings
        width: 1000
        height: 1000
        closePolicy: Popup.NoAutoClose
        onClosed: parent.closed()
        visible: parent.visible
        GridLayout {
            columns: 3
            rows: 10
            anchors.fill: parent
            RowLayout {
                Layout.columnSpan: parent.columns
                Label {
                    text: "Automatic sleep"
                    Layout.columnSpan: parent.columns - 1
                    Layout.fillWidth: true
                }
                BetterCheckBox {
                    tristate: false
                    checkState: controller.automaticSleep ? Qt.Checked : Qt.Unchecked
                    onClicked: controller.automaticSleep = this.checkState === Qt.Checked
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    Layout.fillWidth: false
                }
            }
            RowLayout {
                Layout.columnSpan: parent.columns
                Layout.preferredWidth: parent.width
                enabled: controller.automaticSleep
                Label {
                    text: "Sleep After (minutes)"
                    Layout.fillWidth: true
                }
                BetterSpinBox {
                    id: sleepAfterSpinBox
                    objectName: "sleepAfterSpinBox"
                    from: 1
                    to: 360
                    stepSize: 1
                    value: controller.sleepAfter
                    onValueChanged: {
                        if(this.ignoreNextValueChange){
                            this.ignoreNextValueChange = false;
                            return;
                        }
                        var offset = 0;
                        if(this.value > 10){
                            offset = 4;
                        }
                        if(this.value > 60){
                            offset = 14;
                        }
                        // this event handler will fire a second time
                        // if offset is non-zero
                        this.ignoreNextValueChange = offset !== 0;
                        this.lastDirectionUp ?
                            this.value += offset : this.value -= offset;
                        controller.sleepAfter = this.value;
                    }
                    Layout.preferredWidth: 300
                }
            }
            RowLayout {
                Layout.columnSpan: parent.columns
                Label {
                    text: "Show Battery Percent"
                    Layout.columnSpan: parent.columns - 1
                    Layout.fillWidth: true
                }
                BetterCheckBox {
                    tristate: false
                    checkState: controller.showBatteryPercent ? Qt.Checked : Qt.Unchecked
                    onClicked: controller.showBatteryPercent = this.checkState === Qt.Checked
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    Layout.fillWidth: false
                }
            }
            RowLayout {
                Layout.columnSpan: parent.columns
                Label {
                    text: "Show Battery Temperature"
                    Layout.columnSpan: parent.columns - 1
                    Layout.fillWidth: true
                }
                BetterCheckBox {
                    tristate: false
                    checkState: controller.showBatteryTemperature ? Qt.Checked : Qt.Unchecked
                    onClicked: controller.showBatteryTemperature = this.checkState === Qt.Checked
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    Layout.fillWidth: false
                }
            }
            RowLayout {
                Layout.columnSpan: parent.columns
                Label {
                    text: "Show Wifi DB"
                    Layout.columnSpan: parent.columns - 1
                    Layout.fillWidth: true
                }
                BetterCheckBox {
                    tristate: false
                    checkState: controller.showWifiDb ? Qt.Checked : Qt.Unchecked
                    onClicked: controller.showWifiDb = this.checkState === Qt.Checked
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    Layout.fillWidth: false
                }
            }
            RowLayout {
                Layout.columnSpan: parent.columns
                Layout.preferredWidth: parent.width
                Label {
                    text: "Show Date"
                    Layout.fillWidth: true
                }
                BetterCheckBox {
                    tristate: false
                    checkState: controller.showDate ? Qt.Checked : Qt.Unchecked
                    onClicked: controller.showDate = this.checkState === Qt.Checked
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    Layout.fillWidth: false
                }
            }
            RowLayout {
                Layout.columnSpan: parent.columns
                Layout.preferredWidth: parent.width
                Label {
                    text: "Home Screen Columns"
                    Layout.fillWidth: true
                }
                BetterSpinBox {
                    id: columnsSpinBox
                    objectName: "columnsSpinBox"
                    from: 2
                    to: 10
                    stepSize: 2
                    value: controller.columns
                    onValueChanged: controller.columns = this.value
                    Layout.preferredWidth: 300
                }
            }
            Item {
                Layout.rowSpan: 6
                Layout.columnSpan: parent.columns
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
            BetterButton {
                text: "Reset"
                Layout.columnSpan: parent.columns
                Layout.fillWidth: true
                onClicked: controller.loadSettings()
            }
            BetterButton {
                text: "Close"
                Layout.columnSpan: parent.columns
                Layout.fillWidth: true
                onClicked: {
                    controller.saveSettings();
                    settings.close();
                }
            }
        }
    }
}
