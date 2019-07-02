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
#include "advanceconfig.h"
#include "ui_advanceconfig.h"
#include "config.h"
#include "cursortheme/thememodel.h"
#include "cursortheme/sortproxymodel.h"
#include "cursortheme/cursortheme.h"
#include "sessionmodel.h"
#include "usersmodel.h"

#include <QDebug>
#include <QFontDatabase>
#include <QIntValidator>
#include <QStandardPaths>

#include <KAuth/KAuthActionReply>
#include <kauthexecutejob.h>
#include <KConfigGroup>
#include <KMessageBox>
#include <KUser>

const int MIN_UID = 1000;
const int MAX_UID = 60000;

AdvanceConfig::AdvanceConfig(const KSharedConfigPtr &config, QWidget *parent) :
    QWidget(parent),
    mConfig(config)
{
    configUi = new Ui::AdvanceConfig();
    configUi->setupUi(this);
    configUi->syncExplanation->setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));

    load();

    connect(configUi->userList, SIGNAL(activated(int)), SIGNAL(changed()));
    connect(configUi->sessionList, SIGNAL(activated(int)), SIGNAL(changed()));
    connect(configUi->haltCommand, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->rebootCommand, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->cursorList, SIGNAL(activated(int)), SIGNAL(changed()));
    connect(configUi->minimumUid, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->minimumUid, &QLineEdit::textChanged, this, &AdvanceConfig::slotUidRangeChanged);
    connect(configUi->maximumUid, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->maximumUid, &QLineEdit::textChanged, this, &AdvanceConfig::slotUidRangeChanged);

    // manually emit changed signal since QCheckBox::clicked will pass false to changed() when unchecked
    connect(configUi->autoLogin, &QCheckBox::clicked, this, [this] { emit changed(); });
    connect(configUi->reloginAfterQuit, &QAbstractButton::clicked, this, [this] { emit changed(); });

    connect(configUi->syncSettings, &QPushButton::clicked, this, &AdvanceConfig::syncSettingsChanged);
    connect(configUi->resetSettings, &QPushButton::clicked, this, &AdvanceConfig::resetSettingsChanged);
}

AdvanceConfig::~AdvanceConfig()
{
    delete configUi;
}

void AdvanceConfig::load()
{
    //Cursor themes
    CursorThemeModel *cursorModel = new CursorThemeModel(this);
    proxyCursorModel = new SortProxyModel(this);
    proxyCursorModel->setSourceModel(cursorModel);
    proxyCursorModel->setFilterCaseSensitivity(Qt::CaseSensitive);
    proxyCursorModel->sort(Qt::DisplayRole, Qt::AscendingOrder);

    configUi->cursorList->setModel(proxyCursorModel);
    QString currentCursor = mConfig->group("Theme").readEntry("CursorTheme", "");
    QModelIndex cursorIndex = proxyCursorModel->findIndex(currentCursor);
    configUi->cursorList->setCurrentIndex(cursorIndex.row() < 0 ? 0 : cursorIndex.row());

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

QVariantMap AdvanceConfig::save()
{
    QVariantMap args;

    qDebug() << "idx:" << configUi->cursorList->currentIndex();

    QModelIndex cursorIndex = configUi->cursorList->model()->index(configUi->cursorList->currentIndex(),0);
    if (cursorIndex.isValid()) {
        const CursorTheme *cursorTheme = proxyCursorModel->theme(cursorIndex);
        if (cursorTheme)
            args[QStringLiteral("kde_settings.conf/Theme/CursorTheme")] = cursorTheme->name();
    }

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

    return args;
}

void AdvanceConfig::slotUidRangeChanged()
{
    int minUid = configUi->minimumUid->text().toInt();
    int maxUid = configUi->maximumUid->text().toInt();

    if (!isUidRangeValid(minUid, maxUid)) {
        return;
    }

    userModel->populate(minUid, maxUid);
}

bool AdvanceConfig::isUidRangeValid(int minUid, int maxUid) const
{
    if (minUid < 0 || minUid > maxUid)
        return false;

    return true;
}

void AdvanceConfig::syncSettingsChanged()
{
    const QString fontconfigPath = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, QStringLiteral("fontconfig"), QStandardPaths::LocateDirectory);
    const QString kdeglobalsPath = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, QStringLiteral("kdeglobals"));
    const QString plasmarcPath = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, QStringLiteral("plasmarc"));
    const QString sddmUserConfigPath = KUser("sddm").homeDir() + QStringLiteral("/.config");

    if (fontconfigPath.isEmpty()) {
        qDebug() << "fontconfig folder not found";
    }
    if (kdeglobalsPath.isEmpty()) {
        qDebug() << "kdeglobals file not found";
    }
    if (plasmarcPath.isEmpty()) {
        qDebug() << "plasmarc file not found";
    }

    QVariantMap args;
    args[QStringLiteral("fontconfig")] = fontconfigPath;
    args[QStringLiteral("kdeglobals")] = kdeglobalsPath;
    args[QStringLiteral("plasmarc")] = plasmarcPath;
    args[QStringLiteral("sddmUserConfig")] = sddmUserConfigPath;

    KAuth::Action syncAction(QStringLiteral("org.kde.kcontrol.kcmsddm.sync"));
    syncAction.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
    syncAction.setArguments(args);

    auto job = syncAction.execute();
    job->exec();

    if (job->error()){
        qDebug() << "Synchronization failed";
        qDebug() << job->errorString();
        qDebug() << job->errorText();
        KMessageBox::error(this, job->errorText());
    } else {
        changed(false);
        qDebug() << "Synchronization successful";
    }
}

void AdvanceConfig::resetSettingsChanged()
{
    const QString sddmUserConfigPath = KUser("sddm").homeDir() + QStringLiteral("/.config");

    QVariantMap args;
    args[QStringLiteral("sddmUserConfig")] = sddmUserConfigPath;

    KAuth::Action resetAction(QStringLiteral("org.kde.kcontrol.kcmsddm.reset"));
    resetAction.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
    resetAction.setArguments(args); 

    auto job = resetAction.execute();
    job->exec();

    if (job->error()){
        qDebug() << "Reset failed";
        qDebug() << job->errorString();
        qDebug() << job->errorText();
        KMessageBox::error(this, job->errorText());
    } else {
        changed(false);
        qDebug() << "Reset successful";
    }
}
