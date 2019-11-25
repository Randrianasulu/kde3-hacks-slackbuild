/****************************************************************************
** Form interface generated from reading ui file './kadddevicewifiext.ui'
**
** Created: Вс фев 1 00:56:13 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KADDDEVICEWIFIEXT_H
#define KADDDEVICEWIFIEXT_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QButtonGroup;
class QLabel;
class KLineEdit;
class KPasswordEdit;
class QComboBox;

class KAddDeviceWifiExt : public QWidget
{
    Q_OBJECT

public:
    KAddDeviceWifiExt( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KAddDeviceWifiExt();

    QButtonGroup* gbWirelessSettings;
    QLabel* textLabel2;
    KLineEdit* kleEssid;
    KPasswordEdit* kleWepKey;
    QLabel* textLabel1;
    QLabel* textLabel1_2;
    QComboBox* qcbKeyType;

protected:
    QVBoxLayout* KAddDeviceWifiExtLayout;
    QGridLayout* gbWirelessSettingsLayout;

protected slots:
    virtual void languageChange();

};

#endif // KADDDEVICEWIFIEXT_H
