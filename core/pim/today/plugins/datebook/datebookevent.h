/*
 * datebookplugin.h
 *
 * copyright   : (c) 2002, 2003, 2004 by Maximilian Rei�
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

#ifndef DATEBOOKEVENT_PLUGIN_H
#define DATEBOOKEVENT_PLUGIN_H

#include <opie2/oclickablelabel.h>

#include <qpe/datebookdb.h>


class DateBookEvent: public Opie::Ui::OClickableLabel {
     Q_OBJECT

public:
     DateBookEvent( const EffectiveEvent &ev,
		    QWidget* parent = 0,
		    bool show_location = 0,
		    bool show_notes = 0,
		    bool timeExtraLine = 0,
		    int maxCharClip = 0,
		    const char* name = 0,
		    WFlags fl = 0 );
     ~DateBookEvent();

 signals:
     void editEvent( const EffectiveEvent &e );

private slots:
    void editEventSlot( const EffectiveEvent &e );
    void editMe();

 private:

    QString ampmTime( QTime );
    QString differDate( QDate date );
    const EffectiveEvent event;
    bool ampm;
};

#endif
