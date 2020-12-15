/*
    Copyright 2013 by Reza Fatahilah Shah <rshah0385@kireihana.com>
    Copyright 2019 by Filip Fila <filipfila.kde@gmail.com>
    Copyright 2020 David Redondo <kde@david-redondo.de>

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

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.14 as Kirigami

Kirigami.OverlaySheet {
    property string themeName: ""
    property string previewPath: ""
    property string authorName: ""
    property string description: ""
    property string license: ""
    property string email: ""
    property string website: ""
    property string version: ""
    header: Kirigami.Heading {
        level: 2
        text: i18nc("@title:window, %1 is the theme name, %2 the version", "%1 (%2)", themeName, version)
    }
    contentItem: ColumnLayout {
        Layout.preferredWidth: 0.75 * root.width
        Item {
            // we need this wrapper because of a limitation of Image where the implicitHeight is not adjusted when
            // the height has been set as the layout does
            Layout.fillWidth: true
            implicitHeight: previewImage.implicitHeight
            Image {
                id: previewImage
                readonly property bool available: status === Image.Ready || status === Image.Loading
                source: previewPath
                width: parent.width
                sourceSize.width: parent.width * Screen.devicePixelRatio
                fillMode: Image.PreserveAspectFit
                smooth: true
                DropShadow {
                    source: previewImage
                    anchors.fill: previewImage
                    visible: previewImage.available
                    verticalOffset: 2
                    radius: 10
                    samples: 32
                    z: -1
                    color: Qt.rgba(0, 0, 0, 0.3)
                }
            }
        }
        Kirigami.PlaceholderMessage {
            visible: !previewImage.available
            icon.name: "view-preview"
            text: i18n("No preview available")
        }
        QQC2.Label {
            text: i18nc("%1 is a description of the theme, %2 are the authors, %3 is the license", "%1, by %2 (%3)", description, authorName, license)
            Layout.fillWidth: true
            wrapMode: Text.Wrap
        }

        QQC2.Label {
            visible: website !== ""
            text:("<a href='"+website+"'>"+website+"</a>")
            onLinkActivated: Qt.openUrlExternally(link)
            font.pointSize: theme.smallestFont.pointSize
            Layout.fillWidth: true
            wrapMode: Text.Wrap
        }

        QQC2.Label {
            visible: email !== ""
            text: ("<a href='"+email+"'>"+email+"</a>")
            onLinkActivated: Qt.openUrlExternally("mailto:"+email+"")
            font.pointSize: theme.smallestFont.pointSize
            Layout.fillWidth: true
            Layout.bottomMargin: Math.round(units.smallSpacing * 1.5)
            wrapMode: Text.Wrap
        }
    }
}
