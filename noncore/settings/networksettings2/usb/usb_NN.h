#ifndef USB_NETNODE_H
#define USB_NETNODE_H

#include "netnode.h"

class AUSB;

class USBNetNode : public ANetNode{

    Q_OBJECT

public:

    USBNetNode();
    virtual ~USBNetNode();

    virtual const QString pixmapName() 
      { return "Devices/usb"; }

    virtual const QString nodeName() 
      { return tr("USB Cable Connect"); }

    virtual const QString nodeDescription() ;

    virtual ANetNodeInstance * createInstance( void );

    virtual const char ** needs( void );
    virtual const char * provides( void );

    virtual bool generateProperFilesFor( ANetNodeInstance * NNI );
    virtual bool hasDataFor( const QString & S);
    virtual bool generateDeviceDataForCommonFile( 
        SystemFile & SF, long DevNr );

    virtual QString genNic( long nr );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );
};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
