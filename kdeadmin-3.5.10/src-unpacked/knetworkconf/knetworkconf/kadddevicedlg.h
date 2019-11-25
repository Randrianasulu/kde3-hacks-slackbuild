/****************************************************************************
** Form interface generated from reading ui file './kadddevicedlg.ui'
**
** Created: Вс фев 1 00:56:12 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KADDDEVICEDLG_H
#define KADDDEVICEDLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QButtonGroup;
class KComboBox;
class QRadioButton;
class QCheckBox;
class QLabel;
class KLineEdit;

class KAddDeviceDlg : public QWidget
{
    Q_OBJECT

public:
    KAddDeviceDlg( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KAddDeviceDlg();

    QButtonGroup* gbBasicDeviceInfo;
    KComboBox* kcbAutoBootProto;
    QRadioButton* rbBootProtoManual;
    QRadioButton* rbBootProtoAuto;
    QCheckBox* kcbstartAtBoot;
    KComboBox* kcbNetmask;
    QLabel* TextLabel1_2;
    KLineEdit* kleIPAddress;
    QLabel* TextLabel1;

protected:
    QHBoxLayout* KAddDeviceDlgLayout;
    QGridLayout* gbBasicDeviceInfoLayout;
    QSpacerItem* Spacer4;
    QSpacerItem* spacer5;
    QSpacerItem* spacer9;
    QSpacerItem* spacer11_2;
    QSpacerItem* spacer11;
    QSpacerItem* spacer10;
    QSpacerItem* spacer10_2;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

    void init();

};

#endif // KADDDEVICEDLG_H
