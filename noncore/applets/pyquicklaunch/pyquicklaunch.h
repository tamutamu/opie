/**********************************************************************
** Copyright (C) 2004 Michael 'Mickey' Lauer <mickey@Vanille.de>
** All rights reserved.
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

#ifndef PYQUICKLAUNCHAPPLET_H
#define PYQUICKLAUNCHAPPLET_H

#include <qwidget.h>
#include <qframe.h>
#include <qpixmap.h>

class PyQuicklaunchApplet;

class PyQuicklaunchControl : public QFrame
{
    Q_OBJECT
  public:
    PyQuicklaunchControl( PyQuicklaunchApplet* icon, QWidget *parent=0, const char *name=0 );
    void show( bool );

    void readConfig();
    void writeConfigEntry( const char* entry, int val );

  public slots:

  private:
    PyQuicklaunchApplet* applet;
};

class PyQuicklaunchApplet : public QWidget
{
    Q_OBJECT
  public:
    PyQuicklaunchApplet( QWidget *parent = 0, const char *name=0 );
    ~PyQuicklaunchApplet();
    static int position();
    PyQuicklaunchControl* status;

    virtual void timerEvent( QTimerEvent* );

  protected:
    virtual void mousePressEvent( QMouseEvent * );
    virtual void paintEvent( QPaintEvent* );
};

#endif // PYQUICKLAUNCHAPPLET_H

