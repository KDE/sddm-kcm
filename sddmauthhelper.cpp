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
#include "sddmauthhelper.h"

#include <QFile>

#include <KConfig>
#include <KConfigGroup>

ActionReply SddmAuthHelper::save(const QVariantMap &args)
{
    ActionReply reply;
    //Open config file
    QString configName = "/etc/sddm.conf";
    QFile configFile(configName);
    if (!configFile.exists()) {
        return ActionReply::HelperErrorReply;
    }
    
    QSharedPointer<KConfig> sddmConfig = QSharedPointer<KConfig>(new KConfig(configFile.fileName(), KConfig::SimpleConfig));
    
    QMap<QString, QVariant>::const_iterator iterator;
    for (iterator = args.constBegin() ; iterator != args.constEnd() ; ++iterator) {
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