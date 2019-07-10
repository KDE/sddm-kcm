/*
    Copyright 2013 by Reza Fatahilah Shah <rshah0385@kireihana.com>
    Copyright 2011, 2012 David Edmundson <kde@davidedmundson.co.uk>

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
#include "sddmauthhelper.h"

#include <unistd.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QSharedPointer>

#include <KArchive>
#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KTar>
#include <KUser>
#include <KZip>

static QSharedPointer<KConfig> openConfig(const QString &filePath)
{
    QFile file(filePath);
    if(!file.exists()) {
        // If we are creating the config file, ensure it is world-readable: if
        // we don't do that, KConfig will create a file which is only readable
        // by root
        file.open(QIODevice::WriteOnly);
        file.close();
        file.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
    }
    return QSharedPointer<KConfig>(new KConfig(file.fileName(), KConfig::SimpleConfig));
}

void SddmAuthHelper::copyFile(const QString &source, const QString &destination)
{
    KUser sddmUser(QStringLiteral("sddm"));

    if (QFile::exists(destination)) {
        QFile::remove(destination);
    }

    QFile::copy(source, destination);
    const char* destinationConverted = destination.toLocal8Bit().data();
    if (chown(destinationConverted, sddmUser.userId().nativeId(), sddmUser.groupId().nativeId())) {
        return;
    }
}

ActionReply SddmAuthHelper::sync(const QVariantMap &args)
{
    QDir sddmConfigLocation(args[QStringLiteral("sddmUserConfig")].toString());
    if (!sddmConfigLocation.exists()) {
        QDir().mkpath(sddmConfigLocation.path());
    }

    // copy fontconfig (font, font rendering)
    if (!args[QStringLiteral("fontconfig")].isNull()) {
        QDir fontconfigSource(args[QStringLiteral("fontconfig")].toString());
        QStringList sourceFileEntries = fontconfigSource.entryList (QDir::Files);
        QStringList sourceDirEntries = fontconfigSource.entryList (QDir::AllDirs);
        QDir fontconfigDestination(sddmConfigLocation.path() + QStringLiteral("/fontconfig"));

        if (!fontconfigDestination.exists()) {
            QDir().mkpath(fontconfigDestination.path());
        }

        if (sourceDirEntries.count() != 0) {
            for (int i = 0; i<sourceDirEntries.count(); i++) {
                QString directoriesSource = fontconfigSource.path() + QDir::separator() + sourceDirEntries[i];
                QString directoriesDestination = fontconfigDestination.path() + QDir::separator() + sourceDirEntries[i];
                fontconfigSource.mkpath(directoriesDestination);
                copyFile(directoriesSource, directoriesDestination);
            }
        }

        if (sourceFileEntries.count() != 0) {
            for (int i = 0; i<sourceFileEntries.count(); i++) {
                QString filesSource = fontconfigSource.path() + QDir::separator() + sourceFileEntries[i];
                QString filesDestination = fontconfigDestination.path() + QDir::separator() + sourceFileEntries[i];
                copyFile(filesSource, filesDestination);
            }
        }
    }

    // copy kdeglobals (color scheme)
    if (!args[QStringLiteral("kdeglobals")].isNull()) {
        QDir kdeglobalsSource(args[QStringLiteral("kdeglobals")].toString());
        QDir kdeglobalsDestination(sddmConfigLocation.path() + QStringLiteral("/kdeglobals"));
        copyFile(kdeglobalsSource.path(), kdeglobalsDestination.path());
    }

    // copy plasmarc (icons, UI style)
    if (!args[QStringLiteral("plasmarc")].isNull()) {
        QDir plasmarcSource(args[QStringLiteral("plasmarc")].toString());
        QDir plasmarcDestination(sddmConfigLocation.path() + QStringLiteral("/plasmarc"));
        copyFile(plasmarcSource.path(), plasmarcDestination.path());
    }

    // write cursor theme to config file
    ActionReply reply = ActionReply::HelperErrorReply();
    QSharedPointer<KConfig> sddmConfig = openConfig(args[QStringLiteral("kde_settings.conf")].toString());
    QSharedPointer<KConfig> sddmOldConfig = openConfig(args[QStringLiteral("sddm.conf")].toString());

    QMap<QString, QVariant>::const_iterator iterator;

    for (iterator = args.constBegin() ; iterator != args.constEnd() ; ++iterator) {
        if (iterator.key() == QLatin1String("kde_settings.conf"))
            continue;

        QStringList configFields = iterator.key().split(QLatin1Char('/'));
        if (configFields.size() != 3) {
            continue;
        }

        QSharedPointer<KConfig> config;
        QString fileName = configFields[0];
        QString groupName = configFields[1];
        QString keyName = configFields[2];

        if (fileName == QLatin1String("kde_settings.conf") && iterator.value().isValid()) {
            sddmConfig->group(groupName).writeEntry(keyName, iterator.value());
            sddmOldConfig->group(groupName).deleteEntry(keyName);
        }
    }

    sddmOldConfig->sync();
    sddmConfig->sync();

    return ActionReply::SuccessReply();
}

ActionReply SddmAuthHelper::reset(const QVariantMap &args)
{
    QDir sddmConfigLocation(args[QStringLiteral("sddmUserConfig")].toString());
    QDir fontconfigDir(args[QStringLiteral("sddmUserConfig")].toString() + QStringLiteral("/fontconfig"));

    fontconfigDir.removeRecursively();
    QFile::remove(sddmConfigLocation.path() + QStringLiteral("/kdeglobals"));
    QFile::remove(sddmConfigLocation.path() + QStringLiteral("/plasmarc"));

    // remove cursor theme from config file
    ActionReply reply = ActionReply::HelperErrorReply();
    QSharedPointer<KConfig> sddmConfig = openConfig(args[QStringLiteral("kde_settings.conf")].toString());
    QSharedPointer<KConfig> sddmOldConfig = openConfig(args[QStringLiteral("sddm.conf")].toString());

    QMap<QString, QVariant>::const_iterator iterator;

    for (iterator = args.constBegin() ; iterator != args.constEnd() ; ++iterator) {
        if (iterator.key() == QLatin1String("kde_settings.conf"))
            continue;

        QStringList configFields = iterator.key().split(QLatin1Char('/'));
        if (configFields.size() != 3) {
            continue;
        }

        QSharedPointer<KConfig> config;
        QString fileName = configFields[0];
        QString groupName = configFields[1];
        QString keyName = configFields[2];

        if (fileName == QLatin1String("kde_settings.conf")) {
            sddmConfig->group(groupName).deleteEntry(keyName);
            sddmOldConfig->group(groupName).deleteEntry(keyName);
        }
    }

    sddmOldConfig->sync();
    sddmConfig->sync();

    return ActionReply::SuccessReply();
}

ActionReply SddmAuthHelper::save(const QVariantMap &args)
{
    ActionReply reply = ActionReply::HelperErrorReply();
    QSharedPointer<KConfig> sddmConfig = openConfig(args[QStringLiteral("kde_settings.conf")].toString());
    QSharedPointer<KConfig> sddmOldConfig = openConfig(args[QStringLiteral("sddm.conf")].toString());
    QSharedPointer<KConfig> themeConfig;
    QString themeConfigFile = args[QStringLiteral("theme.conf.user")].toString();

    if (!themeConfigFile.isEmpty()) {
        themeConfig = openConfig(themeConfigFile);
    }

    QMap<QString, QVariant>::const_iterator iterator;

    for (iterator = args.constBegin() ; iterator != args.constEnd() ; ++iterator) {
        if (iterator.key() == QLatin1String("kde_settings.conf") || iterator.key() == QLatin1String("theme.conf.user"))
            continue;

        QStringList configFields = iterator.key().split(QLatin1Char('/'));
        if (configFields.size() != 3) {
            continue;
        }

        QSharedPointer<KConfig> config;
        QString fileName = configFields[0];
        QString groupName = configFields[1];
        QString keyName = configFields[2];

        // if there is an identical keyName in "sddm.conf" we want to delete it so SDDM doesn't read from the old file
        // hierarchically SDDM prefers "etc/sddm.conf" to "/etc/sddm.conf.d/some_file.conf"

        if (fileName == QLatin1String("kde_settings.conf")) {
            sddmConfig->group(groupName).writeEntry(keyName, iterator.value());
            sddmOldConfig->group(groupName).deleteEntry(keyName);
        } else if (fileName == QLatin1String("theme.conf.user") && !themeConfig.isNull()) {
            QFileInfo themeConfigFileInfo(themeConfigFile);
            QDir configRootDirectory = themeConfigFileInfo.absoluteDir();

            if (keyName == QLatin1String("background")) {
                QFileInfo newBackgroundFileInfo(iterator.value().toString());
                QString previousBackground = themeConfig->group(groupName).readEntry(keyName);

                bool backgroundChanged = newBackgroundFileInfo.fileName() != previousBackground;
                if (backgroundChanged) {
                    if (!previousBackground.isEmpty()) {
                        QString previousBackgroundPath = configRootDirectory.filePath(previousBackground);
                        if (QFile::remove(previousBackgroundPath)) {
                            qDebug() << "Removed previous background " << previousBackgroundPath;
                        }
                    }

                    if (newBackgroundFileInfo.exists()) {
                        QString newBackgroundPath = configRootDirectory.filePath(newBackgroundFileInfo.fileName());
                        qDebug() << "Copying background from "  << newBackgroundFileInfo.absoluteFilePath() << " to " << newBackgroundPath;
                        if (QFile::copy(newBackgroundFileInfo.absoluteFilePath(), newBackgroundPath)) {
                            QFile::setPermissions(newBackgroundPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
                            themeConfig->group(groupName).writeEntry(keyName, newBackgroundFileInfo.fileName());
                        }
                    } else {
                        themeConfig->group(groupName).deleteEntry(keyName);
                    }
                }
            } else {
                themeConfig->group(groupName).writeEntry(keyName, iterator.value());
            }
        }
    }

    sddmOldConfig->sync();
    sddmConfig->sync();

    if (!themeConfig.isNull())
        themeConfig->sync();

    return ActionReply::SuccessReply();
}

ActionReply SddmAuthHelper::installtheme(const QVariantMap &args)
{
    const QString filePath = args[QStringLiteral("filePath")].toString();
    if (filePath.isEmpty()) {
        return ActionReply::HelperErrorReply();
    }

    const QString themesBaseDir = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("sddm/themes"), QStandardPaths::LocateDirectory);
    QDir dir(themesBaseDir);
    if (!dir.exists()) {
        return ActionReply::HelperErrorReply();
    }

    qDebug() << "Installing " << filePath << " into " << themesBaseDir;

    if (!QFile::exists(filePath)) {
        return ActionReply::HelperErrorReply();
    }

    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForFile(filePath);
    qWarning() << "Postinstallation: uncompress the file";

    QScopedPointer<KArchive> archive;

    //there must be a better way to do this? If not, make a static bool KZip::supportsMimeType(const QMimeType &type); ?
    //or even a factory class in KArchive

    if (mimeType.inherits(QStringLiteral("application/zip"))) {
        archive.reset(new KZip(filePath));
    } else if (mimeType.inherits(QStringLiteral("application/tar"))
                || mimeType.inherits(QStringLiteral("application/x-gzip"))
                || mimeType.inherits(QStringLiteral("application/x-bzip"))
                || mimeType.inherits(QStringLiteral("application/x-lzma"))
                || mimeType.inherits(QStringLiteral("application/x-xz"))
                || mimeType.inherits(QStringLiteral("application/x-bzip-compressed-tar"))
                || mimeType.inherits(QStringLiteral("application/x-compressed-tar"))) {
        archive.reset(new KTar(filePath));
    } else {
        auto e = ActionReply::HelperErrorReply();
        e.setErrorDescription(i18n("Invalid theme package"));
        return e;    }

    if (!archive->open(QIODevice::ReadOnly)) {
        auto e = ActionReply::HelperErrorReply();
        e.setErrorDescription(i18n("Could not open file"));
        return e;
    }

    auto directory = archive->directory();

    QStringList installedPaths;

    //some basic validation
    //the top level should only have folders, and those folders should contain a valid metadata.desktop file
    //if we get anything else, abort everything before copying
    for(const QString &name: directory->entries()) {
        auto entry = directory->entry(name);
        if (!entry->isDirectory()) {
            auto e = ActionReply::HelperErrorReply();
            e.setErrorDescription(i18n("Invalid theme package"));
            return e;
        }
        auto subDirectory = static_cast<const KArchiveDirectory*>(entry);
        auto metadataFile = subDirectory->file(QStringLiteral("metadata.desktop"));
        if(!metadataFile || !metadataFile->data().contains("[SddmGreeterTheme]")) {
            auto e = ActionReply::HelperErrorReply();
            e.setErrorDescription(i18n("Invalid theme package"));
            return e;
        }
        installedPaths.append(themesBaseDir + QLatin1Char('/') + name);
    }

    if (!directory->copyTo(themesBaseDir)) {
        auto e = ActionReply::HelperErrorReply();
        e.setErrorDescription(i18n("Could not decompress archive"));
        return e;
    }

    auto rc = ActionReply::SuccessReply();
    rc.addData(QStringLiteral("installedPaths"), installedPaths);
    return rc;
}

ActionReply SddmAuthHelper::uninstalltheme(const QVariantMap &args)
{
    const QString themePath = args[QStringLiteral("filePath")].toString();
    const QString themesBaseDir = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("sddm/themes"), QStandardPaths::LocateDirectory);

    QDir dir(themePath);
    if (!dir.exists()) {
        return ActionReply::HelperErrorReply();
    }

    //validate the themePath is directly inside the themesBaseDir
    QDir baseDir(themesBaseDir);
    if(baseDir.absoluteFilePath(dir.dirName()) != dir.absolutePath()) {
        return ActionReply::HelperErrorReply();
    }

    if (!dir.removeRecursively()) {
        return ActionReply::HelperErrorReply();
    }

    return ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.kcontrol.kcmsddm", SddmAuthHelper)

#include "moc_sddmauthhelper.cpp"
