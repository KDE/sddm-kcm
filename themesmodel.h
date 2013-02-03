/*
 *   Copyright 2013 by Reza Fatahilah Shah <rshah0385@kireihana.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef THEMESMODEL_H
#define THEMESMODEL_H

#include <QAbstractListModel>

class ThemeMetadata;

class ThemesModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole,
        PathRole,
        AuthorRole,
        DescriptionRole,
        VersionRole,
        PreviewRole,
        EmailRole,
        WebsiteRole,
        LicenseRole
    };
               
    explicit ThemesModel(QObject *parent=0);
    virtual ~ThemesModel();
    
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    
    void populate();
    
private:
    void add(const QString &name, const QString &path);
    void dump(const QString &id, const QString &path);
    
    QList<ThemeMetadata> mThemeList;
};

#endif //THEMESMODEL_H