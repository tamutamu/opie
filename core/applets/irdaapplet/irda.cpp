/**********************************************************************
** Copyright (C) 2002 David Woodhouse <dwmw2@infradead.org>
** Heavily based on volumeapplet, (C) 2002 L.J. Potter ljp@llornkcor.com
**  All rights reserved.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "irda.h"
#include <qapplication.h>
#include <stdlib.h>


#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpe/timestring.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/config.h>

#include <qdir.h>
#include <qfileinfo.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qfile.h>
#include <qtimer.h>
#include <qpopupmenu.h>

#include <net/if.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>


//===========================================================================

IrdaApplet::IrdaApplet( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
	setFixedHeight( 18 );
	setFixedWidth( 14 );
	sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	irdaOnPixmap = Resource::loadPixmap( "irdaapplet/irdaon" );
	irdaOffPixmap = Resource::loadPixmap( "irdaapplet/irdaoff" );
	startTimer(5000);
	timerEvent(NULL);
}

IrdaApplet::~IrdaApplet()
{
	close(sockfd);
}

int IrdaApplet::checkIrdaStatus()
{
        struct ifreq ifr;

        strcpy(ifr.ifr_name, "irda0");

        if (ioctl(sockfd, SIOCGIFFLAGS, &ifr))
		return -1;

        return (ifr.ifr_flags & IFF_UP)?1:0;
}

int IrdaApplet::setIrdaStatus(int c)
{
        struct ifreq ifr;

        strcpy(ifr.ifr_name, "irda0");

        if (ioctl(sockfd, SIOCGIFFLAGS, &ifr))
		return -1;

	if (c)
		ifr.ifr_flags |= IFF_UP;
	else 
		ifr.ifr_flags &= ~IFF_UP;

        if (ioctl(sockfd, SIOCSIFFLAGS, &ifr))
		return -1;

	return 0;
}

void IrdaApplet::mousePressEvent( QMouseEvent *)
{
	QPopupMenu *menu = new QPopupMenu();
	QString cmd;
	int ret=0;

	/* Refresh active state */
	timerEvent(NULL);

//	menu->insertItem( tr("More..."), 2 );
	if (irdaactive)
		menu->insertItem( tr("Disable IrDA"), 0 );
	else
		menu->insertItem( tr("Enable IrDA"), 1 );

	QPoint p = mapToGlobal( QPoint(1, -menu->sizeHint().height()-1) );
	ret = menu->exec(p, 1);

	qDebug("ret was %d\n", ret);
	
	switch(ret) {
	case 0:
		setIrdaStatus(0);
		timerEvent(NULL);
		break;
	case 1:
		setIrdaStatus(1);
		timerEvent(NULL);
		break;
	case 2:
		qDebug("FIXME: Bring up pretty menu...\n");
		// With 'discovery' button to enable/disable,
		// and table of currently-detected devices.
	}

}

void IrdaApplet::timerEvent( QTimerEvent * )
{
    int oldactive = irdaactive;

    irdaactive = checkIrdaStatus();
    if (irdaactive != oldactive) 
	    paintEvent(NULL);
    
}

void IrdaApplet::paintEvent( QPaintEvent* )
{
	QPainter p(this);
	qDebug("paint irda pixmap");

	if (irdaactive > 0)
		p.drawPixmap( 0, 1,  irdaOnPixmap );
	else 
		p.drawPixmap( 0, 1,  irdaOffPixmap );
}
