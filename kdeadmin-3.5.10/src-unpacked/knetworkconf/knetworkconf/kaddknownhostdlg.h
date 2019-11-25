/****************************************************************************
** Form interface generated from reading ui file './kaddknownhostdlg.ui'
**
** Created: Вс фев 1 00:56:10 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KADDKNOWNHOSTDLG_H
#define KADDKNOWNHOSTDLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <kdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class KLineEdit;
class KPushButton;
class QGroupBox;
class KListBox;
class QListBoxItem;

class KAddKnownHostDlg : public KDialog
{
    Q_OBJECT

public:
    KAddKnownHostDlg( QWidget* parent = 0, const char* name = 0 );
    ~KAddKnownHostDlg();

    QLabel* textLabel1;
    KLineEdit* kleIpAddress;
    KPushButton* kpbCancel;
    KPushButton* kpbAdd;
    QGroupBox* groupBox1;
    KListBox* klbAliases;
    KPushButton* kpbAddHost;
    KPushButton* kpbEditHost;
    KPushButton* kpbRemoveHost;

    bool modified();

protected:
    QGridLayout* KAddKnownHostDlgLayout;
    QSpacerItem* spacer4;
    QSpacerItem* spacer51;
    QSpacerItem* spacer51_2;
    QHBoxLayout* layout12;
    QSpacerItem* spacer7;
    QSpacerItem* spacer16;
    QHBoxLayout* groupBox1Layout;
    QVBoxLayout* layout25;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

    void init();

private slots:
    virtual void validateAddressSlot();
    virtual void makeButtonsResizeable();
    virtual void editHostSlot();
    virtual void removeHostSlot();
    virtual void addHostSlot();

};

#endif // KADDKNOWNHOSTDLG_H
