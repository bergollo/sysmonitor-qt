import QtQuick

Rectangle {
    property string title: ""
    property string valueText: ""
    property real barValue: -1
    property color barColor: "#54b8ff"

    height: barValue >= 0 ? 110 : 70
    radius: 8
    color: "#2b313b"

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 8

        Text {
            text: title + "  " + valueText
            color: "#f2f4f8"
            font.pixelSize: 20
        }

        Rectangle {
            visible: barValue >= 0
            width: parent.width
            height: 12
            radius: 6
            color: "#15181d"

            Rectangle {
                width: parent.width * Math.max(0, Math.min(1, barValue))
                height: parent.height
                radius: parent.radius
                color: barColor
            }
        }
    }
}
