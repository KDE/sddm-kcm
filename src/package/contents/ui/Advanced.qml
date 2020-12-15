/*
    Copyright 2020 David Redondo <kde@david-redondo.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of
    the license or (at your option) at any later version that is
    accepted by the membership of KDE e.V. (or its successor
    approved by the membership of KDE e.V.), which shall act as a
    proxy as defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
*/

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15

import org.kde.kcm 1.5 as KCM
import org.kde.kirigami 2.14 as Kirigami
import org.kde.kitemmodels 1.0 as ItemModels
import org.kde.private.kcms.sddm 1.0

Kirigami.Page {
    title: i18nc("@title", "Behavior")
    Kirigami.FormLayout {
        width: parent.width
        RowLayout {
            Kirigami.FormData.label: i18nc("option:check", "Automatically log in:")
            QQC2.CheckBox {
                id: autologinBox
                text: i18nc("@label:listbox, the following combobox selects the user to log in automatically", "as user:")
                checked: kcm.sddmSettings.user != ""
                KCM.SettingHighlighter {
                    highlight: (kcm.sddmSettings.user != "" && kcm.sddmSettings.defaultUser == "") ||
                                (kcm.sddmSettings.user == "" && kcm.sddmSettings.defaultUser != "")
                }
            }
            QQC2.ComboBox {
                model: ItemModels.KSortFilterProxyModel {
                    sourceModel: UsersModel {
                        id: userModel
                    }
                    filterRowCallback: function(sourceRow, sourceParent) {
                        const id = userModel.data(userModel.index(sourceRow, 0, sourceParent), UsersModel.UidRole)
                        return kcm.sddmSettings.minimumUid <= id && id <= kcm.sddmSettings.maximumUid
                    }
                }
                textRole: "display"
                currentIndex: Math.max(find(kcm.sddmSettings.user), 0)
                onActivated: kcm.sddmSettings.user = currentText
                onEnabledChanged:  enabled ? kcm.sddmSettings.user = currentText : kcm.sddmSettings.user = ""
                KCM.SettingStateBinding {
                    visible: autologinBox.checked
                    configObject: kcm.sddmSettings
                    settingName: "User"
                    extraEnabledConditions: autologinBox.checked
                }
            }
            QQC2.Label {
                enabled: autologinBox.checked
                text: i18nc("@label:listbox, the following combobox selects the session that is started automatically", "with session")
            }
            QQC2.ComboBox{
                model: SessionModel {}
                currentIndex: Math.max(indexOfValue(kcm.sddmSettings.session), 0)
                textRole: "name"
                valueRole: "file"
                onActivated: kcm.sddmSettings.session = currentValue
                onEnabledChanged: enabled ? kcm.sddmSettings.session = currentValue : kcm.sddmSettings.session = ""
                KCM.SettingStateBinding {
                    visible: autologinBox.checked
                    configObject: kcm.sddmSettings
                    settingName: "Session"
                    extraEnabledConditions: autologinBox.checked
                }
            }
        }
        QQC2.CheckBox {
            text: i18nc("@option:check", "Log in again immediately after logging off")
            checked: kcm.sddmSettings.relogin
            onToggled: kcm.sddmSettings.relogin = checked
            KCM.SettingStateBinding {
                configObject: kcm.sddmSettings
                settingName: "Relogin"
                extraEnabledConditions: autologinBox.checked
            }
        }
        Item {
            Kirigami.FormData.isSection: true
        }
        QQC2.SpinBox {
            Kirigami.FormData.label: i18nc("@label:spinbox", "Minimum user UID:")
            id: minSpinBox
            from: 1000
            to: maxSpinBox.value
            value: kcm.sddmSettings.minimumUid
            onValueModified: kcm.sddmSettings.minimumUid = value
            KCM.SettingStateBinding {
                configObject: kcm.sddmSettings
                settingName: "MinimumUid"
            }
        }
        QQC2.SpinBox {
            Kirigami.FormData.label: i18nc("@label:spinbox", "Maximum user UID:")
            id: maxSpinBox
            from: minSpinBox.value
            to: 60000
            value: kcm.sddmSettings.maximumUid
            onValueModified: kcm.sddmSettings.maximumUid = value
            KCM.SettingStateBinding {
                configObject: kcm.sddmSettings
                settingName: "MaximumUid"
            }
        }
        Item {
            Kirigami.FormData.isSection: true
        }
        RowLayout {
            Kirigami.FormData.label: i18nc("@label:textbox", "Halt Command:")
            Layout.fillWidth: true
            Kirigami.ActionTextField {
                id: haltField
                Layout.fillWidth: true
                text: kcm.sddmSettings.haltCommand
                readOnly: true
                onReleased: haltButton.selectFile()
                rightActions: [ Kirigami.Action {
                    iconName: haltField.LayoutMirroring.enabled ? "edit-clear-locationbar-ltr" : "edit-clear-locationbar-rtl"
                    visible: haltField.text.length > 0
                    onTriggered: kcm.sddmSettings.haltCommand = ""
                }]
                QQC2.ToolTip.text: text
                QQC2.ToolTip.visible: hovered && haltField.text.length > 0
                KCM.SettingStateBinding {
                    configObject: kcm.sddmSettings
                    settingName: "HaltCommand"
                }
            }
            QQC2.Button {
                id: haltButton
                icon.name: "document-open-folder"
                enabled: haltField.enabled
                function selectFile() {
                    fileDialog.handler = (url => kcm.sddmSettings.haltCommand = kcm.toLocalFile(url))
                    fileDialog.open()
                }
                onClicked: selectFile()
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Kirigami.FormData.label: i18nc("@label:textbox", "Reboot Command:")
            Kirigami.ActionTextField {
                id: rebootField
                Layout.fillWidth: true
                text: kcm.sddmSettings.rebootCommand
                readOnly: true
                onReleased: rebootButton.selectFile()
                rightActions: [ Kirigami.Action {
                    iconName: rebootField.LayoutMirroring.enabled ? "edit-clear-locationbar-ltr" : "edit-clear-locationbar-rtl"
                    visible: rebootField.text.length > 0
                    onTriggered: kcm.sddmSettings.rebootCommand = ""
                }]
                QQC2.ToolTip.text: text
                QQC2.ToolTip.visible: hovered && haltField.text.length > 0
                KCM.SettingStateBinding {
                    configObject: kcm.sddmSettings
                    settingName: "RebootCommand"
                }
            }
            QQC2.Button {
                id: rebootButton
                icon.name: "document-open-folder"
                enabled: rebootField.enabled
                function selectFile() {
                    fileDialog.handler = (url => kcm.sddmSettings.rebootCommand = kcm.toLocalFile(url))
                    fileDialog.open()
                }
                onClicked: selectFile()
            }
        }
        FileDialog {
            id: fileDialog
            property var handler
            selectExisting: true
            onAccepted: {
                handler(fileUrl)
            }
        }
    }
}
