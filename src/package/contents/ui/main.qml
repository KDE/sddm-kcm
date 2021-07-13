/*
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/


import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.0


import org.kde.kcm 1.4 as KCM
import org.kde.kirigami 2.14 as Kirigami
import org.kde.newstuff 1.81 as NewStuff
import org.kde.private.kcms.sddm 1.0

KCM.GridViewKCM {
    id: root
    header: Kirigami.InlineMessage {
        id: errorMessage
        type: Kirigami.MessageType.Error
        showCloseButton: true
        Connections {
            target: kcm

            function onErrorOccured(message) {
                errorMessage.text = message;
                errorMessage.visible = true
            }

            function onSyncSuccessful() {
                syncSheet.close()
            }

            function onResetSyncedDataSuccessful() {
                syncSheet.close()
            }
        }
    }
    KCM.SettingStateBinding {
        configObject: kcm.sddmSettings
        settingName: "Current"
    }
    view.model: kcm.themesModel
    view.currentIndex: kcm.themesModel.currentIndex
    view.delegate: KCM.GridDelegate {
        id: delegate
        text: model.display
        subtitle: model.author
        thumbnailAvailable: true
        thumbnail: Image {
            anchors.fill: parent
            source: model.path + model.preview
            sourceSize: Qt.size(delegate.GridView.view.cellWidth * Screen.devicePixelRatio,
                                delegate.GridView.view.cellHeight * Screen.devicePixelRatio)
            fillMode: Image.PreserveAspectFit
            smooth: true
        }
        actions: [
            Kirigami.Action {
                iconName: "documentinfo"
                tooltip: i18nc("@info:tooltip", "View details")
                onTriggered: {
                    detailsDialog.themeName = model.display
                    detailsDialog.previewPath = model.path + model.preview
                    detailsDialog.authorName = model.author
                    detailsDialog.description = model.description
                    detailsDialog.license = model.license
                    detailsDialog.email = model.email
                    detailsDialog.website = model.website
                    detailsDialog.version = model.version
                    detailsDialog.open()
                }
            },
            Kirigami.Action {
                iconName: "games-config-background"
                tooltip: i18nc("@info:tooltip", "Change Background")
                onTriggered: {
                    backgroundSheet.modelIndex = view.model.index(index, 0)
                    backgroundSheet.imagePath = Qt.binding(() => model.currentBackground)
                    backgroundSheet.open()
                }
            },
            Kirigami.Action {
                iconName: "edit-delete"
                tooltip: i18nc("@info:tooltip", "Delete")
                onTriggered: kcm.removeTheme(view.model.index(index, 0))
            }
        ]
        onClicked: kcm.sddmSettings.current = model.id

        onDoubleClicked: {
            kcm.save();
        }
    }
    footer: Kirigami.ActionToolBar {
        flat: false
        alignment: Qt.AlignRight
        actions: [
            Kirigami.Action {
                text: i18nc("@action:button", "Behavior...")
                icon.name: "settings-configure"
                onTriggered: { kcm.push("Advanced.qml") }
            },
            Kirigami.Action {
                text: i18nc("@action:button", "Synchronize Settings...")
                icon.name: "view-refresh"
                onTriggered: syncSheet.open()
            },
            Kirigami.Action {
                text: i18nc("@action:button", "Install From File...")
                icon.name: "document-import"
                onTriggered: themeDialog.open()
            },
            NewStuff.Action {
                text: i18nc("@action:button", "Get New SDDM Themes...")
                configFile: "sddmtheme.knsrc"
                function onEntryEvent(entry, event) {
                    kcm.themesModel.populate();
                }
            }
        ]
    }
    FileDialog {
        id: themeDialog
        selectExisting: true
        onAccepted: kcm.installTheme(fileUrl) 
    }
    DetailsDialog {
        id: detailsDialog
    }
    Kirigami.OverlaySheet {
        id: syncSheet
        header: Kirigami.Heading {
            text: i18nc("@title:window", "Settings Synchronization")
            level: 2
        }
        contentItem: Kirigami.InlineMessage {
            implicitWidth: Kirigami.Units.gridUnit * 14
            visible: true
            type: Kirigami.MessageType.Information
            font: Kirigami.Theme.smallFont
            text: i18n("Settings synchronization allows you to transfer the following Plasma settings to SDDM:") +
                xi18nc("@info", "<para><list><item>color scheme,</item><item>cursor theme,</item><item>font,</item><item>font rendering,</item><item>NumLock preference,</item><item>Plasma theme,</item><item>scaling DPI,</item><item>Screen configuration (Wayland only)</item></list></para>") +
                i18n("Please note that theme files must be installed globally for settings synchronization to work.")
        }
        footer: RowLayout {
            Item {
                Layout.fillWidth: true
            }
            QQC2.Button {
                text: i18nc("@action:button", "Synchronize")
                icon.name: "view-refresh"
                onClicked: kcm.synchronizeSettings()
            }
            QQC2.Button {
                text: i18nc("@action:button", "Reset")
                icon.name: "edit-undo"
                onClicked: kcm.resetSyncronizedSettings()
            }
        }
    }
    Kirigami.OverlaySheet {
        id: backgroundSheet
        property var modelIndex
        property string imagePath
        header: Kirigami.Heading {
            text: i18nc("@title:window", "Change Background")
            level: 2
        }
        contentItem: Item {
            implicitWidth: 0.75 * root.width
            implicitHeight: backgroundImage.hasImage ? backgroundImage.implicitHeight : placeHolder.implicitHeight
            Kirigami.PlaceholderMessage  {
                id: placeHolder
                anchors.fill: parent
                visible: !backgroundImage.hasImage
                icon.name: "view-preview"
                text: i18n("No image selected")
            }
            Image {
                id: backgroundImage
                readonly property bool hasImage: status == Image.Ready || status == Image.Loading
                width: parent.width
                source: backgroundSheet.imagePath
                sourceSize.width: width * Screen.devicePixelRatio
                fillMode: Image.PreserveAspectFit
            }
        }
        footer: RowLayout {
            Item {
                Layout.fillWidth: true
            }
            QQC2.Button {
                icon.name: "document-open"
                text: i18nc("@action:button", "Load From File...")
                onClicked: imageDialog.open()
            }
            QQC2.Button {
                icon.name: "edit-clear"
                text: i18nc("@action:button", "Clear Image")
                onClicked: {
                    view.model.setData(backgroundSheet.modelIndex, "", ThemesModel.CurrentBackgroundRole)
                    backgroundSheet.close()
                }
            }
            FileDialog {
                id: imageDialog
                selectExisting: true
                onAccepted: {
                    view.model.setData(backgroundSheet.modelIndex, kcm.toLocalFile(fileUrl), ThemesModel.CurrentBackgroundRole)
                    backgroundSheet.close()
                }
            }
        }
    }
}
