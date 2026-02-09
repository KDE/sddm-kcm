/*
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/


import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Dialogs
import QtMultimedia 6.9
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15


import org.kde.kcmutils as KCM
import org.kde.kirigami as Kirigami
import org.kde.newstuff 1.81 as NewStuff
import org.kde.private.kcms.sddm 1.0

KCM.GridViewKCM {
    id: root
    actions: [
        Kirigami.Action {
            text: i18nc("@action:button", "Behavior…")
            icon.name: "settings-configure"
            onTriggered: { kcm.push("Advanced.qml") }
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Apply Plasma Settings…")
            icon.name: "plasma"
            onTriggered: syncSheet.open()
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Install From File…")
            icon.name: "document-import"
            onTriggered: themeDialog.open()
        },
        NewStuff.Action {
            text: i18nc("@action:button as in, \"get new SDDM themes\"", "Get New…")
            configFile: "sddmtheme.knsrc"
            onEntryEvent: function(entry, event) {
                kcm.themesModel.populate();
            }
        }
    ]

    headerPaddingEnabled: false // Let the InlineMessage touch the edges
    header: Kirigami.InlineMessage {
        id: errorMessage
        position: Kirigami.InlineMessage.Position.Header
        type: Kirigami.MessageType.Error
        showCloseButton: true
        Connections {
            target: kcm

            function onErrorOccured(untranslatedMessage) {
                errorMessage.text = i18n(untranslatedMessage);
                errorMessage.visible = untranslatedMessage.length > 0
            }

            function onSyncAttempted() {
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
            source: model.preview
            sourceSize: Qt.size(delegate.GridView.view.cellWidth * Screen.devicePixelRatio,
                                delegate.GridView.view.cellHeight * Screen.devicePixelRatio)
            fillMode: Image.PreserveAspectFit
            smooth: true
        }
        actions: [
            Kirigami.Action {
                icon.name: "documentinfo"
                tooltip: i18nc("@info:tooltip", "View details")
                onTriggered: {
                    detailsDialog.themeName = model.display
                    detailsDialog.previewPath = model.preview
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
                icon.name: "games-config-background"
                tooltip: i18nc("@info:tooltip", "Change background")
                onTriggered: {
                    const index = view.model.index(view.currentIndex, 0)
                    backgroundSheet.modelIndex = index
                    backgroundSheet.showClock = view.model.data(index, ThemesModel.ShowClockRole)
                    backgroundSheet.imagePath = Qt.binding(() => model.currentBackground)
                    backgroundSheet.open()
                }
            },
            Kirigami.Action {
                icon.name: "edit-delete"
                tooltip: if (enabled) {
                    return i18nc("@info:tooltip", "Remove login screen theme");
                } else if (model.deletable) {
                    return i18nc("@info:tooltip", "Cannot delete the active login screen theme");
                } else {
                    return i18nc("@info:tooltip", "Cannot delete system-installed login screen themes");
                }
                onTriggered: kcm.removeTheme(view.model.index(index, 0))
                enabled: model.deletable && !delegate.GridView.isCurrentItem
            }
        ]
        onClicked: kcm.sddmSettings.current = model.id

        onDoubleClicked: {
            kcm.save();
        }
    }
    FileDialog {
        id: themeDialog
        onAccepted: kcm.installTheme(selectedFile)
    }
    DetailsDialog {
        id: detailsDialog

        implicitWidth: 0.75 * root.width
    }
    Kirigami.PromptDialog {
        id: syncSheet

        padding: Kirigami.Units.largeSpacing
        standardButtons: Kirigami.Dialog.Cancel

        title: i18nc("@title:window", "Apply Plasma Settings")
        subtitle: i18n("This will make the SDDM login screen reflect your customizations to the following Plasma settings:") +
                xi18nc("@info", "<para><list><item>Color scheme</item><item>Cursor theme and size</item><item>Font and font rendering</item><item>NumLock preference</item><item>Plasma theme</item><item>Scaling DPI</item><item>Screen configuration (Wayland only)</item></list></para>") +
                i18n("Please note that theme files must be installed globally to be reflected on the SDDM login screen.")

        customFooterActions: [
            Kirigami.Action {
                text: i18nc("@action:button", "Apply")
                icon.name: "dialog-ok-apply"
                onTriggered: kcm.synchronizeSettings()
            },
            Kirigami.Action {
                text: i18nc("@action:button", "Reset to Default Settings")
                icon.name: "edit-undo"
                onTriggered: kcm.resetSyncronizedSettings()
            }
        ]
    }
    Kirigami.Dialog {
        id: backgroundSheet
        property var modelIndex
        property string imagePath
        property bool showClock

        title: i18nc("@title:window", "Change Background")

        padding: Kirigami.Units.largeSpacing

        Kirigami.AbstractCard {
            id: card

            implicitWidth: 0.75 * root.width
            implicitHeight: contentHeight

            readonly property bool isVideo: backgroundSheet.imagePath.toLowerCase().endsWith(".mp4")
                                            || backgroundSheet.imagePath.toLowerCase().endsWith(".mov")
                                            || backgroundSheet.imagePath.toLowerCase().endsWith(".webm")
                                            || backgroundSheet.imagePath.toLowerCase().endsWith(".avi")
                                            || backgroundSheet.imagePath.toLowerCase().endsWith(".mkv")

            readonly property real contentHeight: {

                if (backgroundImage.hasLoadedImage || backgroundVideo.hasLoadedVideo) {
                    if (isVideo) {
                        var resolution = backgroundVideo.metaData.value(MediaMetaData.Resolution)

                        if (resolution) {
                            return ((resolution.height / resolution.width) * card.implicitWidth) + backgroundVideo.anchors.margins
                        }
                    }
                    else {
                        return backgroundImage.implicitHeight + (backgroundImage.anchors.margins * 2)
                    }
                }

                return placeHolder.implicitHeight + (placeHolder.anchors.margins * 2)
            }

            Image {
                id: backgroundImage

                readonly property bool hasLoadedImage: status == Image.Ready || status == Image.Loading

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    margins: Kirigami.Units.smallSpacing + card.background.borderWidth
                }

                source: !card.isVideo ? "file:" + backgroundSheet.imagePath : ""

                fillMode: Image.PreserveAspectFit
                smooth: true
                visible: !card.isVideo
            }

            Video {
                id: backgroundVideo

                readonly property bool hasLoadedVideo: backgroundVideo.hasVideo && backgroundVideo.error == MediaPlayer.NoError

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    margins: Kirigami.Units.smallSpacing + card.background.borderWidth
                }

                onSourceChanged: {
                    if (source && source.toString().length != 0) {

                        backgroundVideo.play()
                    }
                }

                source: card.isVideo ? "file:" + backgroundSheet.imagePath : ""
                fillMode: VideoOutput.PreserveAspectFit
                loops: MediaPlayer.Infinite
                muted: true
                visible: card.isVideo
            }

            Kirigami.PlaceholderMessage  {
                id: placeHolder
                anchors.fill: parent
                anchors.margins: Kirigami.Units.largeSpacing * 4
                visible: !backgroundImage.hasLoadedImage && !backgroundVideo.hasLoadedVideo
                icon.name: "view-preview"
                text: i18n("No image selected")
            }
        }

        footerLeadingComponent: QQC2.CheckBox {
            text: i18nc("@option:check", "Show clock")
            checked: backgroundSheet.showClock
            onToggled: view.model.setData(backgroundSheet.modelIndex, checked, ThemesModel.ShowClockRole)
        }

        customFooterActions: [
            Kirigami.Action {
                icon.name: "document-open"
                text: i18nc("@action:button", "Load From File…")
                onTriggered: imageDialog.open()
            },
            Kirigami.Action {
                icon.name: "edit-clear"
                text: i18nc("@action:button", "Clear Image")
                onTriggered: {
                    view.model.setData(backgroundSheet.modelIndex, "", ThemesModel.CurrentBackgroundRole)
                    backgroundSheet.close()
                }
            }
        ]

        FileDialog {
            id: imageDialog
            onAccepted: {
                view.model.setData(backgroundSheet.modelIndex, kcm.toLocalFile(selectedFile), ThemesModel.CurrentBackgroundRole)
                backgroundSheet.close()
            }
        }
    }
}
