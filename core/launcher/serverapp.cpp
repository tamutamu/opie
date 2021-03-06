/**********************************************************************
** Copyright (C) 2000-2003 Trolltech AS.  All rights reserved.
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

#include "serverapp.h"
#include "screensaver.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#include <opie2/multiauthpassword.h>

#include <qtopia/config.h>
#include <qtopia/power.h>

#ifdef Q_WS_QWS
#include <qtopia/qcopenvelope_qws.h>
#endif
#include <qtopia/global.h>
using namespace Opie::Core;

/* QT */
#ifdef Q_WS_QWS
#include <qgfx_qws.h>
#endif
#include <qmessagebox.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qfile.h>
#include <qpixmapcache.h>

/* STD */
#ifdef Q_OS_WIN32
#include <io.h>
#include <process.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>

#include <sysfs/libsysfs.h>

// NOTE: when we talk about "login" here we are talking about the
// authentication system (MultiAuth) and not opie-login

static ServerApplication *serverApp = 0;
static bool loginlock = false; // TRUE if the authentication system is currently shown
static int suspendEpoch;
static int resumeEpoch;

QCopKeyRegister::QCopKeyRegister()
    : m_keyCode( 0 )
{

}

QCopKeyRegister::QCopKeyRegister( int k, const QCString& c, const QCString& m )
    :m_keyCode( k ), m_channel( c ), m_message( m )
{

}

int QCopKeyRegister::keyCode() const
{
    return m_keyCode;
}

QCString QCopKeyRegister::channel() const
{
    return m_channel;
}

QCString QCopKeyRegister::message() const
{
    return m_message;
}

bool QCopKeyRegister::send()
{
    if (m_channel.isNull() )
        return false;

    QCopEnvelope e( m_channel, m_message );

    return true;
}

//---------------------------------------------------------------------------

/*
  Priority is number of alerts that are needed to pop up
  alert.
 */
class DesktopPowerAlerter : public QMessageBox
{
    Q_OBJECT
public:
    DesktopPowerAlerter( QWidget *parent, const char *name = 0 )
    : QMessageBox( tr("Battery Status"), tr("Low Battery"),
               QMessageBox::Critical,
               QMessageBox::Ok | QMessageBox::Default,
               QMessageBox::NoButton, QMessageBox::NoButton,
               parent, name, FALSE )
    {
        currentPriority = INT_MAX;
        alertCount = 0;
    }

    void alert( const QString &text, int priority );
    void hideEvent( QHideEvent * );
private:
    int currentPriority;
    int alertCount;
};

void DesktopPowerAlerter::alert( const QString &text, int priority )
{
    alertCount++;
    if ( alertCount < priority )
        return;
    if ( priority > currentPriority )
        return;
    currentPriority = priority;
    setText( text );
    show();
}


void DesktopPowerAlerter::hideEvent( QHideEvent *e )
{
    QMessageBox::hideEvent( e );
    alertCount = 0;
    currentPriority = INT_MAX;
}

//---------------------------------------------------------------------------

KeyFilter::KeyFilter(QObject* parent) : QObject(parent), held_tid(0), heldButton(0),firedHeldButton(0)
{
    /* We don't do this cause it would interfere with ODevice */
#if 0
    qwsServer->setKeyboardFilter(this);
#endif
}

void KeyFilter::timerEvent(QTimerEvent* e)
{
    if ( e->timerId() == held_tid ) {
        killTimer(held_tid);
        // button held
        if ( heldButton ) {
            emit activate(heldButton, TRUE);
            firedHeldButton = heldButton;
            heldButton = 0;
        }
        held_tid = 0;
    }
}

void KeyFilter::registerKey( const QCopKeyRegister& key )
{
    m_keys.insert( key.keyCode(), key );
}

void KeyFilter::unregisterKey( const QCopKeyRegister& key )
{
    m_keys.remove( key.keyCode() );
}

bool KeyFilter::keyRegistered( int key )
{
    /*
     * Check if we've a key registered
     */
    if ( !m_keys[key].send())
        return false;
    else
        return true;
}

bool KeyFilter::checkButtonAction(bool db, int keycode,  int press, int autoRepeat)
{
    bool locked;
    if( serverApp )
        locked = serverApp->screenLocked();
    else
        locked = loginlock;
    
    // Do hardcoded button combos
    if( !autoRepeat && ( locked || !keyRegistered( keycode ) ) ) {
        if( ODevice::inst()->comboKeyEvent( keycode, press, locked ) )
            return true;
    }

    if ( locked
        // Permitted keys
        && keycode != Key_F34 // power
        && keycode != Key_F30 // select
        && keycode != Key_Enter
        && keycode != Key_Return
        && keycode != Key_Space
        && keycode != Key_Left
        && keycode != Key_Right
        && keycode != Key_Up
        && keycode != Key_Down )
    return TRUE;

//    odebug << " KeyFilter::checkButtonAction("<<db<<","<<keycode<<","<<press<<","<<autoRepeat<<")"<<oendl;
        /* check if it was registered */
    if (!db ) {
        if (keycode != 0 &&press && !autoRepeat && keyRegistered(keycode) )
            return true;
    }
    else {
        // First check to see if DeviceButtonManager knows something about this button:
        const ODeviceButton* button = ODevice::inst()->buttonForKeycode(keycode);
        if (button && !autoRepeat) {
            if (firedHeldButton) {
                if (held_tid) {
                    killTimer(held_tid);
                    held_tid = 0;
                }
                if (!press && button == firedHeldButton) {
                    firedHeldButton = 0;
                    return TRUE;
                }
            }
            else if ( held_tid && heldButton != button) {
                killTimer(held_tid);
                held_tid = 0;
            }

            if ( button->heldAction().isNull() ) {
                if ( press )
                    emit activate(button, FALSE);
            }
            else if ( press ) {
                if (heldButton != button) {
                   heldButton = button;
                   held_tid = startTimer( ODevice::inst ()->buttonHoldTime () );
                }
            }
            else if ( heldButton ) {
                heldButton = 0;
                emit activate(button, FALSE);
            }
            QWSServer::screenSaverActivate(FALSE);
            return TRUE;
        }
        return false;
    }
    if ( keycode == HardKey_Suspend ) {
        if ( press ) emit power();
        return TRUE;
    }
    if ( keycode == HardKey_Backlight ) {
        if ( press ) emit backlight();
        return TRUE;
    }
    if ( keycode == Key_F32 ) {
#ifndef QT_NO_COP
        if ( press ) QCopEnvelope e( "QPE/Desktop", "startSync()" );
#endif
        return TRUE;
    }
    if ( keycode == Key_F31 ) {
        if ( press ) emit symbol();
        QWSServer::screenSaverActivate(FALSE);
        return TRUE;
    }

    if ( keycode == Key_NumLock )
        if ( press ) emit numLockStateToggle();

    if ( keycode == Key_CapsLock )
        if ( press ) emit capsLockStateToggle();

    if ( serverApp )
        serverApp->keyClick(keycode,press,autoRepeat);

    return FALSE;
}

enum MemState { MemUnknown, MemVeryLow, MemLow, MemNormal } memstate=MemUnknown;

#if defined(QPE_HAVE_MEMALERTER)
QPE_MEMALERTER_IMPL
#endif



//---------------------------------------------------------------------------

bool ServerApplication::doRestart = FALSE;
bool ServerApplication::allowRestart = TRUE;
bool ServerApplication::ms_is_starting  = TRUE;

void ServerApplication::switchLCD( bool on )
{
    if ( !qApp )
        return;

    ServerApplication *dapp = ServerApplication::me() ;

    if ( !dapp-> m_screensaver )
        return;

    if ( on ) {
        dapp-> m_screensaver-> setDisplayState ( true );
        dapp-> m_screensaver-> setBacklight ( -3 );
    }
    else
        dapp-> m_screensaver-> setDisplayState ( false );


}

ServerApplication::ServerApplication( int& argc, char **argv, Type t )
    : Opie::Core::OApplication( argc, argv, t )
{
    ms_is_starting = true;
    m_login = false;
    m_unlock_resume = false;

    // We know we'll have lots of cached pixmaps due to App/DocLnks
    QPixmapCache::setCacheLimit(512);

    m_ps = new PowerStatus;
    m_ps_last = new PowerStatus;
    pa = new DesktopPowerAlerter( 0 );

    m_apm_timer = new QTimer( this );
    connect(m_apm_timer, SIGNAL( timeout() ),
            this, SLOT( apmTimeout() ) );

    reloadPowerWarnSettings();

    QCopChannel *channel = new QCopChannel( "QPE/System", this );
    connect(channel, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(systemMessage(const QCString&,const QByteArray&) ) );

    channel = new QCopChannel("QPE/Launcher", this );
    connect(channel, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(launcherMessage(const QCString&,const QByteArray&) ) );

    channel = new QCopChannel("QPE/Desktop", this );
    connect(channel, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(desktopMessage(const QCString&,const QByteArray&) ) );

    m_screensaver = new OpieScreenSaver();
    m_screensaver->setInterval( -1 );
    QWSServer::setScreenSaver( m_screensaver );

    connect( qApp, SIGNAL( volumeChanged(bool) ),
             this, SLOT( rereadVolumes() ) );


    /* ### PluginLoader libqtopia SafeMode */
#if 0
    if ( PluginLoader::inSafeMode() )
    QTimer::singleShot(500, this, SLOT(showSafeMode()) );
    QTimer::singleShot(20*1000, this, SLOT(clearSafeMode()) );
#endif

    kf = new KeyFilter(this);

    connect( kf, SIGNAL(launch()), this, SIGNAL(launch()) );
    connect( kf, SIGNAL(power()), this, SIGNAL(power()) );
    connect( kf, SIGNAL(backlight()), this, SIGNAL(backlight()) );
    connect( kf, SIGNAL(symbol()), this, SIGNAL(symbol()));
    connect( kf, SIGNAL(numLockStateToggle()), this,SIGNAL(numLockStateToggle()));
    connect( kf, SIGNAL(capsLockStateToggle()), this,SIGNAL(capsLockStateToggle()));
    connect( kf, SIGNAL(activate(const Opie::Core::ODeviceButton*,bool)),
             this,SIGNAL(activate(const Opie::Core::ODeviceButton*,bool)));


    connect( kf, SIGNAL(backlight()), this, SLOT(toggleLight()) );

    connect( this, SIGNAL(power() ),
             SLOT(togglePower() ) );

    rereadVolumes();

    serverApp = this;

    apmTimeout();
    grabKeyboard();

    /* make sure the event filter is installed */  /* std::limits<short>::max() when you've stdc++ */
    const ODeviceButton* but = ODevice::inst()->buttonForKeycode( SHRT_MAX );
    Q_CONST_UNUSED( but )
}


ServerApplication::~ServerApplication()
{
    ungrabKeyboard();

    delete pa;
    delete m_ps;
    delete m_ps_last;
}

void ServerApplication::apmTimeout()
{
    serverApp-> checkMemory( ); // in case no events are generated
    *m_ps_last = *m_ps;
    *m_ps = PowerStatusManager::readStatus();

    if ( m_ps->acStatus() != m_ps_last-> acStatus() )
        m_screensaver-> powerStatusChanged( *m_ps );

    if ( m_ps->acStatus() == PowerStatus::Online ) {
        return;
    }

    int bat = m_ps-> batteryPercentRemaining();

    if ( bat < m_ps_last-> batteryPercentRemaining() ) {
        if ( bat <= m_powerCritical ) {
        QMessageBox battlow(
        tr("WARNING"),
        tr("<p>The battery level is critical!"
            "<p>Keep power off until AC is restored"),
        QMessageBox::Warning,
        QMessageBox::Cancel, QMessageBox::NoButton, QMessageBox::NoButton,
        0, QString::null, TRUE, WStyle_StaysOnTop);
        battlow.setButtonText(QMessageBox::Cancel, tr("Ok"));
        battlow.exec();
        }
        else if ( bat <= m_powerVeryLow )
            pa->alert( tr( "The battery is running very low. "), 2 );
    }

    if ( m_ps-> backupBatteryStatus() == PowerStatus::VeryLow ) {
        QMessageBox battlow(
        tr("WARNING"),
        tr("<p>The Back-up battery is very low"
            "<p>Please charge the back-up battery"),
        QMessageBox::Warning,
        QMessageBox::Cancel, QMessageBox::NoButton, QMessageBox::NoButton,
        0, QString::null, TRUE, WStyle_StaysOnTop);
        battlow.setButtonText(QMessageBox::Cancel, tr("Ok"));
        battlow.exec();
    }
}

void ServerApplication::systemMessage( const QCString& msg,
                                       const QByteArray& data )
{
    QDataStream stream ( data, IO_ReadOnly );

    if ( msg == "setScreenSaverInterval(int)" ) {
        int time;
        stream >> time;
        m_screensaver-> setInterval( time );
    }
    else if ( msg == "setScreenSaverIntervals(int,int,int)" ) {
        int t1, t2, t3;
        stream >> t1 >> t2 >> t3;
        m_screensaver-> setIntervals( t1, t2, t3 );
    }
    else if ( msg == "setBacklight(int)" ) {
        int bright;
        stream >> bright;
        m_screensaver-> setBacklight( bright );
    }
    else if ( msg == "incBacklight()" ) {
        m_screensaver->adjustBacklight(true);
    }
    else if ( msg == "decBacklight()" ) {
        m_screensaver->adjustBacklight(false);
    }
    else if ( msg == "setScreenSaverMode(int)" ) {
        int mode;
        stream >> mode;
        m_screensaver-> setMode ( mode );
    }
    else if ( msg == "reloadPowerWarnSettings()" ) {
        reloadPowerWarnSettings();
    }
    else if ( msg == "setDisplayState(int)" ) {
        int state;
        stream >> state;
        m_screensaver-> setDisplayState ( state != 0 );
    }
    else if ( msg == "suspend()" ) {
        emit power();
    }
    else if ( msg == "sendBusinessCard()" ) {
        QString card = ::getenv ( "HOME" );
        card += "/Applications/addressbook/businesscard.vcf";

        if ( QFile::exists( card ) ) {
            QCopEnvelope e ( "QPE/Obex", "send(QString,QString,QString)" );
            QString mimetype = "text/x-vCard";
            e << tr( "business card" ) << card << mimetype;
        }
    }
    else if ( msg == "systemSuspend()" ) {
        doBeforeSuspend();
    }
    else if ( msg == "systemResume()" ) {
        doResume();
    }
    else if ( msg == "setVolume(int)" ) {
        int vol;
        stream >> vol;
        adjustVolume( true, vol );
    }
    else if ( msg == "incVolume()" ) {
        adjustVolume( false, 10 );
    }
    else if ( msg == "decVolume()" ) {
        adjustVolume( false, -10 );
    }
    else if ( msg == "toggleMute()" ) {
        toggleMute();
    }
}

void ServerApplication::reloadPowerWarnSettings ( )
{
    Config cfg ( "apm" );
    cfg. setGroup ( "Warnings" );

    int iv = cfg. readNumEntry ( "checkinterval", 10000 );

    m_apm_timer-> stop ( );
    if ( iv )
        m_apm_timer-> start ( iv );

    m_powerVeryLow  = cfg. readNumEntry ( "powerverylow", 10 );
    m_powerCritical = cfg. readNumEntry ( "powervcritical", 5 );
}

void ServerApplication::launcherMessage( const QCString & msg, const QByteArray & data )
{
    QDataStream stream ( data, IO_ReadOnly );

    if ( msg == "deviceButton(int,int,int)" ) {
        int keycode, press, autoRepeat;
        stream >> keycode >> press >> autoRepeat;

        kf->checkButtonAction ( true, keycode, press, autoRepeat );
    }
    else if ( msg == "keyRegister(int,QCString,QCString)" ) {
        int k;
        QCString c, m;
        stream >> k >> c >> m;

        kf -> registerKey( QCopKeyRegister(k, c, m) );
    }
}

void ServerApplication::desktopMessage( const QCString & msg, const QByteArray & data )
{
    if ( msg == "unlocked()" ) {
        // See doResume() for an explanation of this
        if( m_unlock_resume ) {
            m_unlock_resume = false;
            doAfterResume();
        }
    }
}

bool ServerApplication::screenLocked()
{
    return loginlock || Opie::Security::MultiauthPassword::isAuthenticating();
}

bool ServerApplication::login(bool at_poweron)
{
    if(loginlock) {
        return false;
    }

    if( !at_poweron ) {
        // Handle minimum suspend time
        Config pcfg("Security");
        pcfg.setGroup( "Misc" );
        bool skipEnabled = pcfg.readBoolEntry( "suspendTime", false );
        if( skipEnabled ) {
            int skipTime = pcfg.readNumEntry( "suspendTimeMins", 2 ) * 60;
            int secsSuspended = resumeEpoch - suspendEpoch;
            if ( secsSuspended < skipTime )
                return true;
        }
    }

    loginlock = true;

    Global::terminateBuiltin("calibrate"); // No tr
    int lockMode = at_poweron ? Opie::Security::IfPowerOn : Opie::Security::IfResume;
    Opie::Security::MultiauthPassword::authenticate(lockMode);
    loginlock = false;
    return true;
}

#if defined(QPE_HAVE_TOGGLELIGHT)
#include <qtopia/config.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <time.h>
#endif

namespace {
    void execAutoStart() {
        QString appName;
        int delay;
        QDateTime now = QDateTime::currentDateTime();

        Config cfg( "autostart" );
        cfg.setGroup( "AutoStart" );
        appName = cfg.readEntry( "Apps", "" );
        delay = cfg.readNumEntry( "Delay", 0  );

        // If the time between suspend and resume was longer then the
        // value saved as delay, start the app
        if ( (resumeEpoch - suspendEpoch) >= ( delay * 60 ) && !appName.isEmpty() ) {
            QCopEnvelope e( "QPE/System", "execute(QString)" );
            e << QString( appName );
        }
    }
}

int ServerApplication::readRtc()
{
    int ret = 0;
    
    struct sysfs_class *cls = sysfs_open_class( "rtc" );
    if( cls ) {
        struct sysfs_class_device *cdev = sysfs_get_class_device( cls, "rtc0" );
        if( cdev ) {
            struct sysfs_attribute *attr = sysfs_get_classdev_attr( cdev, "since_epoch" );
            if( attr ) {
                if( sysfs_read_attribute( attr ) == 0 )
                    ret = atoi( attr->value );
            }
        }
        sysfs_close_class( cls );
    }

    return ret;
}

void ServerApplication::doBeforeSuspend()
{
    suspendEpoch = readRtc();

#ifdef QWS
    // Notify user that the system is suspending
    QCopEnvelope e( "QPE/TaskBar", "message(QString)" );
    e << QString( tr("Suspending...") );

    if ( !m_login && !loginlock && Opie::Security::MultiauthPassword::needToAuthenticate ( false ) ) {
        if ( qt_screen ) {
            // Should use a big black window instead.
            // But this would not show up fast enough
            QGfx *g = qt_screen-> screenGfx ( );
            g-> fillRect ( 0, 0, qt_screen-> width ( ), qt_screen-> height ( ));
            delete g;
        }

        m_login = true;
    }
#endif
}

void ServerApplication::doResume()
{
#ifdef QWS
    // Clear system suspend notification message
    QCopEnvelope e( "QPE/TaskBar", "message(QString)" );
    e << QString::null;
#endif

    resumeEpoch = readRtc();

    if( !loginlock && Opie::Security::MultiauthPassword::isAuthenticating() ) {
        // Authentication is already taking place but not by us (eg. because the
        // user used the lock applet before suspending), so we need to let it
        // carry on and when we get the unlocked() QCop message we can do our
        // post-resume tasks ( in doAfterResume() ).
        m_unlock_resume = true;
        m_login = false;
        return;
    }

    if( m_login ) {
        // Perform authentication
        if ( !login( false ) ) {
            return;
        }
        m_login = false;
    }

    doAfterResume();
}

void ServerApplication::doAfterResume()
{
    execAutoStart();
}

void ServerApplication::togglePower()
{
    static bool excllock = false;

    if ( excllock )
        return ;

    excllock = true;

    doBeforeSuspend();

    ODevice::inst ( )-> suspend ( );

    ServerApplication::switchLCD ( true ); // force LCD on without slow qcop call
    QWSServer::screenSaverActivate ( false );

    {
        QCopEnvelope( "QPE/Card", "mtabChanged()" ); // might have changed while asleep
    }

    excllock = false;
}

void ServerApplication::toggleLight()
{
#ifndef QT_NO_COP
    QCopEnvelope e("QPE/System", "setBacklight(int)");
    e << -2; // toggle
#endif
}


/*
 * We still listen to key events but handle them in
 * a special class
 */

bool ServerApplication::eventFilter( QObject *o, QEvent *e) {
    if ( e->type() != QEvent::KeyPress &&
         e->type() != QEvent::KeyRelease )
        return QPEApplication::eventFilter( o, e );

    QKeyEvent *ke = static_cast<QKeyEvent*>( e );
    if ( kf->checkButtonAction( true, ke->key(),
                                e->type() == QEvent::KeyPress,
                                ke-> isAutoRepeat() ))
        return true;

    return QPEApplication::eventFilter( o, e );

}

#ifdef Q_WS_QWS
bool ServerApplication::qwsEventFilter( QWSEvent *e )
{
    checkMemory();

    if ( e->type == QWSEvent::Mouse ) {
        QWSMouseEvent *me = (QWSMouseEvent *)e;
        static bool up = TRUE;
        if ( me->simpleData.state&LeftButton ) {
            if ( up ) {
              up = FALSE;
              screenClick(TRUE);
            }
        }
        else if ( !up ) {
            up = TRUE;
            screenClick(FALSE);
        }
    }
    else if ( e->type == QWSEvent::Key ) {
        QWSKeyEvent * ke = static_cast<QWSKeyEvent*>( e );
        if ( kf->checkButtonAction( false,
                                    ke-> simpleData.keycode,
                                    ke-> simpleData.is_press,
                                    ke-> simpleData.is_auto_repeat ) )
            return true;
    }

    return QPEApplication::qwsEventFilter( e );
}
#endif


/* ### FIXME libqtopia Plugin Safe Mode */

void ServerApplication::showSafeMode()
{
#if 0
    if ( QMessageBox::warning(0, tr("Safe Mode"), tr("<P>A system startup error occurred, "
        "and the system is now in Safe Mode. "
        "Plugins are not loaded in Safe Mode. "
        "You can use the Plugin Manager to "
        "disable plugins that cause system error."), tr("OK"), tr("Plugin Manager..."), 0) == 1 )
    {
        Global::execute( "pluginmanager" );
    }
#endif
}

void ServerApplication::clearSafeMode()
{
#if 0
    // If we've been running OK for a while then we won't bother going into
    // safe mode immediately on the next crash.
    Config cfg( "PluginLoader" );
    cfg.setGroup( "Global" );
    QString mode = cfg.readEntry( "Mode", "Normal" );
    if ( mode == "MaybeSafe" ) {
        cfg.writeEntry( "Mode", "Normal" );
    }
#endif
}


void ServerApplication::shutdown()
{
    if ( type() != GuiServer )
        return;
    ShutdownImpl *sd = new ShutdownImpl( 0, 0, WDestructiveClose );
    connect( sd, SIGNAL(shutdown(ShutdownImpl::Type)),
         this, SLOT(shutdown(ShutdownImpl::Type)) );
    QPEApplication::showWidget( sd );
}

void ServerApplication::shutdown( ShutdownImpl::Type t )
{
    char *opt = 0;

    switch ( t ) {
        case ShutdownImpl::ShutdownSystem:
            opt = "-h";
            // fall through
        case ShutdownImpl::RebootSystem:
            if ( opt == 0 )
                opt = "-r";

            if ( execl( "/sbin/shutdown", "shutdown", opt, "now", ( void* ) 0) < 0 )
                perror("shutdown");
//                      ::syslog ( LOG_ERR, "Erroring execing shutdown\n" );

            break;
        case ShutdownImpl::RestartDesktop:
            restart();
            break;
        case ShutdownImpl::TerminateDesktop:
            prepareForTermination( FALSE );

            // This is a workaround for a Qt bug
            // clipboard applet has to stop its poll timer, or Qt/E
            // will hang on quit() right before it emits aboutToQuit()
            emit aboutToQuit ( );

            quit();
            break;
    }
}

void ServerApplication::restart()
{
    if ( allowRestart ) {

        /*
         * Applets and restart is a problem. Some applets delete
         * their widgets even if ownership gets transfered to the
         * parent (Systray ) but deleting the applet may be unsafe
         * as well ( double deletion ). Some have topLevel widgets
         * and when we dlclose and then delete the widget we will
         * crash and an crash during restart is not nice
         */
#ifdef ALL_APPLETS_ON_THIS_WORLD_ARE_FIXED
        /* same as above */
        emit aboutToQuit();
        prepareForTermination(TRUE);
        doRestart = TRUE;
        quit();
#else
        prepareForTermination( true );
        for ( int fd = 3; fd < 100; fd++ )
            close( fd );
        execl( QString( OPIE_BINDIR "/qpe" ).local8Bit(), "qpe", NULL );
        exit( 1 );
#endif
    }
}

void ServerApplication::rereadVolumes()
{
    Config cfg( "qpe" );
    cfg. setGroup ( "Volume" );

    m_screentap_sound = cfg. readBoolEntry ( "TouchSound" );
    m_keyclick_sound  = cfg. readBoolEntry ( "KeySound" );
    m_alarm_sound     = cfg. readBoolEntry ( "AlarmSound" );
    m_alarm_percent   = cfg.readNumEntry ( "AlarmPercent", 65 );
    m_volume_muted    = cfg.readBoolEntry ( "Mute", 0 );
    m_volume_percent  = cfg.readNumEntry ( "VolumePercent", 50 );
}

void ServerApplication::checkMemory()
{
#if defined(QPE_HAVE_MEMALERTER)
    static bool ignoreNormal=TRUE;
    static bool existingMessage=FALSE;

    if(existingMessage)
    return; // don't show a second message while still on first

    existingMessage = TRUE;
    switch ( memstate ) {
        case MemUnknown:
            break;
        case MemLow:
            memstate = MemUnknown;
            if ( !recoverMemory() ) {
            QMessageBox::warning( 0 , tr("Memory Status"),
                tr("Memory Low\nPlease save data.") );
            ignoreNormal = FALSE;
            }
            break;
        case MemNormal:
            memstate = MemUnknown;
            if ( !ignoreNormal ) {
            ignoreNormal = TRUE;
            QMessageBox::information ( 0 , tr("Memory Status"),
                "Memory OK" );
            }
            break;
        case MemVeryLow:
            memstate = MemUnknown;
            QMessageBox::critical( 0 , tr("Memory Status"),
            tr("Critical Memory Shortage\n"
            "Please end this application\n"
            "immediately.") );
            recoverMemory();
    }
    existingMessage = FALSE;
#endif
}

bool ServerApplication::recoverMemory()
{
    return FALSE;
}

void ServerApplication::keyClick(int , bool press, bool )
{
    if ( press && m_keyclick_sound )
        ODevice::inst() -> playKeySound();
}

void ServerApplication::screenClick(bool press)
{
    if ( press && m_screentap_sound )
        ODevice::inst() -> playTouchSound();
}

void ServerApplication::soundAlarm() {
    if ( me ()->m_alarm_sound )
        ODevice::inst()->playAlarmSound();
}

void ServerApplication::setUseAlarmVolume( bool useAlarmVol )
{
#ifndef QT_NO_COP
    // We don't use adjustVolume here because we don't want the volume
    // change to be saved to disk
    int vol;
    if( useAlarmVol )
        vol = me()->m_alarm_percent;
    else
        vol = -1;
    QCopEnvelope e( "QPE/System", "setVolume(int,int)" );
    e << 0;
    e << vol;
#endif
}

void ServerApplication::toggleMute()
{
    m_volume_muted = !m_volume_muted;
    
    Config cfg( "qpe" );
    cfg.setGroup( "Volume" );
    cfg.writeEntry( "Mute", m_volume_muted );

#ifndef QT_NO_COP
    QCopEnvelope e( "QPE/System", "volumeChange(bool)" );
    e << m_volume_muted;
#endif
}

void ServerApplication::adjustVolume( bool absolute, int vol )
{
    int vol_value;

    if( absolute )
        vol_value = vol;
    else
        vol_value = m_volume_percent + vol;
    
    // clamp volume percent to be between 0 and 100
    vol_value = ( vol_value < 0 ) ? 0 : (( vol_value > 100 ) ? 100 : vol_value );

    if( vol_value != m_volume_percent ) {
        Config cfg( "qpe" );
        cfg.setGroup( "Volume" );
        cfg.writeEntry( "VolumePercent", vol_value );

#ifndef QT_NO_COP
        QCopEnvelope e( "QPE/System", "volumeChange(bool)" );
        e << m_volume_muted;
#endif
    }
}

ServerApplication *ServerApplication::me ( )
{
    return static_cast<ServerApplication*>( qApp );
}

bool ServerApplication::isStarting()
{
    return ms_is_starting;
}

int ServerApplication::exec()
{
    ms_is_starting = true;
    odebug << "Serverapp - exec" << oendl;
    return QPEApplication::exec();
}

#include "serverapp.moc"
