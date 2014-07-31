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

#include <QFile>

#include <KConfig>
#include <KConfigGroup>

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

ActionReply SddmAuthHelper::save(const QVariantMap &args)
{
    ActionReply reply = ActionReply::HelperErrorReply();
    QSharedPointer<KConfig> sddmConfig = openConfig(args["sddm.conf"].toString());
    QSharedPointer<KConfig> themeConfig;
    QString themeConfigFile = args["theme.conf.user"].toString();

    if (!themeConfigFile.isEmpty()) {
        themeConfig = openConfig(themeConfigFile);
    }

    QMap<QString, QVariant>::const_iterator iterator;
    
    for (iterator = args.constBegin() ; iterator != args.constEnd() ; ++iterator) {
        if (iterator.key() == "sddm.conf" || iterator.key() == "theme.conf.user")
            continue;

        QStringList configFields = iterator.key().split('/');
        
        QSharedPointer<KConfig> config;
        QString fileName = configFields[0];
        QString groupName = configFields[1];
        QString keyName = configFields[2];

        if (fileName == "sddm.conf") {
            sddmConfig->group(groupName).writeEntry(keyName, iterator.value());
        } else if (fileName == "theme.conf.user" && !themeConfig.isNull()) {
            themeConfig->group(groupName).writeEntry(keyName, iterator.value());
        }
    }

    sddmConfig->sync();

    if (!themeConfig.isNull())
        themeConfig->sync();
    
    return ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.kcontrol.kcmsddm", SddmAuthHelper);

#include "moc_sddmauthhelper.cpp"
