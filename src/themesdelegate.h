/*
    Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
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
#ifndef THEMESDELEGATE_H
#define THEMESDELEGATE_H

#include <QAbstractItemDelegate>

class ThemesDelegate : public QAbstractItemDelegate
{
public:
    explicit ThemesDelegate(QObject *parent = 0);
    
    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
    void setPreviewSize(const QSize &size);
    
    static const int SCREENSHOT_SIZE = 128;
    static const int BLUR_INCREMENT = 9;
    static const int MARGIN = 6;

    void resetMaxHeight() { m_maxHeight = 0; }
    int m_maxHeight;

private:
    int m_maxWidth;
    QSize mSize;
};

#endif //THEMESDELEGATE_H