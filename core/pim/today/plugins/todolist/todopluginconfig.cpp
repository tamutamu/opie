/*
 * todopluginconfig.cpp
 *
 * copyright   : (c) 2002, 2003 by Maximilian Rei�
 * email       : harlekin@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "todopluginconfig.h"

#include <qpe/config.h>

#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qwhatsthis.h>



TodolistPluginConfig::TodolistPluginConfig( QWidget *parent,  const char* name)
    : TodayConfigWidget(parent,  name ) {

    QVBoxLayout * layout = new QVBoxLayout( this );
    layout->setMargin( 20 );

    QHBox *box1 = new QHBox( this );

    QLabel* TextLabel6 = new QLabel( box1, "TextLabel6" );
    TextLabel6->setText( tr( "tasks shown " ) );

    SpinBox2 = new QSpinBox( box1, "SpinBox2" );
    SpinBox2->setMaxValue( 40 );
    QWhatsThis::add( SpinBox2 , tr( "Set the maximum number of task that should be shown" ) );



    QHBox *box2 = new QHBox( this );

    QLabel* clipLabel = new QLabel( box2, "" );
    clipLabel->setText( tr( "Clip line after X chars" ) );

    SpinBoxClip = new QSpinBox( box2, "SpinClip" );
    SpinBoxClip->setMaxValue( 200 );
    QWhatsThis::add( SpinBoxClip , tr( "After how many chars should be the info about the task be cut off" ) );
    layout->addWidget( box1 );
    layout->addWidget( box2 );

    readConfig();
}

void TodolistPluginConfig::readConfig() {
    Config cfg( "todaytodoplugin" );
    cfg.setGroup( "config" );
    m_max_lines_task = cfg.readNumEntry( "maxlinestask", 5 );
    SpinBox2->setValue( m_max_lines_task );
    m_maxCharClip =  cfg.readNumEntry( "maxcharclip", 38 );
    SpinBoxClip->setValue( m_maxCharClip );
}


void TodolistPluginConfig::writeConfig() {
    Config cfg( "todaytodoplugin" );
    cfg.setGroup( "config" );
    m_max_lines_task = SpinBox2->value();
    cfg.writeEntry( "maxlinestask", m_max_lines_task );
    m_maxCharClip = SpinBoxClip->value();
    cfg.writeEntry( "maxcharclip",  m_maxCharClip );
    cfg.write();
}


TodolistPluginConfig::~TodolistPluginConfig() {
}
