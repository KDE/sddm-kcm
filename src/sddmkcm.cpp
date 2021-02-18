/*
    Copyright 2013 by Reza Fatahilah Shah <rshah0385@kireihana.com>
    Copyright 2019 Filip Fila <filipfila.kde@gmail.com>
    Copyright 2020 David Redondo <kde@david-redondo.de>

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

#include "sddmkcm.h"

#include "config.h"
#include "sddmdata.h"
#include "sddmsettings.h"
#include "sessionmodel.h"
#include "themesmodel.h"
#include "usersmodel.h"

#include <QDir>

#include <KAboutData>
#include <KAuthExecuteJob>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KUser>

K_PLUGIN_FACTORY_WITH_JSON(KCMSddmFactory, "kcm_sddm.json", registerPlugin<SddmKcm>(); registerPlugin<SddmData>();)

SddmKcm::SddmKcm(QObject *parent, const QVariantList &args)
    : KQuickAddons::ManagedConfigModule(parent, args)
    , m_data(new SddmData(this))
    , m_themesModel(new ThemesModel(this))
{
    KAboutData *aboutData = new KAboutData(QStringLiteral("kcm_sddm"), i18n("Login Screen (SDDM)"), QStringLiteral(PROJECT_VERSION));

    aboutData->setShortDescription(i18n("Login screen using the SDDM"));
    aboutData->setLicense(KAboutLicense::GPL_V2);
    aboutData->setHomepage(QStringLiteral("https://projects.kde.org/projects/kde/workspace/sddm-kcm"));
    aboutData->addAuthor(QStringLiteral("Reza Fatahilah Shah"), i18n("Author"), QStringLiteral("rshah0385@kireihana.com"));
    aboutData->addAuthor(QStringLiteral("David Edmundson"), i18n("Author"), QStringLiteral("davidedmundson@kde.org"));
    aboutData->addAuthor(QStringLiteral("David Redondo"), i18n("Author"), QStringLiteral("kde@david-redondo.de"));

    setAboutData(aboutData);
    setAuthActionName(QStringLiteral("org.kde.kcontrol.kcmsddm.save"));

    qmlRegisterUncreatableType<ThemesModel>("org.kde.private.kcms.sddm", 1, 0, "ThemesModel", QStringLiteral("Cannot create ThemesModel"));
    qmlRegisterType<UsersModel>("org.kde.private.kcms.sddm", 1, 0, "UsersModel");
    qmlRegisterType<SessionModel>("org.kde.private.kcms.sddm", 1, 0, "SessionModel");
    qmlRegisterAnonymousType<SddmSettings>("org.kde.private.kcms.sddm", 1);

    connect(m_data->sddmSettings(), &SddmSettings::CurrentChanged, this, [this] {
        m_themesModel->setCurrentTheme(m_data->sddmSettings()->current());
    });
    m_themesModel->setCurrentTheme(m_data->sddmSettings()->current());
    // Currently only emmited when background changes
    connect(m_themesModel, &QAbstractItemModel::dataChanged, this, [this] {
        this->setNeedsSave(true);
    });
}

SddmKcm::~SddmKcm()
{
}

SddmSettings *SddmKcm::sddmSettings() const
{
    return m_data->sddmSettings();
}

ThemesModel *SddmKcm::themesModel() const
{
    return m_themesModel;
}

QString SddmKcm::toLocalFile(const QUrl &url)
{
    return url.toLocalFile();
}

void SddmKcm::removeTheme(const QModelIndex &index)
{
    const QString path = m_themesModel->data(index, ThemesModel::PathRole).toString();
    KAuth::Action saveAction(QStringLiteral("org.kde.kcontrol.kcmsddm.uninstalltheme"));
    saveAction.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
    saveAction.addArgument(QStringLiteral("filePath"), path);
    auto job = saveAction.execute();
    connect(job, &KJob::result, this, [this, job] {
        if (job->error()) {
            Q_EMIT errorOccured(job->errorString());
        } else {
            m_themesModel->populate();
        }
    });
    job->start();
}

void SddmKcm::installTheme(const QUrl &url)
{
    KAuth::Action saveAction(QStringLiteral("org.kde.kcontrol.kcmsddm.installtheme"));
    saveAction.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
    saveAction.addArgument(QStringLiteral("filePath"), url.toLocalFile());
    auto job = saveAction.execute();
    connect(job, &KJob::result, this, [this, job] {
        if (job->error()) {
            Q_EMIT errorOccured(job->errorString());
        } else {
            m_themesModel->populate();
        }
    });
    job->start();
}

void SddmKcm::save()
{
    QVariantMap args;

    const QModelIndex currentThemeIndex = m_themesModel->index(m_themesModel->currentIndex());
    const QString themeConfigPath = m_themesModel->data(currentThemeIndex, ThemesModel::PathRole).toString()
        + m_themesModel->data(currentThemeIndex, ThemesModel::ConfigFileRole).toString();
    if (!themeConfigPath.isEmpty()) {
        args[QStringLiteral("theme.conf.user")] = QVariant(themeConfigPath + QStringLiteral(".user"));
        const QString backgroundPath = m_themesModel->data(currentThemeIndex, ThemesModel::CurrentBackgroundRole).toString();
        if (!backgroundPath.isEmpty()) {
            args[QStringLiteral("theme.conf.user/General/background")] = backgroundPath;
            args[QStringLiteral("theme.conf.user/General/type")] = QStringLiteral("image");
        } else {
            args[QStringLiteral("theme.conf.user/General/type")] = QStringLiteral("color");
        }
    }
    args[QStringLiteral("kde_settings.conf/Theme/Current")] = currentThemeIndex.data(ThemesModel::IdRole);
    args[QStringLiteral("kde_settings.conf/Autologin/User")] = m_data->sddmSettings()->user();
    args[QStringLiteral("kde_settings.conf/Autologin/Session")] = m_data->sddmSettings()->session();
    args[QStringLiteral("kde_settings.conf/Autologin/Relogin")] = m_data->sddmSettings()->relogin();
    args[QStringLiteral("kde_settings.conf/Users/MinimumUid")] = m_data->sddmSettings()->minimumUid();
    args[QStringLiteral("kde_settings.conf/Users/MaximumUid")] = m_data->sddmSettings()->maximumUid();
    args[QStringLiteral("kde_settings.conf/General/HaltCommand")] = m_data->sddmSettings()->haltCommand();
    args[QStringLiteral("kde_settings.conf/General/RebootCommand")] = m_data->sddmSettings()->rebootCommand();

    KAuth::Action saveAction(authActionName());
    saveAction.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
    saveAction.setArguments(args);

    auto job = saveAction.execute();
    connect(job, &KJob::result, this, [this, job] {
        if (job->error()) {
            Q_EMIT errorOccured(job->errorString());
        } else {
            m_data->sddmSettings()->load();
        }
    });
    job->start();
}

void SddmKcm::synchronizeSettings()
{
    // initial check for sddm user; abort if user not present
    // we have to check with QString and isEmpty() instead of QDir and exists() because
    // QDir returns "." and true for exists() in the case of a non-existent user;
    QString sddmHomeDirPath = KUser("sddm").homeDir();
    if (sddmHomeDirPath.isEmpty()) {
        Q_EMIT errorOccured(QStringLiteral("Cannot proceed, user 'sddm' does not exist. Please check your SDDM install."));
        return;
    }

    // read Plasma values
    KConfig cursorConfig(QStringLiteral("kcminputrc"));
    KConfigGroup cursorConfigGroup(&cursorConfig, "Mouse");
    QVariant cursorTheme = cursorConfigGroup.readEntry("cursorTheme", QString());

    KConfig dpiConfig(QStringLiteral("kcmfonts"));
    KConfigGroup dpiConfigGroup(&dpiConfig, "General");
    QString dpiValue = dpiConfigGroup.readEntry("forceFontDPI");
    QString dpiArgument = QStringLiteral("-dpi ") + dpiValue;

    KConfig numLockConfig(QStringLiteral("kcminputrc"));
    KConfigGroup numLockConfigGroup(&numLockConfig, "Keyboard");
    QString numLock = numLockConfigGroup.readEntry("NumLock");

    // Syncing the font only works with SDDM >= 0.19, but will not have a negative effect with older versions
    KConfig plasmaFontConfig(QStringLiteral("kdeglobals"));
    KConfigGroup plasmaFontGroup(&plasmaFontConfig, "General");
    QString plasmaFont = plasmaFontGroup.readEntry("font");

    // define paths
    const QString fontconfigPath = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, QStringLiteral("fontconfig"), QStandardPaths::LocateDirectory);
    const QString kdeglobalsPath = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, QStringLiteral("kdeglobals"));
    const QString plasmarcPath = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, QStringLiteral("plasmarc"));

    // send values and paths to helper, debug if it fails
    QVariantMap args;

    args[QStringLiteral("kde_settings.conf")] = QString{QLatin1String(SDDM_CONFIG_DIR "/") + QStringLiteral("kde_settings.conf")};

    args[QStringLiteral("sddm.conf")] = QLatin1String(SDDM_CONFIG_FILE);

    if (!cursorTheme.isNull()) {
        args[QStringLiteral("kde_settings.conf/Theme/CursorTheme")] = cursorTheme;
    } else {
        qDebug() << "Cannot find cursor theme value.";
    }

    if (!dpiValue.isEmpty()) {
        args[QStringLiteral("kde_settings.conf/X11/ServerArguments")] = dpiArgument;
    } else {
        qDebug() << "Cannot find scaling DPI value.";
    }

    if (!numLock.isEmpty()) {
        if (numLock == QStringLiteral("0")) {
            args[QStringLiteral("kde_settings.conf/General/Numlock")] = QStringLiteral("on");
        } else if (numLock == QStringLiteral("1")) {
            args[QStringLiteral("kde_settings.conf/General/Numlock")] = QStringLiteral("off");
        } else if (numLock == QStringLiteral("2")) {
            args[QStringLiteral("kde_settings.conf/General/Numlock")] = QStringLiteral("none");
        }
    } else {
        qDebug() << "Cannot find NumLock value.";
    }

    if (!plasmaFont.isEmpty()) {
        args[QStringLiteral("kde_settings.conf/Theme/Font")] = plasmaFont;
    }
    else {
        qDebug() << "Cannot find Plasma font value.";
    }

    if (!fontconfigPath.isEmpty()) {
        args[QStringLiteral("fontconfig")] = fontconfigPath;
    } else {
        qDebug() << "Cannot find fontconfig folder.";
    }

    if (!kdeglobalsPath.isEmpty()) {
        args[QStringLiteral("kdeglobals")] = kdeglobalsPath;
    } else {
        qDebug() << "Cannot find kdeglobals file.";
    }

    if (!plasmarcPath.isEmpty()) {
        args[QStringLiteral("plasmarc")] = plasmarcPath;
    } else {
        qDebug() << "Cannot find plasmarc file.";
    }

    KAuth::Action syncAction(QStringLiteral("org.kde.kcontrol.kcmsddm.sync"));
    syncAction.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
    syncAction.setArguments(args);

    auto job = syncAction.execute();
    connect(job, &KJob::result, this, [this, job] {
        if (job->error()) {
            qDebug() << "Synchronization failed";
            qDebug() << job->errorString();
            qDebug() << job->errorText();
            if (!job->errorText().isEmpty()) {
                Q_EMIT errorOccured(job->errorText());
            }
        } else {
            qDebug() << "Synchronization successful";
        }
    });
    job->start();
}

void SddmKcm::resetSyncronizedSettings()
{
    // initial check for sddm user; abort if user not present
    // we have to check with QString and isEmpty() instead of QDir and exists() because
    // QDir returns "." and true for exists() in the case of a non-existent user
    QString sddmHomeDirPath = KUser("sddm").homeDir();
    if (sddmHomeDirPath.isEmpty()) {
        Q_EMIT errorOccured(QStringLiteral("Cannot proceed, user 'sddm' does not exist. Please check your SDDM install."));
        return;
    }

    // send paths to helper
    QVariantMap args;

    args[QStringLiteral("kde_settings.conf")] = QString{QLatin1String(SDDM_CONFIG_DIR "/") + QStringLiteral("kde_settings.conf")};

    args[QStringLiteral("sddm.conf")] = QLatin1String(SDDM_CONFIG_FILE);

    args[QStringLiteral("kde_settings.conf/Theme/CursorTheme")] = QVariant();

    args[QStringLiteral("kde_settings.conf/X11/ServerArguments")] = QVariant();

    args[QStringLiteral("kde_settings.conf/General/Numlock")] = QVariant();

    args[QStringLiteral("kde_settings.conf/Theme/Font")] = QVariant();

    KAuth::Action resetAction(QStringLiteral("org.kde.kcontrol.kcmsddm.reset"));
    resetAction.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
    resetAction.setArguments(args);

    auto job = resetAction.execute();

    connect(job, &KJob::result, this, [this, job] {
        if (job->error()) {
            qDebug() << "Reset failed";
            qDebug() << job->errorString();
            qDebug() << job->errorText();
            if (!job->errorText().isEmpty()) {
                Q_EMIT errorOccured(job->errorText());
            }
        } else {
            qDebug() << "Reset successful";
        }
    });
    job->start();
}

#include "sddmkcm.moc"
