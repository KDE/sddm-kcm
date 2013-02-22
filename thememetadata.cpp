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
#include "thememetadata.h"

#include <KDesktopFile>
#include <KConfigGroup>

class ThemeMetadataPrivate
{
public:
    QString themeid;
    QString name;
    QString description;
    QString author;
    QString email;
    QString version;
    QString website;
    QString license;
    QString themeapi;
    QString mainscript;
    QString screenshot;
    QString copyright;
    QString path;
    QString configfile;
};

ThemeMetadata::ThemeMetadata(const QString &id, const QString &path)
    : d(new ThemeMetadataPrivate)
{
    d->path = path + "/";
    read(d->path + "metadata.desktop");
    d->themeid = id;
}

ThemeMetadata::ThemeMetadata(const ThemeMetadata &other)
    : d(new ThemeMetadataPrivate(*other.d))
{
}

ThemeMetadata::~ThemeMetadata()
{
    delete d;
}

void ThemeMetadata::read(const QString &filename)
{
    if (filename.isEmpty()) {
        return;
    }
    
    QSharedPointer<KConfig> configFile = QSharedPointer<KConfig>(new KConfig(filename, KConfig::SimpleConfig));
    
    KConfigGroup config = configFile->group("SddmGreeterTheme");
    
    //d->themeid = config.readEntry("Theme-Id");
    d->name = config.readEntry("Name");
    d->description = config.readEntry("Description");
    d->author = config.readEntry("Author");
    d->email = config.readEntry("Email");
    d->version = config.readEntry("Version");
    d->website = config.readEntry("Website");
    d->license = config.readEntry("License");
    d->themeapi = config.readEntry("Theme-API");
    d->mainscript = config.readEntry("MainScript");
    d->screenshot = config.readEntry("Screenshot");
    d->copyright = config.readEntry("Copyright");
    d->configfile = config.readEntry("ConfigFile");
}

QString ThemeMetadata::path() const
{
    return d->path;
}

QString ThemeMetadata::themeid() const
{
    return d->themeid;
}

QString ThemeMetadata::name() const
{
    return d->name;
}

QString ThemeMetadata::description() const
{
    return d->description;
}

QString ThemeMetadata::author() const
{
    return d->author;
}

QString ThemeMetadata::version() const
{
    return d->version;
}

QString ThemeMetadata::email() const
{
    return d->email;
}

QString ThemeMetadata::website() const
{
    return d->website;
}

QString ThemeMetadata::license() const
{
    return d->license;
}

QString ThemeMetadata::themeapi() const
{
    return d->themeapi;
}

QString ThemeMetadata::mainscript() const
{
    return d->mainscript;
}

QString ThemeMetadata::screenshot() const
{
    return d->screenshot;
}

QString ThemeMetadata::copyright() const
{
    return d->copyright;
}

QString ThemeMetadata::configfile() const
{
    return d->configfile;
}