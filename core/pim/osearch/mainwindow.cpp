/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// (c) 2002 Patrick S. Vogt <tille@handhelds.org>


#include "mainwindow.h"

#include <qpe/qpemenubar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include <opie/owait.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qstring.h>
#include <qlabel.h>
#include <qfile.h>
#include <qhbuttongroup.h>
#include <qpushbutton.h>
#include <qintdict.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qtextbrowser.h>
#include <qregexp.h>

#include "olistview.h"
#include "olistviewitem.h"
#include "resultitem.h"
#include "adresssearch.h"
#include "todosearch.h"
#include "datebooksearch.h"
#include "applnksearch.h"
#include "doclnksearch.h"

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags f ) :
  QMainWindow( parent, name, f ), _currentItem(0)
{
  setCaption( tr("OSearch") );

  setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));

  QFrame *mainFrame = new QFrame( this, "mainFrame" );

  mainLayout = new QVBoxLayout( mainFrame );
  mainLayout->setSpacing( 0 );
  mainLayout->setMargin( 0 );

  resultsList = new OListView( mainFrame );
  resultsList->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));
  mainLayout->addWidget( resultsList, 1 );

  detailsFrame = new QFrame( mainFrame, "detailsFrame" );
  QVBoxLayout *detailsLayout = new QVBoxLayout( detailsFrame );
  richEdit = new QTextView( detailsFrame );
  richEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));
  //richEdit->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ));
  detailsLayout->addWidget( richEdit, 1 );

  buttonGroupActions = new QHButtonGroup( this );
  buttonGroupActions->hide();
  _buttonCount = 0;
//  buttonGroupActions->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

  buttonLayout = new QHBoxLayout( detailsFrame );
  detailsLayout->addLayout( buttonLayout );

  mainLayout->addWidget( detailsFrame );
  detailsFrame->hide();

  searches.append( new AppLnkSearch( resultsList, tr("applications") ) );
  searches.append( new DocLnkSearch( resultsList, tr("documents") ) );
  searches.append( new TodoSearch( resultsList, tr("todo") ) );
  searches.append( new DatebookSearch( resultsList, tr("datebook") ) );
  searches.append( new AdressSearch( resultsList, tr("adressbook") ) );

  setCentralWidget( mainFrame );

  popupTimer = new QTimer();
  searchTimer = new QTimer();

  connect(popupTimer, SIGNAL(timeout()), SLOT(showPopup()));
  connect(searchTimer, SIGNAL(timeout()), SLOT(searchStringChanged()));
  connect(resultsList, SIGNAL(pressed(QListViewItem*)), SLOT(setCurrent(QListViewItem*)));
  connect(resultsList, SIGNAL(clicked(QListViewItem*)), SLOT(stopTimer(QListViewItem*)));
  connect(buttonGroupActions, SIGNAL(clicked(int)), SLOT( slotAction(int) ) );

  makeMenu();

   Config cfg( "osearch", Config::User );
   cfg.setGroup( "search_settings" );
   actionCaseSensitiv->setOn( cfg.readBoolEntry( "caseSensitiv", false ) );
   actionWildcards->setOn( cfg.readBoolEntry( "wildcards", false ) );
//   actionWholeWordsOnly->setOn( cfg.readBoolEntry( "whole_words_only", false ) );
}

void MainWindow::makeMenu()
{
   QPEToolBar *toolBar = new QPEToolBar( this );
   QPEToolBar *searchBar = new QPEToolBar(this);
   QPEMenuBar *menuBar = new QPEMenuBar( toolBar );
   QPopupMenu *searchMenu = new QPopupMenu( menuBar );
//   QPopupMenu *viewMenu = new QPopupMenu( menuBar );
   QPopupMenu *cfgMenu = new QPopupMenu( menuBar );
   QPopupMenu *searchOptions = new QPopupMenu( cfgMenu );

   setToolBarsMovable( false );
   toolBar->setHorizontalStretchable( true );
   menuBar->insertItem( tr( "Search" ), searchMenu );
   menuBar->insertItem( tr( "Settings" ), cfgMenu );

   //SETTINGS MENU
   cfgMenu->insertItem( tr( "Search" ), searchOptions );
   QPopupMenu *pop;
   for (SearchGroup *s = searches.first(); s != 0; s = searches.next() ){
		pop = s->popupMenu();
		if (pop){
			qDebug("inserting settings menu for %s",s->text(0).latin1());
			cfgMenu->insertItem( s->text(0), pop );
			//connect( pop, SIGNAL( activated(int) ), SLOT( optionChanged(int) ) );
		}
  }


  //SEARCH
  SearchAllAction = new QAction( tr("Search all"),QString::null,  0, this, 0 );
  SearchAllAction->setIconSet( Resource::loadIconSet( "find" ) );
  connect( SearchAllAction, SIGNAL(activated()), this, SLOT(searchAll()) );
  SearchAllAction->addTo( searchMenu );
  searchMenu->insertItem( tr( "Options" ), searchOptions );
  //connect( searchOptions, SIGNAL( activated(int) ), SLOT( optionChanged(int) ) );

  //SEARCH OPTIONS
  //actionWholeWordsOnly = new QAction( tr("Whole words only"),QString::null,  0, this, 0, true );
  //actionWholeWordsOnly->addTo( searchOptions );
  actionCaseSensitiv = new QAction( tr("Case sensitiv"),QString::null,  0, this, 0, true );
  actionCaseSensitiv->addTo( searchOptions );
  actionWildcards = new QAction( tr("Use wildcards"),QString::null,  0, this, 0, true );
  actionWildcards->addTo( searchOptions );

  //SEARCH BAR
  addToolBar( searchBar, "Search", QMainWindow::Top, TRUE );
  QLineEdit *searchEdit = new QLineEdit( searchBar, "seachEdit" );
  searchEdit->setFocus();
  searchBar->setHorizontalStretchable( TRUE );
  searchBar->setStretchableWidget( searchEdit );
  SearchAllAction->addTo( searchBar );
  connect( searchEdit, SIGNAL( textChanged( const QString & ) ),
       this, SLOT( setSearch( const QString & ) ) );

}

MainWindow::~MainWindow()
{
   Config cfg( "osearch", Config::User );
   cfg.setGroup( "search_settings" );
   cfg.writeEntry( "caseSensitiv", actionCaseSensitiv->isOn() );
   cfg.writeEntry( "wildcards", actionWildcards->isOn() );
   //cfg.writeEntry( "whole_words_only", actionWholeWordsOnly->isOn() );
}

void MainWindow::setCurrent(QListViewItem *item)
{
	if (!item) return;
	_currentItem = (OListViewItem*)item;
//	_currentItem = dynamic_cast<OListViewItem*>(item);
	if (_currentItem->rtti() == OListViewItem::Result){
		ResultItem *res = (ResultItem*)item;
//		ResultItem *res = dynamic_cast<ResultItem*>(item);
		richEdit->setText( res->toRichText() );
		QIntDict<QString> acts = res->actions();
		QButton *button;
		for (uint i = 0; i < acts.count(); i++){
			button = buttonGroupActions->find( i );
			qDebug("action %i >%s<",i,acts[i]->latin1());
			if (!button) {
				qDebug("BUTTON");
				button = new QPushButton( detailsFrame );
				buttonLayout->addWidget( button, 0 );
				buttonGroupActions->insert( button, i);
			}
			button->setText( *acts[i] );
			button->show();
		}
		for (uint i = acts.count(); i < _buttonCount; i++){
			qDebug("remove button %i of %i",i, _buttonCount);
			button = buttonGroupActions->find( i );
			if (button) button->hide();
		}
		_buttonCount = acts.count();
		detailsFrame->show();
	}else detailsFrame->hide();
	popupTimer->start( 300, true );
}

void MainWindow::stopTimer(QListViewItem*)
{
        popupTimer->stop();
}

void MainWindow::showPopup()
{
	popupTimer->stop();
        if (!_currentItem) return;
	QPopupMenu *pop = _currentItem->popupMenu();
	if (pop) pop->popup( QCursor::pos() );
}

void MainWindow::setSearch( const QString &key )
{
	searchTimer->stop();
	_searchString = key;
	searchTimer->start( 300 );
}

void MainWindow::searchStringChanged()
{
#ifdef NEW_OWAIT
	OWait("setting search string");
#endif
	searchTimer->stop();
	QString ss = _searchString;
	//ss = Global::stringQuote( _searchString );
//	if (actionWholeWordsOnly->isOn())
//		ss = "\\s"+_searchString+"\\s";
//	qDebug(" set searchString >%s<",ss.latin1());
	QRegExp re( ss );
	re.setCaseSensitive( actionCaseSensitiv->isOn() );
	re.setWildcard( actionWildcards->isOn() );
	for (SearchGroup *s = searches.first(); s != 0; s = searches.next() )
		s->setSearch( re );
}

void MainWindow::searchAll()
{
#ifdef NEW_OWAIT
	OWait("searching...");
#endif
	for (SearchGroup *s = searches.first(); s != 0; s = searches.next() ){
		s->doSearch();
		//resultsList->repaint();
	}
}

void MainWindow::slotAction( int act )
{
	if (_currentItem->rtti() == OListViewItem::Result){
		ResultItem *res = (ResultItem*)_currentItem;
//		ResultItem *res = dynamic_cast<ResultItem*>(item);
		res->action(act);
	}
}

void MainWindow::optionChanged(int i)
{
	qDebug("optionChanged(%i)",i);
	searchStringChanged();
}
