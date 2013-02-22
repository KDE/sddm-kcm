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
#ifndef THEMECONFIG_H
#define THEMECONFIG_H

#include <QWidget>

#include <KSharedConfig>

namespace Ui {
    class ThemeConfig;
}

class QModelIndex;

class ThemeConfig : public QWidget
{
    Q_OBJECT
public:
    explicit ThemeConfig(QWidget *parent = 0);
    ~ThemeConfig();
    
    QVariantMap save();
    QString themeConfigPath() const;

signals:
    void changed(bool);
    
private slots:
    void themeSelected(const QModelIndex &index);
    void backgroundChanged(const QString &imagePath);
    
private:
    Ui::ThemeConfig *configUi;
    KSharedConfigPtr mConfig;
    QString mBackgroundPath;
    QString mThemeConfigPath;
    
    void prepareInitialTheme();
    QModelIndex findThemeIndex(const QString &theme) const;
    void prepareConfigurationUi(const QString &configPath);
    void dump();
};

#endif // THEMECONFIG_H