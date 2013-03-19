/*
 * Copyright © 2006-2007 Fredrik Höglund <fredrik@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2 or at your option version 3 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef LEGACYTHEME_H
#define LEGACYTHEME_H

#include "cursortheme.h"

/**
 * The LegacyTheme class is a CursorTheme implementation for the KDE/Qt
 * bitmap cursors and the X11 cursor font.
 *
 * The cursors returned by loadImage() and loadCursor() are created from
 * copies of the bitmaps used to create cursors in the Qt and KDE libs.
 *
 * If any of those bitmaps are changed in either Qt or KDE, or new bitmaps
 * are added, those changes won't automatically be reflected here.
 *
 * Cursors that aren't created from bitmaps are created from the X11 cursor
 * font.
 */
class LegacyTheme : public CursorTheme
{
    public:
        LegacyTheme();
        virtual ~LegacyTheme();

        QImage loadImage(const QString &name, int size = 0) const;
        QCursor loadCursor(const QString &name, int size = 0) const;

    protected:
        LegacyTheme(const QString &title, const QString &description = QString())
            : CursorTheme(title, description) {}

    private:
        class Private;
};

#endif // LEGACYTHEME_H

