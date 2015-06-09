import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

Window {
    visible: true

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 50
        RowLayout {

            Text {
                width: 100
                font.pixelSize: 40
                text: "Roll: " + imudata.pose.x.toFixed(1)
            }
            Text {
                width: 100
                font.pixelSize: 40
                text: "Pitch: " + imudata.pose.y.toFixed(1)
            }
            Text {
                width: 100
                font.pixelSize: 40
                text: "Yaw: " + imudata.pose.z.toFixed(1)
            }
        }

        RowLayout {

            Text {
                width: 100
                font.pixelSize: 40
                text: "Lat " + imudata.geoCoordinate.latitude.toFixed(6)
            }
            Text {
                width: 100
                font.pixelSize: 40
                text: "Lon " + imudata.geoCoordinate.longitude.toFixed(6)
            }
        }
    }


}
