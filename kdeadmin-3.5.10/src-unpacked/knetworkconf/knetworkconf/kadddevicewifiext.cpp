#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './kadddevicewifiext.ui'
**
** Created: Вс фев 1 00:56:24 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "kadddevicewifiext.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <klineedit.h>
#include <kpassdlg.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a KAddDeviceWifiExt as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
KAddDeviceWifiExt::KAddDeviceWifiExt( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KAddDeviceWifiExt" );
    KAddDeviceWifiExtLayout = new QVBoxLayout( this, 0, 6, "KAddDeviceWifiExtLayout"); 

    gbWirelessSettings = new QButtonGroup( this, "gbWirelessSettings" );
    gbWirelessSettings->setColumnLayout(0, Qt::Vertical );
    gbWirelessSettings->layout()->setSpacing( 6 );
    gbWirelessSettings->layout()->setMargin( 11 );
    gbWirelessSettingsLayout = new QGridLayout( gbWirelessSettings->layout() );
    gbWirelessSettingsLayout->setAlignment( Qt::AlignTop );

    textLabel2 = new QLabel( gbWirelessSettings, "textLabel2" );

    gbWirelessSettingsLayout->addWidget( textLabel2, 1, 0 );

    kleEssid = new KLineEdit( gbWirelessSettings, "kleEssid" );

    gbWirelessSettingsLayout->addWidget( kleEssid, 0, 1 );

    kleWepKey = new KPasswordEdit( gbWirelessSettings, "kleWepKey" );

    gbWirelessSettingsLayout->addWidget( kleWepKey, 1, 1 );

    textLabel1 = new QLabel( gbWirelessSettings, "textLabel1" );

    gbWirelessSettingsLayout->addWidget( textLabel1, 0, 0 );

    textLabel1_2 = new QLabel( gbWirelessSettings, "textLabel1_2" );

    gbWirelessSettingsLayout->addWidget( textLabel1_2, 2, 0 );

    qcbKeyType = new QComboBox( FALSE, gbWirelessSettings, "qcbKeyType" );

    gbWirelessSettingsLayout->addWidget( qcbKeyType, 2, 1 );
    KAddDeviceWifiExtLayout->addWidget( gbWirelessSettings );
    languageChange();
    resize( QSize(186, 104).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
KAddDeviceWifiExt::~KAddDeviceWifiExt()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KAddDeviceWifiExt::languageChange()
{
    setCaption( tr2i18n( "Wireless Settings" ) );
    gbWirelessSettings->setTitle( tr2i18n( "Wireless Settings" ) );
    textLabel2->setText( tr2i18n( "WEP key:" ) );
    textLabel1->setText( tr2i18n( "ESSID:" ) );
    textLabel1_2->setText( tr2i18n( "Key type:" ) );
    qcbKeyType->clear();
    qcbKeyType->insertItem( tr2i18n( "ASCII" ) );
    qcbKeyType->insertItem( tr2i18n( "Hexadecimal" ) );
}

#include "kadddevicewifiext.moc"
