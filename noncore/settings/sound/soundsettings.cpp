/**********************************************************************
 ** Copyright (C) 2000 Trolltech AS.  All rights reserved.
 **
 ** This file is part of Qtopia Environment.
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
// parts copyright 2002 L.J. Potter

#include "soundsettings.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/storage.h>
using namespace Opie::Core;

/* QT */
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>

/* STD */
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>


SoundSettings::SoundSettings( QWidget* parent,  const char* objname, WFlags fl )
        : SoundSettingsBase( parent, objname, true, fl )
{
    keyReset=false;
    noWarning=false;
    Config config( "qpe");
    config.setGroup( "Volume" );
    Config cfg("Vmemo");

 
//		connect(qApp,SIGNAL(aboutToQuit()),SLOT(cleanUp()));
		
    AlertCheckBox->setChecked( cfg.readBoolEntry("Alert", 1));
    
    cfg.setGroup("Record");
    int rate=config.readNumEntry("SampleRate", 22050);
    if(rate == 8000)
        sampleRate->setCurrentItem(0);
    else if(rate == 11025)
        sampleRate->setCurrentItem(1);
    else if(rate == 22050)
        sampleRate->setCurrentItem(2);
    else if(rate == 33075)
        sampleRate->setCurrentItem(3);
    else if(rate==44100)
        sampleRate->setCurrentItem(4);

    stereoCheckBox->setChecked(cfg.readNumEntry("Stereo", 0));
//TODO hide if zaurus- mono only

#if defined(QT_QWS_IPAQ) || defined(QT_QWS_SL5XXX)
//since ipaq and zaurus have particular
//devices
    bool systemZaurus=false;
    struct utsname name; /* check for embedix kernel running on the zaurus*/
    if (uname(&name) != -1) {// TODO change this here,...
        QString release=name.release;
        if( release.find("embedix",0,true) != -1) {
            odebug << "IS System Zaurus" << oendl; 
            systemZaurus=true;
        }
    }
    if(!systemZaurus) {
        stereoCheckBox->setChecked(true);
    }
    stereoCheckBox->setEnabled(false);
    sixteenBitCheckBox->setEnabled(false);
#else
#endif
    int sRate=cfg.readNumEntry("SizeLimit", 30);
    odebug << "" << sRate << "" << oendl; 

    if(sRate == 30)
        timeLimitComboBox->setCurrentItem(0);
    else if(sRate == 20)
        timeLimitComboBox->setCurrentItem(1);
    else if(sRate == 15)
        timeLimitComboBox->setCurrentItem(2);
    else if(sRate == 10)
        timeLimitComboBox->setCurrentItem(3);
    else if(sRate == 5)
        timeLimitComboBox->setCurrentItem(4);
    else 
        timeLimitComboBox->setCurrentItem(5);

    sixteenBitCheckBox->setChecked(cfg.readNumEntry("SixteenBit", 1));

    cfg.setGroup("Defaults");
		recordKey = cfg.readNumEntry("toggleKey");
    keyComboBox->setCurrentItem( recordKey);

    updateStorageCombo();

    Config vmCfg("Vmemo");
    vmCfg.setGroup("Defaults");
    adpcmCheckBox->setChecked( vmCfg.readBoolEntry("use_ADPCM", 0));

    connect(LocationComboBox,SIGNAL(activated(const QString&)),this,SLOT(setLocation(const QString&)));
    connect(keyComboBox,SIGNAL(activated(int)),this,SLOT(setKeyButton(int)));
    connect(timeLimitComboBox,SIGNAL(activated(const QString&)),this,SLOT(setSizeLimitButton(const QString&)));
    connect(restartCheckBox,SIGNAL( toggled(bool)),this,SLOT(restartOpie(bool)));
    connect(adpcmCheckBox,SIGNAL( toggled(bool)),this,SLOT(slotAdpcm(bool)));

   //     connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );
}

void SoundSettings::updateStorageCombo() {

    Config config( "Vmemo" );
    config.setGroup( "System" );
    QString loc = config.readEntry("RecLocation","/");
    int i = 0;
    int set = 0; 
    StorageInfo storageInfo;
    QString sName, sPath;
    QStringList list;
    list << "Documents : "+QPEApplication::documentDir();
    list << "tmp : /tmp";

    const QList<FileSystem> &fs = storageInfo.fileSystems();
    QListIterator<FileSystem> it ( fs );
    for( ; it.current(); ++it ){
        const QString name = (*it)->name();
        const QString path = (*it)->path();
        odebug << "storage name "+name +" storage path is "+path << oendl; 
        list << name + ": " +path;
        if( loc.find( path,0,true) != -1)
            set = i;      
//   if(dit.current()->file().find(path) != -1 ) storage=name;
        i++;
    }

    LocationComboBox->insertStringList(list);
    odebug << "set item " << set << "" << oendl; 
    LocationComboBox->setCurrentItem(set);
}

void SoundSettings::setLocation(const QString & string) {
    Config config( "Vmemo" );
    config.setGroup( "System" );
    config.writeEntry("RecLocation",string);
    odebug << "set location "+string << oendl; 
    config.write();
}

 void SoundSettings::accept() {
		 cleanUp();
		 qApp->quit();
}


void SoundSettings::cleanUp() {

    Config cfg("Vmemo");
    cfg.writeEntry("Alert",AlertCheckBox->isChecked());

    cfg.setGroup("Record");
    cfg.writeEntry("SampleRate",sampleRate->currentText());
    cfg.writeEntry("Stereo",stereoCheckBox->isChecked());
    cfg.writeEntry("SixteenBit",sixteenBitCheckBox->isChecked());
    if(keyReset && noWarning) {
        QCopEnvelope ("QPE/System", "restart()");
    }
    cfg.setGroup("Defaults");
    cfg.writeEntry( "toggleKey", recordKey);
    if( recordKey == 1) {
        cfg.writeEntry( "hideIcon", 0 );
    }
    else {
        cfg.writeEntry( "hideIcon", 1);
    }
    cfg.write();
}

void SoundSettings::setKeyButton( int index) {
		recordKey = index;
		restartCheckBox->setChecked(true);
    keyReset = true;
    if( index == 1) {
        keyLabel->setText(tr("Shows icon"));
    }
    else {
        keyLabel->setText(tr("Hides icon"));
    }
}

void SoundSettings::updateLocationCombo() {

}

void SoundSettings::setSizeLimitButton(const QString &index) {

    Config cfg("Vmemo");
    cfg.setGroup("Record");
    if(index.find("Unlimited",0,true) != -1)
        cfg.writeEntry("SizeLimit", -1);
    else
        cfg.writeEntry("SizeLimit", index);
    cfg.write();    
}

void SoundSettings::restartOpie(bool b) {
    noWarning=b;
}

void SoundSettings::slotAdpcm(bool b) {
    Config vmCfg("Vmemo");
    vmCfg.setGroup("Defaults");
    vmCfg.writeEntry("use_ADPCM", b);
    vmCfg.write();
}
