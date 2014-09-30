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

#include <KAuth/KAuthActionReply>
#include <KAuth/KAuthActionReply>

#include <KAboutData>

#include <KTabWidget>
#include <KLocalizedString>
#include <QDebug>

#include "config.h"
#include "themeconfig.h"
#include "advanceconfig.h"

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kdemacros.h>
#include <kauthexecutejob.h>

K_PLUGIN_FACTORY(SddmKcmFactory, registerPlugin<SddmKcm>();)
K_EXPORT_PLUGIN(SddmKcmFactory("kcm_sddm", "kcm_sddm"))

SddmKcm::SddmKcm(QWidget *parent, const QVariantList &args) :
    KCModule(parent, args)
{
    KAboutData* aboutData = new KAboutData("kcmsddm", i18n("SDDM KDE Config"), "0.1");

    aboutData->setVersion(0);

    aboutData->setShortDescription(i18n("Login screen using the SDDM"));
    aboutData->setLicense(KAboutLicense::GPL_V2);
    //aboutData->setHomepage("https://github.com/sddm/sddm");

    aboutData->addAuthor("Reza Fatahilah Shah", i18n("Author"), "rshah0385@kireihana.com");
    aboutData->addAuthor("David Edmundson", i18n("Author"), "davidedmundson@kde.org");
;
    
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

    qDebug() << "Ovr:" << args["theme.conf.user"].toString();
    args.unite(mThemeConfig->save());
    args.unite(mAdvanceConfig->save());

    KAuth::Action saveAction("org.kde.kcontrol.kcmsddm.save");

    saveAction.setHelperId("org.kde.kcontrol.kcmsddm");
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
    KTabWidget* tabHolder = new KTabWidget(this);
    layout->addWidget(tabHolder);
    
    mThemeConfig = new ThemeConfig(this);
    connect(mThemeConfig, SIGNAL(changed(bool)), SIGNAL(changed(bool)));
    
    tabHolder->addTab(mThemeConfig, i18n("Theme"));
    
    mAdvanceConfig = new AdvanceConfig(this);
    connect(mAdvanceConfig, SIGNAL(changed(bool)), SIGNAL(changed(bool)));
    
    tabHolder->addTab(mAdvanceConfig, i18n("Advanced"));
}

#include "sddmkcm.moc"
