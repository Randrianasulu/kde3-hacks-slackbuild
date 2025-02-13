/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : EXIF date and time settings page.
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

// QT includes.

#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>
#include <qvalidator.h>
#include <qcheckbox.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <kdatetimewidget.h>
#include <knuminput.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <kseparator.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "exifdatetime.h"
#include "exifdatetime.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFDateTimePriv
{
public:

    EXIFDateTimePriv()
    {
        dateCreatedSel             = 0;
        dateOriginalSel            = 0;
        dateDigitalizedSel         = 0;
        dateCreatedSubSecEdit      = 0;
        dateOriginalSubSecEdit     = 0;
        dateDigitalizedSubSecEdit  = 0;

        dateCreatedCheck           = 0;
        dateOriginalCheck          = 0;
        dateDigitalizedCheck       = 0;
        dateCreatedSubSecCheck     = 0;
        dateOriginalSubSecCheck    = 0;
        dateDigitalizedSubSecCheck = 0;
        syncHOSTDateCheck          = 0;
        syncIPTCDateCheck          = 0;
    }

    QCheckBox       *dateCreatedCheck;
    QCheckBox       *dateOriginalCheck;
    QCheckBox       *dateDigitalizedCheck;
    QCheckBox       *dateCreatedSubSecCheck;
    QCheckBox       *dateOriginalSubSecCheck;
    QCheckBox       *dateDigitalizedSubSecCheck;
    QCheckBox       *syncHOSTDateCheck;
    QCheckBox       *syncIPTCDateCheck;

    KIntSpinBox     *dateCreatedSubSecEdit;
    KIntSpinBox     *dateOriginalSubSecEdit;
    KIntSpinBox     *dateDigitalizedSubSecEdit;

    KDateTimeWidget *dateCreatedSel;
    KDateTimeWidget *dateOriginalSel;
    KDateTimeWidget *dateDigitalizedSel;
};

EXIFDateTime::EXIFDateTime(QWidget* parent)
            : QWidget(parent)
{
    d = new EXIFDateTimePriv;

    QGridLayout* grid = new QGridLayout(parent, 9, 3, KDialog::spacingHint());

    // --------------------------------------------------------

    d->dateCreatedCheck       = new QCheckBox(i18n("Creation date and time"), parent);
    d->dateCreatedSubSecCheck = new QCheckBox(i18n("Creation sub-second"), parent);
    d->dateCreatedSel         = new KDateTimeWidget(parent);
    d->dateCreatedSubSecEdit  = new KIntSpinBox(0, 999, 1, 0, 10, parent);
    d->dateCreatedSel->setDateTime(QDateTime::currentDateTime());
    d->syncHOSTDateCheck      = new QCheckBox(i18n("Sync creation date entered through %1")
                                              .arg(KApplication::kApplication()->aboutData()->appName()), 
                                              parent);
    d->syncIPTCDateCheck      = new QCheckBox(i18n("Sync IPTC creation date"), parent);
    KSeparator *line          = new KSeparator(Horizontal, parent);

    grid->addMultiCellWidget(d->dateCreatedCheck, 0, 0, 0, 0);
    grid->addMultiCellWidget(d->dateCreatedSubSecCheck, 0, 0, 1, 2);
    grid->addMultiCellWidget(d->dateCreatedSel, 1, 1, 0, 0);
    grid->addMultiCellWidget(d->dateCreatedSubSecEdit, 1, 1, 1, 1);
    grid->addMultiCellWidget(d->syncHOSTDateCheck, 2, 2, 0, 3);
    grid->addMultiCellWidget(d->syncIPTCDateCheck, 3, 3, 0, 3);
    grid->addMultiCellWidget(line, 4, 4, 0, 3);

    QWhatsThis::add(d->dateCreatedSel, i18n("<p>Set here the date and time of image creation. "
                                       "In this standard it is the date and time the file was changed."));
    QWhatsThis::add(d->dateCreatedSubSecEdit, i18n("<p>Set here the fractions of seconds for the date "
                                              "and time of image creation."));

    // --------------------------------------------------------

    d->dateOriginalCheck       = new QCheckBox(i18n("Original date and time"), parent);
    d->dateOriginalSubSecCheck = new QCheckBox(i18n("Original sub-second"), parent);
    d->dateOriginalSel         = new KDateTimeWidget(parent);
    d->dateOriginalSubSecEdit  = new KIntSpinBox(0, 999, 1, 0, 10, parent);
    d->dateOriginalSel->setDateTime(QDateTime::currentDateTime());
    grid->addMultiCellWidget(d->dateOriginalCheck, 5, 5, 0, 0);
    grid->addMultiCellWidget(d->dateOriginalSubSecCheck, 5, 5, 1, 2);
    grid->addMultiCellWidget(d->dateOriginalSel, 6, 6, 0, 0);
    grid->addMultiCellWidget(d->dateOriginalSubSecEdit, 6, 6, 1, 1);
    QWhatsThis::add(d->dateOriginalSel, i18n("<p>Set here the date and time when the original image "
                                        "data was generated. For a digital still camera the date and "
                                        "time the picture was taken are recorded."));
    QWhatsThis::add(d->dateOriginalSubSecEdit, i18n("<p>Set here the fractions of seconds for the date "
                                               "and time when the original image data was generated."));

    // --------------------------------------------------------

    d->dateDigitalizedCheck       = new QCheckBox(i18n("Digitization date and time"), parent);
    d->dateDigitalizedSubSecCheck = new QCheckBox(i18n("Digitization sub-second"), parent);
    d->dateDigitalizedSel         = new KDateTimeWidget(parent);
    d->dateDigitalizedSubSecEdit  = new KIntSpinBox(0, 999, 1, 0, 10, parent);
    d->dateDigitalizedSel->setDateTime(QDateTime::currentDateTime());
    grid->addMultiCellWidget(d->dateDigitalizedCheck, 7, 7, 0, 0);
    grid->addMultiCellWidget(d->dateDigitalizedSubSecCheck, 7, 7, 1, 2);
    grid->addMultiCellWidget(d->dateDigitalizedSel, 8, 8, 0, 0);
    grid->addMultiCellWidget(d->dateDigitalizedSubSecEdit, 8, 8, 1, 1);
    QWhatsThis::add(d->dateDigitalizedSel, i18n("<p>Set here the date and time when the image was "
                                           "stored as digital data. If, for example, an image was "
                                           "captured by a digital still camera and at the same "
                                           "time the file was recorded, then Original and Digitization "
                                           "date and time will have the same contents."));
    QWhatsThis::add(d->dateDigitalizedSubSecEdit, i18n("<p>Set here the fractions of seconds for the date "
                                                  "and time when the image was stored as digital data."));

    grid->setColStretch(3, 10);                     
    grid->setRowStretch(9, 10);                     

    // --------------------------------------------------------

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->dateCreatedSel, SLOT(setEnabled(bool)));

    connect(d->dateOriginalCheck, SIGNAL(toggled(bool)),
            d->dateOriginalSel, SLOT(setEnabled(bool)));

    connect(d->dateDigitalizedCheck, SIGNAL(toggled(bool)),
            d->dateDigitalizedSel, SLOT(setEnabled(bool)));

    connect(d->dateCreatedSubSecCheck, SIGNAL(toggled(bool)),
            d->dateCreatedSubSecEdit, SLOT(setEnabled(bool)));

    connect(d->dateOriginalSubSecCheck, SIGNAL(toggled(bool)),
            d->dateOriginalSubSecEdit, SLOT(setEnabled(bool)));

    connect(d->dateDigitalizedSubSecCheck, SIGNAL(toggled(bool)),
            d->dateDigitalizedSubSecEdit, SLOT(setEnabled(bool)));

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->syncHOSTDateCheck, SLOT(setEnabled(bool)));

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            d->syncIPTCDateCheck, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->dateCreatedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));
    
    connect(d->dateOriginalCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateCreatedSubSecCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateOriginalSubSecCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedSubSecCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->dateCreatedSubSecEdit, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalModified()));
    
    connect(d->dateOriginalSubSecEdit, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedSubSecEdit, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalModified()));

    connect(d->dateCreatedSel, SIGNAL(valueChanged (const QDateTime &)),
            this, SIGNAL(signalModified()));

    connect(d->dateOriginalSel, SIGNAL(valueChanged (const QDateTime &)),
            this, SIGNAL(signalModified()));

    connect(d->dateDigitalizedSel, SIGNAL(valueChanged (const QDateTime &)),
            this, SIGNAL(signalModified()));
}

EXIFDateTime::~EXIFDateTime()
{
    delete d;
}

bool EXIFDateTime::syncHOSTDateIsChecked()
{
    return d->syncHOSTDateCheck->isChecked();
}

bool EXIFDateTime::syncIPTCDateIsChecked()
{
    return d->syncIPTCDateCheck->isChecked();
}

void EXIFDateTime::setCheckedSyncHOSTDate(bool c)
{
    d->syncHOSTDateCheck->setChecked(c);
}

void EXIFDateTime::setCheckedSyncIPTCDate(bool c)
{
    d->syncIPTCDateCheck->setChecked(c);
}

QDateTime EXIFDateTime::getEXIFCreationDate()
{
    return d->dateCreatedSel->dateTime();
}

void EXIFDateTime::readMetadata(QByteArray& exifData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);

    QDateTime datetime;
    QString datetimeStr, data;

    d->dateCreatedSel->setDateTime(QDateTime::currentDateTime());
    d->dateCreatedCheck->setChecked(false);
    datetimeStr = exiv2Iface.getExifTagString("Exif.Image.DateTime", false);
    if (!datetimeStr.isEmpty()) 
    {
        datetime = QDateTime::fromString(datetimeStr, Qt::ISODate);
        if (datetime.isValid())
        {
            d->dateCreatedSel->setDateTime(datetime);
            d->dateCreatedCheck->setChecked(true);
        }
    }    
    d->dateCreatedSel->setEnabled(d->dateCreatedCheck->isChecked());
    d->syncHOSTDateCheck->setEnabled(d->dateCreatedCheck->isChecked());
    d->syncIPTCDateCheck->setEnabled(d->dateCreatedCheck->isChecked());

    d->dateCreatedSubSecEdit->setValue(0);
    d->dateCreatedSubSecCheck->setChecked(false);
    data = exiv2Iface.getExifTagString("Exif.Photo.SubSecTime", false);    
    if (!data.isNull())
    {
        bool ok    = false;
        int subsec = data.toInt(&ok);
        if (ok)
        {
            d->dateCreatedSubSecEdit->setValue(subsec);
            d->dateCreatedSubSecCheck->setChecked(true);
        }
    }
    d->dateCreatedSubSecEdit->setEnabled(d->dateCreatedSubSecCheck->isChecked());

    d->dateOriginalSel->setDateTime(QDateTime::currentDateTime());
    d->dateOriginalCheck->setChecked(false);
    datetimeStr = exiv2Iface.getExifTagString("Exif.Photo.DateTimeOriginal", false);
    if (!datetimeStr.isEmpty()) 
    {
        datetime = QDateTime::fromString(datetimeStr, Qt::ISODate);
        if (datetime.isValid())
        {
            d->dateOriginalSel->setDateTime(datetime);
            d->dateOriginalCheck->setChecked(true);
        }
    }    
    d->dateOriginalSel->setEnabled(d->dateOriginalCheck->isChecked());

    d->dateOriginalSubSecEdit->setValue(0);
    d->dateOriginalSubSecCheck->setChecked(false);
    data = exiv2Iface.getExifTagString("Exif.Photo.SubSecTimeOriginal", false);    
    if (!data.isNull())
    {
        bool ok    = false;
        int subsec = data.toInt(&ok);
        if (ok)
        {
            d->dateOriginalSubSecEdit->setValue(subsec);
            d->dateOriginalSubSecCheck->setChecked(true);
        }
    }
    d->dateOriginalSubSecEdit->setEnabled(d->dateOriginalSubSecCheck->isChecked());

    d->dateDigitalizedSel->setDateTime(QDateTime::currentDateTime());
    d->dateDigitalizedCheck->setChecked(false);
    datetimeStr = exiv2Iface.getExifTagString("Exif.Photo.DateTimeDigitized", false);
    if (!datetimeStr.isEmpty()) 
    {
        datetime = QDateTime::fromString(datetimeStr, Qt::ISODate);
        if (datetime.isValid())
        {
            d->dateDigitalizedSel->setDateTime(datetime);
            d->dateDigitalizedCheck->setChecked(true);
        }
    }    
    d->dateDigitalizedSel->setEnabled(d->dateDigitalizedCheck->isChecked());

    d->dateDigitalizedSubSecEdit->setValue(0);
    d->dateDigitalizedSubSecCheck->setChecked(false);
    data = exiv2Iface.getExifTagString("Exif.Photo.SubSecTimeDigitized", false);    
    if (!data.isNull())
    {
        bool ok    = false;
        int subsec = data.toInt(&ok);
        if (ok)
        {
            d->dateDigitalizedSubSecEdit->setValue(subsec);
            d->dateDigitalizedSubSecCheck->setChecked(true);
        }
    }
    d->dateDigitalizedSubSecEdit->setEnabled(d->dateDigitalizedSubSecCheck->isChecked());

    blockSignals(false);
}

void EXIFDateTime::applyMetadata(QByteArray& exifData, QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setExif(exifData);
    exiv2Iface.setIptc(iptcData);

    if (d->dateCreatedCheck->isChecked())
    {
        exiv2Iface.setExifTagString("Exif.Image.DateTime",
                   d->dateCreatedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).ascii());

        if (syncIPTCDateIsChecked())
        {
            exiv2Iface.setIptcTagString("Iptc.Application2.DateCreated",
                       d->dateCreatedSel->dateTime().date().toString(Qt::ISODate));
            exiv2Iface.setIptcTagString("Iptc.Application2.TimeCreated",
                       d->dateCreatedSel->dateTime().time().toString(Qt::ISODate));
        }
    }
    else
        exiv2Iface.removeExifTag("Exif.Image.DateTime");

    if (d->dateCreatedSubSecCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.SubSecTime",
                   QString::number(d->dateCreatedSubSecEdit->value()).ascii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.SubSecTime");

    if (d->dateOriginalCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.DateTimeOriginal",
                   d->dateOriginalSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).ascii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.DateTimeOriginal");

    if (d->dateOriginalSubSecCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.SubSecTimeOriginal",
                   QString::number(d->dateOriginalSubSecEdit->value()).ascii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.SubSecTimeOriginal");

    if (d->dateDigitalizedCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.DateTimeDigitized",
                   d->dateDigitalizedSel->dateTime().toString(QString("yyyy:MM:dd hh:mm:ss")).ascii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.DateTimeDigitized");

    if (d->dateDigitalizedSubSecCheck->isChecked())
        exiv2Iface.setExifTagString("Exif.Photo.SubSecTimeDigitized",
                   QString::number(d->dateDigitalizedSubSecEdit->value()).ascii());
    else
        exiv2Iface.removeExifTag("Exif.Photo.SubSecTimeDigitized");

    exifData = exiv2Iface.getExif();
    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

