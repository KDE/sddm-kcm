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
#include "sddmdata.h"

#include "config.h"
#include "sddmsettings.h"

#include <KSharedConfig>

#include <QDir>

SddmData::SddmData(QObject *parent, const QVariantList &args)
    : KCModuleData(parent, args)
{
    auto config = KSharedConfig::openConfig(QStringLiteral(SDDM_CONFIG_FILE), KConfig::CascadeConfig);
    QStringList configFiles = QDir(QStringLiteral(SDDM_CONFIG_DIR)).entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::LocaleAware);
    std::transform(configFiles.begin(), configFiles.end(), configFiles.begin(), [] (const QString &filename) -> QString {
        return QStringLiteral(SDDM_CONFIG_DIR "/") + filename;
    });
    config->addConfigSources(configFiles);
    m_settings = new SddmSettings(config, this);
    autoRegisterSkeletons();
}

SddmSettings *SddmData::sddmSettings() const
{
    return m_settings;
}
