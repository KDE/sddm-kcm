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

#include "dummytheme.h"

#include <QImage>
#include <QCursor>

#include <KLocalizedString>

DummyTheme::DummyTheme()
    : CursorTheme(i18n("Default"), i18n("The default cursor theme in SDDM"))
{
}


DummyTheme::~DummyTheme()
{
}


QImage DummyTheme::loadImage(const QString &name, int) const
{
    Q_UNUSED(name)

    return QImage();
}


qulonglong DummyTheme::loadCursor(const QString &name, int) const
{
    Q_UNUSED(name)

    return 0;
}
