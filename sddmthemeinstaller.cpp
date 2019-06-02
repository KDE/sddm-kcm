/*
 *  Copyright (C) 2016 Marco Martin <mart@kde.org>
 *  Copyright (C) 2016 David Edmundson <davidedmundson@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <KLocalizedString>
#include <KAuthAction>
#include <KAuthExecuteJob>
#include <KMessageBox>
#include <KSharedConfig>
#include <KConfigGroup>

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    QApplication app(argc, argv); //because GHNS doesn't do it's own error reporting on installation failing..

    const QString description = i18n("SDDM theme installer");
    const char version[] = "1.0";

    app.setApplicationVersion(QString::fromLatin1(version));
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription(description);
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("i") << QStringLiteral("install"), i18n("Install a theme.")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("u") << QStringLiteral("uninstall"), i18n("Uninstall a theme.")));

    parser.addPositionalArgument(QStringLiteral("themefile"), i18n("The theme to install, must be an existing archive file."));

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        qWarning() << "No theme file specified.";
        return 0;
    }

    if (parser.isSet(QStringLiteral("install"))) {
        const QFileInfo themefile(args.first());
        if (!themefile.exists()) {
            qWarning() << "Specified theme file does not exist";
            return 0;
        }

        KAuth::Action action(QStringLiteral("org.kde.kcontrol.kcmsddm.installtheme"));
        action.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
        action.addArgument(QStringLiteral("filePath"), themefile.absoluteFilePath());

        KAuth::ExecuteJob *job = action.execute();
        bool rc = job->exec();
        if (!rc) {
            QString errorString = job->errorString();
            qWarning() << job->error() << errorString;
            KMessageBox::sorry(nullptr, errorString, i18n("Unable to install theme"));
            return -1;
        }

        KConfigGroup cg(KSharedConfig::openConfig(QStringLiteral("sddmthemeinstallerrc"), KConfig::SimpleConfig), "DownloadedThemes");
        cg.writeEntry(themefile.absoluteFilePath(), job->data().value(QStringLiteral("installedPaths")).toStringList());
        return 0;
    }
    if (parser.isSet(QStringLiteral("uninstall"))) {
        KConfigGroup cg(KSharedConfig::openConfig(QStringLiteral("sddmthemeinstallerrc"), KConfig::SimpleConfig), "DownloadedThemes");
        const QStringList installed = cg.readEntry(args.first(), QStringList());
        for (const QString &installedTheme: installed) {
            KAuth::Action action(QStringLiteral("org.kde.kcontrol.kcmsddm.uninstalltheme"));
            action.setHelperId(QStringLiteral("org.kde.kcontrol.kcmsddm"));
            action.addArgument(QStringLiteral("filePath"), installedTheme);
            KAuth::ExecuteJob *job = action.execute();
            job->exec();
        }
        return 0;
    }
    qWarning() << "either install or uninstall must be passed as an argument";
    return -1;
}
