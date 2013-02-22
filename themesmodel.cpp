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
#include "themesmodel.h"

#include <QString>

#include <KGlobal>
#include <KStandardDirs>
#include <KDebug>

#include <Plasma/Package>

#include "thememetadata.h"

ThemesModel::ThemesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

ThemesModel::~ThemesModel()
{
}

int ThemesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return mThemeList.size();
}

QVariant ThemesModel::data(const QModelIndex &index, int role) const
{
    const ThemeMetadata metadata = mThemeList[index.row()];

    switch(role) {
        case Qt::DisplayRole:
            return metadata.name();
        case ThemesModel::IdRole:
            return metadata.themeid();
        case ThemesModel::AuthorRole:
            return metadata.author();
        case ThemesModel::DescriptionRole:
            return metadata.description();
        case ThemesModel::LicenseRole:
            return metadata.license();
        case ThemesModel::EmailRole:
            return metadata.email();
        case ThemesModel::WebsiteRole:
            return metadata.website();
        case ThemesModel::CopyrightRole:
            return metadata.copyright();
        case ThemesModel::VersionRole:
            return metadata.version();
        case ThemesModel::ThemeApiRole:
            return metadata.themeapi();
        case ThemesModel::PreviewRole:
            return metadata.screenshot();
        case ThemesModel::PathRole:
            return metadata.path();
        case ThemesModel::ConfigFileRole:
            return metadata.configfile();
    }

    return QVariant();
}

void ThemesModel::populate()
{
    QStringList themesBaseDirs = KGlobal::dirs()->findDirs("data", "sddm/themes");

    if (themesBaseDirs.isEmpty())
        return;

    foreach (const QString &id, Plasma::Package::listInstalledPaths(themesBaseDirs.last())) {
        QString path = themesBaseDirs.last() + id;

        dump(id, path);

        add(id, path);
    }
}

void ThemesModel::add(const QString &id, const QString &path)
{
    beginInsertRows(QModelIndex(), mThemeList.count(), mThemeList.count());

    mThemeList.append( ThemeMetadata(id, path) );

    endInsertRows();
}

void ThemesModel::dump(const QString &id, const QString &path)
{
    Q_UNUSED(id)

    ThemeMetadata metadata(path);

    kDebug() << "Theme Path:" << metadata.path();
    kDebug() << "Name: " << metadata.name();
    kDebug() << "Version: " << metadata.version();
    kDebug() << "Author: " << metadata.author();
    kDebug() << "Description: " << metadata.description();
    kDebug() << "Email: " << metadata.email();
    kDebug() << "License: " << metadata.license();
    kDebug() << "Copyright: " << metadata.copyright();
    kDebug() << "Screenshot: " << metadata.screenshot();
}
