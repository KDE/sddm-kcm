/*
    SPDX-FileCopyrightText: 2020 David Redondo <kde@david-redondo.de>
    SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "sddmdata.h"

#include "config.h"
#include "sddmsettings.h"

#include <KSharedConfig>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDir>

SddmData::SddmData(QObject *parent)
    : KCModuleData(parent)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.systemd1"),
                                                      QStringLiteral("/org/freedesktop/systemd1"),
                                                      QStringLiteral("org.freedesktop.systemd1.Manager"),
                                                      QStringLiteral("GetUnitFileState"));
    msg << QStringLiteral("sddm.service");

    // it seems system settings wants things to be sync
    // It's not like systemd will ever be down
    QDBusReply<QString> hasSddmReply = QDBusConnection::systemBus().call(msg);
    // a quirk is that if sddm is uninstalled systemd replies with an error of invalid args
    if (hasSddmReply.value() == QLatin1String("enabled")) {
        setRelevant(true);
    } else if (hasSddmReply.error().type() == QDBusError::ServiceUnknown) { // user isn't installing systemd, we can't query it
        setRelevant(true);
    } else {
        setRelevant(false);
    }

    auto config = KSharedConfig::openConfig(QStringLiteral(SDDM_CONFIG_FILE), KConfig::CascadeConfig);
    QStringList configFiles = QDir(QStringLiteral(SDDM_CONFIG_DIR)).entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::LocaleAware);
    std::transform(configFiles.begin(), configFiles.end(), configFiles.begin(), [](const QString &filename) -> QString {
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
