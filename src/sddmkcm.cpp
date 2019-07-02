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
#include "advanceconfig.h"
#include "config.h"
#include "sddmkcm.h"
#include "themeconfig.h"

#include <algorithm>

#include <QDebug>
#include <QDir>
#include <QHBoxLayout>
#include <QTabWidget>

#include <KAboutData>
#include <KAuth/KAuthActionReply>
#include <kauthexecutejob.h>
#include <KLocalizedString>
#include <KPluginFactory>
#include <kpluginfactory.h>
#include <kpluginloader.h>

K_PLUGIN_FACTORY(SddmKcmFactory, registerPlugin<SddmKcm>();)


SddmKcm::SddmKcm(QWidget *parent, const QVariantList &args) :
    KCModule(parent, args)
{
    KAboutData* aboutData = new KAboutData(QStringLiteral("kcmsddm"), i18n("SDDM KDE Config"), QLatin1String(PROJECT_VERSION));

    aboutData->setShortDescription(i18n("Login screen using the SDDM"));
    aboutData->setLicense(KAboutLicense::GPL_V2);
    aboutData->setHomepage(QStringLiteral("https://projects.kde.org/projects/kde/workspace/sddm-kcm"));

    aboutData->addAuthor(QStringLiteral("Reza Fatahilah Shah"), i18n("Author"), QStringLiteral("rshah0385@kireihana.com"));
    aboutData->addAuthor(QStringLiteral("David Edmundson"), i18n("Author"), QStringLiteral("davidedmundson@kde.org"));

    setAboutData(aboutData);
    setNeedsAuthorization(true);

    mSddmConfig = KSharedConfig::openConfig(QStringLiteral(SDDM_CONFIG_FILE), KConfig::CascadeConfig);

    mSddmOldConfig = KSharedConfig::openConfig(QStringLiteral("sddm.conf"), KConfig::CascadeConfig);

    // This does not listen for new config files in the directory.
    QStringList configFiles = QDir(QLatin1String(SDDM_CONFIG_DIR)).entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::LocaleAware),
                systemConfigFiles = QDir(QLatin1String(SDDM_SYSTEM_CONFIG_DIR)).entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::LocaleAware);

    // QStringBuilder keeps dangling references, so force return of QString (QTBUG-47066)
    std::transform(systemConfigFiles.begin(), systemConfigFiles.end(), systemConfigFiles.begin(),
                    [](const QString &filename) -> QString { return QStringLiteral(SDDM_SYSTEM_CONFIG_DIR "/") + filename; });
    std::transform(configFiles.begin(), configFiles.end(), configFiles.begin(),
                    [](const QString &filename) -> QString { return QStringLiteral(SDDM_CONFIG_DIR "/") + filename; });

    mSddmConfig->addConfigSources(systemConfigFiles + configFiles);
    mSddmOldConfig->addConfigSources(systemConfigFiles + configFiles);

    prepareUi();
}

SddmKcm::~SddmKcm()
{
}

void SddmKcm::save()
{
    QVariantMap args;

    args[QStringLiteral("kde_settings.conf")] = QString {QLatin1String(SDDM_CONFIG_DIR "/") + QStringLiteral("kde_settings.conf")};
    args[QStringLiteral("sddm.conf")] = QLatin1String(SDDM_CONFIG_FILE);

    if (!mThemeConfig->themeConfigPath().isEmpty()) {
        args[QStringLiteral("theme.conf.user")] = QString(mThemeConfig->themeConfigPath() + QLatin1String(".user"));
    }

    qDebug() << "Ovr:" << args[QStringLiteral("theme.conf.user")].toString();
    args.unite(mThemeConfig->save());
    args.unite(mAdvanceConfig->save());

    KAuth::Action saveAction = authAction();

    saveAction.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
    saveAction.setArguments(args);

    auto job = saveAction.execute();
    job->exec();

    if (job->error()){
        qDebug() << "Save Failed";
        qDebug() << job->errorString();
        qDebug() << job->errorText();
    } else {
        changed(false);
        qDebug() << "Option saved";
    }

}

void SddmKcm::prepareUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QTabWidget* tabHolder = new QTabWidget(this);
    layout->addWidget(tabHolder);

    mThemeConfig = new ThemeConfig(mSddmConfig, this);
    connect(mThemeConfig, SIGNAL(changed(bool)), SIGNAL(changed(bool)));

    tabHolder->addTab(mThemeConfig, i18n("Theme"));

    mAdvanceConfig = new AdvanceConfig(mSddmConfig, this);
    connect(mAdvanceConfig, SIGNAL(changed(bool)), SIGNAL(changed(bool)));

    tabHolder->addTab(mAdvanceConfig, i18n("Advanced"));
}

#include "sddmkcm.moc"
