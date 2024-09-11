/*
    SPDX-FileCopyrightText: 2013 Reza Fatahilah Shah <rshah0385@kireihana.com>
    SPDX-FileCopyrightText: 2019 Filip Fila <filipfila.kde@gmail.com>
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Window

import org.kde.kirigami as Kirigami

Kirigami.Dialog {
    id: dialog

    property string themeName: ""
    property string previewPath: ""
    property string authorName: ""
    property string description: ""
    property string license: ""
    property string email: ""
    property string website: ""
    property string version: ""

    title: i18nc("@title:window, %1 is the theme name, %2 the version", "%1 (%2)", themeName, version)

    padding: Kirigami.Units.largeSpacing

    ColumnLayout {
        id: layout

        spacing: Kirigami.Units.smallSpacing

        Kirigami.AbstractCard {
            id: card

            Layout.fillWidth: true
            implicitHeight: previewImage.implicitHeight + (previewImage.anchors.margins * 2)

            Image {
                id: previewImage

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    margins: Kirigami.Units.smallSpacing + card.background.borderWidth
                }

                source: dialog.previewPath
                fillMode: Image.PreserveAspectFit
                smooth: true
            }
        }

        Kirigami.PlaceholderMessage {
            visible: previewImage.status !== Image.Ready && previewImage.status !== Image.Loading
            icon.name: "view-preview"
            text: i18n("No preview available")
        }

        QQC2.Label {
            Layout.fillWidth: true

            text: i18nc("%1 is a description of the theme, %2 are the authors, %3 is the license", "%1, by %2 (%3)", dialog.description, dialog.authorName, dialog.license)
            wrapMode: Text.Wrap
        }

        QQC2.Label {
            Layout.fillWidth: true

            visible: dialog.website !== ""

            text:("<a href='"+dialog.website+"'>"+dialog.website+"</a>")
            font: Kirigami.Theme.smallFont
            wrapMode: Text.Wrap

            onLinkActivated: link => Qt.openUrlExternally(link)
        }

        QQC2.Label {
            Layout.fillWidth: true
            Layout.bottomMargin: Math.round(Kirigami.Units.smallSpacing * 1.5)

            visible: dialog.email !== ""

            text: ("<a href='"+dialog.email+"'>"+dialog.email+"</a>")
            font: Kirigami.Theme.smallFont
            wrapMode: Text.Wrap

            onLinkActivated: Qt.openUrlExternally("mailto:"+dialog.email+"")
        }
    }
}
