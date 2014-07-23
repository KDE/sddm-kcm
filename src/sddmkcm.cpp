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
#include "sddmkcm.h"

#include <QHBoxLayout>

#include <KPluginFactory>
#include <KAuth/Action>
#include <KAuth/ActionReply>
#include <K4AboutData>
#include <KTabWidget>
#include <KLocalizedString>
#include <KDebug>

#include "config.h"
#include "themeconfig.h"
#include "advanceconfig.h"

K_PLUGIN_FACTORY(SddmKcmFactory, registerPlugin<SddmKcm>();)
K_EXPORT_PLUGIN(SddmKcmFactory("kcm_sddm", "kcm_sddm"))

SddmKcm::SddmKcm(QWidget *parent, const QVariantList &args) :
    KCModule(parent, args)
{
    K4AboutData* aboutData = new K4AboutData("kcmsddm", 0, ki18n("SDDM KDE Config"), "0");

    aboutData->setVersion(0);

    aboutData->setShortDescription(ki18n("Login screen using the SDDM"));
    aboutData->setLicense(K4AboutData::License_GPL);
    aboutData->setCopyrightStatement(ki18n("(c) 2013 Reza Fatahilah Shah"));
    //aboutData->setHomepage("https://github.com/sddm/sddm");

    aboutData->addAuthor(ki18n("Reza Fatahilah Shah"), ki18n("Author"), "rshah0385@kireihana.com");
    
    //FIXME
//     setAboutData(aboutData);
    
    setNeedsAuthorization(true);
    
    prepareUi();
}

SddmKcm::~SddmKcm()
{
}

void SddmKcm::save()
{
    QVariantMap args;
    
    args["sddm.conf"] = SDDM_CONFIG_FILE;

    if (!mThemeConfig->themeConfigPath().isEmpty()) {
        args["theme.conf.user"] = mThemeConfig->themeConfigPath() + ".user";
    }

    kDebug() << "Ovr:" << args["theme.conf.user"].toString();
    args.unite(mThemeConfig->save());
    args.unite(mAdvanceConfig->save());

    KAuth::Action saveAction("org.kde.kcontrol.kcmsddm.save");

    //FIXME
//     saveAction.setHelperID("org.kde.kcontrol.kcmsddm");
    saveAction.setArguments(args);
    
    //FIXME
    KAuth::ActionReply reply;// = saveAction.execute();
    
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
    
    mAdvanceConfig = new AdvanceConfig(this);
    connect(mAdvanceConfig, SIGNAL(changed(bool)), SIGNAL(changed(bool)));
    
    tabHolder->addTab(mAdvanceConfig, i18n("Advanced"));
}
