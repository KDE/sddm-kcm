/*
    Copyright 2013 by Reza Fatahilah Shah <rshah0385@kireihana.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick 2.1

Rectangle {
    id: root
    color: "#b7bdc0"
    width: 450
    height:450

    property string themeName: ""
    property string previewPath: ""
    property string authorName: ""
    property string description: ""
    property string license: ""
    property string email: ""
    property string website: ""
    property string copyright: ""
    property string version: ""

    Image {
        id: previewImage
        clip: true
        smooth: true
        width: 350
        height: 200
        source: previewPath
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
            anchors {
                left: parent.left
                right: parent.right
            }
        }

        Text {
            text: description
            wrapMode: Text.WordWrap
            anchors {
                left: parent.left
                right: parent.right
            }
        }

        Text {
            text: email
            anchors {
                left: parent.left
                right: parent.right
            }
        }

        Text {
            text: website
            anchors {
                left: parent.left
                right: parent.right
            }
        }
    }
}
