/*
                             This file is part of the Opie Project
                             (C) 2002-2006 The Opie Team <opie-devel@lists.sourceforge.net>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
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

#ifndef ODEVICE_H_
#define ODEVICE_H_

/* OPIE */
#include <opie2/odevicebutton.h>
#include <qpe/qpeapplication.h> /* for Transformation enum.. */

/* QT */
#include <qnamespace.h>
#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qdict.h>
#include <qtimer.h>


#include <qstrlist.h>
#include <qwindowsystem_qws.h>


class Sound;
class Config;

namespace Opie{
namespace Core{

class ODeviceData;
/**
 * The available devices
 */
enum OModel {
    Model_Unknown, // = 0

    Model_Series_Mask   = 0xfff00000,

    Model_iPAQ          = ( 1 << 20 ),

    Model_iPAQ_All      = ( Model_iPAQ | 0xfffff ),
    Model_iPAQ_H31xx    = ( Model_iPAQ | 0x00001 ),
    Model_iPAQ_H36xx    = ( Model_iPAQ | 0x00002 ),
    Model_iPAQ_H37xx    = ( Model_iPAQ | 0x00004 ),
    Model_iPAQ_H38xx    = ( Model_iPAQ | 0x00008 ),
    Model_iPAQ_H39xx    = ( Model_iPAQ | 0x00010 ),
    Model_iPAQ_H5xxx    = ( Model_iPAQ | 0x00020 ),
    Model_iPAQ_H22xx    = ( Model_iPAQ | 0x00040 ),
    Model_iPAQ_H191x    = ( Model_iPAQ | 0x00080 ),
    Model_iPAQ_H1940    = ( Model_iPAQ | 0x00100 ),
    Model_iPAQ_HX4700   = ( Model_iPAQ | 0x00200 ),
    Model_iPAQ_H4xxx    = ( Model_iPAQ | 0x00400 ),
    Model_iPAQ_RX3xxx   = ( Model_iPAQ | 0x00800 ),
    Model_iPAQ_RX1950   = ( Model_iPAQ | 0x01000 ),

    Model_Jornada       = ( 2 << 20 ),

    Model_Jornada_56x   = ( Model_Jornada | 0x00001 ),
    Model_Jornada_720   = ( Model_Jornada | 0x00002 ),
    Model_Jornada_820   = ( Model_Jornada | 0x00004 ),

    Model_Zaurus        = ( 3 << 20 ),

    Model_Zaurus_All    = ( Model_Zaurus | 0xfffff ),
    Model_Zaurus_SL5000 = ( Model_Zaurus | 0x00001 ),
    Model_Zaurus_SL5500 = ( Model_Zaurus | 0x00002 ),
    Model_Zaurus_SLA300 = ( Model_Zaurus | 0x00004 ),
    Model_Zaurus_SLB600 = ( Model_Zaurus | 0x00008 ),
    Model_Zaurus_SLC7x0 = ( Model_Zaurus | 0x00010 ),
    Model_Zaurus_SL6000 = ( Model_Zaurus | 0x00020 ),
    Model_Zaurus_SLC3000 = ( Model_Zaurus | 0x00040 ),
    Model_Zaurus_SLC1000 = ( Model_Zaurus | 0x00080 ),
    Model_Zaurus_SLC3100 = ( Model_Zaurus | 0x00100 ),

    Model_SIMpad        = ( 4 << 20 ),

    Model_SIMpad_All    = ( Model_SIMpad | 0xfffff ),
    Model_SIMpad_CL4    = ( Model_SIMpad | 0x00001 ),
    Model_SIMpad_SL4    = ( Model_SIMpad | 0x00002 ),
    Model_SIMpad_SLC    = ( Model_SIMpad | 0x00004 ),
    Model_SIMpad_TSinus = ( Model_SIMpad | 0x00008 ),

    Model_Yopy          = ( 6 << 20 ),

    Model_Yopy_All      = ( Model_Yopy | 0xfffff ),
    Model_Yopy_3000     = ( Model_Yopy | 0x00001 ),
    Model_Yopy_3500     = ( Model_Yopy | 0x00002 ),
    Model_Yopy_3700     = ( Model_Yopy | 0x00004 ),

    Model_Beagle        = ( 7 << 20 ),

    Model_Beagle_All    = ( Model_Beagle | 0xfffff ),
    Model_Beagle_PA100  = ( Model_Beagle | 0x00001 ),

    Model_GenuineIntel  = ( 8 << 20 ),

    Model_MyPal         = ( 9 << 20 ),

    Model_MyPal_All     = ( Model_MyPal | 0xfffff ),
    Model_MyPal_620     = ( Model_MyPal | 0x00001 ),
    Model_MyPal_716     = ( Model_MyPal | 0x00002 ),
    Model_MyPal_730     = ( Model_MyPal | 0x00004 ),

    Model_HTC           = ( 10 << 20 ),

    Model_HTC_All       = ( Model_HTC | 0xfffff ),
    Model_HTC_Universal = ( Model_HTC | 0x00001 ),
    Model_HTC_Alpine    = ( Model_HTC | 0x00002 ),
    Model_HTC_Apache    = ( Model_HTC | 0x00004 ),
    Model_HTC_Beetles   = ( Model_HTC | 0x00008 ),
    Model_HTC_Blueangel = ( Model_HTC | 0x00010 ),
    Model_HTC_Himalaya  = ( Model_HTC | 0x00020 ),
    Model_HTC_Magician  = ( Model_HTC | 0x00040 ),

    Model_Motorola      = ( 11 << 20 ),

    Model_Motorola_All  = ( Model_Motorola | 0xfffff ),
    Model_Motorola_EZX  = ( Model_Motorola | 0x00001 ),

    Model_Palm          = ( 12 << 20 ),

    Model_Palm_All      = ( Model_Palm | 0xfffff ),
    Model_Palm_TT       = ( Model_Palm | 0x00001 ),
    Model_Palm_TT2      = ( Model_Palm | 0x00002 ),
    Model_Palm_TT3      = ( Model_Palm | 0x00004 ),
    Model_Palm_TT5      = ( Model_Palm | 0x00008 ),
    Model_Palm_TC       = ( Model_Palm | 0x00010 ),
    Model_Palm_TE       = ( Model_Palm | 0x00020 ),
    Model_Palm_TE2      = ( Model_Palm | 0x00040 ),
    Model_Palm_LD       = ( Model_Palm | 0x00080 ),
    Model_Palm_TX       = ( Model_Palm | 0x00100 ),
    Model_Palm_Z71      = ( Model_Palm | 0x00200 ),
    Model_Palm_Z72      = ( Model_Palm | 0x00400 ),
    Model_Palm_T600     = ( Model_Palm | 0x00800 ),
    Model_Palm_T650     = ( Model_Palm | 0x01000 ),
    Model_Palm_T680     = ( Model_Palm | 0x02000 ),
    Model_Palm_T700W    = ( Model_Palm | 0x04000 ),
    Model_Palm_T700P    = ( Model_Palm | 0x08000 ),
    Model_Palm_T750     = ( Model_Palm | 0x10000 ),
    Model_Palm_T755P    = ( Model_Palm | 0x20000 ),
    Model_Palm_FOLEO    = ( Model_Palm | 0x40000 ),

};

/**
 * The vendor of the device
 */
enum OVendor {
    Vendor_Unknown,

    Vendor_HP,
    Vendor_Sharp,
    Vendor_SIEMENS,
    Vendor_MundN,
    Vendor_GMate,
    Vendor_MasterIA,
    Vendor_GenuineIntel,
    Vendor_Asus,
    Vendor_HTC,
    Vendor_Motorola,
    Vendor_Palm,
};

/**
 * The System used
 */
enum OSystem {
    System_Unknown,

    System_Familiar,
    System_Zaurus,
    System_OpenZaurus,
    System_Linupy,
    System_OpenEmbedded,
    System_PC,
    System_OpenEZX,
    System_Angstrom,
};

typedef struct {
    OSystem system;
    const char* sysstr;
    const char* sysvfile;
} ODistribution;

extern ODistribution distributions[];


enum OLedState {
    Led_Off,
    Led_On,
    Led_BlinkSuperSlow,
    Led_BlinkSlow,
    Led_BlinkFast,
    Led_BlinkSuperFast
};

enum OHardKey {
    HardKey_Datebook  = Qt::Key_F9,
    HardKey_Contacts  = Qt::Key_F10,
    HardKey_Menu      = Qt::Key_F11,
    HardKey_Home      = Qt::Key_F12,
    HardKey_Mail      = Qt::Key_F13,
    HardKey_Record    = Qt::Key_F24,
    HardKey_Suspend   = Qt::Key_F34,
    HardKey_Backlight = Qt::Key_F35,
    HardKey_Action    = Qt::Key_F10,
    HardKey_OK        = Qt::Key_F11,
    HardKey_End       = Qt::Key_F12,
};

enum ODirection {
    CW   = 0,
    CCW  = 1,
    Flip = 2,
};

enum OHingeStatus {
    CASE_CLOSED = 3,
    CASE_PORTRAIT = 2,
    CASE_LANDSCAPE = 0,
    CASE_UNKNOWN = 1,
};

/* default button for qvfb or such
 * see odevice.cpp for details.
 * hint: manage a user defined button for qvfb?
 * alwin
 */
struct ODeviceButtonComboStruct {
    uint model;
    Qt::Key keycode1;
    Qt::Key keycode2;
    Qt::Key keycode3;
    bool hold;
    const char *keycodedesc;
    const char *actionchan;
    const char *actionmsg;
    const char *actiondesc;
    bool lockedonly;
};

class OLedTimer;

/**
 * A singleton which gives informations about device specefic option
 * like the Hardware used, LEDs, the Base Distribution and
 * hardware key mappings.
 *
 * @short A small class for device specific options
 * @see QObject
 * @author Robert Griebl
 * @version 1.0
 */
class ODevice : public QObject
{
    Q_OBJECT

    friend class OLedTimer;

private:
    /* disable copy */
    ODevice ( const ODevice & );

protected:
    ODevice();
    virtual void init(const QString&);
    virtual void initButtons();
    virtual void initButtonCombos();
    static void sendSuspendmsg();
    void loadButtonCombos( const ODeviceButtonComboStruct combos[], uint length );
    bool apmSuspend( int timeout );
    bool setLedInternal ( const QString &led, bool on );

    ODeviceData *d;

public:
    // sandman do we want to allow destructions? -zecke?
    virtual ~ODevice();
    static ODevice *inst();

    // information
    QString modelString() const;
    OModel model() const;
    inline OModel series() const { return (OModel) ( model() & Model_Series_Mask ); }

    QString vendorString() const;
    OVendor vendor() const;

    QString systemString() const;
    OSystem system() const;

    QString systemVersionString() const;

    virtual Transformation rotation() const;
    virtual ODirection direction() const;

    QString qteDriver() const;

    // system
    virtual bool suspend();

    virtual bool setDisplayStatus ( bool on );
    virtual bool setDisplayBrightness ( int brightness );
    virtual int displayBrightnessResolution() const;
    virtual bool setDisplayContrast ( int contrast );
    virtual int displayContrastResolution() const;

    // don't add new virtual methods, use this:
    //  /*virtual */ void boo(int i ) { return virtual_hook(1,&i); };
    // and in your subclass do overwrite
    //  protected virtual int virtual_hook(int, void *)
    // which is defined below

    // input / output
    virtual void playAlarmSound();
    virtual void playKeySound();
    virtual void playTouchSound();

    virtual QStringList ledList() const;
    virtual OLedState ledState ( const QString &led ) const;
    virtual bool setLedState ( const QString &led, OLedState st );

    virtual bool hasLightSensor() const;
    virtual int readLightSensor();
    virtual int lightSensorResolution() const;

    virtual bool hasHingeSensor() const;
    virtual OHingeStatus readHingeSensor()const;

    virtual bool hasWaveSpeaker() const;

    const QStrList &allowedCpuFrequencies() const;
    bool setCurrentCpuFrequency(uint index);

    /**
     * Returns the available buttons on this device.  The number and location
     * of buttons will vary depending on the device.  Button numbers will be assigned
     * by the device manufacturer and will be from most preferred button to least preffered
     * button.  Note that this list only contains "user mappable" buttons.
     *
     * @todo Make method const and take care of calling initButtons or make that const too
     *
     */
    const QValueList<ODeviceButton> &buttons();

    /**
     * Returns the DeviceButton for the \a keyCode.  If \a keyCode is not found, it
     * returns 0L
     */
    const ODeviceButton *buttonForKeycode ( ushort keyCode );

    /**
     * Reassigns the pressed action for \a button.  To return to the factory
     * default pass an empty string as \a qcopMessage.
     */
    void remapPressedAction ( int button, const OQCopMessage &qcopMessage );

    /**
     * Reassigns the held action for \a button.  To return to the factory
     * default pass an empty string as \a qcopMessage.
     */
    void remapHeldAction ( int button, const OQCopMessage &qcopMessage );

    /**
     * How long (in ms) you have to press a button for a "hold" action
     */
    uint buttonHoldTime() const;

    /**
     * Register a new custom button
     */
    void registerCustomButton( int keycode, const QString &usertext, const QString &icon );

    /**
     * Un-register a custom button
     */
    void unregisterCustomButton( int keycode );

    /**
     * Returns the button combos set up for this device. At the moment these are intended
     * to be hard-coded eg. for launching screen calibration.
     *
     */
    const QValueList<ODeviceButtonCombo> &buttonCombos();

    /**
     * Try all registered button combos and launch action if one has been activated
     * @returns true if the combo was activated, false otherwise
     */
    bool comboKeyEvent( ushort keycode, bool press, bool locked );

    /**
     * Get the string representation of a button
     */
    static QString keyToString( int keycode );

    /**
     * Get the keycode from its string representation
     */
    static int stringToKey( const QString &keystr );

signals:
    void buttonMappingChanged();

private slots:
    void systemMessage ( const QCString &, const QByteArray & );
    void holdCheckCombo();

protected:
    void addPreHandler(QWSServer::KeyboardFilter*aFilter);
    void remPreHandler(QWSServer::KeyboardFilter*aFilter);
    void reloadButtonMapping();
    void reloadCustomButtons();
    void readButtonConfig( Config &cfg, bool custom );
    void writeButtonUserSettings( const ODeviceButton &button, int idx );

    /*  ugly virtual hook */
    virtual void virtual_hook( int id, void* data );
};

class OLedTimer : public QTimer {
    Q_OBJECT
public:
    OLedTimer ( const QString &led, ODevice *odev, const char * name=0 );

private slots:
    void fire();
private:
    QString led;
    ODevice *odev;
    bool on;
};


class ODeviceData {

  public:
    QString m_vendorstr;
    OVendor m_vendor;

    QString m_modelstr;
    OModel m_model;

    QString m_systemstr;
    OSystem m_system;

    QString m_sysverstr;

    Transformation m_rotation;
    ODirection m_direction;

    QString m_qteDriver;

    QValueList <ODeviceButton> *m_buttons;
    QValueList <ODeviceButtonCombo> *m_buttonCombos;
    uint                        m_holdtime;
    QStrList                   *m_cpu_frequencies;
    bool    m_initializedButtonQcop : 1;
    ODeviceButtonCombo         *m_heldCombo;
    QTimer                      *m_heldComboTimer;

    int m_brightnessRes;
    QString m_backlightDev;

    QMap<QString,OLedState> m_ledState;
    QDict<OLedTimer> m_ledTimers;

    int m_customButtonCount;
};

extern bool isQWS();
extern QCString makeChannel ( const char *str );
}
}

#endif
