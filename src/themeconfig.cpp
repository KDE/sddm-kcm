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
#include "themeconfig.h"
#include "ui_themeconfig.h"
#include "themesmodel.h"
#include "themesdelegate.h"

#include <QFile>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlContext>
#include <QDebug>
#include <QStandardPaths>
#include <QPointer>
#include <QFileDialog>

#include <KMessageBox>
#include <KConfigGroup>
#include <KNewStuff3/KNS3/DownloadDialog>
#include <KAuthAction>
#include <KAuthActionReply>
#include <KAuthExecuteJob>

#include "config.h"

ThemeConfig::ThemeConfig(const KSharedConfigPtr &config, QWidget *parent) :
    QWidget(parent),
    mConfig(config)
{
    configUi = new Ui::ThemeConfig();
    configUi->setupUi(this);
    configUi->messageWidget->setVisible(false);

    ThemesModel *model = new ThemesModel(this);
    configUi->themesListView->setModel(model);

    ThemesDelegate *delegate = new ThemesDelegate(configUi->themesListView);
    delegate->setPreviewSize(QSize(128,128));
    configUi->themesListView->setItemDelegate(delegate);
    model->populate();
    connect(this, &ThemeConfig::themesChanged, model, &ThemesModel::populate);

    connect(configUi->themesListView, SIGNAL(activated(QModelIndex)), SLOT(themeSelected(QModelIndex)));
    connect(configUi->themesListView, SIGNAL(clicked(QModelIndex)), SLOT(themeSelected(QModelIndex)));
    connect(configUi->selectBackgroundButton, SIGNAL(imagePathChanged(QString)), SLOT(backgroundChanged(QString)));

    connect(configUi->getNewButton, &QPushButton::clicked, this, &ThemeConfig::getNewStuffClicked);
    connect(configUi->installFromFileButton, &QPushButton::clicked, this, &ThemeConfig::installFromFileClicked);
    connect(configUi->removeThemeButton, &QPushButton::clicked, this, &ThemeConfig::removeThemeClicked);


    prepareInitialTheme();
}

ThemeConfig::~ThemeConfig()
{
    delete configUi;
}

QVariantMap ThemeConfig::save()
{
    QModelIndex index = configUi->themesListView->currentIndex();
    if (!index.isValid()) {
        return QVariantMap();
    }

    QVariantMap args;

    args["sddm.conf/Theme/Current"] = index.data(ThemesModel::IdRole);

    if (!mThemeConfigPath.isEmpty()) {
        if (!mBackgroundPath.isEmpty()) {
            args["theme.conf.user/General/background"] = mBackgroundPath;
            args["theme.conf.user/General/type"] = QStringLiteral("image");
        } else {
            args["theme.conf.user/General/type"] = QStringLiteral("color");
        }
    }
    return args;
}

QString ThemeConfig::themeConfigPath() const
{
    return mThemeConfigPath;
}

void ThemeConfig::prepareInitialTheme()
{
    const QString initialTheme = mConfig->group("Theme").readEntry("Current");

    QModelIndex index = findThemeIndex(initialTheme);
    if (!index.isValid() && configUi->themesListView->model()->rowCount() > 0) {
        //if we can't find the currently configured theme from the config, just select the first theme in the UI
        index = configUi->themesListView->model()->index(0,0);
    }
    configUi->themesListView->setCurrentIndex(index);
    themeSelected(index);
}

QModelIndex ThemeConfig::findThemeIndex(const QString &id) const
{
    QAbstractItemModel* model = configUi->themesListView->model();
    
    for (int i=0; i < model->rowCount(); i++) {
        QModelIndex index = model->index(i, 0);
        if (index.data(ThemesModel::IdRole).toString() == id) {
            return index;
        }
    }
    
    return QModelIndex();
}

void ThemeConfig::themeSelected(const QModelIndex &index)
{
    if (!configUi->quickWidget->source().isValid()) {
        const QString mainQmlPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "sddm-kcm/main.qml");
        configUi->quickWidget->setSource(QUrl::fromLocalFile(mainQmlPath));
    }

    QString themePath = index.data(ThemesModel::PathRole).toString();
    QString previewFilename = themePath + index.data(ThemesModel::PreviewRole).toString();

    configUi->quickWidget->rootObject()->setProperty("themeName", index.data().toString());
    configUi->quickWidget->rootObject()->setProperty("previewPath", previewFilename);
    configUi->quickWidget->rootObject()->setProperty("authorName", index.data(ThemesModel::AuthorRole).toString());
    configUi->quickWidget->rootObject()->setProperty("description", index.data(ThemesModel::DescriptionRole).toString());
    configUi->quickWidget->rootObject()->setProperty("license", index.data(ThemesModel::LicenseRole).toString());
    configUi->quickWidget->rootObject()->setProperty("email", index.data(ThemesModel::EmailRole).toString());
    configUi->quickWidget->rootObject()->setProperty("website", index.data(ThemesModel::WebsiteRole).toString());
    configUi->quickWidget->rootObject()->setProperty("copyright", index.data(ThemesModel::CopyrightRole).toString());
    configUi->quickWidget->rootObject()->setProperty("version", index.data(ThemesModel::VersionRole).toString());

    //Check if we need to display configuration group
    QString configPath = themePath + index.data(ThemesModel::ConfigFileRole).toString();
    prepareConfigurationUi(configPath);

    emit changed(true);
}

void ThemeConfig::backgroundChanged(const QString &imagePath)
{
    mBackgroundPath = imagePath;
    emit changed(true);
}

void ThemeConfig::prepareConfigurationUi(const QString &configPath)
{
    mThemeConfigPath = configPath;

    QFile configFile(configPath);
    
    if (configFile.exists()) {
        KSharedConfigPtr themeConfig = KSharedConfig::openConfig(configFile.fileName() + ".user", KConfig::CascadeConfig);
        themeConfig->addConfigSources({configFile.fileName()});

        configUi->customizeBox->setVisible(true);
        configUi->selectBackgroundButton->setImagePath(themeConfig->group("General").readEntry("background"));
    } else {
        configUi->customizeBox->setVisible(false);
    }
}

void ThemeConfig::dump()
{
    //dump sddm conf
    KConfigGroup config = mConfig->group("General");

    qDebug() << "Current theme:" << config.readEntry("CurrentTheme");
}

void ThemeConfig::getNewStuffClicked()
{
    QPointer<KNS3::DownloadDialog> dialog(new KNS3::DownloadDialog(QStringLiteral("sddmtheme.knsrc"), this));

    dialog->setWindowTitle(i18n("Download New SDDM Themes"));
    if (dialog->exec()) {
        emit themesChanged();
    }
    delete dialog.data();
}

void ThemeConfig::installFromFileClicked()
{
    QPointer<QFileDialog> dialog(new QFileDialog(this));
    dialog->exec();
    QStringList files = dialog->selectedFiles();
    if (files.count() == 1) {
        QString file = files.first();
        KAuth::Action saveAction(QStringLiteral("org.kde.kcontrol.kcmsddm.installtheme"));
        saveAction.setHelperId("org.kde.kcontrol.kcmsddm");
        saveAction.addArgument(QStringLiteral("filePath"), file);
        auto job = saveAction.execute();
        if (!job->exec()) {
            configUi->messageWidget->setMessageType(KMessageWidget::Warning);
            configUi->messageWidget->setText(job->errorString());
            configUi->messageWidget->animatedShow();
        } else {
            emit themesChanged();
        }
    }

    delete dialog.data();
}

void ThemeConfig::removeThemeClicked()
{
    if (!configUi->themesListView->currentIndex().isValid()) {
        return;
    }

    const QString path = configUi->themesListView->currentIndex().data(ThemesModel::PathRole).toString();
     KAuth::Action saveAction(QStringLiteral("org.kde.kcontrol.kcmsddm.uninstalltheme"));
    saveAction.setHelperId("org.kde.kcontrol.kcmsddm");
    saveAction.addArgument(QStringLiteral("filePath"), path);
    auto job = saveAction.execute();
    if (!job->exec()) {
        configUi->messageWidget->setMessageType(KMessageWidget::Warning);
        configUi->messageWidget->setText(job->errorString());
        configUi->messageWidget->animatedShow();
    } else {
        emit themesChanged();
    }
}

