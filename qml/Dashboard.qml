import QtQuick
import "components"

Rectangle {
    color: "#20242b"

    Column {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 18

        Text {
            text: "QML Dashboard"
            color: "#f2f4f8"
            font.pixelSize: 26
            font.bold: true
        }

        Text {
            text: "The same statsReady signal, exposed through StatsViewModel"
            color: "#aeb6c2"
        }

        StatCard {
            width: parent.width
            title: "CPU"
            valueText: statsModel ? statsModel.cpuPercent.toFixed(1) + "%" : "waiting"
            barValue: statsModel ? statsModel.cpuPercent / 100 : -1
            barColor: "#54b8ff"
        }

        StatCard {
            width: parent.width
            title: "Memory"
            valueText: statsModel
                ? statsModel.memoryUsedMb + " / " + statsModel.memoryTotalMb + " MiB"
                : "waiting"
            barValue: statsModel ? statsModel.memoryPercent / 100 : -1
            barColor: "#9b8cff"
        }

        StatCard {
            width: parent.width
            title: "Temperature"
            valueText: statsModel ? statsModel.temperature : "waiting"
            barValue: -1
        }

        Text {
            text: "CPU samples: " + (statsModel ? statsModel.cpuHistory.length : 0)
            color: "#aeb6c2"
        }
    }
}
