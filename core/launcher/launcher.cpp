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

// WARNING: Do *NOT* define this yourself. The SL5xxx from SHARP does NOT
//      have this class.
#define QTOPIA_INTERNAL_FSLP

#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#include <qpe/mimetype.h>
#include <qpe/storage.h>
#include <qpe/palmtoprecord.h>

#include <qpe/version.h>

#include <qdir.h>
#ifdef QWS
#include <qwindowsystem_qws.h>
#endif
#include <qtimer.h>
#include <qcombobox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qstyle.h>
#include <qpushbutton.h>
#include <qtabbar.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qframe.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qtextstream.h>
#include <qpopupmenu.h>

#include <opie/owait.h>


#include "launcherview.h"
#include "launcher.h"
#include "syncdialog.h"
#include "desktop.h"
#include <qpe/lnkproperties.h>
//#include "mrulist.h"
#include "qrsync.h"
#include <stdlib.h>
#include <unistd.h>

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <stdio.h>
#include <sys/vfs.h>
#include <mntent.h>
#endif

#include <qpe/storage.h>
#include "mediummountgui.h"

namespace {
  QStringList configToMime( Config *cfg ){
    QStringList mimes;
    bool tmpMime = true;
    cfg->setGroup("mimetypes" );
    tmpMime = cfg->readBoolEntry("all" ,true);
    if( tmpMime ){
      mimes << QString::null;
      return mimes;
    }else{
      tmpMime = cfg->readBoolEntry("audio", true );
      if(tmpMime )
	mimes.append("audio/*" );

      tmpMime = cfg->readBoolEntry("image", true );
      if(tmpMime )
	mimes.append("image/*" );

      tmpMime = cfg->readBoolEntry("text", true );
      if(tmpMime )
	mimes.append("text/*");

      tmpMime = cfg->readBoolEntry("video", true );
      if(tmpMime )
	mimes.append("video/*" );
    }
    return mimes;
  }

}



//#define SHOW_ALL

class CategoryTab : public QTab
{
public:
    CategoryTab( const QIconSet &icon, const QString &text=QString::null )
	: QTab( icon, text )
    {
    }

    QColor bgColor;
    QColor fgColor;
};

//===========================================================================

CategoryTabWidget::CategoryTabWidget( QWidget* parent ) :
    QVBox( parent )
{
    categoryBar = 0;
    stack = 0;
}

void CategoryTabWidget::prevTab()
{
    if ( categoryBar ) {
	int n = categoryBar->count();
	int tab = categoryBar->currentTab();
	if ( tab >= 0 )
            categoryBar->setCurrentTab( (tab - 1 + n)%n );
    }
}

void CategoryTabWidget::nextTab()
{
    if ( categoryBar ) {
	int n = categoryBar->count();
        int tab = categoryBar->currentTab();
	categoryBar->setCurrentTab( (tab + 1)%n );
    }
}


void CategoryTabWidget::showTab(const QString& id)
{
    if ( categoryBar ) {
	int idx = ids.findIndex( id );
	categoryBar->setCurrentTab( idx );
    }
}

void CategoryTabWidget::addItem( const QString& linkfile )
{
    int i=0;
    AppLnk *app = new AppLnk(linkfile);
    if ( !app->isValid() ) {
	delete app;
	app=0;
    }
    if ( !app || !app->file().isEmpty() ) {
	// A document
	delete app;
	app = new DocLnk(linkfile);
	if ( app->fileKnown() ) {
	    ((LauncherView*)(stack->widget(ids.count()-1)))->addItem(app);
	} else {
	    ((LauncherView*)(stack->widget(ids.count()-1)))->sort();
	    delete app;
	}
	return;
    }
    // An application
    for ( QStringList::Iterator it=ids.begin(); it!=ids.end(); ++it) {
	if ( !(*it).isEmpty() ) {
	    QRegExp tf(*it,FALSE,TRUE);
	    if ( tf.match(app->type()) >= 0 ) {
		((LauncherView*)stack->widget(i))->addItem(app);
		return;
	    }
	    i++;
	}
    }

    QCopEnvelope e("QPE/TaskBar","reloadApps()");
}

void CategoryTabWidget::initializeCategories(AppLnkSet* rootFolder,
	AppLnkSet* docFolder, const QList<FileSystem> &fs)
{
    QString current;
    if ( categoryBar ) {
        int c = categoryBar->currentTab();
        if ( c >= 0 ) current = ids[c];
    }

    delete categoryBar;
    categoryBar = new CategoryTabBar( this );
    QPalette pal = categoryBar->palette();
    pal.setColor( QColorGroup::Light, pal.color(QPalette::Active,QColorGroup::Shadow) );
    pal.setColor( QColorGroup::Background, pal.active().background().light(110) );
    categoryBar->setPalette( pal );

    delete stack;
    stack = new QWidgetStack(this);
    tabs=0;

    ids.clear();

    Config cfg("Launcher");

    QStringList types = rootFolder->types();
    for ( QStringList::Iterator it=types.begin(); it!=types.end(); ++it) {
	if ( !(*it).isEmpty() ) {
	    (void)newView(*it,rootFolder->typePixmap(*it),rootFolder->typeName(*it));
	    setTabAppearance( *it, cfg );
	}
    }
    QListIterator<AppLnk> it( rootFolder->children() );
    AppLnk* l;
    while ( (l=it.current()) ) {
	if ( l->type() == "Separator" ) { // No tr
	    rootFolder->remove(l);
	    delete l;
	} else {
	    int i=0;
	    for ( QStringList::Iterator it=types.begin(); it!=types.end(); ++it) {
		if ( *it == l->type() )
		    ((LauncherView*)stack->widget(i))->addItem(l,FALSE);
		i++;
	    }
	}
	++it;
    }
    rootFolder->detachChildren();
    for (int i=0; i<tabs; i++)
	((LauncherView*)stack->widget(i))->sort();

    // all documents
    QImage img( Resource::loadImage( "DocsIcon" ) );
    QPixmap pm;
    pm = img.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
    docview = newView( "Documents", // No tr
	pm, tr("Documents"));
    docview->populate( docFolder, QString::null );
    docFolder->detachChildren();
    docview->setFileSystems(fs);
    docview->setToolsEnabled(TRUE);
    setTabAppearance( "Documents", cfg ); // No tr

    connect( categoryBar, SIGNAL(selected(int)), stack, SLOT(raiseWidget(int)) );

    ((LauncherView*)stack->widget(0))->setFocus();

    cfg. setGroup ( "GUI" );
    setBusyIndicatorType ( cfg. readEntry ( "BusyType", QString::null ));

    if ( !current.isNull() ) {
        showTab(current);
    }

    categoryBar->show();
    stack->show();

    QCopEnvelope e("QPE/TaskBar","reloadApps()");
}

void CategoryTabWidget::setTabAppearance( const QString &id, Config &cfg )
{
    QString grp( "Tab %1" ); // No tr
    cfg.setGroup( grp.arg(id) );
    LauncherView *v = view( id );
    int idx = ids.findIndex( id );
    CategoryTab *tab = (CategoryTab *)categoryBar->tab( idx );

    // View
    QString view = cfg.readEntry( "View", "Icon" );
    if ( view == "List" ) // No tr
	v->setViewMode( LauncherView::List );
    QString bgType = cfg.readEntry( "BackgroundType", "Image" );
    if ( bgType == "Image" ) { // No tr
	QString pm = cfg.readEntry( "BackgroundImage", "launcher/opie-background" );
	v->setBackgroundType( LauncherView::Image, pm );
    } else if ( bgType == "SolidColor" ) {
	QString c = cfg.readEntry( "BackgroundColor" );
	v->setBackgroundType( LauncherView::SolidColor, c );
    }
    QString textCol = cfg.readEntry( "TextColor" );
    if ( textCol.isEmpty() )
	v->setTextColor( QColor() );
    else
	v->setTextColor( QColor(textCol) );
    QStringList font = cfg.readListEntry( "Font", ',' );
    if ( font.count() == 4 )
	v->setViewFont( QFont(font[0], font[1].toInt(), font[2].toInt(), font[3].toInt()!=0) );

    // Tabs
    QString tabCol = cfg.readEntry( "TabColor" );
    if ( tabCol.isEmpty() )
	tab->bgColor = QColor();
    else
	tab->bgColor = QColor(tabCol);
    QString tabTextCol = cfg.readEntry( "TabTextColor" );
    if ( tabTextCol.isEmpty() )
	tab->fgColor = QColor();
    else
	tab->fgColor = QColor(tabTextCol);
}

void CategoryTabWidget::updateDocs(AppLnkSet* docFolder, const QList<FileSystem> &fs)
{
    docview->populate( docFolder, QString::null );
    docFolder->detachChildren();
    docview->setFileSystems(fs);
    docview->updateTools();
}

void CategoryTabWidget::tabProperties()
{
    LauncherView *view = (LauncherView*)stack->widget( categoryBar->currentTab() );
    QPopupMenu *m = new QPopupMenu( this );
    m->insertItem( tr("Icon View"), LauncherView::Icon );
    m->insertItem( tr("List View"), LauncherView::List );
    m->setItemChecked( (int)view->viewMode(), TRUE );
    int rv = m->exec( QCursor::pos() );
    if ( rv >= 0 && rv != view->viewMode() ) {
	view->setViewMode( (LauncherView::ViewMode)rv );
    }

    delete m;
}

QString CategoryTabWidget::getAllDocLinkInfo() const
{
    return docview->getAllDocLinkInfo();
}

LauncherView* CategoryTabWidget::newView( const QString& id, const QPixmap& pm, const QString& label )
{
    LauncherView* view = new LauncherView( stack );
    connect( view, SIGNAL(clicked(const AppLnk*)),
	    this, SIGNAL(clicked(const AppLnk*)));
    connect( view, SIGNAL(rightPressed(AppLnk*)),
	    this, SIGNAL(rightPressed(AppLnk*)));
    ids.append(id);
    categoryBar->addTab( new CategoryTab( pm, label ) );
    stack->addWidget( view, tabs++ );
    return view;
}

void CategoryTabWidget::updateLink(const QString& linkfile)
{
    int i=0;
    LauncherView* view;
    qApp->processEvents();
    while ((view = (LauncherView*)stack->widget(i++))) {
	if ( view->removeLink(linkfile) )
	    break;
    }
    addItem(linkfile);
    docview->updateTools();
}

void CategoryTabWidget::paletteChange( const QPalette &p )
{
    QVBox::paletteChange( p );
    QPalette pal = palette();
    pal.setColor( QColorGroup::Light, pal.color(QPalette::Active,QColorGroup::Shadow) );
    pal.setColor( QColorGroup::Background, pal.active().background().light(110) );
    categoryBar->setPalette( pal );
    categoryBar->update();
}

void CategoryTabWidget::setBusy(bool on)
{
    if ( on )
	((LauncherView*)stack->visibleWidget())->setBusy(TRUE);
    else
	for (int i=0; i<tabs; i++)
	    ((LauncherView*)stack->widget(i))->setBusy(FALSE);
}

LauncherView *CategoryTabWidget::view( const QString &id )
{
    int idx = ids.findIndex( id );
    return (LauncherView *)stack->widget(idx);
}

void CategoryTabWidget::setBusyIndicatorType ( const QString &type )
{
	for ( QStringList::Iterator it = ids. begin ( ); it != ids. end ( ); ++it )
		view ( *it )-> setBusyIndicatorType ( type );
}

//===========================================================================

CategoryTabBar::CategoryTabBar( QWidget *parent, const char *name )
    : QTabBar( parent, name )
{
    setFocusPolicy( NoFocus );
    connect( this, SIGNAL( selected(int) ), this, SLOT( layoutTabs() ) );
}

CategoryTabBar::~CategoryTabBar()
{
}

void CategoryTabBar::layoutTabs()
{
    if ( !count() )
	return;

//    int percentFalloffTable[] = { 100, 70, 40, 12, 6, 3, 1, 0 };
    int available = width()-1;
    QFontMetrics fm = fontMetrics();
    int hiddenTabWidth = -7;
    int middleTab = currentTab();
    int hframe, vframe, overlap;
    style().tabbarMetrics( this, hframe, vframe, overlap );
    int x = 0;
    QRect r;
    QTab *t;
    int required = 0;
    int eventabwidth = (width()-1)/count();
    enum Mode { HideBackText, Pack, Even } mode=Even;
    for ( int i = 0; i < count(); i++ ) {
	t = tab(i);
	int iw = fm.width( t->text() ) + hframe - overlap;
	if ( i != middleTab ) {
	    available -= hiddenTabWidth + hframe - overlap;
	    if ( t->iconSet() != 0 )
		available -= t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width();
	}
	if ( t->iconSet() != 0 )
	    iw += t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width();
	required += iw;
	// As space gets tight, packed looks better than even. "10" must be at least 0.
	if ( iw >= eventabwidth-10 )
	    mode = Pack;
    }
    if ( mode == Pack && required > width()-1 )
	mode = HideBackText;
    for ( int i = 0; i < count(); i++ ) {
	t = tab(i);
	if ( mode != HideBackText ) {
	    int w = fm.width( t->text() );
	    int ih = 0;
	    if ( t->iconSet() != 0 ) {
		w += t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width();
		ih = t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height();
	    }
	    int h = QMAX( fm.height(), ih );
	    h = QMAX( h, QApplication::globalStrut().height() );

	    h += vframe;
	    w += hframe;

	    QRect tr(x, 0,
		mode == Even ? eventabwidth : w * (width()-1)/required, h);
	    t->setRect(tr);
	    x += tr.width() - overlap;
	    r = r.unite(tr);
	} else if ( i != middleTab ) {
	    int w = hiddenTabWidth;
	    int ih = 0;
	    if ( t->iconSet() != 0 ) {
		w += t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width();
		ih = t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height();
	    }
	    int h = QMAX( fm.height(), ih );
	    h = QMAX( h, QApplication::globalStrut().height() );

	    h += vframe;
	    w += hframe;

	    t->setRect( QRect(x, 0, w, h) );
	    x += t->rect().width() - overlap;
	    r = r.unite( t->rect() );
	} else {
	    int ih = 0;
	    if ( t->iconSet() != 0 ) {
		ih = t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height();
	    }
	    int h = QMAX( fm.height(), ih );
	    h = QMAX( h, QApplication::globalStrut().height() );

	    h += vframe;

	    t->setRect( QRect(x, 0, available, h) );
	    x += t->rect().width() - overlap;
	    r = r.unite( t->rect() );
	}
    }

    QRect rr = tab(count()-1)->rect();
    rr.setRight(width()-1);
    tab(count()-1)->setRect( rr );

    for ( t = tabList()->first(); t; t = tabList()->next() ) {
       QRect tr = t->rect();
       tr.setHeight( r.height() );
       t->setRect( tr );
    }

    update();
}


void CategoryTabBar::paint( QPainter * p, QTab * t, bool selected ) const
{
    CategoryTabBar *that = (CategoryTabBar *) this;
    CategoryTab *ct = (CategoryTab *)t;
    QPalette pal = palette();
    bool setPal = FALSE;
    if ( ct->bgColor.isValid() ) {
	pal.setColor( QPalette::Active, QColorGroup::Background, ct->bgColor );
	pal.setColor( QPalette::Active, QColorGroup::Button, ct->bgColor );
	pal.setColor( QPalette::Inactive, QColorGroup::Background, ct->bgColor );
	pal.setColor( QPalette::Inactive, QColorGroup::Button, ct->bgColor );
	that->setUpdatesEnabled( FALSE );
	that->setPalette( pal );
	setPal = TRUE;
    }
#if QT_VERSION >= 300
    QStyle::SFlags flags = QStyle::Style_Default;
    if ( selected )
        flags |= QStyle::Style_Selected;
    style().drawControl( QStyle::CE_TabBarTab, p, this, t->rect(),
                         colorGroup(), flags, QStyleOption(t) );
#else
    style().drawTab( p, this, t, selected );
#endif

    QRect r( t->rect() );
    QFont f( font() );
    if ( selected )
	f.setBold( TRUE );
    p->setFont( f );

    if ( ct->fgColor.isValid() ) {
	pal.setColor( QPalette::Active, QColorGroup::Foreground, ct->fgColor );
	pal.setColor( QPalette::Inactive, QColorGroup::Foreground, ct->fgColor );
	that->setUpdatesEnabled( FALSE );
	that->setPalette( pal );
	setPal = TRUE;
    }
    int iw = 0;
    int ih = 0;
    if ( t->iconSet() != 0 ) {
	iw = t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).width() + 2;
	ih = t->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height();
    }
    int w = iw + p->fontMetrics().width( t->text() ) + 4;
    int h = QMAX(p->fontMetrics().height() + 4, ih );
    paintLabel( p, QRect( r.left() + (r.width()-w)/2 - 3,
			  r.top() + (r.height()-h)/2, w, h ), t,
#if QT_VERSION >= 300
			    t->identifier() == keyboardFocusTab()
#else
			    t->identitifer() == keyboardFocusTab()
#endif
		);
    if ( setPal ) {
	that->unsetPalette();
	that->setUpdatesEnabled( TRUE );
    }
}


void CategoryTabBar::paintLabel( QPainter* p, const QRect&,
			  QTab* t, bool has_focus ) const
{
    QRect r = t->rect();
    //    if ( t->id != currentTab() )
    //r.moveBy( 1, 1 );
    //
    if ( t->iconSet() ) {
	// the tab has an iconset, draw it in the right mode
	QIconSet::Mode mode = (t->isEnabled() && isEnabled()) ? QIconSet::Normal : QIconSet::Disabled;
	if ( mode == QIconSet::Normal && has_focus )
	    mode = QIconSet::Active;
	QPixmap pixmap = t->iconSet()->pixmap( QIconSet::Small, mode );
	int pixw = pixmap.width();
	int pixh = pixmap.height();
	p->drawPixmap( r.left() + 6, r.center().y() - pixh / 2 + 1, pixmap );
	r.setLeft( r.left() + pixw + 5 );
    }

    QRect tr = r;

    if ( r.width() < 20 )
	return;

    if ( t->isEnabled() && isEnabled()  ) {
#if defined(_WS_WIN32_)
	if ( colorGroup().brush( QColorGroup::Button ) == colorGroup().brush( QColorGroup::Background ) )
	    p->setPen( colorGroup().buttonText() );
	else
	    p->setPen( colorGroup().foreground() );
#else
	p->setPen( colorGroup().foreground() );
#endif
	p->drawText( tr, AlignCenter | AlignVCenter | ShowPrefix, t->text() );
    } else {
	p->setPen( palette().disabled().foreground() );
	p->drawText( tr, AlignCenter | AlignVCenter | ShowPrefix, t->text() );
    }
}

//---------------------------------------------------------------------------

Launcher::Launcher( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    setCaption( tr("Launcher") );

    syncDialog = 0;

    // we have a pretty good idea how big we'll be
    setGeometry( 0, 0, qApp->desktop()->width(), qApp->desktop()->height() );

    tabs = 0;
    rootFolder = 0;
    docsFolder = 0;

    int stamp = uidgen.generate(); // this is our timestamp to see which devices we know
    //uidgen.store( stamp );
    m_timeStamp = QString::number( stamp  );

    tabs = new CategoryTabWidget( this );
    setCentralWidget( tabs );

    connect( tabs, SIGNAL(selected(const QString&)),
	this, SLOT(viewSelected(const QString&)) );
    connect( tabs, SIGNAL(clicked(const AppLnk*)),
	this, SLOT(select(const AppLnk*)));
    connect( tabs, SIGNAL(rightPressed(AppLnk*)),
	this, SLOT(properties(AppLnk*)));

#if !defined(QT_NO_COP)
    QCopChannel* sysChannel = new QCopChannel( "QPE/System", this );
    connect( sysChannel, SIGNAL(received(const QCString &, const QByteArray &)),
             this, SLOT(systemMessage( const QCString &, const QByteArray &)) );
    QCopChannel *channel = new QCopChannel( "QPE/Launcher", this );
    connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
             this, SLOT(launcherMessage(const QCString&, const QByteArray&)) );
#endif

    storage = new StorageInfo( this );
    connect( storage, SIGNAL( disksChanged() ), SLOT( storageChanged() ) );

    updateTabs();

    preloadApps();

    in_lnk_props = FALSE;
    got_lnk_change = FALSE;
}

Launcher::~Launcher()
{
    delete rootFolder;
    delete docsFolder;
}

static bool isVisibleWindow(int wid)
{
#ifdef QWS
    const QList<QWSWindow> &list = qwsServer->clientWindows();
    QWSWindow* w;
    for (QListIterator<QWSWindow> it(list); (w=it.current()); ++it) {
	if ( w->winId() == wid )
	    return !w->isFullyObscured();
    }
#endif
    return FALSE;
}

void Launcher::showMaximized()
{
    if ( isVisibleWindow( winId() ) )
	doMaximize();
    else
	QTimer::singleShot( 20, this, SLOT(doMaximize()) );
}

void Launcher::doMaximize()
{
    QMainWindow::showMaximized();
    tabs->setMaximumWidth( qApp->desktop()->width() );
}

void Launcher::updateMimeTypes()
{
    MimeType::clear();
    updateMimeTypes(rootFolder);
}

void Launcher::updateMimeTypes(AppLnkSet* folder)
{
    for ( QListIterator<AppLnk> it( folder->children() ); it.current(); ++it ) {
	AppLnk *app = it.current();
	if ( app->type() == "Folder" ) // No tr
	    updateMimeTypes((AppLnkSet *)app);
	else {
	    MimeType::registerApp(*app);
	}
    }
}

/** This is a HACK....
 * Reason: scanning huge mediums, microdirvers for examples
 * consomes time. To avoid that we invented the MediumMountCheck
 *
 * a) the user globally disabled medium checking. We can ignore
 *    all removable medium
 * b) the user enabled medium checking globally and we need to use this mimefilter
 * c) the user enabled medium checking on a per medium bases
 *  c1) we already checked and its not ask again turns
 *  c2) we need to ask and then apply the mimefilter
 */
void Launcher::loadDocs() // ok here comes a hack belonging to Global::
{

    OWait *owait = new OWait();
    Global::statusMessage( tr( "Finding documents" ) );

    owait->show();
    qApp->processEvents();

    delete docsFolder;
    docsFolder = new DocLnkSet;

    DocLnkSet *tmp = 0;
    QString home = QString(getenv("HOME"))  + "/Documents";
    tmp = new DocLnkSet( home , QString::null);
    docsFolder->appendFrom( *tmp );
    delete tmp;

    // RAM documents
    StorageInfo storage;
    const QList<FileSystem> &fileSystems = storage.fileSystems();
    QListIterator<FileSystem> it ( fileSystems );

    for ( ; it.current(); ++it ) {
	if ( (*it)->disk() == "/dev/mtdblock6" || (*it)->disk() == "tmpfs" ) {
	    tmp = new DocLnkSet( (*it)->path(), QString::null );
	    docsFolder->appendFrom( *tmp );
	    delete tmp;
	}
    }

    Config mediumCfg( "medium");
    mediumCfg.setGroup("main");
    // a) -zecke we don't want to check
    if(!mediumCfg.readBoolEntry("use", true ) )
      return;

    // find out wich filesystems are new in this round
    // We will do this by having a timestamp inside each mountpoint
    // if the current timestamp doesn't match this is a new file system and
    // come up with our MediumMountGui :) let the hacking begin
    int stamp = uidgen.generate();

    QString newStamp = QString::number( stamp ); // generates newtime Stamp

    // b)
    if( mediumCfg.readBoolEntry("global", true ) ){
      QString mime = configToMime(&mediumCfg).join(";");
      for( it.toFirst(); it.current(); ++it ){
	if( (*it)->isRemovable() ){
	  tmp = new DocLnkSet( (*it)->path(), mime );
	  docsFolder->appendFrom( *tmp );
	  delete tmp;
	}
      } // done
      return; // save the else
    }
    // c) zecke
    for ( it.toFirst(); it.current(); ++it ) {
      if ( (*it)->isRemovable() ) { // let's find out  if we should search on it
	Config cfg( (*it)->path() + "/.opiestorage.cf", Config::File);
	cfg.setGroup("main");
	QString stamp = cfg.readEntry("timestamp", QDateTime::currentDateTime().toString() );
	/** This medium is uptodate
	*/
	if( stamp == m_timeStamp ){ // ok we know this card
	  cfg.writeEntry("timestamp", newStamp ); //just write a new timestamp
	  // we need to scan the list now. Hopefully the cache will be there
	  // read the mimetypes from the config and search for documents
 	  QStringList mimetypes = configToMime( &cfg);
          //qApp->processEvents();
                  Global::statusMessage( tr( "Searching documents" ) );
  	  tmp = new DocLnkSet( (*it)->path(), mimetypes.join(";")  );
	  docsFolder->appendFrom( *tmp );
	  delete tmp;

	}else{ // come up with the gui cause this a new card
	  MediumMountGui medium(&cfg, (*it)->path() );
	  if( medium.check() ){ // we did not ask before or ask again is off
	    /** c2) */
	    if( medium.exec()  ){ // he clicked yes so search it
	      // speicher
	      //cfg.read(); // cause of a race we need to reread - fixed
	      cfg.setGroup("main");
	      cfg.writeEntry("timestamp", newStamp );
	      cfg.write();

              //qApp->processEvents();
              tmp = new DocLnkSet( (*it)->path(), medium.mimeTypes().join(";" ) );
      	      docsFolder->appendFrom( *tmp );
	      delete tmp;
	    }// no else
	    /** c1) */
	  }else{ // we checked
	    // do something different see what we need to do
	    // let's see if we should check the device
	    cfg.setGroup("main" );
	    bool check = cfg.readBoolEntry("autocheck", true );
	    if( check ){ // find the documents

                //qApp->processEvents();
                      Global::statusMessage( tr( "Searching documents" ) );
                      tmp = new DocLnkSet( (*it)->path(), configToMime(&cfg ).join(";") );
	      docsFolder->appendFrom( *tmp );
	      delete tmp;
	    }
	  }
	}
       }
    }
    m_timeStamp = newStamp;
    owait->hide();
    delete owait;
}

void Launcher::updateTabs()
{
    MimeType::updateApplications(); // ### reads all applnks twice

    delete rootFolder;
    rootFolder = new AppLnkSet( MimeType::appsFolderName() );

    loadDocs();

    tabs->initializeCategories(rootFolder, docsFolder, storage->fileSystems());
}

void Launcher::updateDocs()
{
    loadDocs();
    tabs->updateDocs(docsFolder,storage->fileSystems());
}

void Launcher::viewSelected(const QString& s)
{
    setCaption( s + tr(" - Launcher") );
}

void Launcher::nextView()
{
    tabs->nextTab();
}

void Launcher::showTab(const QString& id)
{
    tabs->showTab(id);
    raise();
}

void Launcher::select( const AppLnk *appLnk )
{
    if ( appLnk->type() == "Folder" ) { // No tr
	// Not supported: flat is simpler for the user
    } else {
	if ( appLnk->exec().isNull() ) {
	    QMessageBox::information(this,tr("No application"),
		tr("<p>No application is defined for this document."
		"<p>Type is %1.").arg(appLnk->type()));
	    return;
	}
	tabs->setBusy(TRUE);
	emit executing( appLnk );
	appLnk->execute();
    }
}

void Launcher::externalSelected(const AppLnk *appLnk)
{
    tabs->setBusy(TRUE);
    emit executing( appLnk );
}

void Launcher::properties( AppLnk *appLnk )
{
    if ( appLnk->type() == "Folder" ) { // No tr
	// Not supported: flat is simpler for the user
    } else {
	in_lnk_props = TRUE;
	got_lnk_change = FALSE;
	LnkProperties prop(appLnk);
	connect(&prop, SIGNAL(select(const AppLnk *)), this, SLOT(externalSelected(const AppLnk *)));
	prop.showMaximized();
	prop.exec();
	in_lnk_props = FALSE;
	if ( got_lnk_change ) {
	    updateLink(lnk_change);
	}
    }
}

void Launcher::updateLink(const QString& link)
{
    bool notify_sm = false;

    if (link.isNull()) {
	updateTabs();
	notify_sm = true;
    }
    else if (link.isEmpty()) {
	updateDocs();
    }
    else {
	tabs->updateLink(link);
	notify_sm = true;
    }

    if ( notify_sm )
    	QCopEnvelope e ( "QPE/TaskBar", "reloadApps()" );
}

void Launcher::systemMessage( const QCString &msg, const QByteArray &data)
{
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "linkChanged(QString)" ) {
	QString link;
	stream >> link;
	if ( in_lnk_props ) {
	    got_lnk_change = TRUE;
	    lnk_change = link;
	} else {
	    updateLink(link);
	}
    } else if ( msg == "busy()" ) {
	emit busy();
    } else if ( msg == "notBusy(QString)" ) {
	QString app;
	stream >> app;
	tabs->setBusy(FALSE);
	emit notBusy(app);
    } else if ( msg == "mkdir(QString)" ) {
	QString dir;
	stream >> dir;
	if ( !dir.isEmpty() )
	    mkdir( dir );
    } else if ( msg == "rdiffGenSig(QString,QString)" ) {
	QString baseFile, sigFile;
	stream >> baseFile >> sigFile;
	QRsync::generateSignature( baseFile, sigFile );
    } else if ( msg == "rdiffGenDiff(QString,QString,QString)" ) {
	QString baseFile, sigFile, deltaFile;
	stream >> baseFile >> sigFile >> deltaFile;
	QRsync::generateDiff( baseFile, sigFile, deltaFile );
    } else if ( msg == "rdiffApplyPatch(QString,QString)" ) {
	QString baseFile, deltaFile;
	stream >> baseFile >> deltaFile;
	if ( !QFile::exists( baseFile ) ) {
	    QFile f( baseFile );
	    f.open( IO_WriteOnly );
	    f.close();
	}
	QRsync::applyDiff( baseFile, deltaFile );
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "patchApplied(QString)" );
	e << baseFile;
#endif
    } else if ( msg == "rdiffCleanup()" ) {
	mkdir( "/tmp/rdiff" );
	QDir dir;
	dir.setPath( "/tmp/rdiff" );
	QStringList entries = dir.entryList();
	for ( QStringList::Iterator it = entries.begin(); it != entries.end(); ++it )
	    dir.remove( *it );
    } else if ( msg == "sendHandshakeInfo()" ) {
	QString home = getenv( "HOME" );
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "handshakeInfo(QString,bool)" );
	e << home;
	int locked = (int) Desktop::screenLocked();
	e << locked;
#endif
    } else if ( msg == "autoStart(QString)" ) {
        QString appName;
        stream >> appName;
        Config cfg( "autostart" );
        cfg.setGroup( "AutoStart" );
        if ( appName.compare("clear") == 0){
            cfg.writeEntry("Apps", "");
        }
    } else if ( msg == "autoStart(QString,QString)" ) {
        QString modifier, appName;
        stream >> modifier >> appName;
        Config cfg( "autostart" );
        cfg.setGroup( "AutoStart" );
        if ( modifier.compare("add") == 0 ){
            // only add if appname is entered
            if (!appName.isEmpty()) {
                cfg.writeEntry("Apps", appName);
            }
        } else if (modifier.compare("remove") == 0 ) {
            // need to change for multiple entries
            // actually remove is right now simular to clear, but in future there
            // should be multiple apps in autostart possible.
            QString checkName;
            checkName = cfg.readEntry("Apps", "");
            if (checkName == appName) {
                cfg.writeEntry("Apps", "");
            }
        }
        // case the autostart feature should be delayed
    } else if ( msg == "autoStart(QString,QString,QString)") {
         QString modifier, appName, delay;
         stream >> modifier >> appName >> delay;
        Config cfg( "autostart" );
        cfg.setGroup( "AutoStart" );
        if ( modifier.compare("add") == 0 ){
            // only add it appname is entered
            if (!appName.isEmpty()) {
                cfg.writeEntry("Apps", appName);
                cfg.writeEntry("Delay", delay);
            }
        } else {
        }
    }
    /*
     * QtopiaDesktop relies on the major number
     * to start with 1. We're at 0.9
     * so wee need to fake at least 1.4 to be able
     * to sync with QtopiaDesktop1.6
     */
    else if ( msg == "sendVersionInfo()" ) {
	QCopEnvelope e( "QPE/Desktop", "versionInfo(QString)" );
	QString v2 = QString::fromLatin1("1.4");
	e << v2;
	//qDebug("version %s\n", line.latin1());
    } else if ( msg == "sendCardInfo()" ) {
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "cardInfo(QString)" );
#endif
	const QList<FileSystem> &fs = storage->fileSystems();
	QListIterator<FileSystem> it ( fs );
	QString s;
	QString homeDir = getenv("HOME");
	QString hardDiskHome, hardDiskHomePath;
	for ( ; it.current(); ++it ) {
	    int k4 = (*it)->blockSize()/256;
	    if ( (*it)->isRemovable() || (*it)->disk() == "/dev/mtdblock6" || (*it)->disk() == "tmpfs") {
		s += (*it)->name() + "=" + (*it)->path() + "/Documents "
		     + QString::number( (*it)->availBlocks() * k4/4 )
		     + "K " + (*it)->options() + ";";
	    } else if ( (*it)->disk() == "/dev/mtdblock1" ||
		      (*it)->disk() == "/dev/mtdblock/1" ) {
		s += (*it)->name() + "=" + homeDir + "/Documents "
		     + QString::number( (*it)->availBlocks() * k4/4 )
		     + "K " + (*it)->options() + ";";
	    } else if ( (*it)->name().contains( "Hard Disk") &&
		      homeDir.contains( (*it)->path() ) &&
		      (*it)->path().length() > hardDiskHomePath.length() ) {
		hardDiskHomePath = (*it)->path();
		hardDiskHome =
		    (*it)->name() + "=" + homeDir + "/Documents "
		    + QString::number( (*it)->availBlocks() * k4/4 )
		    + "K " + (*it)->options() + ";";
	    }
	}
	if ( !hardDiskHome.isEmpty() )
	    s += hardDiskHome;

#ifndef QT_NO_COP
	e << s;
#endif
    } else if ( msg == "sendSyncDate(QString)" ) {
	QString app;
	stream >> app;
	Config cfg( "qpe" );
	cfg.setGroup("SyncDate");
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "syncDate(QString,QString)" );
	e  << app  << cfg.readEntry( app );
#endif
	//qDebug("QPE/System sendSyncDate for %s: response %s", app.latin1(),
	//cfg.readEntry( app ).latin1() );
    } else if ( msg == "setSyncDate(QString,QString)" ) {
	QString app, date;
	stream >> app >> date;
	Config cfg( "qpe" );
	cfg.setGroup("SyncDate");
	cfg.writeEntry( app, date );
	//qDebug("setSyncDate(QString,QString) %s %s", app.latin1(), date.latin1());
    } else if ( msg == "startSync(QString)" ) {
	QString what;
	stream >> what;
	delete syncDialog; syncDialog = 0;
	syncDialog = new SyncDialog( this, "syncProgress", FALSE,
				     WStyle_Tool | WStyle_Customize |
				     Qt::WStyle_StaysOnTop );
	syncDialog->showMaximized();
	syncDialog->whatLabel->setText( "<b>" + what + "</b>" );
	connect( syncDialog->buttonCancel, SIGNAL( clicked() ),
		 SLOT( cancelSync() ) );
    } else if ( msg == "stopSync()") {
	delete syncDialog; syncDialog = 0;
    } else if ( msg == "getAllDocLinks()" ) {
	loadDocs();

                // directly show updated docs in document tab
                updateDocs();

                QString contents;

//	Categories cats;
	for ( QListIterator<DocLnk> it( docsFolder->children() ); it.current(); ++it ) {
	    DocLnk *doc = it.current();
	    QFileInfo fi( doc->file() );
	    if ( !fi.exists() )
		continue;

	    bool fake = !doc->linkFileKnown();
	    if ( !fake ) {
		QFile f( doc->linkFile() );
		if ( f.open( IO_ReadOnly ) ) {
		    QTextStream ts( &f );
		    ts.setEncoding( QTextStream::UnicodeUTF8 );
		    contents += ts.read();
		    f.close();
		} else
		    fake = TRUE;
	    }
	    if (fake) {
		contents += "[Desktop Entry]\n";
		contents += "Categories = " + // No tr
//		    cats.labels("Document View",doc->categories()).join(";") + "\n"; // No tr
			Qtopia::Record::idsToString( doc->categories() ) + "\n";
		contents += "Name = "+doc->name()+"\n"; // No tr
		contents += "Type = "+doc->type()+"\n"; // No tr
	    }
	    contents += "File = "+doc->file()+"\n"; // No tr // (resolves path)
	    contents += QString("Size = %1\n").arg( fi.size() ); // No tr
	}

	//qDebug( "sending length %d", contents.length() );
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "docLinks(QString)" );
	e << contents;
#endif

 	//qDebug( "================ \n\n%s\n\n===============",
	//contents.latin1() );

	delete docsFolder;
	docsFolder = 0;
#ifdef QWS
    } else if ( msg == "setMouseProto(QString)" ) {
	QString mice;
	stream >> mice;
	setenv("QWS_MOUSE_PROTO",mice.latin1(),1);
	qwsServer->openMouse();
    } else if ( msg == "setKeyboard(QString)" ) {
	QString kb;
	stream >> kb;
	setenv("QWS_KEYBOARD",kb.latin1(),1);
	qwsServer->openKeyboard();
#endif
    }
}

void Launcher::cancelSync()
{
#ifndef QT_NO_COP
    QCopEnvelope e( "QPE/Desktop", "cancelSync()" );
#endif
}

void Launcher::launcherMessage( const QCString &msg, const QByteArray &data)
{
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "setTabView(QString,int)" ) {
	QString id;
	stream >> id;
	int mode;
	stream >> mode;
	if ( tabs->view(id) )
	    tabs->view(id)->setViewMode( (LauncherView::ViewMode)mode );
    } else if ( msg == "setTabBackground(QString,int,QString)" ) {
	QString id;
	stream >> id;
	int mode;
	stream >> mode;
	QString pixmapOrColor;
	stream >> pixmapOrColor;
	if ( tabs->view(id) )
	    tabs->view(id)->setBackgroundType( (LauncherView::BackgroundType)mode, pixmapOrColor );
    } else if ( msg == "setTextColor(QString,QString)" ) {
	QString id;
	stream >> id;
	QString color;
	stream >> color;
	if ( tabs->view(id) )
	    tabs->view(id)->setTextColor( QColor(color) );
    } else if ( msg == "setFont(QString,QString,int,int,int)" ) {
	QString id;
	stream >> id;
	QString fam;
	stream >> fam;
	int size;
	stream >> size;
	int weight;
	stream >> weight;
	int italic;
	stream >> italic;
	if ( tabs->view(id) )
	    if ( !fam. isEmpty ( ))
		tabs->view(id)->setViewFont( QFont(fam, size, weight, italic!=0) );
	    else
		tabs->view(id)->unsetViewFont();
	qDebug( "setFont: %s, %d, %d, %d", fam.latin1(), size, weight, italic );
    }
    else if ( msg == "setBusyIndicatorType(QString)" ) {
	QString type;
	stream >> type;
	tabs->setBusyIndicatorType(type);
    }
    else if ( msg == "home()" ) {
	if ( isVisibleWindow( winId ( )))
	   nextView ( );
	else
	   raise ( );

    }
}

void Launcher::storageChanged()
{
    if ( in_lnk_props ) {
	got_lnk_change = TRUE;
	lnk_change = QString::null;
    } else {
	updateLink( QString::null );
    }
}


bool Launcher::mkdir(const QString &localPath)
{
    QDir fullDir(localPath);
    if (fullDir.exists())
	return true;

    // at this point the directory doesn't exist
    // go through the directory tree and start creating the direcotories
    // that don't exist; if we can't create the directories, return false

    QString dirSeps = "/";
    int dirIndex = localPath.find(dirSeps);
    QString checkedPath;

    // didn't find any seps; weird, use the cur dir instead
    if (dirIndex == -1) {
	//qDebug("No seperators found in path %s", localPath.latin1());
	checkedPath = QDir::currentDirPath();
    }

    while (checkedPath != localPath) {
	// no more seperators found, use the local path
	if (dirIndex == -1)
	    checkedPath = localPath;
	else {
	    // the next directory to check
	    checkedPath = localPath.left(dirIndex) + "/";
	    // advance the iterator; the next dir seperator
	    dirIndex = localPath.find(dirSeps, dirIndex+1);
	}

	QDir checkDir(checkedPath);
	if (!checkDir.exists()) {
	    //qDebug("mkdir making dir %s", checkedPath.latin1());

	    if (!checkDir.mkdir(checkedPath)) {
		qDebug("Unable to make directory %s", checkedPath.latin1());
		return FALSE;
	    }
	}

    }
    return TRUE;
}

void Launcher::preloadApps()
{
    Config cfg("Launcher");
    cfg.setGroup("Preload");
    QStringList apps = cfg.readListEntry("Apps",',');
    for (QStringList::ConstIterator it=apps.begin(); it!=apps.end(); ++it) {
#ifndef QT_NO_COP
	QCopEnvelope e("QPE/Application/"+(*it).local8Bit(), "enablePreload()");
#endif
    }
}
