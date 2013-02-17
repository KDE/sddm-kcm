import QtQuick 1.1

Rectangle {
    id: root
    color: "#b7bdc0"
    width: 450
    height:450
    /*anchors {
        fill: parent
    }*/

    /*border.width: 3
    border.color: "#aaff00ff"*/

    Image {
        id: previewImage
        clip: true
        smooth: true
        width: 350
        height: 200
        source: previewPath
        //source: "../../sddm-themes/elarun/screenshot.png"
        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        fillMode: Image.PreserveAspectFit
    }

    Rectangle { // rounded corners for img
        id: border
        width: previewImage.paintedWidth + 2
        height: previewImage.paintedHeight + 2
        color: "transparent"
        border.color: "#aa000000" // color of background
        border.width: 2
        radius: 4
        anchors.centerIn: previewImage
    }

    Rectangle {
        id: rectangle1
        width: 430
        height: 80
        anchors {
            topMargin: -20
            top: previewImage.bottom
            left: root.left
            right: root.right
        }

        color: "#bb000000"
        border.color: "#000000"
        Column {
            anchors {
                margins: 10
                fill:parent
            }

            spacing: 4

            Text {
                id: name
                text: themeName
                color: "white"
                font.bold: true
                font.pointSize: 13
                anchors {
                    //horizontalCenter: parent.horizontalCenter
                    right: parent.right
                }
            }
            Text {
                text: authorName
                color: "white"
                font.pointSize: 10
                anchors {
                    //horizontalCenter: parent.horizontalCenter
                    right: parent.right
                }
            }
            Text {
                text: copyright
                color: "white"
                font.bold: true
                font.pointSize: 7
                anchors {
                    //horizontalCenter: parent.horizontalCenter
                    right: parent.right
                }
            }
            Text {
                text: license
                color: "white"
                font.bold: true
                font.pointSize: 7
                anchors {
                    //horizontalCenter: parent.horizontalCenter
                    right: parent.right
                }
            }
        }

    }

    Column {
        anchors {
            margins: 10
            top: rectangle1.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        spacing: 4

        Text {
            text: version
            font.pointSize: 9
            anchors {
                left: parent.left
                right: parent.right
            }
        }

        Text {
            text: description
            font.pointSize: 9
            wrapMode: Text.WordWrap
            anchors {
                left: parent.left
                right: parent.right
            }
        }

        Text {
            text: email
            font.pointSize: 9
            anchors {
                left: parent.left
                right: parent.right
            }
        }

        Text {
            text: website
            font.pointSize: 9
            anchors {
                left: parent.left
                right: parent.right
            }
        }


    }




}
