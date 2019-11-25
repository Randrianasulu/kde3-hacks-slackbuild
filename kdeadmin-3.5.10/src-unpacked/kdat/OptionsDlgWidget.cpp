#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './OptionsDlgWidget.ui'
**
** Created: Вс фев 1 00:54:21 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "OptionsDlgWidget.h"

#include <qvariant.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kdialog.h>
#include <qpushbutton.h>
#include <knuminput.h>
#include <kcombobox.h>
#include <qlabel.h>
#include <kpushbutton.h>
#include <klineedit.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "./OptionsDlgWidget.ui.h"

/*
 *  Constructs a OptionsDlgWidget as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
OptionsDlgWidget::OptionsDlgWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "OptionsDlgWidget" );
    OptionsDlgWidgetLayout = new QHBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint(), "OptionsDlgWidgetLayout"); 

    layout4 = new QVBoxLayout( 0, 0, KDialog::spacingHint(), "layout4"); 

    layout5 = new QGridLayout( 0, 1, 1, 0, KDialog::spacingHint(), "layout5"); 

    _defaultTapeSize = new KIntSpinBox( this, "_defaultTapeSize" );
    _defaultTapeSize->setMinimumSize( QSize( 48, 0 ) );

    layout5->addWidget( _defaultTapeSize, 0, 1 );

    _tapeBlockSize = new KIntSpinBox( this, "_tapeBlockSize" );
    _tapeBlockSize->setMinimumSize( QSize( 48, 0 ) );

    layout5->addWidget( _tapeBlockSize, 1, 1 );

    _defaultTapeSizeUnits = new KComboBox( FALSE, this, "_defaultTapeSizeUnits" );

    layout5->addWidget( _defaultTapeSizeUnits, 0, 2 );

    textLabel3 = new QLabel( this, "textLabel3" );

    layout5->addWidget( textLabel3, 1, 2 );

    textLabel2 = new QLabel( this, "textLabel2" );

    layout5->addWidget( textLabel2, 1, 0 );

    textLabel1 = new QLabel( this, "textLabel1" );

    layout5->addWidget( textLabel1, 0, 0 );
    layout4->addLayout( layout5 );

    layout1 = new QGridLayout( 0, 1, 1, 0, KDialog::spacingHint(), "layout1"); 

    browseTarCommand = new KPushButton( this, "browseTarCommand" );

    layout1->addWidget( browseTarCommand, 1, 2 );

    _tapeDevice = new KLineEdit( this, "_tapeDevice" );

    layout1->addWidget( _tapeDevice, 0, 1 );

    textLabel5 = new QLabel( this, "textLabel5" );

    layout1->addWidget( textLabel5, 1, 0 );

    browseTapeDevice = new KPushButton( this, "browseTapeDevice" );

    layout1->addWidget( browseTapeDevice, 0, 2 );

    _tarCommand = new KLineEdit( this, "_tarCommand" );

    layout1->addWidget( _tarCommand, 1, 1 );

    textLabel4 = new QLabel( this, "textLabel4" );

    layout1->addWidget( textLabel4, 0, 0 );
    layout4->addLayout( layout1 );

    buttonGroup1 = new QButtonGroup( this, "buttonGroup1" );
    buttonGroup1->setColumnLayout(0, Qt::Vertical );
    buttonGroup1->layout()->setSpacing( KDialog::spacingHint() );
    buttonGroup1->layout()->setMargin( KDialog::marginHint() );
    buttonGroup1Layout = new QVBoxLayout( buttonGroup1->layout() );
    buttonGroup1Layout->setAlignment( Qt::AlignTop );

    _loadOnMount = new QCheckBox( buttonGroup1, "_loadOnMount" );
    buttonGroup1Layout->addWidget( _loadOnMount );

    _lockOnMount = new QCheckBox( buttonGroup1, "_lockOnMount" );
    buttonGroup1Layout->addWidget( _lockOnMount );

    _ejectOnUnmount = new QCheckBox( buttonGroup1, "_ejectOnUnmount" );
    buttonGroup1Layout->addWidget( _ejectOnUnmount );

    _variableBlockSize = new QCheckBox( buttonGroup1, "_variableBlockSize" );
    buttonGroup1Layout->addWidget( _variableBlockSize );
    layout4->addWidget( buttonGroup1 );
    spacer1 = new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout4->addItem( spacer1 );
    OptionsDlgWidgetLayout->addLayout( layout4 );
    languageChange();
    resize( QSize(325, 332).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( _defaultTapeSize, SIGNAL( valueChanged(int) ), this, SLOT( slotValueChanged() ) );
    connect( _tapeBlockSize, SIGNAL( valueChanged(int) ), this, SLOT( slotValueChanged() ) );
    connect( _defaultTapeSizeUnits, SIGNAL( activated(int) ), this, SLOT( slotValueChanged() ) );
    connect( _tapeDevice, SIGNAL( textChanged(const QString&) ), this, SLOT( slotValueChanged() ) );
    connect( _tarCommand, SIGNAL( textChanged(const QString&) ), this, SLOT( slotValueChanged() ) );
    connect( _loadOnMount, SIGNAL( toggled(bool) ), this, SLOT( slotValueChanged() ) );
    connect( _lockOnMount, SIGNAL( toggled(bool) ), this, SLOT( slotValueChanged() ) );
    connect( _ejectOnUnmount, SIGNAL( toggled(bool) ), this, SLOT( slotValueChanged() ) );
    connect( _variableBlockSize, SIGNAL( toggled(bool) ), this, SLOT( slotValueChanged() ) );
    connect( browseTapeDevice, SIGNAL( clicked() ), this, SLOT( slotBrowseTapeDevice() ) );
    connect( browseTarCommand, SIGNAL( clicked() ), this, SLOT( slotBrowseTarCommand() ) );

    // buddies
    textLabel2->setBuddy( _tapeBlockSize );
    textLabel1->setBuddy( _defaultTapeSize );
    textLabel5->setBuddy( _tarCommand );
    textLabel4->setBuddy( _tapeDevice );
}

/*
 *  Destroys the object and frees any allocated resources
 */
OptionsDlgWidget::~OptionsDlgWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void OptionsDlgWidget::languageChange()
{
    setCaption( tr2i18n( "Options Widget" ) );
    QWhatsThis::add( _defaultTapeSize, tr2i18n( "This setting determines the capacity that KDat assumes your backup tapes to be.  This is used when formatting the tapes." ) );
    QWhatsThis::add( _tapeBlockSize, tr2i18n( "Tape drives read and write data in individual blocks.  This setting controls the size of each block, and should be set to your tape drive's block size.  For floppy tape drives this should be set to <b>10240</b> bytes." ) );
    _defaultTapeSizeUnits->clear();
    _defaultTapeSizeUnits->insertItem( tr2i18n( "MB" ) );
    _defaultTapeSizeUnits->insertItem( tr2i18n( "GB" ) );
    QToolTip::add( _defaultTapeSizeUnits, QString::null );
    QWhatsThis::add( _defaultTapeSizeUnits, tr2i18n( "This option chooses whether the default tape size to the left is in megabytes (MB) or gigabytes (GB)." ) );
    textLabel3->setText( tr2i18n( "bytes" ) );
    textLabel2->setText( tr2i18n( "Tape block size:" ) );
    textLabel1->setText( tr2i18n( "Default tape size:" ) );
    browseTarCommand->setText( tr2i18n( "..." ) );
    QToolTip::add( browseTarCommand, tr2i18n( "Browse for the tar command." ) );
    QWhatsThis::add( _tapeDevice, tr2i18n( "The location in the filesystem of the <em>non-rewinding</em> tape device.  The default is <b>/dev/tape</b>." ) );
    textLabel5->setText( tr2i18n( "Tar command:" ) );
    browseTapeDevice->setText( tr2i18n( "..." ) );
    QToolTip::add( browseTapeDevice, tr2i18n( "Browse for the tape device." ) );
    QWhatsThis::add( _tarCommand, tr2i18n( "This setting controls the command that KDat uses to perform the tape backup.  The full path should be given.  The default is <b>tar</b>." ) );
    textLabel4->setText( tr2i18n( "Tape device:" ) );
    buttonGroup1->setTitle( tr2i18n( "Tape Drive Options" ) );
    _loadOnMount->setText( tr2i18n( "Load tape on mount" ) );
    QToolTip::add( _loadOnMount, tr2i18n( "<qt>Issue an <tt>mtload</tt> command prior to mounting the tape.</qt>" ) );
    QWhatsThis::add( _loadOnMount, tr2i18n( "This command issues an <tt>mtload</tt> command to the tape device before trying to mount it.\n"
"\n"
"This is required by some tape drives." ) );
    _lockOnMount->setText( tr2i18n( "Lock tape drive on mount" ) );
    QToolTip::add( _lockOnMount, tr2i18n( "Disable the eject button after mounting the tape." ) );
    QWhatsThis::add( _lockOnMount, tr2i18n( "This option makes KDat try to disable the eject button on the tape drive after the tape has been mounted.\n"
"\n"
"This doesn't work for all tape drives." ) );
    _ejectOnUnmount->setText( tr2i18n( "Eject tape on unmount" ) );
    QToolTip::add( _ejectOnUnmount, tr2i18n( "Try to eject the tape after it is unmounted.  Don't use this for ftape." ) );
    QWhatsThis::add( _ejectOnUnmount, tr2i18n( "Try to eject the tape after it has been unmounted.\n"
"\n"
"This option should not be used for floppy-tape drives." ) );
    _variableBlockSize->setText( tr2i18n( "Variable block size" ) );
    QToolTip::add( _variableBlockSize, tr2i18n( "Enable variable-block size support in the tape drive." ) );
    QWhatsThis::add( _variableBlockSize, tr2i18n( "Some tape drives support different sizes of the data block.  With this option, KDat will attempt to enable that support.\n"
"\n"
"You must still specify the block size." ) );
}

#include "OptionsDlgWidget.moc"
