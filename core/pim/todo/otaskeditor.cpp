#include <qdatetime.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmultilineedit.h>

#include <opie/orecurrancewidget.h>

#include "taskeditoroverview.h"
#include "taskeditorstatus.h"
#include "taskeditoralarms.h"

#include "otaskeditor.h"

OTaskEditor::OTaskEditor(int cur)
    : QDialog( 0, 0, TRUE, WStyle_ContextHelp ) {
    init();
    init( cur );
}
OTaskEditor::OTaskEditor( const OTodo& to)
    : QDialog( 0, 0, TRUE, WStyle_ContextHelp ) {
    init();
    init( to );
}
OTaskEditor::~OTaskEditor() {

}
void OTaskEditor::init( int cur ) {
    OTodo to;
    to.setUid( 1 ); // generate a new uid
    if ( cur != 0 )
        to.setCategories( cur );
    load(to);
}
void OTaskEditor::init( const OTodo& to ) {
    load( to );
}
OTodo OTaskEditor::todo()const{
    qWarning("saving!");
    OTodo to ( m_todo );
    m_overView->save( to );
    m_stat->save( to );
    to.setRecurrence( m_rec->recurrence() );
    m_alarm->save( to );

    return to;
}
void OTaskEditor::load(const OTodo& to) {
    m_overView->load( to );
    m_stat->load( to );
    m_rec->setRecurrence( to.recurrence(), to.hasDueDate() ? to.dueDate() : QDate::currentDate() );
    m_alarm->load( to );
    
    m_todo = to;
}
void OTaskEditor::init() {
    setCaption(tr("Task Editor") );

    QVBoxLayout* layo = new QVBoxLayout( this );
    m_tab = new OTabWidget( this );
    layo->addWidget( m_tab );

    /*
     * Add the Widgets
     */
    m_overView = new TaskEditorOverView( m_tab );
    m_tab->addTab( m_overView, "todo/info", tr("Information") );

    m_stat = new TaskEditorStatus( m_tab );
    m_tab->addTab( m_stat, "todo/TodoList", tr("Status") );

    m_alarm = new TaskEditorAlarms( m_tab );
    m_tab->addTab( m_alarm, "todo/alarm", tr("Alarms") );

//    m_remind = new TaskEditorAlarms( m_tab );
//    m_tab->addTab( m_remind, "todo/reminder", tr("Reminders") );

//    QLabel* lbl = new QLabel( m_tab );
//    lbl->setText( tr("X-Ref") );
//    m_tab->addTab( lbl, "todo/xref", tr("X-Ref") );

    m_rec = new ORecurranceWidget( true, QDate::currentDate(), this );
    m_tab->addTab( m_rec, "repeat", tr("Recurrence") );


    /* signal and slots */
    connect(m_overView, SIGNAL(recurranceEnabled(bool) ),
            m_rec, SLOT(setEnabled(bool) ) );

    /* connect due date changed to the recurrence tab */
    connect(m_stat, SIGNAL(dueDateChanged(const QDate&) ),
            m_rec, SLOT(setStartDate(const QDate& ) ) );


    m_tab->setCurrentTab( m_overView );
}
