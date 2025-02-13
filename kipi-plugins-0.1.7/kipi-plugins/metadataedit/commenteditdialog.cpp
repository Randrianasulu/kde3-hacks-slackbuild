/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-11-20
 * Description : a dialog to batch edit comments
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// Qt includes.

#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>

// KDE includes.

#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <ktextedit.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>

// Local includes.

#include "pluginsversion.h"
#include "kpaboutdata.h"
#include "commenteditdialog.h"
#include "commenteditdialog.moc"

namespace KIPIMetadataEditPlugin
{

class CommentEditDialogDialogPrivate
{

public:

    CommentEditDialogDialogPrivate()
    {
        userCommentEdit      = 0;
        about                = 0;
        syncJFIFCommentCheck = 0;
        syncEXIFCommentCheck = 0;
        syncIPTCCaptionCheck = 0;
    }

    QCheckBox                *syncJFIFCommentCheck;
    QCheckBox                *syncEXIFCommentCheck;
    QCheckBox                *syncIPTCCaptionCheck;

    KTextEdit                *userCommentEdit;

    KIPIPlugins::KPAboutData *about;
};

CommentEditDialog::CommentEditDialog(QWidget* parent)
                 : KDialogBase(Plain, i18n("Edit Image Caption"),
                               Help|Ok|Cancel, Ok,
                               parent, 0, true, true)
{
    d = new CommentEditDialogDialogPrivate;

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(I18N_NOOP("Edit Metadata"),
                                            0,
                                            KAboutData::License_GPL,
                                            I18N_NOOP("A Plugin to edit images' metadata"),
                                            "(c) 2006-2008, Gilles Caulier");

    d->about->addAuthor("Gilles Caulier", I18N_NOOP("Author and Maintainer"),
                        "caulier dot gilles at gmail dot com");

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup( helpMenu->menu() );

    // ------------------------------------------------------------

    QVBoxLayout *vlay = new QVBoxLayout(plainPage(), 0, KDialog::spacingHint());

    QLabel *title = new QLabel(i18n("<p>Enter the image caption hosted by <b>%1</b>. "
                                    "This field is not limited (excepted with IPTC). UTF-8 encoding "
                                    "will be used to save text.")
                                    .arg(KApplication::kApplication()->aboutData()->appName()),
                               plainPage());

    d->userCommentEdit  = new KTextEdit(plainPage());

    d->syncJFIFCommentCheck = new QCheckBox(i18n("Sync JFIF Comment section"), plainPage());
    d->syncEXIFCommentCheck = new QCheckBox(i18n("Sync EXIF Comment"), plainPage());
    d->syncIPTCCaptionCheck = new QCheckBox(i18n("Sync IPTC caption (warning: limited to 2000 printable "
                                                 "Ascii characters set)"), plainPage());

    QLabel *note = new QLabel(i18n("<b>Note: captions from currently selected images "
                                   "will be permanently replaced.</b>"), plainPage());
    
    // ------------------------------------------------------------

    vlay->addWidget(title);
    vlay->addWidget(d->userCommentEdit);
    vlay->addWidget(d->syncJFIFCommentCheck);
    vlay->addWidget(d->syncEXIFCommentCheck);
    vlay->addWidget(d->syncIPTCCaptionCheck);
    vlay->addWidget(note);

    // ------------------------------------------------------------

    readSettings();
}

CommentEditDialog::~CommentEditDialog()
{
    delete d->about;
    delete d;
}

void CommentEditDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("metadataedit", "kipi-plugins");
}

void CommentEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void CommentEditDialog::slotCancel()
{
    saveSettings();
    KDialogBase::slotCancel();
}

void CommentEditDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("Comments Edit Settings");
    setCheckedSyncJFIFComment(config.readBoolEntry("Sync JFIF Comment", true));
    setCheckedSyncEXIFComment(config.readBoolEntry("Sync EXIF Comment", true));
    setCheckedSyncIPTCCaption(config.readBoolEntry("Sync IPTC Caption", true));
    resize(configDialogSize(config, QString("Comments Edit Dialog")));
}

void CommentEditDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("Comments Edit Settings");
    config.writeEntry("Sync JFIF Comment", syncJFIFCommentIsChecked());
    config.writeEntry("Sync EXIF Comment", syncEXIFCommentIsChecked());
    config.writeEntry("Sync IPTC Caption", syncIPTCCaptionIsChecked());
    saveDialogSize(config, QString("Comments Edit Dialog"));
    config.sync();
}

void CommentEditDialog::slotOk()
{
    saveSettings();
    accept();
}

bool CommentEditDialog::syncJFIFCommentIsChecked()
{
    return d->syncJFIFCommentCheck->isChecked();
}

bool CommentEditDialog::syncEXIFCommentIsChecked()
{
    return d->syncEXIFCommentCheck->isChecked();
}

bool CommentEditDialog::syncIPTCCaptionIsChecked()
{
    return d->syncIPTCCaptionCheck->isChecked();
}

QString CommentEditDialog::getComments()
{
    return d->userCommentEdit->text();
}

void CommentEditDialog::setCheckedSyncJFIFComment(bool c)
{
    d->syncJFIFCommentCheck->setChecked(c);
}

void CommentEditDialog::setCheckedSyncEXIFComment(bool c)
{
    d->syncEXIFCommentCheck->setChecked(c);
}

void CommentEditDialog::setCheckedSyncIPTCCaption(bool c)
{
    d->syncIPTCCaptionCheck->setChecked(c);
}

}  // namespace KIPIMetadataEditPlugin
