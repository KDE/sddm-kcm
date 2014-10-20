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

#include <QIntValidator>

#include <QDebug>
#include <KConfigGroup>
#include <KUser>

#include "usersmodel.h"
#include "sessionmodel.h"
#include "config.h"
#include "cursortheme/thememodel.h"
#include "cursortheme/sortproxymodel.h"
#include "cursortheme/cursortheme.h"

const int MIN_UID = 1000;
const int MAX_UID = 65000;

AdvanceConfig::AdvanceConfig(QWidget *parent) :
    QWidget(parent)
{
    mConfig = KSharedConfig::openConfig(SDDM_CONFIG_FILE, KConfig::SimpleConfig);

    configUi = new Ui::AdvanceConfig();
    configUi->setupUi(this);

    load();

    connect(configUi->userList, SIGNAL(activated(int)), SIGNAL(changed()));
    connect(configUi->sessionList, SIGNAL(activated(int)), SIGNAL(changed()));
    connect(configUi->haltCommand, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->rebootCommand, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->cursorList, SIGNAL(activated(int)), SIGNAL(changed()));
    connect(configUi->minimumUid, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->minimumUid, SIGNAL(textChanged(QString)), SLOT(slotUidRangeChanged()));
    connect(configUi->maximumUid, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->maximumUid, SIGNAL(textChanged(QString)), SLOT(slotUidRangeChanged()));
    connect(configUi->autoLogin, SIGNAL(clicked()), SIGNAL(changed()));
    connect(configUi->reloginAfterQuit, SIGNAL(clicked()), SIGNAL(changed()));
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
    QString currentCursor = mConfig->group("General").readEntry("CursorTheme", "");
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
    configUi->haltCommand->setUrl(mConfig->group("General").readEntry("HaltCommand"));
    configUi->rebootCommand->setUrl(mConfig->group("General").readEntry("RebootCommand"));
}

QVariantMap AdvanceConfig::save()
{
    QVariantMap args;

    qDebug() << "idx:" << configUi->cursorList->currentIndex();

    QModelIndex cursorIndex = configUi->cursorList->model()->index(configUi->cursorList->currentIndex(),0);
    if (cursorIndex.isValid()) {
        const CursorTheme *cursorTheme = proxyCursorModel->theme(cursorIndex);
        if (cursorTheme)
            args["sddm.conf/Theme/CursorTheme"] = cursorTheme->name();
    }

    args["sddm.conf/Autologin/User"] = ( configUi->autoLogin->isChecked() ) ? configUi->userList->currentText() : "";
    args["sddm.conf/Autologin/Session"] = ( configUi->autoLogin->isChecked() ) ? configUi->sessionList->currentData() : "";

    args["sddm.conf/Autologin/Relogin"] = configUi->reloginAfterQuit->isChecked();
    //TODO session

    int minUid = configUi->minimumUid->text().toInt();
    int maxUid = configUi->maximumUid->text().toInt();
    if (isUidRangeValid(minUid, maxUid)) {
        args["sddm.conf/Users/MinimumUid"] = configUi->minimumUid->text();
        args["sddm.conf/Users/MaximumUid"] = configUi->maximumUid->text();
    }

    args["sddm.conf/General/HaltCommand"] = configUi->haltCommand->url().path();
    args["sddm.conf/General/RebootCommand"] = configUi->rebootCommand->url().path();

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
    if (minUid < MIN_UID || minUid > maxUid)
        return false;
    
    return true;
}
