/*
                             This file is part of the Opie Project

                             Copyright (c)  2002 Dan Williams <drw@handhelds.org>
                             Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef EDITSCHEME_H
#define EDITSCHEME_H

#include <opie2/ocolorbutton.h>

#include <qdialog.h>

class QColor;
namespace Opie {namespace Ui {class OColor;}}

class EditScheme : public QDialog {
    Q_OBJECT

public:
    EditScheme ( int cnt, const QString *labels, QColor *colors, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    virtual ~EditScheme ( );

protected slots:
    virtual void accept ( );

private:
    int m_count;
    QColor *m_colors;
    Opie::OColorButton **m_buttons;
};

#endif
