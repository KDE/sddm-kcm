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
#ifndef SDDMAUTHHELPER_H
#define SDDMAUTHHELPER_H

#include <KAuth>

using namespace KAuth;

class SddmAuthHelper: public QObject
{
    Q_OBJECT
public Q_SLOTS:
    ActionReply save(const QVariantMap &args);
    ActionReply installtheme(const QVariantMap &args);
    ActionReply uninstalltheme(const QVariantMap &args);
};

#endif //SDDMAUTHHELPER_H
