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
#ifndef THEMEMETADATA_H
#define THEMEMETADATA_H

#include <QString>

class ThemeMetadataPrivate;

class ThemeMetadata
{
public:
    explicit ThemeMetadata(const QString &id, const QString &path = QString());
    ThemeMetadata(const ThemeMetadata &other);
    
    ~ThemeMetadata();
    
    QString path() const;
    QString name() const;
    QString description() const;
    QString author() const;
    QString email() const;
    QString version() const;
    QString website() const;
    QString license() const;
    QString themeapi() const;
    QString screenshot() const;
    QString mainscript() const;
    QString copyright() const;
    QString themeid() const;
    QString configfile() const;

private:
    void read(const QString &filename);
    
private:
    ThemeMetadataPrivate * const d;
};
#endif //THEMEMETADATA_H