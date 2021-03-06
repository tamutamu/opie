/*
Dagger - A Bible study program utilizing the Sword library.
Copyright (c) 2004 Dan Williams <drw@handhelds.org>

This file is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This file is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
file; see the file COPYING. If not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "searchbar.h"
#include "textwidget.h"

#include <opie2/oresource.h>
#include <opie2/owait.h>

#include <qpe/qpeapplication.h>

#include <qaction.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qwhatsthis.h>

#include <listkey.h>
#include <regex.h>
#include <versekey.h>

void searchCallback( char /*percent*/, void */*userData*/ )
{
    qApp->processEvents();
}

SearchBar::SearchBar( QMainWindow *parent )
    : QToolBar( QString::null, parent, QMainWindow::Top, true )
    , m_currText( 0x0 )
{
    // Initialize UI
    m_searchText = new QLineEdit( this );
    setStretchableWidget( m_searchText );
    QWhatsThis::add( m_searchText, tr( "Enter text to search for here." ) );
    connect(m_searchText, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotTextChanged(const QString &)) );

    m_actionFind = new QAction( tr( "Find" ),
                                Opie::Core::OResource::loadPixmap( "find", Opie::Core::OResource::SmallIcon ),
                                QString::null, 0, this, 0 );
    m_actionFind->setEnabled( false );
    m_actionFind->addTo( this );
    m_actionFind->setWhatsThis( tr( "Tap here search the current module for the text entered to the left." ) );
    connect( m_actionFind, SIGNAL(activated()), this, SLOT(slotFind()) );

    addSeparator();

    m_actionPrev = new QAction( tr( "Previous result" ),
                                Opie::Core::OResource::loadPixmap( "back", Opie::Core::OResource::SmallIcon ),
                                QString::null, 0, this, 0 );
    m_actionPrev->setEnabled( false );
    m_actionPrev->addTo( this );
    m_actionPrev->setWhatsThis( tr( "Tap here to view the previous search result." ) );
    connect( m_actionPrev, SIGNAL(activated()), this, SLOT(slotPrev()) );

    m_resultList = new QComboBox( this );
    m_resultList->setEnabled( false );
    QWhatsThis::add( m_resultList, tr( "Select the desired search result here." ) );
    connect( m_resultList, SIGNAL(activated(const QString &)), this, SIGNAL(sigResultClicked(const QString &)) );

    m_actionNext = new QAction( tr( "Next result" ),
                                Opie::Core::OResource::loadPixmap( "forward", Opie::Core::OResource::SmallIcon ),
                                QString::null, 0, this, 0 );
    m_actionNext->setEnabled( false );
    m_actionNext->addTo( this );
    m_actionNext->setWhatsThis( tr( "Tap here to view the next search result." ) );
    connect( m_actionNext, SIGNAL(activated()), this, SLOT(slotNext()) );

    if ( parent )
    {
        installEventFilter( parent );
        // TODO - install for all controls
        m_searchText->installEventFilter( parent );
    }
}

void SearchBar::setCurrModule( TextWidget *currText )
{
    m_actionFind->setEnabled( ( m_searchText->text() != "" ) && currText );

    if ( !m_currText || !currText || ( currText->getModuleName() != m_currText->getModuleName() ) )
    {
        m_actionPrev->setEnabled( false );
        m_resultList->clear();
        m_resultList->setEnabled( false );
        m_actionNext->setEnabled( false );
    }

    m_currText = currText;
}

void SearchBar::slotTextChanged( const QString &newText )
{
    m_actionFind->setEnabled( ( newText != "" ) && m_currText );
}

void SearchBar::slotFind()
{
    m_resultList->clear();

    // Change application title and display Opie wait dialog to indicate search is beginning
    QWidget *pWidget = reinterpret_cast<QWidget *>(parent());
    QString caption = pWidget->caption();
    pWidget->setCaption( "Searching..." );

    Opie::Ui::OWait wait( pWidget );
    wait.show();
    qApp->processEvents();

    // Perform search
    // TODO - implement search callback function to animate wait cursor
    sword::ListKey results = m_currText->getModule()->Search( m_searchText->text().latin1(), 0, REG_ICASE, 0, 0,
                                                              &searchCallback );

    // Process results
    int count = results.Count();
    bool found = count > 0;
    if ( found )
    {
        // Populate results combo box
        sword::VerseKey key;
        for ( int i = 0; i < count; i++ )
        {
            key.setText( results.GetElement( i )->getText() );
            m_resultList->insertItem( key.getShortText() );
        }

        // Goto first result in list
        m_resultList->setCurrentItem( 0 );
        emit sigResultClicked( m_resultList->currentText() );
    }
    else
    {
        // Reset application title
        pWidget->setCaption( caption );
    }

    // UI clean-up
    wait.hide();

    m_actionPrev->setEnabled( false );
    m_resultList->setEnabled( found );
    m_actionNext->setEnabled( count > 1 );
}

void SearchBar::slotPrev()
{
    int item = m_resultList->currentItem() - 1;
    m_resultList->setCurrentItem( item );
    emit sigResultClicked( m_resultList->currentText() );

    m_actionPrev->setEnabled( item > 0 );
    m_actionNext->setEnabled( item < m_resultList->count() - 1 );
}

void SearchBar::slotNext()
{
    int item = m_resultList->currentItem() + 1;
    m_resultList->setCurrentItem( item );
    emit sigResultClicked( m_resultList->currentText() );

    m_actionPrev->setEnabled( true );
    m_actionNext->setEnabled( item < m_resultList->count() - 1 );
}

void SearchBar::slotCloseBtn()
{
    hide();
}
