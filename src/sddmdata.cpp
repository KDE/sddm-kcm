/*
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "sddmdata.h"

#include "config.h"
#include "sddmsettings.h"

#include <KSharedConfig>

#include <QDir>
#include <QTemporaryFile>

SddmData::SddmData(QObject *parent, const QVariantList &args)
    : KCModuleData(parent, args)
{
    // The defaults should be read from the defaults config (see sddmsettingsbase.h) but KConfig
    // considers everything but the main file to provide defaults. To work around reading of wrong
    // defaults we provide an empty dummy config so we read the correct defaults. Afterwards the
    // current values are reread from the actual config files.
    m_settings = new SddmSettings(KSharedConfig::openConfig(QTemporaryFile().fileName()), this);
    auto config = KSharedConfig::openConfig(QStringLiteral(SDDM_CONFIG_FILE), KConfig::CascadeConfig);
    QStringList configFiles = QDir(QStringLiteral(SDDM_CONFIG_DIR)).entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::LocaleAware);
    std::transform(configFiles.begin(), configFiles.end(), configFiles.begin(), [](const QString &filename) -> QString {
        return QStringLiteral(SDDM_CONFIG_DIR "/") + filename;
    });
    config->addConfigSources(configFiles);
    m_settings->setSharedConfig(config);
    m_settings->read();
    autoRegisterSkeletons();
}

SddmSettings *SddmData::sddmSettings() const
{
    return m_settings;
}
