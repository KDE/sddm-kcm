/*
 * Copyright © 2006-2007 Fredrik Höglund <fredrik@kde.org>
 *
 * Parts of this file are based on code from xserver/dix/glyphcurs.c
 * in X.org,
 *
 * Copyright © 1987, 1998 The Open Group
 * Copyright © 1987       Digital Equipment Corporation
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

#include <KLocale>

#include <QCursor>
#include <QImage>
#include <QHash>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xlibint.h>

#include "legacytheme.h"
#include "bitmaps.h"


namespace {

    // Borrowed from xc/lib/Xcursor/library.c
    static const char * const standard_names[] = {
        /* 0 */
        "X_cursor",         "arrow",            "based_arrow_down",     "based_arrow_up",
        "boat",             "bogosity",         "bottom_left_corner",   "bottom_right_corner",
        "bottom_side",      "bottom_tee",       "box_spiral",           "center_ptr",
        "circle",           "clock",            "coffee_mug",           "cross",

        /* 32 */
        "cross_reverse",    "crosshair",        "diamond_cross",        "dot",
        "dotbox",           "double_arrow",     "draft_large",          "draft_small",
        "draped_box",       "exchange",         "fleur",                "gobbler",
        "gumby",            "hand1",            "hand2",                "heart",

        /* 64 */
        "icon",             "iron_cross",       "left_ptr",             "left_side",
        "left_tee",         "leftbutton",       "ll_angle",             "lr_angle",
        "man",              "middlebutton",     "mouse",                "pencil",
        "pirate",           "plus",             "question_arrow",       "right_ptr",

        /* 96 */
        "right_side",       "right_tee",        "rightbutton",          "rtl_logo",
        "sailboat",         "sb_down_arrow",    "sb_h_double_arrow",    "sb_left_arrow",
        "sb_right_arrow",   "sb_up_arrow",      "sb_v_double_arrow",    "shuttle",
        "sizing",           "spider",           "spraycan",             "star",

        /* 128 */
        "target",           "tcross",           "top_left_arrow",       "top_left_corner",
        "top_right_corner", "top_side",         "top_tee",              "trek",
        "ul_angle",         "umbrella",         "ur_angle",             "watch",
        "xterm",
    };
}


struct CursorBitmap
{
    CursorBitmap(const char * const *xpm, const QPoint &hotspot)
        : xpm(xpm), hotspot(hotspot) {}
    const char * const *xpm;
    QPoint hotspot;
};


struct CursorMetrics
{
    int xhot, yhot;
    int width, height;
};


class LegacyTheme::Private
{
    public:
        static int cursorShape(const QString &name);
        static CursorMetrics cursorMetrics(int shape);
        static QImage fontImage(const QString &name, int *xhot = 0, int *yhot = 0);
        static QImage bitmapImage(const QString &name, int *xhot = 0, int *yhot = 0);

    private:
        static QHash<QString, int> shapes;
        static QHash<QString, CursorBitmap*> bitmaps;
        static XFontStruct *xfs;
};

QHash<QString, int> LegacyTheme::Private::shapes;
QHash<QString, CursorBitmap*> LegacyTheme::Private::bitmaps;
XFontStruct *LegacyTheme::Private::xfs = NULL;


int LegacyTheme::Private::cursorShape(const QString &name)
{
    // A font cursor is created from two glyphs; a shape glyph and a mask glyph
    // stored in pairs in the font, with the shape glyph first. There's only one
    // name for each pair. This function always returns the index for the
    // shape glyph.
    if (shapes.isEmpty())
    {
        int num = XC_num_glyphs / 2;
        shapes.reserve(num + 5);

        for (int i = 0; i < num; ++i)
            shapes.insert(standard_names[i], i << 1);

        // Qt uses alternative names for some core cursors
        shapes.insert("size_all",      XC_fleur);
        shapes.insert("up_arrow",      XC_center_ptr);
        shapes.insert("ibeam",         XC_xterm);
        shapes.insert("wait",          XC_watch);
        shapes.insert("pointing_hand", XC_hand2);
    }

    return shapes.value(name, -1);
}


CursorMetrics LegacyTheme::Private::cursorMetrics(int shape)
{
    CursorMetrics metrics;	

    // Get the metrics for the mask glyph
    XCharStruct xcs = xfs->per_char[shape + 1];

    // Compute the width, height and cursor hotspot from the glyph metrics.
    // Note that the X11 definition of right bearing is the right-ward distance
    // from the X origin to the X coordinate of the rightmost pixel in the glyph.
    // In QFontMetrics the right bearing is defined as the left-ward distance
    // from the X origin of the hypothetical subsequent glyph to the X coordinate
    // of the rightmost pixel in this glyph.
    metrics.width  = xcs.rbearing - xcs.lbearing;
    metrics.height = xcs.ascent   + xcs.descent;

    // The cursor hotspot is defined as the X and Y origin of the glyph.
    if (xcs.lbearing < 0) {
        metrics.xhot = -xcs.lbearing;
        if (xcs.rbearing < 0)           // rbearing can only be < 0 when lbearing < 0
            metrics.width -= xcs.rbearing;
    } else {                            // If the ink starts to the right of the X coordinate.
        metrics.width += xcs.lbearing;  // With cursors this is probably never the case in practice,
        metrics.xhot = 0;               // since it would put the hotspot outside the image.
    }

    if (xcs.ascent > 0) {
        metrics.yhot = xcs.ascent;
        if (xcs.descent < 0)            // descent can only be < 0 when ascent > 0
            metrics.height -= xcs.descent;
    } else {                            // If the ink starts below the baseline.
        metrics.height -= xcs.ascent;   // With cursors this is probably never the case in practice,
        metrics.yhot = 0;               // since it would put the hotspot outside the image.
    }

    return metrics;
}


QImage LegacyTheme::Private::fontImage(const QString &name, int *xhot_return, int *yhot_return)
{
    // Note that the reason we need this function is that XcursorLibraryLoadImage()
    // doesn't work with the core theme, and X11 doesn't provide any other means to
    // obtain the image of a cursor other than that of the active one.
    Display *dpy = QX11Info::display();
    QImage image;

    Q_ASSERT(name.length() > 0);

    // Make sure the cursor font is loaded
    if (dpy->cursor_font == None)
        dpy->cursor_font = XLoadFont(dpy, CURSORFONT);

    // Query the font metrics for the cursor font
    if (dpy->cursor_font && !xfs)
        xfs = XQueryFont(dpy, dpy->cursor_font);

    // Get the glyph shape index for the cursor name
    int shape = cursorShape(name);

    // If we there's no matching cursor in the font, if the font couldn't be loaded,
    // or the font metrics couldn't be queried, return a NULL image.
    if (shape == -1 || dpy->cursor_font == None || xfs == NULL)
        return image;

    // Get the cursor metrics for the shape
    CursorMetrics m = cursorMetrics(shape);

    // Get the 16 bit bitmap and mask glyph indexes
    char source2b[2], mask2b[2];
    source2b[0] = uchar(shape >> 8);
    source2b[1] = uchar(shape & 0xff);

    mask2b[0] = uchar((shape + 1) >> 8);
    mask2b[1] = uchar((shape + 1) & 0xff);

    // Create an 8 bit pixmap and draw the glyphs on the pixmap
    Pixmap pm = XCreatePixmap(dpy, QX11Info::appRootWindow(), m.width, m.height, 8);
    GC gc = XCreateGC(dpy, pm, 0, NULL);
    XSetFont(dpy, gc, dpy->cursor_font);

    enum Colors { BackgroundColor = 0, MaskColor = 1, ShapeColor = 2 };

    // Clear the pixmap to transparent
    XSetForeground(dpy, gc, BackgroundColor);
    XFillRectangle(dpy, pm, gc, 0, 0, m.width, m.height);

    // Draw the mask
    XSetForeground(dpy, gc, MaskColor);
    XDrawString16(dpy, pm, gc, m.xhot, m.yhot, (XChar2b*)mask2b, 1);

    // Draw the shape
    XSetForeground(dpy, gc, ShapeColor );
    XDrawString16(dpy, pm, gc, m.xhot, m.yhot, (XChar2b*)source2b, 1);
    XFreeGC(dpy, gc);

    // Convert the pixmap to an XImage
    XImage *ximage = XGetImage(dpy, pm, 0, 0, m.width, m.height, AllPlanes, ZPixmap);
    XFreePixmap(dpy, pm);

    // Background color, mask color, shape color
    static const quint32 color[] =
    {
        0x00000000, // black, fully transparent
        0xffffffff, // white, fully opaque
        0xff000000, // black, fully opaque
    };

    // Convert the XImage to a QImage
    image = QImage(ximage->width, ximage->height, QImage::Format_ARGB32_Premultiplied);
    for (int y = 0; y < ximage->height; y++)
    {
        quint8  *s = reinterpret_cast<quint8*>(ximage->data + (y * ximage->bytes_per_line));
        quint32 *d = reinterpret_cast<quint32*>(image.scanLine(y));

        for (int x = 0; x < ximage->width; x++)
            *(d++) = color[*(s++)];
    }

    // Free the XImage
    free(ximage->data);
    ximage->data = NULL;
    XDestroyImage(ximage);

    // Return the cursor hotspot to the caller
    if (xhot_return)
        *xhot_return = m.xhot;

    if (yhot_return)
        *yhot_return = m.yhot;

    return image;
}


QImage LegacyTheme::Private::bitmapImage(const QString &name, int *xhot_return, int *yhot_return)
{
    const CursorBitmap *bitmap;
    QImage image;

    if (bitmaps.isEmpty())
    {
        // These bitmap images are created from the XPM's in bitmaps.h.
        bitmaps.reserve(13);
        bitmaps.insert("size_ver",       new CursorBitmap(size_ver_xpm,   QPoint( 8,  8)));
        bitmaps.insert("size_hor",       new CursorBitmap(size_hor_xpm,   QPoint( 8,  8)));
        bitmaps.insert("size_bdiag",     new CursorBitmap(size_bdiag_xpm, QPoint( 8,  8)));
        bitmaps.insert("size_fdiag",     new CursorBitmap(size_fdiag_xpm, QPoint( 8,  8)));
        bitmaps.insert("left_ptr_watch", new CursorBitmap(busy_xpm,       QPoint( 0,  0)));
        bitmaps.insert("forbidden",      new CursorBitmap(forbidden_xpm,  QPoint(10, 10)));
        //bitmaps.insert("hand2",          new CursorBitmap(kde_hand_xpm,   QPoint( 7,  0)));
        //bitmaps.insert("pointing_hand",  new CursorBitmap(kde_hand_xpm,   QPoint( 7,  0)));
        bitmaps.insert("whats_this",     new CursorBitmap(whats_this_xpm, QPoint( 0,  0)));
        bitmaps.insert("split_h",        new CursorBitmap(split_h_xpm,    QPoint(16, 16)));
        bitmaps.insert("split_v",        new CursorBitmap(split_v_xpm,    QPoint(16, 16)));
        bitmaps.insert("openhand",       new CursorBitmap(openhand_xpm,   QPoint( 8,  8)));
        bitmaps.insert("closedhand",     new CursorBitmap(closedhand_xpm, QPoint( 8,  8)));
    }

    if ((bitmap = bitmaps.value(name)))
    {
        image = QPixmap(bitmap->xpm).toImage()
                .convertToFormat(QImage::Format_ARGB32_Premultiplied);

        // Return the hotspot to the caller
        if (xhot_return)
            *xhot_return = bitmap->hotspot.x();

        if (yhot_return)
            *yhot_return = bitmap->hotspot.y();
    }	

    return image;
}



// ---------------------------------------------------------------------------



LegacyTheme::LegacyTheme()
    : CursorTheme(i18n("KDE Classic"), i18n("The default cursor theme in KDE 2 and 3"))
{
    setName("#kde_legacy#");
}


LegacyTheme::~LegacyTheme()
{
}


QImage LegacyTheme::loadImage(const QString &name, int) const
{
    QImage image;

    // Try to load the image from a bitmap first
    image = Private::bitmapImage(name);

    // If that fails, try to load it from the cursor font
    if (image.isNull())
        image = Private::fontImage(name);
    else
        // Autocrop the image if we created it from a bitmap
        image = autoCropImage(image);

    return image;
}


QCursor LegacyTheme::loadCursor(const QString &name, int) const
{
    QImage image;
    int xhot = 0, yhot = 0;

    // Try to load the image from a bitmap first
    image = Private::bitmapImage(name, &xhot, &yhot);

    // If that fails, try to load it from the cursor font
    if (image.isNull())
        image = Private::fontImage(name, &xhot, &yhot);

    // Return the default cursor if that fails as well
    if (image.isNull())
        return QCursor(); 

    QPixmap pixmap = QPixmap::fromImage(image);
    QCursor cursor(pixmap, xhot, yhot);

    setCursorName(cursor, name);
    return cursor;
}

