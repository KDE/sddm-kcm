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
#ifndef SDDMKCM_H
#define SDDMKCM_H

#include <KCModule>
#include <KSharedConfig>

class ThemeConfig;
class AdvancedConfig;

class SddmKcm : public KCModule
{
    Q_OBJECT
public:
    explicit SddmKcm(QWidget *parent, const QVariantList &args);
    ~SddmKcm() Q_DECL_OVERRIDE;

public Q_SLOTS:
    void save() Q_DECL_OVERRIDE;

private:
    void prepareUi();

private:
    KSharedConfigPtr mSddmConfig;
    ThemeConfig *mThemeConfig;
    AdvancedConfig *mAdvancedConfig;
};

#endif // SDDMKCM_H
