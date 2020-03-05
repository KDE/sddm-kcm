/*
    Copyright 2019 Filip Fila <filipfila.kde@gmail.com>
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
#ifndef ADVANCEDCONFIG_H
#define ADVANCEDCONFIG_H

#include <QWidget>

#include <KSharedConfig>

namespace Ui {
    class AdvancedConfig;
}

class SortProxyModel;
class UsersModel;
class SessionModel;

class AdvancedConfig : public QWidget
{
    Q_OBJECT
public:
    explicit AdvancedConfig(const KSharedConfigPtr &config, QWidget *parent = nullptr);
    ~AdvancedConfig();

    void save(QVariantMap &args);

Q_SIGNALS:
    void changed(bool changed=true);

public Q_SLOTS:
    void syncSettingsChanged();
    void resetSettingsChanged();

private Q_SLOTS:
    void slotUidRangeChanged();

private:
    void load();
    bool isUidRangeValid(int minUid, int maxUid) const;

private:
    Ui::AdvancedConfig *configUi;
    KSharedConfigPtr mConfig;
    UsersModel *userModel;
    SessionModel *sessionModel;
};

#endif // ADVANCEDCONFIG_H
