#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './kselectdistrodlg.ui'
**
** Created: Вс фев 1 00:56:14 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "kselectdistrodlg.h"

#include <qvariant.h>
#include <klistbox.h>
#include <kpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const unsigned char img0_kselectdistrodlg[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x03,
    0x9e, 0x49, 0x44, 0x41, 0x54, 0x18, 0x95, 0x95, 0x95, 0xcd, 0x6b, 0x1d,
    0x55, 0x18, 0xc6, 0x7f, 0x93, 0x4e, 0x9a, 0x77, 0x64, 0x52, 0xcf, 0x40,
    0x94, 0x8e, 0x74, 0x91, 0x5b, 0x2a, 0xf6, 0xaa, 0x28, 0x97, 0xa6, 0x60,
    0x2a, 0x2e, 0x6e, 0x6a, 0x5d, 0x74, 0xe3, 0xda, 0xfe, 0x03, 0xd2, 0x8f,
    0x8d, 0x0b, 0x71, 0x29, 0x2e, 0xba, 0x31, 0x28, 0x08, 0x05, 0x29, 0x64,
    0xe3, 0x42, 0x97, 0x15, 0x44, 0x42, 0x11, 0x9b, 0x2e, 0x44, 0x23, 0x4a,
    0xac, 0xa8, 0xed, 0x15, 0x13, 0x32, 0xc5, 0x86, 0xcc, 0xd8, 0x5c, 0xef,
    0x1c, 0xc9, 0xe8, 0xbc, 0xd6, 0x93, 0x8e, 0x8b, 0x9b, 0x7b, 0x13, 0x6b,
    0x6e, 0x4b, 0x5e, 0x18, 0xe6, 0x83, 0xf3, 0x3e, 0xf3, 0x9c, 0xe7, 0x77,
    0x38, 0xc7, 0xab, 0xaa, 0x8a, 0x9d, 0xca, 0xf3, 0x3c, 0x2e, 0xbc, 0x77,
    0x61, 0xe2, 0xe4, 0x89, 0x93, 0x17, 0x11, 0x79, 0xaa, 0xf7, 0x5d, 0x7c,
    0x50, 0xb7, 0xfd, 0x2e, 0x80, 0x82, 0x2f, 0x55, 0x7a, 0x33, 0x99, 0xbf,
    0xf4, 0xf1, 0xa5, 0x57, 0xcf, 0x4f, 0x9f, 0x5f, 0xf4, 0x77, 0x54, 0xdd,
    0x2c, 0x33, 0x66, 0x0e, 0x89, 0x31, 0xb5, 0x0f, 0xaf, 0xa6, 0x23, 0x3a,
    0x1c, 0x10, 0x19, 0x41, 0x7c, 0x41, 0xa4, 0x2b, 0xaa, 0x4e, 0x29, 0x0b,
    0x8b, 0xae, 0x43, 0x3c, 0xaa, 0x4c, 0x3d, 0x1d, 0x3f, 0x89, 0xcf, 0x38,
    0x70, 0x7f, 0x61, 0x41, 0x86, 0x70, 0x78, 0x57, 0x7f, 0xb6, 0x94, 0x12,
    0x10, 0x85, 0x8a, 0x31, 0x02, 0x9b, 0x5d, 0x5a, 0x80, 0x2d, 0x20, 0xcf,
    0x72, 0x1a, 0xe3, 0xc2, 0xe4, 0x21, 0x3c, 0x60, 0x0f, 0xf4, 0x87, 0x0c,
    0xa8, 0x61, 0x50, 0x94, 0xb2, 0x00, 0x5b, 0xe4, 0x40, 0x84, 0xaa, 0xed,
    0x3b, 0xc6, 0x41, 0x6e, 0x73, 0x6c, 0xa1, 0xd8, 0x76, 0x37, 0x11, 0x75,
    0xca, 0x83, 0x85, 0xbb, 0xae, 0xb1, 0x36, 0x27, 0xd5, 0xa0, 0xeb, 0x32,
    0x14, 0x28, 0x40, 0x00, 0xab, 0x8a, 0x16, 0x90, 0xde, 0xca, 0xa9, 0x9b,
    0x68, 0x5b, 0xe6, 0x0f, 0x10, 0xee, 0x0d, 0x12, 0x09, 0xd0, 0xa2, 0x24,
    0xb7, 0x42, 0xa0, 0x8a, 0x88, 0x60, 0x9d, 0x82, 0x42, 0x5e, 0x28, 0xb8,
    0x9e, 0x53, 0x01, 0xd7, 0xed, 0xf5, 0x07, 0xd1, 0x5f, 0xfd, 0x75, 0x15,
    0x75, 0x0c, 0x81, 0x0e, 0xcf, 0xbc, 0x56, 0x27, 0xd8, 0xa4, 0x2f, 0xbe,
    0x30, 0xf7, 0x53, 0xc2, 0x47, 0xf3, 0x42, 0xaa, 0x10, 0xf9, 0x42, 0xee,
    0xc0, 0x88, 0xe9, 0xc6, 0xb3, 0x69, 0xd5, 0x87, 0xdd, 0xd3, 0x9f, 0x3c,
    0x1c, 0x33, 0xfb, 0x63, 0x02, 0x61, 0x4c, 0xee, 0xe7, 0x5d, 0xa7, 0xb2,
    0x09, 0x74, 0x7b, 0xc6, 0xbb, 0xa5, 0x3f, 0x75, 0x18, 0xce, 0x4e, 0x82,
    0x19, 0x03, 0x5c, 0xb4, 0xa1, 0xae, 0x04, 0x87, 0x67, 0xdb, 0xd9, 0x9f,
    0x38, 0xfe, 0xda, 0xca, 0x78, 0x97, 0xf4, 0x4b, 0x85, 0xbc, 0xc8, 0x91,
    0x50, 0xaa, 0x74, 0x25, 0x6d, 0x67, 0x69, 0xf6, 0xa5, 0x3a, 0x4d, 0x73,
    0x9b, 0xb7, 0x92, 0x24, 0x69, 0x01, 0x0c, 0xfd, 0x8f, 0x7e, 0x66, 0xb1,
    0xed, 0x9c, 0xbc, 0x28, 0x49, 0xdb, 0x25, 0xa5, 0x2d, 0x49, 0xdb, 0x39,
    0xd6, 0x76, 0xe9, 0xab, 0x2a, 0xea, 0x20, 0x36, 0x31, 0x51, 0x18, 0x79,
    0xc7, 0x26, 0x8e, 0x3d, 0xdc, 0x68, 0x34, 0x46, 0x4a, 0x5b, 0x7e, 0x30,
    0x77, 0x75, 0x6e, 0xe6, 0xec, 0x99, 0xb3, 0xed, 0xad, 0x28, 0x76, 0x4b,
    0xdf, 0x29, 0xea, 0xba, 0x20, 0xad, 0xda, 0x3d, 0xaa, 0x1a, 0x59, 0xb5,
    0x7b, 0xeb, 0x8f, 0xd7, 0xef, 0x1c, 0x7f, 0xe9, 0x78, 0xb5, 0x15, 0x85,
    0x0f, 0xf8, 0x82, 0x88, 0x10, 0x1b, 0x41, 0x42, 0xc1, 0x84, 0xd2, 0x9f,
    0x8b, 0x55, 0xbd, 0x97, 0x7e, 0x95, 0x2c, 0x26, 0x7f, 0xb4, 0x16, 0x5b,
    0xd7, 0xb4, 0xd0, 0xeb, 0xc9, 0x52, 0xf2, 0x49, 0x6b, 0xb1, 0xf5, 0xc3,
    0xec, 0xe5, 0xd9, 0xfe, 0xc6, 0xd3, 0x5b, 0xc7, 0x1e, 0x28, 0x91, 0x11,
    0xac, 0x0f, 0xb1, 0x89, 0x90, 0xb0, 0x3b, 0x13, 0x75, 0x4a, 0xa0, 0xc1,
    0xbd, 0xf4, 0x37, 0xd2, 0x76, 0xba, 0x92, 0x65, 0xf6, 0x7d, 0x33, 0x66,
    0x6e, 0x34, 0x26, 0x9a, 0xa5, 0xd9, 0x5f, 0x93, 0x66, 0xf3, 0x64, 0xf1,
    0xfa, 0x1b, 0xe7, 0xb6, 0x1c, 0xdb, 0x75, 0x9b, 0x0b, 0xac, 0xcf, 0x9c,
    0xae, 0xed, 0x53, 0x47, 0xd5, 0xfd, 0x11, 0x43, 0xaa, 0xdd, 0x45, 0x5f,
    0xaa, 0x82, 0x8b, 0xaa, 0x3e, 0x7d, 0x9b, 0xed, 0xa9, 0xd7, 0xeb, 0x4f,
    0xc4, 0xfb, 0xf5, 0x5d, 0x09, 0x64, 0x4d, 0xc2, 0xe0, 0xae, 0x64, 0x7c,
    0x7e, 0xed, 0xfb, 0xef, 0xde, 0x02, 0xca, 0xbe, 0x70, 0xb2, 0x94, 0x7c,
    0x3b, 0x37, 0x3c, 0xf7, 0xb6, 0xf8, 0x52, 0x57, 0xa7, 0xe3, 0xb5, 0x03,
    0xb5, 0x17, 0x22, 0x13, 0x8d, 0x02, 0x94, 0xae, 0x44, 0x9d, 0x56, 0xc9,
    0xcd, 0xe4, 0xef, 0xec, 0x56, 0xf6, 0x7b, 0xe9, 0x4a, 0xc1, 0x71, 0x57,
    0x55, 0x0b, 0x2d, 0x74, 0x4d, 0x9d, 0x2e, 0x4b, 0x68, 0xee, 0x26, 0x4b,
    0xad, 0xdf, 0xec, 0x9a, 0xfd, 0xef, 0x1e, 0xbc, 0xf0, 0xd5, 0x82, 0x77,
    0xea, 0x95, 0x53, 0x23, 0x33, 0x17, 0x67, 0x9a, 0xcb, 0x37, 0x96, 0xbf,
    0xe8, 0xdc, 0xee, 0xdc, 0xe9, 0xac, 0x76, 0xaa, 0xce, 0xed, 0x4e, 0xd5,
    0x59, 0xed, 0x54, 0xcb, 0xbf, 0x2c, 0x57, 0x9d, 0xd5, 0x4e, 0xb9, 0xf0,
    0xcd, 0xc2, 0xa7, 0xd3, 0xe7, 0xa7, 0x8f, 0x36, 0x5f, 0x6c, 0x8e, 0x5c,
    0xf9, 0xec, 0x8a, 0x07, 0x50, 0x55, 0xd5, 0x8e, 0xd7, 0x10, 0xc0, 0x91,
    0x63, 0x47, 0xaa, 0xc6, 0x44, 0xe3, 0x1f, 0x09, 0x85, 0x5c, 0xf3, 0x11,
    0x55, 0xf5, 0xd4, 0x29, 0xaa, 0x8a, 0x55, 0x7b, 0x5f, 0xfa, 0x83, 0xca,
    0xdb, 0xfe, 0x72, 0xe6, 0xf4, 0x99, 0xd1, 0xda, 0x78, 0xed, 0xb9, 0xc8,
    0x44, 0x2f, 0x9b, 0xd0, 0x3c, 0x1b, 0xc5, 0xd1, 0x33, 0xa5, 0x2d, 0x35,
    0xcb, 0xb3, 0xeb, 0xdb, 0xe8, 0x7f, 0x3d, 0x7b, 0x79, 0x76, 0xbd, 0xd7,
    0x33, 0xf0, 0x04, 0xea, 0x3d, 0x4c, 0xbf, 0x33, 0x33, 0x14, 0xc0, 0xa3,
    0x66, 0xbf, 0x79, 0xc8, 0x66, 0x76, 0xb4, 0x74, 0xfa, 0xfc, 0xd4, 0x89,
    0xe6, 0xb9, 0xf9, 0xf9, 0xf9, 0xb5, 0x64, 0x29, 0xb9, 0x60, 0xc6, 0xcc,
    0x8d, 0xda, 0x81, 0x7a, 0x99, 0xac, 0xdc, 0x5c, 0x17, 0x9f, 0x76, 0x8f,
    0xfe, 0x20, 0xe1, 0xfe, 0xb6, 0x69, 0xdb, 0x69, 0x10, 0x1c, 0x88, 0xdf,
    0x8c, 0x1f, 0x89, 0x8f, 0x9a, 0xd1, 0x68, 0x38, 0xf0, 0x79, 0x4c, 0xc0,
    0x4c, 0x36, 0x1a, 0x07, 0x4d, 0x68, 0x0e, 0x0e, 0xa2, 0x3f, 0xa8, 0xfa,
    0xc2, 0xc9, 0x4a, 0xb2, 0x91, 0x17, 0x36, 0x49, 0xb3, 0x74, 0x1f, 0xb0,
    0x57, 0x7c, 0x10, 0x11, 0x8b, 0x03, 0x6b, 0x6d, 0xe7, 0xbe, 0xf4, 0x77,
    0xa8, 0x7f, 0x01, 0x8b, 0x9f, 0x68, 0x1d, 0xd4, 0xdb, 0x61, 0x8a, 0x00,
    0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};


/*
 *  Constructs a KSelectDistroDlg which is a child of 'parent', with the
 *  name 'name'.' 
 */
KSelectDistroDlg::KSelectDistroDlg( QWidget* parent,  const char* name )
    : KDialog( parent, name )
{
    QImage img;
    img.loadFromData( img0_kselectdistrodlg, sizeof( img0_kselectdistrodlg ), "PNG" );
    image0 = img;
    if ( !name )
	setName( "KSelectDistroDlg" );
    setIcon( image0 );
    setSizeGripEnabled( TRUE );
    KSelectDistroDlgLayout = new QGridLayout( this, 1, 1, 11, 6, "KSelectDistroDlgLayout"); 

    klbDistroList = new KListBox( this, "klbDistroList" );

    KSelectDistroDlgLayout->addMultiCellWidget( klbDistroList, 2, 2, 0, 3 );

    kpbOk = new KPushButton( this, "kpbOk" );
    kpbOk->setAutoDefault( TRUE );
    kpbOk->setDefault( TRUE );

    KSelectDistroDlgLayout->addWidget( kpbOk, 3, 2 );

    kpbCancel = new KPushButton( this, "kpbCancel" );
    kpbCancel->setAutoDefault( TRUE );

    KSelectDistroDlgLayout->addWidget( kpbCancel, 3, 3 );

    cbAskAgain = new QCheckBox( this, "cbAskAgain" );

    KSelectDistroDlgLayout->addWidget( cbAskAgain, 3, 0 );
    Horizontal_Spacing2 = new QSpacerItem( 130, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    KSelectDistroDlgLayout->addItem( Horizontal_Spacing2, 3, 1 );

    textLabel1 = new QLabel( this, "textLabel1" );

    KSelectDistroDlgLayout->addMultiCellWidget( textLabel1, 0, 0, 0, 3 );

    textLabel2 = new QLabel( this, "textLabel2" );

    KSelectDistroDlgLayout->addMultiCellWidget( textLabel2, 1, 1, 0, 3 );
    languageChange();
    resize( QSize(372, 499).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( kpbOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( kpbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( klbDistroList, SIGNAL( doubleClicked(QListBoxItem*) ), this, SLOT( accept() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
KSelectDistroDlg::~KSelectDistroDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KSelectDistroDlg::languageChange()
{
    setCaption( tr2i18n( "Unsupported Platform" ) );
    kpbOk->setText( tr2i18n( "&OK" ) );
    kpbOk->setAccel( QKeySequence( QString::null ) );
    kpbCancel->setText( tr2i18n( "&Cancel" ) );
    kpbCancel->setAccel( QKeySequence( QString::null ) );
    cbAskAgain->setText( tr2i18n( "Do not ask again" ) );
    textLabel1->setText( tr2i18n( "<font size=\"+1\"><p align=\"center\"><b>Your Platform is Not Supported</b></p></font>" ) );
    textLabel2->setText( tr2i18n( "You may choose one of the following supported platforms if you are <b>sure</b> your platform behaves the same as the chosen one. Please be sure, because your current network configuration could be damaged." ) );
}

void KSelectDistroDlg::exitSlot()
{
    qWarning( "KSelectDistroDlg::exitSlot(): Not implemented yet" );
}

#include "kselectdistrodlg.moc"
