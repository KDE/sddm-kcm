/*
    Copyright 2019 Filip Fila <filipfila.kde@gmail.com>
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
#include "advancedconfig.h"
#include "ui_advancedconfig.h"
#include "config.h"
#include "sessionmodel.h"
#include "usersmodel.h"

#include <QDebug>
#include <QFontDatabase>
#include <QIntValidator>
#include <QStandardPaths>

#include <KAuth/KAuthActionReply>
#include <KAuthExecuteJob>
#include <KConfigGroup>
#include <KMessageBox>
#include <KUser>

const int MIN_UID = 1000;
const int MAX_UID = 60000;

AdvancedConfig::AdvancedConfig(const KSharedConfigPtr &config, QWidget *parent) :
    QWidget(parent),
    mConfig(config)
{
    configUi = new Ui::AdvancedConfig();
    configUi->setupUi(this);
    configUi->syncExplanation->setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));
    configUi->syncWarning->setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));

    load();

    connect(configUi->userList, SIGNAL(activated(int)), SIGNAL(changed()));
    connect(configUi->sessionList, SIGNAL(activated(int)), SIGNAL(changed()));
    connect(configUi->haltCommand, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->rebootCommand, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->minimumUid, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->minimumUid, &QLineEdit::textChanged, this, &AdvancedConfig::slotUidRangeChanged);
    connect(configUi->maximumUid, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->maximumUid, &QLineEdit::textChanged, this, &AdvancedConfig::slotUidRangeChanged);

    connect(configUi->autoLogin, &QCheckBox::toggled, this, [this] { emit changed(); });
    connect(configUi->reloginAfterQuit, &QAbstractButton::toggled, this, [this] { emit changed(); });

    connect(configUi->syncSettings, &QPushButton::clicked, this, &AdvancedConfig::syncSettingsChanged);
    connect(configUi->resetSettings, &QPushButton::clicked, this, &AdvancedConfig::resetSettingsChanged);
}

AdvancedConfig::~AdvancedConfig()
{
    delete configUi;
}

void AdvancedConfig::load()
{
    //User list
    int minUid, maxUid;
    minUid = mConfig->group("Users").readEntry("MinimumUid", MIN_UID);
    maxUid = mConfig->group("Users").readEntry("MaximumUid", MAX_UID);

    userModel = new UsersModel(this);
    configUi->userList->setModel(userModel);
    userModel->populate( minUid, maxUid );

    sessionModel = new SessionModel(this);
    configUi->sessionList->setModel(sessionModel);

    const QString currentUser = mConfig->group("Autologin").readEntry("User", "");
    configUi->userList->setCurrentIndex(userModel->indexOf(currentUser));

    const QString autologinSession = mConfig->group("Autologin").readEntry("Session", "");
    configUi->sessionList->setCurrentIndex(sessionModel->indexOf(autologinSession));

    configUi->autoLogin->setChecked(!currentUser.isEmpty());
    configUi->reloginAfterQuit->setChecked(mConfig->group("Autologin").readEntry("Relogin", false));

    QValidator *uidValidator = new QIntValidator(MIN_UID, MAX_UID, configUi->minimumUid);
    configUi->minimumUid->setValidator(uidValidator);
    configUi->minimumUid->setText(QString::number(minUid));

    configUi->maximumUid->setValidator(uidValidator);
    configUi->maximumUid->setText(QString::number(maxUid));


    //Commands
    configUi->haltCommand->setUrl(QUrl::fromLocalFile(mConfig->group("General").readEntry("HaltCommand")));
    configUi->rebootCommand->setUrl(QUrl::fromLocalFile(mConfig->group("General").readEntry("RebootCommand")));
}

void AdvancedConfig::save(QVariantMap &args)
{
    args[QStringLiteral("kde_settings.conf/Autologin/User")] = ( configUi->autoLogin->isChecked() ) ? configUi->userList->currentText() : QString();
    args[QStringLiteral("kde_settings.conf/Autologin/Session")] = ( configUi->autoLogin->isChecked() ) ? configUi->sessionList->currentData() : QString();

    args[QStringLiteral("kde_settings.conf/Autologin/Relogin")] = configUi->reloginAfterQuit->isChecked();
    //TODO session

    int minUid = configUi->minimumUid->text().toInt();
    int maxUid = configUi->maximumUid->text().toInt();
    if (isUidRangeValid(minUid, maxUid)) {
        args[QStringLiteral("kde_settings.conf/Users/MinimumUid")] = configUi->minimumUid->text();
        args[QStringLiteral("kde_settings.conf/Users/MaximumUid")] = configUi->maximumUid->text();
    }

    args[QStringLiteral("kde_settings.conf/General/HaltCommand")] = configUi->haltCommand->url().toLocalFile();
    args[QStringLiteral("kde_settings.conf/General/RebootCommand")] = configUi->rebootCommand->url().toLocalFile();
}

void AdvancedConfig::slotUidRangeChanged()
{
    int minUid = configUi->minimumUid->text().toInt();
    int maxUid = configUi->maximumUid->text().toInt();

    if (!isUidRangeValid(minUid, maxUid)) {
        return;
    }

    userModel->populate(minUid, maxUid);
}

bool AdvancedConfig::isUidRangeValid(int minUid, int maxUid) const
{
    if (minUid < 0 || minUid > maxUid)
        return false;

    return true;
}

void AdvancedConfig::syncSettingsChanged()
{
    // initial check for sddm user; abort if user not present
    // we have to check with QString and isEmpty() instead of QDir and exists() because
    // QDir returns "." and true for exists() in the case of a non-existent user;
    QString sddmHomeDirPath = KUser("sddm").homeDir();
    if (sddmHomeDirPath.isEmpty()) {
        KMessageBox::error(this, QStringLiteral("Cannot proceed, user 'sddm' does not exist. Please check your SDDM install."));
        return;
    }

    // read Plasma values
    KConfig cursorConfig(QStringLiteral("kcminputrc"));
    KConfigGroup cursorConfigGroup(&cursorConfig, "Mouse");
    QVariant cursorTheme = cursorConfigGroup.readEntry("cursorTheme", QString());

    KConfig dpiConfig(QStringLiteral("kcmfonts"));
    KConfigGroup dpiConfigGroup(&dpiConfig, "General");
    QString dpiValue = dpiConfigGroup.readEntry("forceFontDPI");
    QString dpiArgument = QStringLiteral("-dpi ") + dpiValue;

    KConfig numLockConfig(QStringLiteral("kcminputrc"));
    KConfigGroup numLockConfigGroup(&numLockConfig, "Keyboard");
    QString numLock = numLockConfigGroup.readEntry("NumLock");

    // define paths
    const QString fontconfigPath = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, QStringLiteral("fontconfig"), QStandardPaths::LocateDirectory);
    const QString kdeglobalsPath = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, QStringLiteral("kdeglobals"));
    const QString plasmarcPath = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, QStringLiteral("plasmarc"));

    // send values and paths to helper, debug if it fails
    QVariantMap args;

    args[QStringLiteral("kde_settings.conf")] = QString {QLatin1String(SDDM_CONFIG_DIR "/") + QStringLiteral("kde_settings.conf")};

    args[QStringLiteral("sddm.conf")] = QLatin1String(SDDM_CONFIG_FILE);

    if (!cursorTheme.isNull()) {
        args[QStringLiteral("kde_settings.conf/Theme/CursorTheme")] = cursorTheme;
    }
    else {
        qDebug() << "Cannot find cursor theme value.";
    }

    if (!dpiValue.isEmpty()) {
        args[QStringLiteral("kde_settings.conf/X11/ServerArguments")] = dpiArgument;
    }
    else {
        qDebug() << "Cannot find scaling DPI value.";
    }

    if (!numLock.isEmpty()) {
        if (numLock == QStringLiteral("0")) {
            args[QStringLiteral("kde_settings.conf/General/Numlock")] = QStringLiteral("on");
        }
        else if (numLock == QStringLiteral("1")) {
            args[QStringLiteral("kde_settings.conf/General/Numlock")] = QStringLiteral("off");
        }
        else if (numLock == QStringLiteral("2")) {
            args[QStringLiteral("kde_settings.conf/General/Numlock")] = QStringLiteral("none");
        }
    }
    else {
        qDebug() << "Cannot find NumLock value.";
    }

    if (!fontconfigPath.isEmpty()) {
        args[QStringLiteral("fontconfig")] = fontconfigPath;
    }
    else {
        qDebug() << "Cannot find fontconfig folder.";
    }

    if (!kdeglobalsPath.isEmpty()) {
        args[QStringLiteral("kdeglobals")] = kdeglobalsPath;
    }
    else {
        qDebug() << "Cannot find kdeglobals file.";
    }

    if (!plasmarcPath.isEmpty()) {
        args[QStringLiteral("plasmarc")] = plasmarcPath;
    }
    else {
        qDebug() << "Cannot find plasmarc file.";
    }

    KAuth::Action syncAction(QStringLiteral("org.kde.kcontrol.kcmsddm.sync"));
    syncAction.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
    syncAction.setArguments(args);

    auto job = syncAction.execute();
    job->exec();

    if (job->error()){
        qDebug() << "Synchronization failed";
        qDebug() << job->errorString();
        qDebug() << job->errorText();
        if (!job->errorText().isEmpty()) {
            KMessageBox::error(this, job->errorText());
        }
    } else {
        Q_EMIT changed(false);
        qDebug() << "Synchronization successful";
    }
}

void AdvancedConfig::resetSettingsChanged()
{
    // initial check for sddm user; abort if user not present
    // we have to check with QString and isEmpty() instead of QDir and exists() because
    // QDir returns "." and true for exists() in the case of a non-existent user
    QString sddmHomeDirPath = KUser("sddm").homeDir();
    if (sddmHomeDirPath.isEmpty()) {
        KMessageBox::error(this, QStringLiteral("Cannot proceed, user 'sddm' does not exist. Please check your SDDM install."));
        return;
    }

    // send paths to helper
    QVariantMap args;

    args[QStringLiteral("kde_settings.conf")] = QString {QLatin1String(SDDM_CONFIG_DIR "/") + QStringLiteral("kde_settings.conf")};

    args[QStringLiteral("sddm.conf")] = QLatin1String(SDDM_CONFIG_FILE);

    args[QStringLiteral("kde_settings.conf/Theme/CursorTheme")] = QVariant();

    args[QStringLiteral("kde_settings.conf/X11/ServerArguments")] = QVariant();

    args[QStringLiteral("kde_settings.conf/General/Numlock")] = QVariant();

    KAuth::Action resetAction(QStringLiteral("org.kde.kcontrol.kcmsddm.reset"));
    resetAction.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
    resetAction.setArguments(args); 

    auto job = resetAction.execute();
    job->exec();

    if (job->error()){
        qDebug() << "Reset failed";
        qDebug() << job->errorString();
        qDebug() << job->errorText();
        if (!job->errorText().isEmpty()) {
            KMessageBox::error(this, job->errorText());
        }
    } else {
        Q_EMIT changed(false);
        qDebug() << "Reset successful";
    }
}
