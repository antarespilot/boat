import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

Window {
    visible: true
    color: "black"

    property int fontSize: 70

    Rectangle {
        color: (imudata.ad2 > 0.5) ? "red" : "blue"
        radius: 25
        width: 100
        height: 100

        x: (parent.width-width)*imudata.ad0
        y: (parent.height-height)*imudata.ad1
    }

    Text {
        anchors.top: parent.top
        anchors.left: parent.left
        text: "PROPELL!" + imudata.ad3
        font.pixelSize: fontSize
        color: "white"
    }

    ColumnLayout {
        anchors.centerIn: parent
        transform: [
            Rotation { axis.x: 1; axis.y: 0; axis.z: 0; angle:  imudata.pose.y },
            Rotation { axis.x: 0; axis.y: 1; axis.z: 0; angle: -imudata.pose.z },
            Rotation { axis.x: 0; axis.y: 0; axis.z: 1; angle: -imudata.pose.x }
        ]

        spacing: 50
        RowLayout {

            Text {
                width: 100
                font.pixelSize: fontSize
                color: "yellow"
                text: "Roll: " + imudata.pose.x.toFixed(1)
            }
            Text {
                width: 100
                font.pixelSize: fontSize
                color: "yellow"
                text: "Pitch: " + imudata.pose.y.toFixed(1)
            }
            Text {
                width: 100
                font.pixelSize: fontSize
                color: "yellow"
                text: "Yaw: " + imudata.pose.z.toFixed(1)
            }
        }

        RowLayout {

            Text {
                width: 100
                font.pixelSize: fontSize
                color: "yellow"
                text: "Lat " + imudata.geoCoordinate.latitude.toFixed(6)
            }
            Text {
                width: 100
                font.pixelSize: fontSize
                color: "yellow"
                text: "Lon " + imudata.geoCoordinate.longitude.toFixed(6)
            }
        }
    }

    SequentialAnimation {
        NumberAnimation {
            target: imudata
            properties: "servoPosition"
            from: 1000
            to: 1600
            duration: 2000
            easing.type: Easing.OutBounce
        }
        NumberAnimation {
            target: imudata
            properties: "servoPosition"
            to: 1000
            duration: 2000
            easing.type: Easing.OutBounce
        }
        loops: Animation.Infinite
        running: true
    }
}
