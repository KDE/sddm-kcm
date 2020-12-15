/*
    Copyright 2020 David Redondo <kde@david-redondo.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of
    the license or (at your option) at any later version that is
    accepted by the membership of KDE e.V. (or its successor
    approved by the membership of KDE e.V.), which shall act as a
    proxy as defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
*/

#include "sddmsettingsbase.h"

#include "config.h"

#include <QDir>
#include <QDebug>

#include <algorithm>

SddmSettingsBase::SddmSettingsBase(KSharedConfigPtr config, QObject* parent)
    : KConfigSkeleton(config, parent)
{
    auto defaultFiles = QDir(QStringLiteral(SDDM_SYSTEM_CONFIG_DIR)).entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::LocaleAware);
    std::transform(defaultFiles.begin(), defaultFiles.end(), defaultFiles.begin(), [] (const QString &filename) -> QString {
        return QStringLiteral(SDDM_SYSTEM_CONFIG_DIR "/") + filename;
    });
    // If no filename is set, KConfig will not parse any file
    if (!defaultFiles.isEmpty()) {
        m_defaultConfig = KSharedConfig::openConfig(defaultFiles.takeLast(), KConfig::CascadeConfig);
    } else {
        m_defaultConfig = KSharedConfig::openConfig(QString(), KConfig::CascadeConfig);
    }
    m_defaultConfig->addConfigSources(defaultFiles);
}

QString SddmSettingsBase::defaultCurrent() const
{
    return m_defaultConfig->group("Theme").readEntry("Current");
}

unsigned int SddmSettingsBase::defaultMinimumUid() const
{
    return m_defaultConfig->group("Users").readEntry("MinimumUid", 1000);
}

unsigned int SddmSettingsBase::defaultMaximumUid() const
{
    return m_defaultConfig->group("Users").readEntry("MaximumUid", 60000);
}

QString SddmSettingsBase::defaultUser() const
{
    return m_defaultConfig->group("AutoLogin").readEntry("User");
}

QString SddmSettingsBase::defaultSession() const
{
    return m_defaultConfig->group("AutoLogin").readEntry("Session");
}

bool SddmSettingsBase::defaultRelogin() const
{
    return m_defaultConfig->group("AutoLogin").readEntry("Relogin", false);
}

QString SddmSettingsBase::defaultHaltCommand() const
{
    return m_defaultConfig->group("General").readEntry("HaltCommand");
}

 QString SddmSettingsBase::defaultRebootCommand() const
{
    return m_defaultConfig->group("General").readEntry("RebootCommand");
}
