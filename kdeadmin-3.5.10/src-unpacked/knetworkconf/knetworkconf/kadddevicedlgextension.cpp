#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './kadddevicedlgextension.ui'
**
** Created: Вс фев 1 00:56:21 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "kadddevicedlgextension.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <klineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "./kadddevicedlgextension.ui.h"

/*
 *  Constructs a KAddDeviceDlgExtension as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
KAddDeviceDlgExtension::KAddDeviceDlgExtension( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KAddDeviceDlgExtension" );
    KAddDeviceDlgExtensionLayout = new QHBoxLayout( this, 11, 6, "KAddDeviceDlgExtensionLayout"); 

    gbAdvancedDeviceInfo = new QGroupBox( this, "gbAdvancedDeviceInfo" );
    gbAdvancedDeviceInfo->setColumnLayout(0, Qt::Vertical );
    gbAdvancedDeviceInfo->layout()->setSpacing( 6 );
    gbAdvancedDeviceInfo->layout()->setMargin( 11 );
    gbAdvancedDeviceInfoLayout = new QGridLayout( gbAdvancedDeviceInfo->layout() );
    gbAdvancedDeviceInfoLayout->setAlignment( Qt::AlignTop );

    textLabel1 = new QLabel( gbAdvancedDeviceInfo, "textLabel1" );

    gbAdvancedDeviceInfoLayout->addWidget( textLabel1, 0, 0 );

    kleDescription = new KLineEdit( gbAdvancedDeviceInfo, "kleDescription" );

    gbAdvancedDeviceInfoLayout->addWidget( kleDescription, 0, 1 );

    kleBroadcast = new KLineEdit( gbAdvancedDeviceInfo, "kleBroadcast" );

    gbAdvancedDeviceInfoLayout->addWidget( kleBroadcast, 1, 1 );

    TextLabel1_3 = new QLabel( gbAdvancedDeviceInfo, "TextLabel1_3" );

    gbAdvancedDeviceInfoLayout->addWidget( TextLabel1_3, 2, 0 );

    kleGateway = new KLineEdit( gbAdvancedDeviceInfo, "kleGateway" );

    gbAdvancedDeviceInfoLayout->addWidget( kleGateway, 2, 1 );

    textLabel1_2_2 = new QLabel( gbAdvancedDeviceInfo, "textLabel1_2_2" );

    gbAdvancedDeviceInfoLayout->addWidget( textLabel1_2_2, 1, 0 );
    KAddDeviceDlgExtensionLayout->addWidget( gbAdvancedDeviceInfo );
    languageChange();
    resize( QSize(298, 131).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( kleBroadcast, SIGNAL( textChanged(const QString&) ), this, SLOT( valueChanged(const QString&) ) );
    connect( kleDescription, SIGNAL( textChanged(const QString&) ), this, SLOT( valueChanged(const QString&) ) );
    connect( kleGateway, SIGNAL( textChanged(const QString&) ), this, SLOT( valueChanged(const QString&) ) );

    // buddies
    TextLabel1_3->setBuddy( kleGateway );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
KAddDeviceDlgExtension::~KAddDeviceDlgExtension()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KAddDeviceDlgExtension::languageChange()
{
    setCaption( tr2i18n( "Advanced Options" ) );
    gbAdvancedDeviceInfo->setTitle( tr2i18n( "Advanced Device Information" ) );
    QToolTip::add( gbAdvancedDeviceInfo, tr2i18n( "Set advanced setting for the network device" ) );
    textLabel1->setText( tr2i18n( "Description:" ) );
    QToolTip::add( textLabel1, tr2i18n( "IP address of the network device" ) );
    kleDescription->setText( QString::null );
    QToolTip::add( kleDescription, tr2i18n( "Simply enter a short human-readable description for this device" ) );
    QWhatsThis::add( kleDescription, tr2i18n( "Simply enter a short human-readable description for this device" ) );
    kleBroadcast->setText( QString::null );
    QToolTip::add( kleBroadcast, tr2i18n( "IP address of the network device" ) );
    QWhatsThis::add( kleBroadcast, tr2i18n( "The Broadcast is a special address. All devices of a network respond if packages are sent to this address." ) );
    TextLabel1_3->setText( tr2i18n( "Gateway:" ) );
    QToolTip::add( TextLabel1_3, tr2i18n( "An IP address is an unique identifier of a networking device in an TCP/IP network" ) );
    QWhatsThis::add( TextLabel1_3, tr2i18n( "An IP address is an unique identifier of a networking device in an TCP/IP network" ) );
    kleGateway->setText( QString::null );
    QToolTip::add( kleGateway, tr2i18n( "Default gateway for the network device" ) );
    QWhatsThis::add( kleGateway, tr2i18n( "<p>Here you should enter the default gateway for the network device.</p>" ) );
    textLabel1_2_2->setText( tr2i18n( "Broadcast:" ) );
}

#include "kadddevicedlgextension.moc"
