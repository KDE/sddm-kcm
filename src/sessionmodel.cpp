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

#include "sessionmodel.h"

#include <QDir>
#include <QFile>
#include <QList>
#include <QTextStream>

#include <memory>

class Session {
public:
    QString file;
    QString name;
    QString exec;
    QString comment;
};

typedef std::shared_ptr<Session> SessionPtr;

class SessionModelPrivate {
public:
    int lastIndex { 0 };
    QList<SessionPtr> sessions;
};

SessionModel::SessionModel(QObject *parent) : QAbstractListModel(parent), d(new SessionModelPrivate()) {
    // read session files
    QDir dir("/usr/share/xsessions");
    dir.setNameFilters(QStringList() << "*.desktop");
    dir.setFilter(QDir::Files);
    // read session
    foreach(const QString &session, dir.entryList()) {
        QFile inputFile(dir.absoluteFilePath(session));
        if (!inputFile.open(QIODevice::ReadOnly))
            continue;
        SessionPtr si { new Session { session, "", "", "" } };
        bool isHidden = false;
        QString current_section;
        QTextStream in(&inputFile);
        while (!in.atEnd()) {
            QString line = in.readLine();

            if (line.startsWith(QLatin1String("["))) {
                // The section name ends before the last ] before the start of a comment
                int end = line.lastIndexOf(QLatin1Char(']'), line.indexOf(QLatin1Char('#')));
                if (end != -1)
                    current_section = line.mid(1, end - 1);
            }

            if (current_section != QLatin1String("Desktop Entry"))
                continue; // We are only interested in the "Desktop Entry" section

            if (line.startsWith("Name="))
                si->name = line.mid(5);
            if (line.startsWith("Exec="))
                si->exec = line.mid(5);
            if (line.startsWith("Comment="))
                si->comment = line.mid(8);
            if (line.startsWith(QLatin1String("Hidden=")))
                isHidden = line.mid(7).toLower() == QLatin1String("true");
        }
        if (!isHidden) {
            // add to sessions list
            d->sessions.push_back(si);
        }

        // close file
        inputFile.close();
    }
}

SessionModel::~SessionModel() {
    delete d;
}

QHash<int, QByteArray> SessionModel::roleNames() const {
    // set role names
    QHash<int, QByteArray> roleNames;
    roleNames[FileRole] = "file";
    roleNames[NameRole] = "name";
    roleNames[ExecRole] = "exec";
    roleNames[CommentRole] = "comment";

    return roleNames;
}

int SessionModel::rowCount(const QModelIndex &parent) const {
    return d->sessions.length();
}

QVariant SessionModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= d->sessions.count())
        return QVariant();

    // get session
    SessionPtr session = d->sessions[index.row()];

    // return correct value
    if (role == FileRole)
        return session->file;
    else if (role == NameRole)
        return session->name;
    else if (role == ExecRole)
        return session->exec;
    else if (role == CommentRole)
        return session->comment;

    // return empty value
    return QVariant();
}

int SessionModel::indexOf(const QString &sessionId) const
{
    for (int i = 0; i < d->sessions.length(); i++) {
        if (d->sessions[i]->file == sessionId) {
            return i;
        }
    }
    return 0;
}


