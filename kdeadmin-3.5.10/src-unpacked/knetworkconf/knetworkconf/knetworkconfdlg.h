/****************************************************************************
** Form interface generated from reading ui file './knetworkconfdlg.ui'
**
** Created: Вс фев 1 00:56:12 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KNETWORKCONFDLG_H
#define KNETWORKCONFDLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QTabWidget;
class QLabel;
class KListView;
class QListViewItem;
class KPushButton;
class QGroupBox;
class KLineEdit;
class KComboBox;
class KListBox;
class QListBoxItem;
class QButtonGroup;

class KNetworkConfDlg : public QWidget
{
    Q_OBJECT

public:
    KNetworkConfDlg( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KNetworkConfDlg();

    QTabWidget* tabWidget;
    QWidget* tab;
    QLabel* textLabel1_2;
    KListView* klvCardList;
    KPushButton* kpbConfigureNetworkInterface;
    KPushButton* kpbUpButton;
    KPushButton* kpbDownButton;
    QWidget* tab_2;
    QGroupBox* gbDefaultGateway;
    QLabel* TextLabel3;
    KLineEdit* kleDefaultRoute;
    QLabel* TextLabel1_2;
    KComboBox* kcbGwDevice;
    QWidget* tab_3;
    QGroupBox* gbDNSServersList;
    KListBox* klbDomainServerList;
    KPushButton* kpbUpDomainServerList;
    KPushButton* kpbDownDomainServerList;
    KPushButton* kpbAddDomainServer;
    KPushButton* kpbEditDomainServer;
    KPushButton* kpbRemoveDomainServer;
    QGroupBox* gbKnownHostsList;
    KListView* klvKnownHosts;
    KPushButton* kpbAddKnownHost;
    KPushButton* kpbEditKnownHost;
    KPushButton* kpbRemoveKnownHost;
    QLabel* tlHostName;
    KLineEdit* kleHostName;
    KLineEdit* kleDomainName;
    QLabel* tlDomainName;
    QWidget* TabPage;
    QButtonGroup* buttonGroup2;
    KListView* klvProfilesList;
    KPushButton* kpbEnableProfile;
    KPushButton* kpbSaveProfile;
    KPushButton* kpbCreateProfile;
    KPushButton* kpbDeleteProfile;

public slots:
    virtual void helpSlot();
    virtual void aboutSlot();
    virtual void updateProfileNameSlot();

protected:
    QHBoxLayout* KNetworkConfDlgLayout;
    QVBoxLayout* tabLayout;
    QSpacerItem* spacer23;
    QHBoxLayout* layout16;
    QSpacerItem* spacer24;
    QHBoxLayout* layout11;
    QVBoxLayout* tabLayout_2;
    QSpacerItem* Spacer7;
    QVBoxLayout* gbDefaultGatewayLayout;
    QHBoxLayout* Layout11;
    QSpacerItem* Spacer4;
    QHBoxLayout* Layout27;
    QSpacerItem* Spacer12;
    QGridLayout* tabLayout_3;
    QHBoxLayout* gbDNSServersListLayout;
    QVBoxLayout* layout23;
    QSpacerItem* Spacer10;
    QSpacerItem* Spacer11;
    QVBoxLayout* layout25;
    QSpacerItem* spacer27;
    QSpacerItem* spacer27_2;
    QVBoxLayout* gbKnownHostsListLayout;
    QHBoxLayout* layout20;
    QVBoxLayout* TabPageLayout;
    QSpacerItem* spacer18;
    QGridLayout* buttonGroup2Layout;
    QSpacerItem* spacer25;
    QVBoxLayout* layout28;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;

private slots:
    virtual void enableButtonsSlot();
    virtual void quitSlot();
    virtual void moveUpServerSlot();
    virtual void moveDownServerSlot();
    virtual void configureDeviceSlot();
    virtual void enableApplyButtonSlot( bool );
    virtual void enableApplyButtonSlot( const QString & );
    virtual void addServerSlot();
    virtual void removeServerSlot();
    virtual void saveInfoSlot();
    virtual void upDownInterfaceSlot();
    virtual void addKnownHostSlot();
    virtual void removeKnownHostSlot();
    virtual void editKnownHostSlot();
    virtual void startNetworkSlot();
    virtual void stopNetworkSlot();
    virtual void enableInterfaceSlot();
    virtual void disableInterfaceSlot();
    virtual void editServerSlot();
    virtual void showInterfaceContextMenuSlot();
    virtual void enableProfileSlot();
    virtual void createProfileSlot();
    virtual void updateProfileNameSlot(QListViewItem *item);
    virtual void removeProfileSlot();
    virtual void updateProfileSlot();

};

#endif // KNETWORKCONFDLG_H
