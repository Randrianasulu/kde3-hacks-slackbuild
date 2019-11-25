/****************************************************************************
** Form interface generated from reading ui file './kadddevicedlgextension.ui'
**
** Created: Вс фев 1 00:56:13 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KADDDEVICEDLGEXTENSION_H
#define KADDDEVICEDLGEXTENSION_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class KLineEdit;

class KAddDeviceDlgExtension : public QWidget
{
    Q_OBJECT

public:
    KAddDeviceDlgExtension( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KAddDeviceDlgExtension();

    QGroupBox* gbAdvancedDeviceInfo;
    QLabel* textLabel1;
    KLineEdit* kleDescription;
    KLineEdit* kleBroadcast;
    QLabel* TextLabel1_3;
    KLineEdit* kleGateway;
    QLabel* textLabel1_2_2;

signals:
    void valueChangedSignal(int);

protected:
    QHBoxLayout* KAddDeviceDlgExtensionLayout;
    QGridLayout* gbAdvancedDeviceInfoLayout;

protected slots:
    virtual void languageChange();

private:
    void init();

private slots:
    virtual void valueChanged( const QString & s );

};

#endif // KADDDEVICEDLGEXTENSION_H
