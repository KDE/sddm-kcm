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
#ifndef ADVANCECONFIG_H
#define ADVANCECONFIG_H

#include <QWidget>

#include <KSharedConfig>

namespace Ui {
    class AdvanceConfig;
}

class SortProxyModel;
class CursorTheme;
class UsersModel;

class AdvanceConfig : public QWidget
{
    Q_OBJECT
public:
    explicit AdvanceConfig(QWidget *parent = 0);
    ~AdvanceConfig();
    
    QVariantMap save();
    
signals:
    void changed(bool changed=true);

private slots:
    void slotUidRangeChanged();

private:
    void load();
    bool isUidRangeValid(int minUid, int maxUid) const;

private:
    Ui::AdvanceConfig *configUi;
    KSharedConfigPtr mConfig;
    SortProxyModel *proxyCursorModel;
    UsersModel *userModel;
};

#endif // ADVANCECONFIG_H