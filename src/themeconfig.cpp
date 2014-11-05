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

#include <KMessageBox>
#include <KConfigGroup>

#include "config.h"

ThemeConfig::ThemeConfig(QWidget *parent) :
    QWidget(parent)
{
    mConfig = KSharedConfig::openConfig(SDDM_CONFIG_FILE, KConfig::SimpleConfig);
    
    configUi = new Ui::ThemeConfig();
    configUi->setupUi(this);
//     configUi->customizeBox->setVisible(false);
    
    ThemesModel *model = new ThemesModel(this);
    configUi->themesListView->setModel(model);

    ThemesDelegate *delegate = new ThemesDelegate(configUi->themesListView);
    delegate->setPreviewSize(QSize(128,128));
    configUi->themesListView->setItemDelegate(delegate);
    model->populate();

    connect(configUi->themesListView, SIGNAL(activated(QModelIndex)), SLOT(themeSelected(QModelIndex)));
    connect(configUi->themesListView, SIGNAL(clicked(QModelIndex)), SLOT(themeSelected(QModelIndex)));
    connect(configUi->selectBackgroundButton, SIGNAL(imagePathChanged(QString)), SLOT(backgroundChanged(QString)));

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
        args["theme.conf.user/General/background"] = mBackgroundPath;
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
    
    const QModelIndex index = findThemeIndex(initialTheme);
    if (!index.isValid()) {
        //KMessageBox::error(this, i18n("Could not find any themes. \nPlease install SDDM themes."), i18n("No SDDM themes"));
        return;
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
        const QString mainQmlPath = QStandardPaths::locate(QStandardPaths::ApplicationsLocation, "sddm-kcm/main.qml");
        configUi->quickWidget->setSource(mainQmlPath);
    }

    QString themePath = index.model()->data(index, ThemesModel::PathRole).toString();
    QString previewFilename = themePath + index.model()->data(index, ThemesModel::PreviewRole).toString();

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
        KSharedConfigPtr themeConfig = KSharedConfig::openConfig(configFile.fileName() + ".user", KConfig::SimpleConfig);
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
