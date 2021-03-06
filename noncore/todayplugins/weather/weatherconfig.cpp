/*
                     This file is part of the OPIE Project
               =.
      .=l.            Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
     .>+-=
_;:,   .>  :=|.         This file is free software; you can
.> <`_,  > .  <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This file is distributed in the hope that
  + . -:.    =       it will be useful, but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=    =    ;      Public License for more details.
++=  -.   .`   .:
:   = ...= . :.=-        You should have received a copy of the GNU
-.  .:....=;==+<;          General Public License along with this file;
 -_. . .  )=. =           see the file COPYING. If not, write to the
  --    :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "weatherconfig.h"

#include <qpe/config.h>
#include <qpe/qpeapplication.h>

#include <qcheckbox.h>
#include <qclipboard.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qwhatsthis.h>

#include <stdlib.h>

WeatherPluginConfig::WeatherPluginConfig( QWidget *parent,  const char* name)
	: TodayConfigWidget(parent,  name )
{
	Config config( "todayweatherplugin");
	config.setGroup( "Config" );

	QFontMetrics fm = fontMetrics();
	int fh = fm.height();

	QGridLayout *layout = new QGridLayout( this );
	layout->setSpacing( 4 );
	layout->setMargin( 4 );

	QLabel *label = new QLabel( tr( "Enter ICAO location identifier:" ), this );
	label->setMaximumHeight( fh + 3 );
	layout->addMultiCellWidget( label, 0, 0, 0, 1 );
    QWhatsThis::add( label, tr( "Enter the 4 letter code for the desired location here.  See http://www.nws.noaa.gov/tg/siteloc.shtml to find a location near you." ) );

	locationEdit = new QLineEdit( config.readEntry( "Location", "" ), this );
	locationEdit->setMaximumHeight( fh + 5 );
	locationEdit->setFocus();
    layout->addMultiCellWidget( locationEdit, 1, 1, 0, 1 );
    QWhatsThis::add( locationEdit, tr( "Enter the 4 letter code for the desired location here.  See http://www.nws.noaa.gov/tg/siteloc.shtml to find a location near you." ) );

	label = new QLabel( tr( "Visit http://www.nws.noaa.gov/tg/siteloc.shtml to find the nearest location." ), this );
	label->setAlignment( AlignHCenter | WordBreak );
	label->setMaximumHeight( label->height() );
	layout->addMultiCellWidget( label, 2, 2, 0, 1 );

	metricCB = new QCheckBox( tr( "Use metric units" ), this );
	metricCB->setMaximumHeight( fh + 5 );
	metricCB->setChecked( config.readBoolEntry( "Metric", TRUE ) );
	layout->addMultiCellWidget( metricCB, 3, 3, 0, 1 );
    QWhatsThis::add( metricCB, tr( "Click here to select type of units displayed." ) );

	label = new QLabel( tr( "Update frequency (in minutes):" ), this );
	label->setMaximumHeight( fh + 3 );
	layout->addWidget( label, 4, 0 );
    QWhatsThis::add( label, tr( "Select how often (in minutes) you want the weather to be updated." ) );

	timerDelaySB = new QSpinBox( 1, 60, 1, this );
	timerDelaySB->setMaximumHeight( fh + 5 );
	timerDelaySB->setValue( config.readNumEntry( "Frequency", 5 ) );
	layout->addWidget( timerDelaySB, 4, 1 );
    QWhatsThis::add( timerDelaySB, tr( "Select how often (in minutes) you want the weather to be updated." ) );
}


void WeatherPluginConfig::writeConfig()
{
	Config config( "todayweatherplugin");
	config.setGroup( "Config" );

	config.writeEntry( "Location", locationEdit->text().upper().stripWhiteSpace() );
	config.writeEntry( "Metric", metricCB->isChecked() );
	config.writeEntry( "Frequency", timerDelaySB->value() );

	config.write();
}

WeatherPluginConfig::~WeatherPluginConfig()
{
}

void WeatherPluginConfig::doLookup()
{
	system( "weather" );
}

/*

Doesn't seem to like QPEApplication::clipboard()...

void WeatherPluginConfig::slotCopyLink()
{
	QPEApplication::clipboard()->setText( "http://www.nws.noaa.gov/tg/siteloc.shtml" );
}
*/
