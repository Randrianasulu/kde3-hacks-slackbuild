/****************************************************************************
** Form interface generated from reading ui file './kadddnsserverdlg.ui'
**
** Created: Вс фев 1 00:56:12 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KADDDNSSERVERDLG_H
#define KADDDNSSERVERDLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <kdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class KLineEdit;
class QLabel;
class KPushButton;

class KAddDNSServerDlg : public KDialog
{
    Q_OBJECT

public:
    KAddDNSServerDlg( QWidget* parent = 0, const char* name = 0 );
    ~KAddDNSServerDlg();

    KLineEdit* kleNewServer;
    QLabel* lIPAddress;
    KPushButton* kpbAddServer;
    KPushButton* kpbCancel;

    void setAddingAlias( bool add );
    virtual bool modified();

protected:
    QGridLayout* KAddDNSServerDlgLayout;
    QSpacerItem* Spacer7;
    QSpacerItem* Spacer2;
    QSpacerItem* spacer13;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

    void init();

private slots:
    virtual void validateAddressSlot();
    virtual void makeButtonsResizeable();

};

#endif // KADDDNSSERVERDLG_H
