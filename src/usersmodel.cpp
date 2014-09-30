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

#include "usersmodel.h"

#include <KUser>
#include <KLocalizedString>

UsersModel::UsersModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

UsersModel::~UsersModel()
{
}

int UsersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return mUserList.size();
}

QVariant UsersModel::data(const QModelIndex &index, int role) const
{
    const KUser user = mUserList[index.row()];

    switch(role) {
        case Qt::DisplayRole:
            return user.loginName();
    }

    return QVariant();
}

void UsersModel::add(const KUser &user)
{
    beginInsertRows(QModelIndex(), mUserList.count(), mUserList.count());

    mUserList.append( KUser(user) );

    endInsertRows();
}

void UsersModel::populate(const uint minimumUid, const uint maximumUid) {
    mUserList.clear();
    
    QList< KUser > userList = KUser::allUsers();

    KUser user;
    
    foreach( user, userList ) {
        K_UID uuid = user.uid();

        // invalid user
        if (uuid == (uid_t) -1) {
            continue;
        }

        if (uuid >= minimumUid && uuid <= maximumUid) {
            add(user);
        }
        /*qDebug() << user.loginName() << ",uid" << uuid;
        qDebug() << " home:" << user.homeDir();
        qDebug() << " isSuperUser:" << user.isSuperUser() << ",isValid:" << user.isValid();
        qDebug() << " faceIconPath:" << user.faceIconPath();*/
    }

}

int UsersModel::indexOf(const QString &user) {
    if (user.isEmpty())
        return 0;
    // find user index
    for (int i = 0; i < mUserList.size(); ++i)
        if (mUserList.at(i).loginName() == user)
            return i;
    // user not found
    return 0;
}
