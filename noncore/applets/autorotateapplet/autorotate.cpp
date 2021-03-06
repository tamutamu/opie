/*
 * copyright   : (c) 2003 by Greg Gilbert
 * email       : greg@treke.net
 * based on the cardmon applet by Max Reiss
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 *************************************************************************/

#include "autorotate.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/otaskbarapplet.h>
#include <opie2/oresource.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
using namespace Opie::Core;

/* QT */
#include <qpainter.h>

AutoRotate::AutoRotate(QWidget * parent):QWidget(parent)
{
    setFixedWidth( AppLnk::smallIconSize() );
    setFixedHeight( AppLnk::smallIconSize() );

    enabledPm = Opie::Core::OResource::loadImage("autorotate/rotate", Opie::Core::OResource::SmallIcon);
    disabledPm = Opie::Core::OResource::loadImage("autorotate/norotate", Opie::Core::OResource::SmallIcon);

    repaint(true);
    popupMenu = 0;
    show();
}

AutoRotate::~AutoRotate()
{
    if (popupMenu) {
        delete popupMenu;
    }
}

int AutoRotate::position()
{
    return 7;
}

void AutoRotate::mousePressEvent(QMouseEvent *)
{
    QPopupMenu *menu = new QPopupMenu(this);
    menu->insertItem( isRotateEnabled()? "Disable Rotation" : "Enable Rotation" ,1 );

    QPoint p = mapToGlobal(QPoint(0, 0));
    QSize s = menu->sizeHint();
    int opt = menu->exec(QPoint(p.x() + (width() / 2) - (s.width() / 2), p.y() - s.height()), 0);

    if (opt==1)
    {
        setRotateEnabled( !isRotateEnabled() );
        repaint(true);
    }

    delete menu;
}

void AutoRotate::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap( 0, 0, isRotateEnabled()? enabledPm : disabledPm );
}

void AutoRotate::setRotateEnabled(bool status)
{
    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );
    cfg.writeEntry( "rotateEnabled", status );

}
bool AutoRotate::isRotateEnabled()
{
    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );

    bool res = cfg.readBoolEntry( "rotateEnabled", true );

    if (res )
        odebug << "Enabled" << oendl;
    else
        odebug << "Disabled" << oendl;
    return res;
}

EXPORT_OPIE_APPLET_v1( AutoRotate )

