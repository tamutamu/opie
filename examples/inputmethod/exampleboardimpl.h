#ifndef EXAMPLEBOARDIMPL_H
#define EXAMPLEBOARDIMPL_H

#include <qhbox.h>

#include <qpe/inputmethodinterface.h>

class QPixmap;
class QCheckBox;
class ExampleBoard : public QHBox {
	Q_OBJECT
public:
	ExampleBoard( QWidget *par, WFlags f );
	~ExampleBoard();
	void resetState();
private slots:
	void slotKey(int);
	void slotShift(bool);
	void slotAlt(bool);
	void slotCtrl(bool);
signals:
	void key(ushort,ushort,ushort,bool,bool);
private:
	int m_state;
	QCheckBox *m_alt,*m_shi,*m_ctrl;
};

class ExampleboardImpl : public InputMethodInterface
{
public:
    ExampleboardImpl();
    virtual ~ExampleboardImpl();

#ifndef QT_NO_COMPONENT
    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT
#endif

    virtual QWidget *inputMethod( QWidget *parent, Qt::WFlags f );
    virtual void resetState();
    virtual QPixmap *icon();
    virtual QString name();
    virtual void onKeyPress( QObject *receiver, const char *slot );

private:
    ExampleBoard *m_pickboard;
    QPixmap *m_icn;
};

#endif
