
#include "accountitem.h"
#include "accountview.h"
#include "newmaildir.h"
#include "nntpgroupsdlg.h"
#include "defines.h"

#include <libmailwrapper/mailtypes.h>
#include <libmailwrapper/abstractmail.h>
#include <libmailwrapper/mailwrapper.h>

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;

/* QT */
#include <qpopupmenu.h>
#include <qmessagebox.h>

#define SETPIX(x) if (!account->getOffline()) {setPixmap( 0,x);} else {setPixmap( 0, PIXMAP_OFFLINE );}
/**
 * POP3 Account stuff
 */
POP3viewItem::POP3viewItem( POP3account *a, AccountView *parent )
        : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    SETPIX(PIXMAP_POP3FOLDER);
#if 0
    if (!account->getOffline())
    {
        setPixmap( 0,  );
    }
    else
    {
        setPixmap( 0, PIXMAP_OFFLINE );
    }
#endif
    setText( 0, account->getAccountName() );
    setOpen( true );
}

POP3viewItem::~POP3viewItem()
{
    delete wrapper;
}

AbstractMail *POP3viewItem::getWrapper()
{
    return wrapper;
}

void POP3viewItem::refresh(QValueList<Opie::Core::OSmartPointer<RecMail> > & )
{
    refresh();
}

void POP3viewItem::refresh()
{
    if (account->getOffline()) return;
    QValueList<FolderP> *folders = wrapper->listFolders();
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    QValueList<FolderP>::ConstIterator it;
    QListViewItem*item = 0;
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        item = new POP3folderItem( (*it), this , item );
        item->setSelectable( (*it)->may_select());
    }
    delete folders;
}

RECBODYP POP3viewItem::fetchBody( const RecMailP &mail )
{
    odebug << "POP3 fetchBody" << oendl;
    return wrapper->fetchBody( mail );
}

QMap<int,QString> POP3viewItem::serverMenu()
{
    QMap<int,QString> _entries;
    if (!account->getOffline()) {
        _entries[SERVER_MENU_DISCONNECT]=QObject::tr("Disconnect",contextName);
        _entries[SERVER_MENU_OFFLINE]=QObject::tr("Set offline",contextName);
    } else {
        _entries[SERVER_MENU_OFFLINE]=QObject::tr("Set online",contextName);
    }
    return _entries;
}

void POP3viewItem::disconnect()
{
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    wrapper->logout();
}

void POP3viewItem::setOnOffline()
{
    if (!account->getOffline())
    {
        disconnect();
    }
    account->setOffline(!account->getOffline());
    account->save();
    SETPIX(PIXMAP_POP3FOLDER);
    refresh();
}

bool POP3viewItem::contextMenuSelected(int which)
{
    switch (which)
    {
    case SERVER_MENU_DISCONNECT:
        disconnect();
        break;
    case SERVER_MENU_OFFLINE:
        setOnOffline();
        break;
    }
    return false;
}

POP3folderItem::~POP3folderItem()
{}

POP3folderItem::POP3folderItem( const FolderP&folderInit, POP3viewItem *parent , QListViewItem*after  )
        : AccountViewItem(folderInit,parent,after )
{
    pop3 = parent;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_POP3FOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

void POP3folderItem::refresh(QValueList<RecMailP>&target)
{
    if (folder->may_select())
        pop3->getWrapper()->listMessages( folder->getName(),target );
}

RECBODYP POP3folderItem::fetchBody(const RecMailP&aMail)
{
    return pop3->getWrapper()->fetchBody(aMail);
}

QMap<int,QString> POP3folderItem::folderMenu()
{
    QMap<int,QString> _entries;
    _entries[FOLDER_MENU_REFRESH_HEADER]=QObject::tr("Refresh header list",contextName);
    _entries[FOLDER_MENU_DELETE_ALL_MAILS]=QObject::tr("Delete all mails",contextName);
    _entries[FOLDER_MENU_MOVE_MAILS]=QObject::tr("Move/Copie all mails",contextName);
    return _entries;
}

void POP3folderItem::downloadMails()
{
    AccountView*bl = pop3->accountView();
    if (!bl) return;
    bl->downloadMails(folder,pop3->getWrapper());
}

bool POP3folderItem::contextMenuSelected(int which)
{
    AccountView * view = (AccountView*)listView();
    switch (which)
    {
    case FOLDER_MENU_REFRESH_HEADER:
        /* must be 'cause pop3 lists are cached */
        pop3->getWrapper()->logout();
        view->refreshCurrent();
        break;
    case FOLDER_MENU_DELETE_ALL_MAILS:
        deleteAllMail(pop3->getWrapper(),folder);
        break;
    case FOLDER_MENU_MOVE_MAILS:
        downloadMails();
        break;
    default:
        break;
    }
    return false;
}

/**
 * NNTP Account stuff
 */
NNTPviewItem::NNTPviewItem( NNTPaccount *a, AccountView *parent )
        : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    //FIXME
    SETPIX(PIXMAP_POP3FOLDER);
#if 0
    if (!account->getOffline())
    {
        setPixmap( 0,  );
    }
    else
    {
        setPixmap( 0, PIXMAP_OFFLINE );
    }
#endif
    setText( 0, account->getAccountName() );
    setOpen( true );
}

NNTPviewItem::~NNTPviewItem()
{
    delete wrapper;
}

AbstractMail *NNTPviewItem::getWrapper()
{
    return wrapper;
}

void NNTPviewItem::refresh( QValueList<RecMailP> & )
{
    refresh();
}

void NNTPviewItem::refresh()
{
    if (account->getOffline()) return;
    QValueList<FolderP> *folders = wrapper->listFolders();

    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    QValueList<FolderP>::ConstIterator it;
    QListViewItem*item = 0;
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        item = new NNTPfolderItem( (*it), this , item );
        item->setSelectable( (*it)->may_select());
    }
    delete folders;
}

RECBODYP NNTPviewItem::fetchBody( const RecMailP &mail )
{
    odebug << "NNTP fetchBody" << oendl;
    return wrapper->fetchBody( mail );
}

QMap<int,QString> NNTPviewItem::serverMenu()
{
    QMap<int,QString> _entries;
    if (!account->getOffline())
    {
        _entries[SERVER_MENU_DISCONNECT]=QObject::tr("Disconnect",contextName);
        _entries[SERVER_MENU_OFFLINE]=QObject::tr("Set offline",contextName);
        _entries[SERVER_MENU_SUBSCRIBE]=QObject::tr("(Un-)Subscribe groups",contextName);
    } else {
        _entries[SERVER_MENU_OFFLINE]=QObject::tr("Set online",contextName);
    }
    return _entries;
}

void NNTPviewItem::subscribeGroups()
{
    NNTPGroupsDlg dlg(account);
    if (QPEApplication::execDialog(&dlg)== QDialog::Accepted ){
        refresh();
    }
}

void NNTPviewItem::disconnect()
{
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
    wrapper->logout();
}

void NNTPviewItem::setOnOffline()
{
    if (!account->getOffline())
    {
        disconnect();
    }
    account->setOffline(!account->getOffline());
    account->save();
   //FIXME
    SETPIX(PIXMAP_POP3FOLDER);
    refresh();
}

bool NNTPviewItem::contextMenuSelected(int which)
{
    switch (which)
    {
    case SERVER_MENU_DISCONNECT:
        disconnect();
        break;
    case SERVER_MENU_OFFLINE:
        setOnOffline();
        break;
    case SERVER_MENU_SUBSCRIBE:
        subscribeGroups();
        break;
    }
    return false;
}

NNTPfolderItem::~NNTPfolderItem()
{}

NNTPfolderItem::NNTPfolderItem( const FolderP &folderInit, NNTPviewItem *parent , QListViewItem*after  )
        : AccountViewItem( folderInit, parent,after )
{
    nntp = parent;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_POP3FOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

void NNTPfolderItem::refresh(QValueList<RecMailP>&target)
{
    if (folder->may_select())
        nntp->getWrapper()->listMessages( folder->getName(),target );
}

RECBODYP NNTPfolderItem::fetchBody(const RecMailP&aMail)
{
    return nntp->getWrapper()->fetchBody(aMail);
}

QMap<int,QString> NNTPfolderItem::folderMenu()
{
    QMap<int,QString> _entries;
    _entries[FOLDER_MENU_REFRESH_HEADER]=QObject::tr("Refresh header list",contextName);
    _entries[FOLDER_MENU_MOVE_MAILS]=QObject::tr("Copy all postings",contextName);
    return _entries;
}

void NNTPfolderItem::downloadMails()
{
    AccountView*bl = nntp->accountView();
    if (!bl) return;
    bl->downloadMails(folder,nntp->getWrapper());
}

bool NNTPfolderItem::contextMenuSelected(int which)
{
    AccountView * view = (AccountView*)listView();
    switch (which)
    {
    case FOLDER_MENU_REFRESH_HEADER:
        /* must be 'cause nntp lists are cached */
        nntp->getWrapper()->logout();
        view->refreshCurrent();
        break;
    case FOLDER_MENU_MOVE_MAILS:
        downloadMails();
        break;
    default:
        break;
    }
    return false;
}

/**
 * IMAP Account stuff
 */
IMAPviewItem::IMAPviewItem( IMAPaccount *a, AccountView *parent )
        : AccountViewItem( parent )
{
    account = a;
    wrapper = AbstractMail::getWrapper( account );
    SETPIX(PIXMAP_IMAPFOLDER);
    setText( 0, account->getAccountName() );
    setOpen( true );
}

IMAPviewItem::~IMAPviewItem()
{
    delete wrapper;
}

AbstractMail *IMAPviewItem::getWrapper()
{
    return wrapper;
}

void IMAPviewItem::refresh(QValueList<RecMailP>&)
{
    refreshFolders(false);
}

const QStringList&IMAPviewItem::subFolders()
{
    return currentFolders;
}

void IMAPviewItem::refreshFolders(bool force)
{
    if (childCount()>0 && force==false) return;
    if (account->getOffline()) return;

    removeChilds();
    currentFolders.clear();
    QValueList<FolderP> * folders = wrapper->listFolders();

    QValueList<FolderP>::Iterator it;
    QListViewItem*item = 0;
    QListViewItem*titem = 0;
    QString fname,del,search;
    int pos;

    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        if ((*it)->getDisplayName().lower()=="inbox")
        {
            item = new IMAPfolderItem( (*it), this , item );
            folders->remove(it);
            odebug << "inbox found" << oendl;
            break;
        }
    }
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        fname = (*it)->getDisplayName();
        currentFolders.append((*it)->getName());
        pos = fname.findRev((*it)->Separator());
        if (pos != -1)
        {
            fname = fname.left(pos);
        }
        IMAPfolderItem*pitem = (IMAPfolderItem*)findSubItem(fname);
        if (pitem)
        {
            titem = item;
            item = new IMAPfolderItem( (*it),pitem,pitem->firstChild(),this);
            /* setup the short name */
            item->setText(0,(*it)->getDisplayName().right((*it)->getDisplayName().length()-pos-1));
            item = titem;
        }
        else
        {
            item = new IMAPfolderItem( (*it), this , item );
        }
    }
    delete folders;
}

QMap<int,QString> IMAPviewItem::serverMenu()
{
    QMap<int,QString> e;
    if (!account->getOffline()) {
        e[SERVER_MENU_DISCONNECT]=QObject::tr("Disconnect",contextName);
        e[SERVER_MENU_OFFLINE]=QObject::tr("Set offline",contextName);
        e[SERVER_MENU_REFRESH_FOLDER]=QObject::tr("Refresh folder list",contextName);
        e[SERVER_MENU_CREATE_FOLDER]=QObject::tr("Create new folder",contextName);
    } else {
        e[SERVER_MENU_OFFLINE]=QObject::tr("Set online",contextName);
    }
    return e;
}

void IMAPviewItem::createNewFolder()
{
    Newmdirdlg ndirdlg;
    if ( QPEApplication::execDialog( &ndirdlg ))
    {
        QString ndir = ndirdlg.Newdir();
        bool makesubs = ndirdlg.subpossible();
        QString delemiter = "/";
        IMAPfolderItem*item = (IMAPfolderItem*)firstChild();
        if (item)
        {
            delemiter = item->Delemiter();
        }
        if (wrapper->createMbox(ndir,0,delemiter,makesubs))
        {
            refreshFolders(true);
        }
    }
}

bool IMAPviewItem::contextMenuSelected(int id)
{
    odebug << "Id selected: " << id << "" << oendl;
    switch (id)
    {
    case SERVER_MENU_REFRESH_FOLDER:
        refreshFolders(true);
        break;
    case SERVER_MENU_CREATE_FOLDER:
        createNewFolder();
        break;
    case SERVER_MENU_DISCONNECT:
        removeChilds();
        wrapper->logout();
        break;
    case SERVER_MENU_OFFLINE:
        if (account->getOffline()==false)
        {
            removeChilds();
            wrapper->logout();
        }
        account->setOffline(!account->getOffline());
        account->save();
        SETPIX(PIXMAP_IMAPFOLDER);
        refreshFolders(false);
        break;
    default:
        break;
    }
    return false;
}

RECBODYP IMAPviewItem::fetchBody(const RecMailP&)
{
    return new RecBody();
}

bool IMAPviewItem::offline()
{
    return account->getOffline();
}

IMAPfolderItem::IMAPfolderItem( const FolderP& folderInit, IMAPviewItem *parent , QListViewItem*after )
        : AccountViewItem( folderInit, parent , after )
{
    imap = parent;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_IMAPFOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

IMAPfolderItem::IMAPfolderItem(const FolderP &folderInit, IMAPfolderItem *parent , QListViewItem*after, IMAPviewItem *master  )
        : AccountViewItem(folderInit, parent,after )
{
    imap = master;
    if (folder->getDisplayName().lower()!="inbox")
    {
        setPixmap( 0, PIXMAP_IMAPFOLDER );
    }
    else
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    }
    setText( 0, folder->getDisplayName() );
}

IMAPfolderItem::~IMAPfolderItem()
{}

const QString& IMAPfolderItem::Delemiter()const
{
    return folder->Separator();
}

void IMAPfolderItem::refresh(QValueList<RecMailP>&target)
{
    if (folder->may_select())
    {
        imap->getWrapper()->listMessages( folder->getName(),target );
    }
    else
    {
        target.clear();
    }
}

RECBODYP IMAPfolderItem::fetchBody(const RecMailP&aMail)
{
    return imap->getWrapper()->fetchBody(aMail);
}

QMap<int,QString> IMAPfolderItem::folderMenu()
{
    QMap<int,QString> e;
    if (folder->may_select()) {
        e[FOLDER_MENU_REFRESH_HEADER]=QObject::tr("Refresh header list",contextName);
        e[FOLDER_MENU_MOVE_MAILS]=QObject::tr("Move/Copy all mails",contextName);
        e[FOLDER_MENU_DELETE_ALL_MAILS]=QObject::tr("Delete all mails",contextName);
    }
    if (folder->no_inferior()==false) {
        e[FOLDER_MENU_NEW_SUBFOLDER]=QObject::tr("Create new subfolder",contextName);
    }
    if (folder->getDisplayName().lower()!="inbox") {
        e[FOLDER_MENU_DELETE_FOLDER]=QObject::tr("Delete folder",contextName);
    }
    return e;
}

void IMAPfolderItem::createNewFolder()
{
    Newmdirdlg ndirdlg;
    if ( QPEApplication::execDialog( &ndirdlg ) )
    {
        QString ndir = ndirdlg.Newdir();
        bool makesubs = ndirdlg.subpossible();
        QString delemiter = Delemiter();
        if (imap->wrapper->createMbox(ndir,folder,delemiter,makesubs))
        {
            imap->refreshFolders(true);
        }
    }
}

bool IMAPfolderItem::deleteFolder()
{
    int yesno = QMessageBox::warning(0,QObject::tr("Delete folder",contextName),
                                     QObject::tr("<center>Realy delete folder <br><b>%1</b><br>and all if it content?</center>",contextName).arg(folder->getDisplayName()),
                                     QObject::tr("Yes",contextName),
                                     QObject::tr("No",contextName),QString::null,1,1);
    odebug << "Auswahl: " << yesno << "" << oendl;
    if (yesno == 0)
    {
        if (imap->getWrapper()->deleteMbox(folder))
        {
            QListView*v=listView();
            IMAPviewItem * box = imap;
            /* be carefull - after that this object is destroyd so don't use
             * any member of it after that call!!*/
            imap->refreshFolders(true);
            if (v)
            {
                v->setSelected(box,true);
            }
            return true;
        }
    }
    return false;
}

void IMAPfolderItem::downloadMails()
{
    AccountView*bl = imap->accountView();
    if (!bl) return;
    bl->downloadMails(folder,imap->getWrapper());
}

bool IMAPfolderItem::contextMenuSelected(int id)
{
    odebug << "Selected id: " << id << "" << oendl;
    AccountView * view = (AccountView*)listView();
    switch(id)
    {
    case FOLDER_MENU_REFRESH_HEADER:
        view->refreshCurrent();
        break;
    case FOLDER_MENU_DELETE_ALL_MAILS:
        deleteAllMail(imap->getWrapper(),folder);
        break;
    case FOLDER_MENU_NEW_SUBFOLDER:
        if (folder->no_inferior()==false) {
            createNewFolder();
        }
        break;
    case FOLDER_MENU_DELETE_FOLDER:
        if (folder->getDisplayName().lower()!="inbox") {
            return deleteFolder();
        }
        break;
    case FOLDER_MENU_MOVE_MAILS:
        downloadMails();
        break;
    default:
        break;
    }
    return false;
}

/**
 * MH Account stuff
 */
/* MH is a little bit different - the top folder can contains messages other than in IMAP and
   POP3 and MBOX */
MHviewItem::MHviewItem( const QString&aPath, AccountView *parent )
        : AccountViewItem( parent )
{
    m_Path = aPath;
    /* be carefull - the space within settext is wanted - thats why the string twice */
    wrapper = AbstractMail::getWrapper( m_Path,"Local Folders");
    setPixmap( 0, PIXMAP_LOCALFOLDER );
    setText( 0, " Local Folders" );
    setOpen( true );
    folder = 0;
}

MHviewItem::~MHviewItem()
{
    delete wrapper;
}

AbstractMail *MHviewItem::getWrapper()
{
    return wrapper;
}

void MHviewItem::refresh( QValueList<RecMailP> & target)
{
    refresh(false);
    getWrapper()->listMessages( "",target );
}

void MHviewItem::refresh(bool force)
{
    if (childCount()>0 && force==false) return;
    odebug << "Refresh mh folders" << oendl;
    removeChilds();
    currentFolders.clear();
    QValueList<FolderP> *folders = wrapper->listFolders();
    QValueList<FolderP>::ConstIterator it;
    MHfolderItem*item = 0;
    MHfolderItem*pmaster = 0;
    QString fname = "";
    int pos;
    for ( it = folders->begin(); it!=folders->end(); ++it)
    {
        fname = (*it)->getDisplayName();
        /* this folder itself */
        if (fname=="/")
        {
            currentFolders.append(fname);
            folder = (*it);
            continue;
        }
        currentFolders.append(fname);
        pos = fname.findRev("/");
        if (pos > 0)
        {
            fname = fname.left(pos);
            pmaster = (MHfolderItem*)findSubItem(fname);
        }
        else
        {
            pmaster = 0;
        }
        if (pmaster)
        {
            item = new MHfolderItem( (*it), pmaster, item, this );
        }
        else
        {
            item = new MHfolderItem( (*it), this , item );
        }
        item->setSelectable((*it)->may_select());
    }
    delete folders;
}

RECBODYP MHviewItem::fetchBody( const RecMailP &mail )
{
    odebug << "MH fetchBody" << oendl;
    return wrapper->fetchBody( mail );
}

QMap<int,QString> MHviewItem::serverMenu()
{
    QMap<int,QString> e;
    e[SERVER_MENU_REFRESH_FOLDER]=QObject::tr("Refresh folder list",contextName);
    return e;
}

QMap<int,QString> MHviewItem::folderMenu()
{
    QMap<int,QString> e;
    e[FOLDER_MENU_REFRESH_HEADER]=QObject::tr("Refresh header list",contextName);
    e[FOLDER_MENU_NEW_SUBFOLDER]=QObject::tr("Create new folder",contextName);
    e[FOLDER_MENU_DELETE_ALL_MAILS]=QObject::tr("Delete all mails",contextName);
    e[FOLDER_MENU_MOVE_MAILS]=QObject::tr("Move/Copie all mails",contextName);
    return e;
}

void MHviewItem::createFolder()
{
    Newmdirdlg ndirdlg(0,0,true);
    if ( QPEApplication::execDialog( &ndirdlg ) )
    {
        QString ndir = ndirdlg.Newdir();
        if (wrapper->createMbox(ndir))
        {
            refresh(true);
        }
    }
}

void MHviewItem::downloadMails()
{
    AccountView*bl = accountView();
    if (!bl) return;
    bl->downloadMails(folder,getWrapper());
}

QStringList MHviewItem::subFolders()
{
    return currentFolders;
}

bool MHviewItem::contextMenuSelected(int which)
{
    AccountView*view = 0;

    switch (which)
    {
    case SERVER_MENU_REFRESH_FOLDER:
        refresh(true);
        break;
    case FOLDER_MENU_NEW_SUBFOLDER:
        createFolder();
        break;
    case FOLDER_MENU_DELETE_ALL_MAILS:
        deleteAllMail(getWrapper(),folder);
        break;
    case FOLDER_MENU_MOVE_MAILS:
        downloadMails();
        break;
    case FOLDER_MENU_REFRESH_HEADER:
        view = (AccountView*)listView();
        if (view) view->refreshCurrent();
        break;
    default:
        break;
    }
    return false;
}

MHfolderItem::~MHfolderItem()
{}

MHfolderItem::MHfolderItem( const FolderP &folderInit, MHviewItem *parent , QListViewItem*after  )
        : AccountViewItem(folderInit, parent,after )
{
    mbox = parent;
    initName();
}

MHfolderItem::MHfolderItem(const FolderP& folderInit, MHfolderItem *parent, QListViewItem*after, MHviewItem*master)
        : AccountViewItem(folderInit, parent,after )
{
    folder = folderInit;
    mbox = master;
    initName();
}

void MHfolderItem::initName()
{
    QString bName = folder->getDisplayName();
    if (bName.startsWith("/")&&bName.length()>1)
    {
        bName.replace(0,1,"");
    }
    int pos = bName.findRev("/");
    if (pos > 0)
    {
        bName.replace(0,pos+1,"");
    }
    if (bName.lower() == "outgoing")
    {
        setPixmap( 0, PIXMAP_OUTBOXFOLDER );
    }
    else if (bName.lower() == "inbox")
    {
        setPixmap( 0, PIXMAP_INBOXFOLDER);
    } else if (bName.lower() == "drafts") {
        setPixmap(0, Opie::Core::OResource::loadPixmap("edit", Opie::Core::OResource::SmallIcon));
    } else {
        setPixmap( 0, PIXMAP_MBOXFOLDER );
    }
    setText( 0, bName );
}

const FolderP&MHfolderItem::getFolder()const
{
    return folder;
}

void MHfolderItem::refresh(QValueList<RecMailP>&target)
{
    if (folder->may_select())
        mbox->getWrapper()->listMessages( folder->getName(),target );
}

RECBODYP MHfolderItem::fetchBody(const RecMailP&aMail)
{
    return mbox->getWrapper()->fetchBody(aMail);
}

bool MHfolderItem::deleteFolder()
{
    int yesno = QMessageBox::warning(0,QObject::tr("Delete folder",contextName),
                                     QObject::tr("<center>Realy delete folder <br><b>%1</b><br>and all if it content?</center>",contextName).arg(folder->getDisplayName()),
                                     QObject::tr("Yes",contextName),
                                     QObject::tr("No",contextName),QString::null,1,1);
    odebug << "Auswahl: " << yesno << "" << oendl;
    if (yesno == 0)
    {
        if (mbox->getWrapper()->deleteMbox(folder))
        {
            QListView*v=listView();
            MHviewItem * box = mbox;
            /* be carefull - after that this object is destroyd so don't use
             * any member of it after that call!!*/
            mbox->refresh(true);
            if (v)
            {
                v->setSelected(box,true);
            }
            return true;
        }
    }
    return false;
}

QMap<int,QString> MHfolderItem::folderMenu()
{
    QMap<int,QString> e;
    e[FOLDER_MENU_NEW_SUBFOLDER]=QObject::tr("Create new subfolder",contextName);
    e[FOLDER_MENU_REFRESH_HEADER]=QObject::tr("Refresh header list",contextName);
    e[FOLDER_MENU_MOVE_MAILS]=QObject::tr("Move/Copie all mails",contextName);
    e[FOLDER_MENU_DELETE_ALL_MAILS]=QObject::tr("Delete all mails",contextName);
    e[FOLDER_MENU_DELETE_FOLDER]=QObject::tr("Delete folder",contextName);
    return e;
}

void MHfolderItem::downloadMails()
{
    AccountView*bl = mbox->accountView();
    if (!bl) return;
    bl->downloadMails(folder,mbox->getWrapper());
}

void MHfolderItem::createFolder()
{
    Newmdirdlg ndirdlg(0,0,true);
    if ( QPEApplication::execDialog( &ndirdlg ) )
    {
        QString ndir = ndirdlg.Newdir();
        if (mbox->getWrapper()->createMbox(ndir,folder))
        {
            QListView*v=listView();
            MHviewItem * box = mbox;
            /* be carefull - after that this object is destroyd so don't use
             * any member of it after that call!!*/
            mbox->refresh(true);
            if (v)
            {
                v->setSelected(box,true);
            }
        }
    }
}

bool MHfolderItem::contextMenuSelected(int which)
{
    AccountView*view = 0;
    switch(which)
    {
    case FOLDER_MENU_DELETE_ALL_MAILS:
        deleteAllMail(mbox->getWrapper(),folder);
        break;
    case FOLDER_MENU_DELETE_FOLDER:
        return deleteFolder();
        break;
    case FOLDER_MENU_MOVE_MAILS:
        downloadMails();
        break;
    case FOLDER_MENU_NEW_SUBFOLDER:
        createFolder();
        break;
    case FOLDER_MENU_REFRESH_HEADER:
        view = (AccountView*)listView();
        if (view) view->refreshCurrent();
        break;
    default:
        break;
    }
    return false;
}

bool MHfolderItem::isDraftfolder()
{
    if (folder && folder->getName()==AbstractMail::defaultLocalfolder()+"/"+AbstractMail::draftFolder()) return true;
    return false;
}

/**
 * Generic stuff
 */

const QString AccountViewItem::contextName="AccountViewItem";

AccountViewItem::AccountViewItem( AccountView *parent )
        : QListViewItem( parent )
{
    init();
    m_Backlink = parent;
}

AccountViewItem::AccountViewItem( QListViewItem *parent)
        : QListViewItem( parent),folder(0)
{
    init();
}

AccountViewItem::AccountViewItem( QListViewItem *parent , QListViewItem*after  )
        :QListViewItem( parent,after ),folder(0)
{
    init();
}

AccountViewItem::AccountViewItem( const Opie::Core::OSmartPointer<Folder>&folderInit,QListViewItem *parent , QListViewItem*after  )
       :QListViewItem( parent,after ),folder(folderInit)
{
    init();
}

void AccountViewItem::init()
{
    m_Backlink = 0;
}

AccountViewItem::~AccountViewItem()
{
    folder = 0;
}

AccountView*AccountViewItem::accountView()
{
    return m_Backlink;
}

void AccountViewItem::deleteAllMail(AbstractMail*wrapper,const FolderP&folder)
{
    if (!wrapper) return;
    QString fname="";
    if (folder) fname = folder->getDisplayName();
    int yesno = QMessageBox::warning(0,QObject::tr("Delete all mails",contextName),
                                     QObject::tr("<center>Realy delete all mails in box <br>%1</center>",contextName).
                                     arg(fname),
                                     QObject::tr("Yes",contextName),
                                     QObject::tr("No",contextName),QString::null,1,1);
    odebug << "Auswahl: " << yesno << "" << oendl;
    if (yesno == 0)
    {
        if (wrapper->deleteAllMail(folder))
        {
            AccountView * view = (AccountView*)listView();
            if (view) view->refreshCurrent();
        }
    }
}

void AccountViewItem::removeChilds()
{
    QListViewItem *child = firstChild();
    while ( child )
    {
        QListViewItem *tmp = child;
        child = child->nextSibling();
        delete tmp;
    }
}

bool AccountViewItem::matchName(const QString&name)const
{
    if (!folder) return false;
    return folder->getDisplayName()==name;
}


AccountViewItem*AccountViewItem::findSubItem(const QString&path,AccountViewItem*start)
{
    AccountViewItem*pitem,*sitem;
    if (!start) pitem = (AccountViewItem*)firstChild();
    else pitem = (AccountViewItem*)start->firstChild();
    while (pitem)
    {
        if (pitem->matchName(path))
        {
            break;
        }
        if (pitem->childCount()>0)
        {
            sitem = findSubItem(path,pitem);
            if (sitem)
            {
                pitem = sitem;
                break;
            }
        }
        pitem=(AccountViewItem*)pitem->nextSibling();
    }
    return pitem;
}

bool AccountViewItem::isDraftfolder()
{
    return false;
}

QMap<int,QString> AccountViewItem::serverMenu()
{
    return QMap<int,QString>();
}

QMap<int,QString> AccountViewItem::folderMenu()
{
    return QMap<int,QString>();
}

QPopupMenu * AccountViewItem::getContextMenu()
{
    QPopupMenu *m = new QPopupMenu(0);
    if (m)
    {
        QMap<int,QString> entries;
        entries = folderMenu();
        QMap<int,QString>::Iterator it;
        for (it=entries.begin();it!=entries.end();++it) {
            m->insertItem(it.data(),it.key());
        }
        entries = serverMenu();
        for (it=entries.begin();it!=entries.end();++it) {
            m->insertItem(it.data(),it.key());
        }
    }
    return m;
}
