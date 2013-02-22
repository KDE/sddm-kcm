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
#include <KDebug>

static QSharedPointer<KConfig> openConfig(const QString &filePath)
{
    kDebug() << "Open config" << filePath;
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
    ActionReply reply = ActionReply::HelperErrorReply;
    QSharedPointer<KConfig> sddmConfig = openConfig(args["sddm.conf"].toString());
    
    QMap<QString, QVariant>::const_iterator iterator;
    iterator = args.constBegin();
    iterator++;
    for ( ; iterator != args.constEnd() ; ++iterator) {
        QStringList configFields = iterator.key().split('/');
        
        QSharedPointer<KConfig> config;
        QString fileName = configFields[0];
        QString groupName = configFields[1];
        QString keyName = configFields[2];

        if (fileName == "sddm.conf") {
            config = sddmConfig;
        }

        config->group(groupName).writeEntry(keyName, iterator.value());
    }
    sddmConfig->sync();
    
    return ActionReply::SuccessReply;
}

KDE4_AUTH_HELPER_MAIN("org.kde.kcontrol.kcmsddm", SddmAuthHelper);

#include "moc_sddmauthhelper.cpp"