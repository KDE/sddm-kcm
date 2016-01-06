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
import QtQuick.Layouts 1.1

Rectangle {
    id: root
    color: "#b7bdc0"
    width: 360
    height:300

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
        width: root.paintedWidth
        height: 210
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
        border.color: "#bb000000" // color of background
        border.width: 2
        radius: 2
        anchors.centerIn: previewImage
    }

        GridLayout {
            id: texttable
            width: previewImage.paintedWidth
                anchors {
                    top: previewImage.bottom
                    left: previewImage.left
                    right: previewImage.right
                }
                columns: 2

            Text {
                text: description
                font.bold: true
                font.pointSize: 13
            }
            Text {
                text: version
                font.bold: true
                font.pointSize: 10
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            }
            Text {
                text: authorName
                font.pointSize: 10
            }
            Text {
                text: license
                font.pointSize: 7
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            }
            Text {
                text: website
                font.pointSize: 7
            }
            Text {
                text: email
                font.pointSize: 7
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            }
        }
}
