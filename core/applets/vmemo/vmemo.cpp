/************************************************************************************
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ************************************************************************************/
// copyright 2002 Jeremy Cowgar <jc@cowgar.com>
// copyright 2002 and 2003  L.J.Potter <ljp@llornkcor.com>

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

extern "C" {
#include "adpcm.h"
}

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#include <errno.h>

typedef struct _waveheader {
  u_long  main_chunk; /* 'RIFF'  */
  u_long  length;     /* filelen */
  u_long  chunk_type; /* 'WAVE'  */
  u_long  sub_chunk;  /* 'fmt '                                               */
  u_long  sc_len;     /* length of sub_chunk, =16
                         (chunckSize) format len */
  u_short format;     /* should be 1 for PCM-code        (formatTag)          */

  u_short modus;      /* 1 Mono, 2 Stereo                (channels)           */
  u_long  sample_fq;  /* samples per second              (samplesPerSecond)   */
  u_long  byte_p_sec; /* avg bytes per second            (avgBytePerSecond)   */
  u_short byte_p_spl; /* samplesize; 1 or 2 bytes        (blockAlign)         */
  u_short bit_p_spl;  /* 8, 12 or 16 bit                 (bitsPerSample)      */

  u_long  data_chunk; /* 'data'      */

  u_long  data_length;/* samplecount */
} WaveHeader;

#define RIFF        0x46464952
#define WAVE        0x45564157
#define FMT         0x20746D66
#define DATA        0x61746164
#define PCM_CODE    1
#define WAVE_MONO   1
#define WAVE_STEREO 2

struct adpcm_state encoder_state;
//struct adpcm_state decoder_state;

#define WAVE_FORMAT_DVI_ADPCM (0x0011)
#define WAVE_FORMAT_PCM (0x0001)


#include "vmemo.h"

#include <opie2/otaskbarapplet.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>

#include <qpainter.h>
#include <qmessagebox.h>

int seq = 0;

/* XPM */
static char * vmemo_xpm[] = {
  "16 16 102 2",
  "   c None",
  ".  c #60636A",
  "+  c #6E6E72",
  "@  c #68696E",
  "#  c #4D525C",
  "$  c #6B6C70",
  "%  c #E3E3E8",
  "&  c #EEEEF2",
  "*  c #EAEAEF",
  "=  c #CACAD0",
  "-  c #474A51",
  ";  c #171819",
  ">  c #9B9B9F",
  ",  c #EBEBF0",
  "'  c #F4F4F7",
  ")  c #F1F1F5",
  "!  c #DEDEE4",
  "~  c #57575C",
  "{  c #010101",
  "]  c #A2A2A6",
  "^  c #747477",
  "/  c #B5B5B8",
  "(  c #AEAEB2",
  "_  c #69696D",
  ":  c #525256",
  "<  c #181C24",
  "[  c #97979B",
  "}  c #A7A7AC",
  "|  c #B0B0B4",
  "1  c #C8C8D1",
  "2  c #75757B",
  "3  c #46464A",
  "4  c #494A4F",
  "5  c #323234",
  "6  c #909095",
  "7  c #39393B",
  "8  c #757578",
  "9  c #87878E",
  "0  c #222224",
  "a  c #414144",
  "b  c #6A6A6E",
  "c  c #020C16",
  "d  c #6B6B6F",
  "e  c #68686D",
  "f  c #5B5B60",
  "g  c #8A8A8F",
  "h  c #6B6B6E",
  "i  c #ADADB2",
  "j  c #828289",
  "k  c #3E3E41",
  "l  c #CFCFD7",
  "m  c #4C4C50",
  "n  c #000000",
  "o  c #66666A",
  "p  c #505054",
  "q  c #838388",
  "r  c #A1A1A7",
  "s  c #A9A9AE",
  "t  c #A8A8B0",
  "u  c #5E5E63",
  "v  c #3A3A3E",
  "w  c #BDBDC6",
  "x  c #59595E",
  "y  c #76767C",
  "z  c #373738",
  "A  c #717174",
  "B  c #727278",
  "C  c #1C1C1E",
  "D  c #3C3C3F",
  "E  c #ADADB6",
  "F  c #54555A",
  "G  c #8B8C94",
  "H  c #5A5A5F",
  "I  c #BBBBC3",
  "J  c #C4C4CB",
  "K  c #909098",
  "L  c #737379",
  "M  c #343437",
  "N  c #8F8F98",
  "O  c #000407",
  "P  c #2D3137",
  "Q  c #B0B1BC",
  "R  c #3B3C40",
  "S  c #6E6E74",
  "T  c #95959C",
  "U  c #74747A",
  "V  c #1D1D1E",
  "W  c #91929A",
  "X  c #42444A",
  "Y  c #22282E",
  "Z  c #B0B2BC",
  "`  c #898A90",
  " . c #65656A",
  ".. c #999AA2",
  "+. c #52535A",
  "@. c #151B21",
  "#. c #515257",
  "$. c #B5B5BE",
  "%. c #616167",
  "&. c #1A1D22",
  "*. c #000713",
  "=. c #1F1F21",
  "                                ",
  "            . + @ #             ",
  "          $ % & * = -           ",
  "        ; > , ' ) ! ~           ",
  "        { ] ^ / ( _ :           ",
  "        < [ } | 1 2 3           ",
  "      4 5 6 7 8 9 0 a b c       ",
  "    d e f g h i j 3 k l m n     ",
  "      o p q r s t u v w n       ",
  "      o x y z A B C D E n       ",
  "      F G H I J K L M N O       ",
  "      P Q R S T U V W X         ",
  "        Y Z ` b  ...+.          ",
  "          @.#.$.%.&.            ",
  "            *.B =.              ",
  "      n n n n n n n n n         "};


using namespace Opie::Ui;
VMemo::VMemo( QWidget *parent, const char *_name )
  : QWidget( parent, _name ) {
  setFixedHeight( 18 );
  setFixedWidth( 14 );

  t_timer = new QTimer( this );
  connect( t_timer, SIGNAL( timeout() ), SLOT( timerBreak() ) );

    Config vmCfg("Vmemo");
    vmCfg.setGroup("Defaults");
    int toggleKey = setToggleButton(vmCfg.readNumEntry("toggleKey", -1));
    useADPCM = vmCfg.readBoolEntry("use_ADPCM", 0);

    odebug << "toggleKey " << toggleKey << "" << oendl;

//     if ( QFile::exists ( "/dev/sharp_buz" ) || QFile::exists ( "/dev/sharp_led" ))
//          systemZaurus=TRUE;
//     else
      systemZaurus = FALSE;

//    myChannel = new QCopChannel( "QPE/VMemo", this );
    myChannel = new QCopChannel( "QPE/VMemo", this );

    connect( myChannel, SIGNAL(received(const QCString&,const QByteArray&)),
             this, SLOT(receive(const QCString&,const QByteArray&)) );


    if( toggleKey != -1 ) {
                odebug << "Register key " << toggleKey << "" << oendl;
      QCopEnvelope e("QPE/Launcher", "keyRegister(int,QCString,QCString)");
      //           e << 4096; // Key_Escape
      //          e << Key_F5; //4148
      e << toggleKey;
      e << QCString("QPE/VMemo");
      e << QCString("toggleRecord()");
    }
    if(toggleKey == 1)
        usingIcon = TRUE;
    else
        usingIcon = FALSE;
//     if( vmCfg.readNumEntry("hideIcon",0) == 1)
	if (!usingIcon)
			hide();
    recording = FALSE;
    // }
}

VMemo::~VMemo() {
}

int VMemo::position()
{
    return 6;
}

void VMemo::receive( const QCString &msg, const QByteArray &data ) {
  QDataStream stream( data, IO_ReadOnly );

  if (msg == "toggleRecord()")  {
      if (recording) {
          fromToggle = TRUE;
          stopRecording();
      }  else {
          fromToggle = TRUE;
          startRecording();
      }
  }
}

void VMemo::paintEvent( QPaintEvent* ) {
  QPainter p(this);
  p.drawPixmap( 0, 1,( const char** )  vmemo_xpm );
}

void VMemo::mousePressEvent( QMouseEvent * /*me*/) {
  /*  No mousePress/mouseRelease recording on the iPAQ. The REC button on the iPAQ calls these functions
         mousePressEvent and mouseReleaseEvent with a NULL parameter.  */

//  if (!systemZaurus && me != NULL)
//        return;
//    }

  if(!recording)
       startRecording();
   else
       stopRecording();
}

void VMemo::mouseReleaseEvent( QMouseEvent * ) {
}

bool VMemo::startRecording() {
  Config config( "Vmemo" );
  config.setGroup( "System" );

  useAlerts = config.readBoolEntry("Alert",1);
  if(useAlerts) {

    msgLabel = new QLabel( 0, "alertLabel" );
    msgLabel->setText("<B><P><font size=+2>VMemo-Recording</font></B>");
    msgLabel->show();
  }

  odebug << "Start recording engines" << oendl;
  recording = TRUE;

  if (openDSP() == -1)  {
    recording = FALSE;
    return FALSE;
  }

  config.setGroup("Defaults");

  date = TimeString::dateString( QDateTime::currentDateTime(),false,true);
  date.replace(QRegExp("'"),"");
  date.replace(QRegExp(" "),"_");
  date.replace(QRegExp(":"),"-");
  date.replace(QRegExp(","),"");

  QString fName;
  config.setGroup( "System" );
  fName = QPEApplication::documentDir() ;
  fileName = config.readEntry("RecLocation", fName);

  int s;
  s=fileName.find(':');
  if(s)
    fileName=fileName.right(fileName.length()-s-2);
  odebug << "pathname will be "+fileName << oendl;

  if( fileName.left(1).find('/') == -1)
    fileName="/"+fileName;
  if( fileName.right(1).find('/') == -1)
    fileName+="/";
  fName = "vm_"+ date + ".wav";

  fileName+=fName;
  odebug << "filename is " + fileName << oendl;
// open tmp file here
  char *pointer;
  pointer=tmpnam(NULL);
  odebug << "Opening tmp file " << pointer << "" << oendl;

  if(openWAV(pointer ) == -1)  {

    QString err("Could not open the temp file\n");
    err += fileName;
    QMessageBox::critical(0, "vmemo", err, "Abort");
    ::close(dsp);
    return FALSE;
  }
  if( record() ) {

  QString cmd;
  if( fileName.find(".wav",0,TRUE) == -1)
      fileName += ".wav";

  cmd.sprintf("mv %s "+fileName, pointer);
// move tmp file to regular file here

  system(cmd.latin1());

  QArray<int> cats(1);
  cats[0] = config.readNumEntry("Category", 0);

  QString dlName("vm_");
  dlName += date;
  DocLnk l;
  l.setFile(fileName);
  l.setName(dlName);
  l.setType("audio/x-wav");
  l.setCategories(cats);
  l.writeLink();
  return TRUE;
  } else
      return FALSE;

}

void VMemo::stopRecording() {
//    show();
    odebug << "Stopped recording" << oendl;
    recording = FALSE;
    if(useAlerts) {
        msgLabel->close();
        msgLabel=0;
        delete msgLabel;
    }
    t_timer->stop();
    Config cfg("Vmemo");
    cfg.setGroup("Defaults");
//     if( cfg.readNumEntry("hideIcon",0) == 1 )
//         hide();
}

int VMemo::openDSP() {
  Config cfg("Vmemo");
  cfg.setGroup("Record");

  speed = cfg.readNumEntry("SampleRate", 22050);
  channels = cfg.readNumEntry("Stereo", 1) ? 2 : 1; // 1 = stereo(2), 0 = mono(1)
  if (cfg.readNumEntry("SixteenBit", 1)==1)  {
    format = AFMT_S16_LE;
    resolution = 16;
  } else {
    format = AFMT_U8;
    resolution = 8;
  }

  odebug << "samplerate: " << speed << ", channels " << channels << ", resolution " << resolution << "" << oendl;

  if(systemZaurus) {
    dsp = open("/dev/dsp1", O_RDONLY); //Zaurus needs /dev/dsp1
    channels=1; //zaurus has one input channel
  }  else {
#ifdef QT_QWS_DEVFS
    dsp = open("/dev/sound/dsp", O_RDONLY);
#else
    dsp = open("/dev/dsp", O_RDONLY);
#endif
  }

  if (dsp == -1)  {
      msgLabel->close();
      msgLabel=0;
      delete msgLabel;

      perror("open(\"/dev/dsp\")");
      errorMsg="open(\"/dev/dsp\")\n "+(QString)strerror(errno);
      QMessageBox::critical(0, "vmemo", errorMsg, "Abort");
      return -1;
  }

  if(ioctl(dsp, SNDCTL_DSP_SETFMT , &format)==-1)  {
    perror("ioctl(\"SNDCTL_DSP_SETFMT\")");
    return -1;
  }
  if(ioctl(dsp, SNDCTL_DSP_CHANNELS , &channels)==-1)  {
    perror("ioctl(\"SNDCTL_DSP_CHANNELS\")");
    return -1;
  }
  if(ioctl(dsp, SNDCTL_DSP_SPEED , &speed)==-1)  {
    perror("ioctl(\"SNDCTL_DSP_SPEED\")");
    return -1;
  }
  if(ioctl(dsp, SOUND_PCM_READ_RATE , &rate)==-1)  {
    perror("ioctl(\"SOUND_PCM_READ_RATE\")");
    return -1;
  }

  QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << FALSE; //mute

  return 1;
}

int VMemo::openWAV(const char *filename) {
  track.setName(filename);
  if(!track.open(IO_WriteOnly|IO_Truncate|IO_Raw)) {
    errorMsg=filename;
    return -1;
  }

  wav=track.handle();
    Config vmCfg("Vmemo");
    vmCfg.setGroup("Defaults");
  useADPCM = vmCfg.readBoolEntry("use_ADPCM", 0);

  WaveHeader wh;

  wh.main_chunk = RIFF;
  wh.length=0;
  wh.chunk_type = WAVE;
  wh.sub_chunk  = FMT;
  wh.sc_len     = 16;
  if(useADPCM)
      wh.format     = WAVE_FORMAT_DVI_ADPCM;//PCM_CODE;
  else
      wh.format     = PCM_CODE;
  wh.modus      = channels;
  wh.sample_fq  = speed;
  wh.byte_p_sec = speed * channels *  resolution/8;
  wh.byte_p_spl = channels * (resolution / 8);
  wh.bit_p_spl  = resolution;
  wh.data_chunk = DATA;
  wh.data_length= 0;
  //    odebug << "Write header channels " << wh.modus << ", speed " << wh.sample_fq << ", b/s "
  //           << wh.byte_p_sec << ", blockalign " << wh.byte_p_spl << ", bitrate " << wh.bit_p_spl << oendl;
  write (wav, &wh, sizeof(WaveHeader));

  return 1;
}

bool VMemo::record() {
        length = 0;
				int bytesWritten = 0;
        int result = 0;
        int value = 0;

        QString msg;
        msg.sprintf("Recording format %d", format);
        odebug << msg << oendl;

        Config config("Vmemo");
        config.setGroup("Record");
        int sRate = config.readNumEntry("SizeLimit", 30);
				odebug << "VMEMO rate" << sRate << oendl;

         if(sRate > 0) {
 						t_timer->start( sRate * 1000+1000, TRUE);
 				}

     msg.sprintf("Recording format other");
     odebug << msg << oendl;

    config.setGroup("Defaults");
    useADPCM = config.readBoolEntry("use_ADPCM", 0);

    int bufsize = config.readNumEntry("BufferSize",1024);
    unsigned short sound[bufsize]; //, monoBuffer[bufsize];
    char abuf[bufsize / 2];
    short sbuf[bufsize];
		odebug << "ready to record"<< oendl;
        if(useADPCM) {
						odebug << "usr ADPCM" << oendl;

                while(recording) {
                        result = ::read(dsp, sbuf, bufsize); // adpcm read
                        if( result <= 0) {
                                perror("recording error ");
                                QMessageBox::message(tr("Note"),tr("error recording"));
                                recording = FALSE;
                                break;
                                return FALSE;
                        }
                        adpcm_coder( sbuf, abuf, result/2, &encoder_state);
                        bytesWritten = ::write(wav, abuf, result/4); // adpcm write
                        length += bytesWritten;

                        if(length < 0) {
                                recording = false;
                                perror("dev/dsp's is a lookin' messy");
                                QMessageBox::message("Vmemo","Error writing to file\n"+ fileName);
                                break;
                                return FALSE;
                        }
                                   printf("%d\r", length);
                                   fflush(stdout);
                        qApp->processEvents();
                }
        } else {
						odebug << "use regular wav" << oendl;
                while(recording) {
                        result = ::read(dsp, sound, bufsize); // read
                        if( result <= 0) {
                                perror("recording error ");
                                QMessageBox::message(tr("Note"),tr("error recording"));
                                recording = FALSE;
                                break;
                                return FALSE;
												}

                                bytesWritten = ::write(wav, sound, result); // write
                                length += bytesWritten;

                                if(length < 0) {
                                        recording = false;
                                        perror("dev/dsp's is a lookin' messy");
                                        QMessageBox::message("Vmemo","Error writing to file\n"+ fileName);
                                        break;
                                        return FALSE;
                                }
//                                            printf("%d\r", length);
//                                            fflush(stdout);
                                qApp->processEvents();
                        }
//												odebug << "result is " << result << oendl;
                }
				odebug << "file has length of " << length << " lasting " << (( length / speed) / channels) / 2  << " seconds" << oendl;

        value = length + 36;

        lseek(wav, 4, SEEK_SET);
        write(wav, &value, 4);
        lseek(wav, 40, SEEK_SET);

        write(wav, &length, 4);

        track.close();
        odebug << "Track closed" << oendl;

        if( ioctl( dsp, SNDCTL_DSP_RESET,0) == -1)
                perror("ioctl(\"SNDCTL_DSP_RESET\")");

        ::close(dsp);

        Config cfgO("OpieRec");
        cfgO.setGroup("Sounds");

        int nFiles = cfgO.readNumEntry( "NumberofFiles",0);

        QString currentFileName = fileName;
        QString currentFile = "vm_"+ date;

        float numberOfRecordedSeconds = (float) length / (float)speed * (float)2;

        cfgO.writeEntry( "NumberofFiles", nFiles + 1);
        cfgO.writeEntry( QString::number( nFiles + 1), currentFile);
        cfgO.writeEntry( currentFile, currentFileName);

        QString time;
        time.sprintf("%.2f", numberOfRecordedSeconds);
        cfgO.writeEntry( currentFileName, time );
      //      odebug << "writing config numberOfRecordedSeconds "+time << oendl;

        cfgO.write();

        odebug << "done recording "+fileName << oendl;

        Config cfg("qpe");
        cfg.setGroup("Volume");
        QString foo = cfg.readEntry("Mute","TRUE");
        if(foo.find("TRUE",0,TRUE) != -1)
                QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << TRUE; //mute
        return TRUE;
}

int VMemo::setToggleButton(int tog) {

  for( int i=0; i < 10;i++) {
    switch (tog) {
    case 0:
      return -1;
      break;
    case 1:
      return 0;
      break;
    case 2:
        return Key_F24; //was Escape
      break;
    case 3:
      return Key_Space;
      break;
    case 4:
      return Key_F12;
      break;
    case 5:
      return Key_F9;
      break;
    case 6:
      return Key_F10;
      break;
    case 7:
      return Key_F11;
      break;
    case 8:
      return Key_F13;
      break;
    };
  }
  return -1;
}

void VMemo::timerBreak() {
  //stop
    stopRecording();
  QMessageBox::message("Vmemo","Vmemo recording has ended");
}


//EXPORT_OPIE_APPLET_v1( VMemo )

