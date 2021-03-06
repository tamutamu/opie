/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef FLAT_H
#define FLAT_H

#include <qtopia/windowdecorationinterface.h>
#include <qcache.h>
#include <qimage.h>

class FlatDecoration : public WindowDecorationInterface
{
public:
    FlatDecoration();
    virtual ~FlatDecoration();

    virtual int metric( Metric m, const WindowData * ) const;
    virtual void drawArea( Area a, QPainter *, const WindowData * ) const;
    virtual void drawButton( Button b, QPainter *, const WindowData *, int x, int y, int w, int h, QWSButton::State ) const;
    virtual QRegion mask( const WindowData * ) const;
    virtual QString name() const;
    virtual QPixmap icon() const;

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

private:
    QCache<QImage> buttonCache;
};

#endif
