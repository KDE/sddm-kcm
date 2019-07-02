/***************************************************************************
* Copyright (c) 2013 Abdurrahman AVCI <abdurrahmanavci@gmail.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the
* Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
***************************************************************************/
#ifndef SDDM_SESSIONMODEL_H
#define SDDM_SESSIONMODEL_H

#include <QAbstractListModel>
#include <QHash>

class SessionModelPrivate;

class SessionModel : public QAbstractListModel {
    Q_OBJECT
    Q_DISABLE_COPY(SessionModel)

    enum SessionType {
        SessionTypeX,
        SessionTypeWayland
    };

public:
    enum SessionRole {
        NameRole = Qt::DisplayRole,
        FileRole = Qt::UserRole,
        ExecRole,
        CommentRole
    };

    explicit SessionModel(QObject *parent = nullptr);
    ~SessionModel() Q_DECL_OVERRIDE;

    void loadDir(const QString &path, SessionType type);

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int indexOf(const QString& sessionId) const;

private:
    SessionModelPrivate *d { nullptr };
};


#endif // SDDM_SESSIONMODEL_H
