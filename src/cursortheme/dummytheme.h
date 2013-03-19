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
#ifndef DUMMYTHEME_H
#define DUMMYTHEME_H

#include "cursortheme.h"

class DummyTheme : public CursorTheme
{
    public:
        DummyTheme();
        virtual ~DummyTheme();

        QImage loadImage(const QString &name, int size = 0) const;
        QCursor loadCursor(const QString &name, int size = 0) const;

    protected:
        DummyTheme(const QString &title, const QString &description = QString())
            : CursorTheme(title, description) {}

    private:
        class Private;
};

#endif //DUMMYTHEME_H