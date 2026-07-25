import QtQuick

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

        Rectangle {
            width: parent.width
            height: 110
            radius: 8
            color: "#2b313b"

            Column {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 8

                Text {
                    text: "CPU  " + statsModel.cpuPercent.toFixed(1) + "%"
                    color: "#f2f4f8"
                    font.pixelSize: 20
                }

                Rectangle {
                    width: parent.width
                    height: 12
                    radius: 6
                    color: "#15181d"

                    Rectangle {
                        width: parent.width * statsModel.cpuPercent / 100
                        height: parent.height
                        radius: parent.radius
                        color: "#54b8ff"
                    }
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 110
            radius: 8
            color: "#2b313b"

            Column {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 8

                Text {
                    text: "Memory  " + statsModel.memoryUsedMb + " / "
                          + statsModel.memoryTotalMb + " MiB"
                    color: "#f2f4f8"
                    font.pixelSize: 20
                }

                Rectangle {
                    width: parent.width
                    height: 12
                    radius: 6
                    color: "#15181d"

                    Rectangle {
                        width: parent.width * statsModel.memoryPercent / 100
                        height: parent.height
                        radius: parent.radius
                        color: "#9b8cff"
                    }
                }
            }
        }

        Text {
            text: "Temperature  " + statsModel.temperature
            color: "#f2f4f8"
            font.pixelSize: 20
        }

        Text {
            text: "CPU samples: " + statsModel.cpuHistory.length
            color: "#aeb6c2"
        }
    }
}
