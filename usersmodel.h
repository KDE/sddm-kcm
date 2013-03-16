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
#ifndef USERSMODEL_H
#define USERSMODEL_H

#include <QAbstractListModel>

class KUser;

class UsersModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        UserNameRole = Qt::UserRole + 1,
        RealNameRole,
        HomeDirRole,
        IconRole
    };

    explicit UsersModel(QObject *parent=0);
    virtual ~UsersModel();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void populate(const uint minimumUid);
    int indexOf(const QString &user);

private:
    void add(const KUser &user);

    QList<KUser> mUserList;

};
#endif //USERSMODEL_H
