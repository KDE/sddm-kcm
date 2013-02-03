#include "sddmkcm.h"

#include <QHBoxLayout>

#include <KPluginFactory>
#include <KAuth/Action>
#include <KAuth/ActionReply>
#include <KAboutData>
#include <KTabWidget>
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
#include <KLocalizedString>
#include <KDebug>

#include "themeconfig.h"

K_PLUGIN_FACTORY(SddmKcmFactory, registerPlugin<SddmKcm>();)
K_EXPORT_PLUGIN(SddmKcmFactory("kcm_sddm", "kcm_sddm"))

SddmKcm::SddmKcm(QWidget *parent, const QVariantList &args) :
    KCModule(SddmKcmFactory::componentData(), parent, args)
{
    KAboutData* aboutData = new KAboutData("kcmsddm", 0, ki18n("SDDM KDE Config"), "0");

    aboutData->setVersion(0);

    aboutData->setShortDescription(ki18n("Login screen using the SDDM"));
    aboutData->setLicense(KAboutData::License_GPL);
    aboutData->setCopyrightStatement(ki18n("(c) 2013 Reza Fatahilah Shah"));
    //aboutData->setHomepage("https://github.com/sddm/sddm");

    aboutData->addAuthor(ki18n("Reza Fatahilah Shah"), ki18n("Author"), "rshah0385@kireihana.com");
    
    setAboutData(aboutData);
    
    setNeedsAuthorization(true);
    
    prepareUi();
}

SddmKcm::~SddmKcm()
{
}

void SddmKcm::save()
{
    QVariantMap args;
    
    args = mThemeConfig->save();
    
    KAuth::Action saveAction("org.kde.kcontrol.kcmsddm.save");
    saveAction.setHelperID("org.kde.kcontrol.kcmsddm");
    saveAction.setArguments(args);
    
    KAuth::ActionReply reply = saveAction.execute();
    
    if (reply.failed()){
        kDebug() << "Failed";
    } else {
        changed(false);
        kDebug() << "Option saved";
    }
    
}

void SddmKcm::prepareUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    KTabWidget* tabHolder = new KTabWidget(this);
    layout->addWidget(tabHolder);
    
    mThemeConfig = new ThemeConfig(this);
    connect(mThemeConfig, SIGNAL(changed(bool)), SIGNAL(changed(bool)));
    
    tabHolder->addTab(mThemeConfig, i18n("Theme"));
}