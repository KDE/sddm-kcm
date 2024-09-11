/*
    SPDX-FileCopyrightText: 2013 Reza Fatahilah Shah <rshah0385@kireihana.com>
    SPDX-FileCopyrightText: 2019 Filip Fila <filipfila.kde@gmail.com>
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.kde.kirigami as Kirigami

Kirigami.OverlaySheet {
    property string themeName: ""
    property string previewPath: ""
    property string authorName: ""
    property string description: ""
    property string license: ""
    property string email: ""
    property string website: ""
    property string version: ""
    title: i18nc("@title:window, %1 is the theme name, %2 the version", "%1 (%2)", themeName, version)
    ColumnLayout {
        id: layout
        Layout.preferredWidth: 0.75 * root.width
        spacing: Kirigami.Units.smallSpacing

        Kirigami.AbstractCard {
            id: card

            Layout.fillWidth: true
            implicitHeight: previewImage.implicitHeight + (previewImage.anchors.margins * 2)

            Image {
                id: previewImage
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Kirigami.Units.smallSpacing + card.background.borderWidth
                source: previewPath
                fillMode: Image.PreserveAspectFit
                smooth: true
                clip: true
            }
        }
        Kirigami.PlaceholderMessage {
            visible: previewImage.status !== Image.Ready && previewImage.status !== Image.Loading
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
            onLinkActivated: link => Qt.openUrlExternally(link)
            font: Kirigami.Theme.smallFont
            Layout.fillWidth: true
            wrapMode: Text.Wrap
        }

        QQC2.Label {
            visible: email !== ""
            text: ("<a href='"+email+"'>"+email+"</a>")
            onLinkActivated: Qt.openUrlExternally("mailto:"+email+"")
            font: Kirigami.Theme.smallFont
            Layout.fillWidth: true
            Layout.bottomMargin: Math.round(Kirigami.Units.smallSpacing * 1.5)
            wrapMode: Text.Wrap
        }
    }
}
