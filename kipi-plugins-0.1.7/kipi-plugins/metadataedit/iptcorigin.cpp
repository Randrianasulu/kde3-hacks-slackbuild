/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-13
 * Description : IPTC origin settings page.
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
#include <qlistbox.h>
#include <qcombobox.h>
#include <qmap.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <kactivelabel.h>
#include <kseparator.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "metadatacheckbox.h"
#include "iptcorigin.h"
#include "iptcorigin.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCOriginPriv
{
public:

    IPTCOriginPriv()
    {
        objectNameEdit     = 0;
        cityEdit           = 0;
        sublocationEdit    = 0;
        provinceEdit       = 0;
        locationEdit       = 0;
        originalTransEdit  = 0;
        objectNameCheck    = 0;
        cityCheck          = 0;
        sublocationCheck   = 0;
        provinceCheck      = 0;
        countryCheck       = 0;
        locationCheck      = 0;
        originalTransCheck = 0;

        // Standard ISO 3166 country codes.

        countryCodeMap.insert( "AFG", i18n("Afghanistan") );
        countryCodeMap.insert( "ALB", i18n("Albania") );
        countryCodeMap.insert( "DZA", i18n("Algeria") );
        countryCodeMap.insert( "ASM", i18n("American Samoa") );
        countryCodeMap.insert( "AND", i18n("Andorra") );
        countryCodeMap.insert( "AGO", i18n("Angola") );
        countryCodeMap.insert( "AIA", i18n("Anguilla") );
        countryCodeMap.insert( "AGO", i18n("Angola") );
        countryCodeMap.insert( "ATA", i18n("Antarctica") );
        countryCodeMap.insert( "ATG", i18n("Antigua and Barbuda") );
        countryCodeMap.insert( "ARG", i18n("Argentina") );
        countryCodeMap.insert( "ARM", i18n("Armenia") );
        countryCodeMap.insert( "ABW", i18n("Aruba") );
        countryCodeMap.insert( "AUS", i18n("Australia") );
        countryCodeMap.insert( "AUT", i18n("Austria") );
        countryCodeMap.insert( "AZE", i18n("Azerbaijan") );
        countryCodeMap.insert( "BHS", i18n("Bahamas") );
        countryCodeMap.insert( "BHR", i18n("Bahrain") );
        countryCodeMap.insert( "BGD", i18n("Bangladesh") );
        countryCodeMap.insert( "BRB", i18n("Barbados") );
        countryCodeMap.insert( "BLR", i18n("Belarus") );
        countryCodeMap.insert( "BEL", i18n("Belgium") );
        countryCodeMap.insert( "BLZ", i18n("Belize") );
        countryCodeMap.insert( "BEN", i18n("Benin") );
        countryCodeMap.insert( "BMU", i18n("Bermuda") );
        countryCodeMap.insert( "BTN", i18n("Bhutan") );
        countryCodeMap.insert( "BOL", i18n("Bolivia") );
        countryCodeMap.insert( "BIH", i18n("Bosnia and Herzegovina") );
        countryCodeMap.insert( "BWA", i18n("Botswana") );
        countryCodeMap.insert( "BVT", i18n("Bouvet Island") );
        countryCodeMap.insert( "BRA", i18n("Brazil") );
        countryCodeMap.insert( "IOT", i18n("British Indian Ocean Territory") );
        countryCodeMap.insert( "VGB", i18n("British Virgin Islands") );
        countryCodeMap.insert( "BRN", i18n("Brunei Darussalam") );
        countryCodeMap.insert( "BGR", i18n("Bulgaria") );
        countryCodeMap.insert( "BFA", i18n("Burkina Faso") );
        countryCodeMap.insert( "BDI", i18n("Burundi") );
        countryCodeMap.insert( "KHM", i18n("Cambodia") );
        countryCodeMap.insert( "CMR", i18n("Cameroon") );
        countryCodeMap.insert( "CAN", i18n("Canada") );
        countryCodeMap.insert( "CPV", i18n("Cape Verde") );
        countryCodeMap.insert( "CYM", i18n("Cayman Islands") );
        countryCodeMap.insert( "CAF", i18n("Central African Republic") );
        countryCodeMap.insert( "TCD", i18n("Chad") );
        countryCodeMap.insert( "CHL", i18n("Chile") );
        countryCodeMap.insert( "CHN", i18n("China") );
        countryCodeMap.insert( "CXR", i18n("Christmas Island ") );
        countryCodeMap.insert( "CCK", i18n("Cocos Islands") );
        countryCodeMap.insert( "COL", i18n("Colombia") );
        countryCodeMap.insert( "COM", i18n("Comoros") );
        countryCodeMap.insert( "COD", i18n("Zaire") );
        countryCodeMap.insert( "COG", i18n("Congo") );
        countryCodeMap.insert( "COK", i18n("Cook Islands") );
        countryCodeMap.insert( "CRI", i18n("Costa Rica") );
        countryCodeMap.insert( "CIV", i18n("Ivory Coast") );
        countryCodeMap.insert( "CUB", i18n("Cuba") );
        countryCodeMap.insert( "CYP", i18n("Cyprus") );
        countryCodeMap.insert( "CZE", i18n("Czech Republic") );
        countryCodeMap.insert( "DNK", i18n("Denmark") );
        countryCodeMap.insert( "DJI", i18n("Djibouti") );
        countryCodeMap.insert( "DMA", i18n("Dominica") );
        countryCodeMap.insert( "DOM", i18n("Dominican Republic") );
        countryCodeMap.insert( "ECU", i18n("Ecuador") );
        countryCodeMap.insert( "EGY", i18n("Egypt") );
        countryCodeMap.insert( "SLV", i18n("El Salvador") );
        countryCodeMap.insert( "GNQ", i18n("Equatorial Guinea") );
        countryCodeMap.insert( "ERI", i18n("Eritrea") );
        countryCodeMap.insert( "EST", i18n("Estonia") );
        countryCodeMap.insert( "ETH", i18n("Ethiopia") );
        countryCodeMap.insert( "FRO", i18n("Faeroe Islands") );
        countryCodeMap.insert( "FLK", i18n("Falkland Islands") );
        countryCodeMap.insert( "FJI", i18n("Fiji Islands") );
        countryCodeMap.insert( "FIN", i18n("Finland") );
        countryCodeMap.insert( "FRA", i18n("France") );
        countryCodeMap.insert( "GUF", i18n("French Guiana") );
        countryCodeMap.insert( "PYF", i18n("French Polynesia") );
        countryCodeMap.insert( "ATF", i18n("French Southern Territories") );
        countryCodeMap.insert( "GAB", i18n("Gabon") );
        countryCodeMap.insert( "GMB", i18n("Gambia") );
        countryCodeMap.insert( "GEO", i18n("Georgia") );
        countryCodeMap.insert( "DEU", i18n("Germany") );
        countryCodeMap.insert( "GHA", i18n("Ghana") );
        countryCodeMap.insert( "GIB", i18n("Gibraltar") );
        countryCodeMap.insert( "GRC", i18n("Greece") );
        countryCodeMap.insert( "GRL", i18n("Greenland") );
        countryCodeMap.insert( "GRD", i18n("Grenada") );
        countryCodeMap.insert( "GLP", i18n("Guadaloupe") );
        countryCodeMap.insert( "GUM", i18n("Guam") );
        countryCodeMap.insert( "GTM", i18n("Guatemala") );
        countryCodeMap.insert( "GIN", i18n("Guinea") );
        countryCodeMap.insert( "GNB", i18n("Guinea-Bissau") );
        countryCodeMap.insert( "GUY", i18n("Guyana") );
        countryCodeMap.insert( "HTI", i18n("Haiti") );
        countryCodeMap.insert( "HMD", i18n("Heard and McDonald Islands") );
        countryCodeMap.insert( "VAT", i18n("Vatican") );
        countryCodeMap.insert( "HND", i18n("Honduras") );
        countryCodeMap.insert( "HKG", i18n("Hong Kong") );
        countryCodeMap.insert( "HRV", i18n("Croatia") );
        countryCodeMap.insert( "HUN", i18n("Hungary") );
        countryCodeMap.insert( "ISL", i18n("Iceland") );
        countryCodeMap.insert( "IND", i18n("India") );
        countryCodeMap.insert( "IDN", i18n("Indonesia") );
        countryCodeMap.insert( "IRN", i18n("Iran") );
        countryCodeMap.insert( "IRQ", i18n("Iraq") );
        countryCodeMap.insert( "IRL", i18n("Ireland") );
        countryCodeMap.insert( "ISR", i18n("Israel") );
        countryCodeMap.insert( "ITA", i18n("Italy") );
        countryCodeMap.insert( "JAM", i18n("Jamaica") );
        countryCodeMap.insert( "JPN", i18n("Japan") );
        countryCodeMap.insert( "JOR", i18n("Jordan") );
        countryCodeMap.insert( "KAZ", i18n("Kazakhstan") );
        countryCodeMap.insert( "KEN", i18n("Kenya") );
        countryCodeMap.insert( "KIR", i18n("Kiribati") );
        countryCodeMap.insert( "PRK", i18n("Korea") );
        countryCodeMap.insert( "KOR", i18n("Korea") );
        countryCodeMap.insert( "KWT", i18n("Kuwait") );
        countryCodeMap.insert( "KGZ", i18n("Kyrgyz Republic") );
        countryCodeMap.insert( "LAO", i18n("Lao") );
        countryCodeMap.insert( "LVA", i18n("Latvia") );
        countryCodeMap.insert( "LBN", i18n("Lebanon") );
        countryCodeMap.insert( "LSO", i18n("Lesotho") );
        countryCodeMap.insert( "LBR", i18n("Liberia") );
        countryCodeMap.insert( "LBY", i18n("Libyan Arab Jamahiriya") );
        countryCodeMap.insert( "LIE", i18n("Liechtenstein") );
        countryCodeMap.insert( "LTU", i18n("Lithuania") );
        countryCodeMap.insert( "LUX", i18n("Luxembourg") );
        countryCodeMap.insert( "MAC", i18n("Macao") );
        countryCodeMap.insert( "MKD", i18n("Macedonia") );
        countryCodeMap.insert( "MDG", i18n("Madagascar") );
        countryCodeMap.insert( "MWI", i18n("Malawi") );
        countryCodeMap.insert( "MYS", i18n("Malaysia") );
        countryCodeMap.insert( "MDV", i18n("Maldives") );
        countryCodeMap.insert( "MLI", i18n("Mali") );
        countryCodeMap.insert( "MLT", i18n("Malta") );
        countryCodeMap.insert( "MHL", i18n("Marshall Islands") );
        countryCodeMap.insert( "MTQ", i18n("Martinique") );
        countryCodeMap.insert( "MRT", i18n("Mauritania") );
        countryCodeMap.insert( "MUS", i18n("Mauritius") );
        countryCodeMap.insert( "MYT", i18n("Mayotte") );
        countryCodeMap.insert( "MEX", i18n("Mexico") );
        countryCodeMap.insert( "FSM", i18n("Micronesia") );
        countryCodeMap.insert( "MDA", i18n("Moldova") );
        countryCodeMap.insert( "MCO", i18n("Monaco") );
        countryCodeMap.insert( "MNG", i18n("Mongolia") );
        countryCodeMap.insert( "MSR", i18n("Montserrat") );
        countryCodeMap.insert( "MAR", i18n("Morocco") );
        countryCodeMap.insert( "MOZ", i18n("Mozambique") );
        countryCodeMap.insert( "MMR", i18n("Myanmar") );
        countryCodeMap.insert( "NAM", i18n("Namibia") );
        countryCodeMap.insert( "NRU", i18n("Nauru") );
        countryCodeMap.insert( "NPL", i18n("Nepal") );
        countryCodeMap.insert( "ANT", i18n("Netherlands Antilles") );
        countryCodeMap.insert( "NLD", i18n("Netherlands") );
        countryCodeMap.insert( "NCL", i18n("New Caledonia") );
        countryCodeMap.insert( "NZL", i18n("New Zealand") );
        countryCodeMap.insert( "NIC", i18n("Nicaragua") );
        countryCodeMap.insert( "NER", i18n("Niger") );
        countryCodeMap.insert( "NGA", i18n("Nigeria") );
        countryCodeMap.insert( "NIU", i18n("Niue") );
        countryCodeMap.insert( "NFK", i18n("Norfolk Island") );
        countryCodeMap.insert( "MNP", i18n("Northern Mariana Islands") );
        countryCodeMap.insert( "NOR", i18n("Norway") );
        countryCodeMap.insert( "OMN", i18n("Oman") );
        countryCodeMap.insert( "PAK", i18n("Pakistan") );
        countryCodeMap.insert( "PLW", i18n("Palau") );
        countryCodeMap.insert( "PSE", i18n("Palestinian Territory") );
        countryCodeMap.insert( "PAN", i18n("Panama") );
        countryCodeMap.insert( "PNG", i18n("Papua New Guinea") );
        countryCodeMap.insert( "PRY", i18n("Paraguay") );
        countryCodeMap.insert( "PER", i18n("Peru") );
        countryCodeMap.insert( "PHL", i18n("Philippines") );
        countryCodeMap.insert( "PCN", i18n("Pitcairn Island") );
        countryCodeMap.insert( "POL", i18n("Poland") );
        countryCodeMap.insert( "PRT", i18n("Portugal") );
        countryCodeMap.insert( "PRI", i18n("Puerto Rico") );
        countryCodeMap.insert( "QAT", i18n("Qatar") );
        countryCodeMap.insert( "REU", i18n("Reunion") );
        countryCodeMap.insert( "ROU", i18n("Romania") );
        countryCodeMap.insert( "RUS", i18n("Russian Federation") );
        countryCodeMap.insert( "RWA", i18n("Rwanda") );
        countryCodeMap.insert( "SHN", i18n("St. Helena") );
        countryCodeMap.insert( "KNA", i18n("St. Kitts and Nevis") );
        countryCodeMap.insert( "LCA", i18n("St. Lucia") );
        countryCodeMap.insert( "SPM", i18n("St. Pierre and Miquelon") );
        countryCodeMap.insert( "VCT", i18n("St. Vincent and the Grenadines") );
        countryCodeMap.insert( "WSM", i18n("Samoa") );
        countryCodeMap.insert( "SMR", i18n("San Marino") );
        countryCodeMap.insert( "STP", i18n("Sao Tome and Principe") );
        countryCodeMap.insert( "SAU", i18n("Saudi Arabia") );
        countryCodeMap.insert( "SEN", i18n("Senegal") );
        countryCodeMap.insert( "SCG", i18n("Serbia and Montenegro") );
        countryCodeMap.insert( "SYC", i18n("Seychelles") );
        countryCodeMap.insert( "SLE", i18n("Sierra Leone") );
        countryCodeMap.insert( "SGP", i18n("Singapore") );
        countryCodeMap.insert( "SVK", i18n("Slovakia") );
        countryCodeMap.insert( "SVN", i18n("Slovenia") );
        countryCodeMap.insert( "SLB", i18n("Solomon Islands") );
        countryCodeMap.insert( "SOM", i18n("Somalia") );
        countryCodeMap.insert( "ZAF", i18n("South Africa") );
        countryCodeMap.insert( "SGS", i18n("South Georgia and the South Sandwich Islands") );
        countryCodeMap.insert( "ESP", i18n("Spain") );
        countryCodeMap.insert( "LKA", i18n("Sri Lanka") );
        countryCodeMap.insert( "SDN", i18n("Sudan") );
        countryCodeMap.insert( "SUR", i18n("Suriname") );
        countryCodeMap.insert( "SJM", i18n("Svalbard & Jan Mayen Islands") );
        countryCodeMap.insert( "SWZ", i18n("Swaziland") );
        countryCodeMap.insert( "SWE", i18n("Sweden") );
        countryCodeMap.insert( "CHE", i18n("Switzerland") );
        countryCodeMap.insert( "SYR", i18n("Syrian Arab Republic") );
        countryCodeMap.insert( "TWN", i18n("Taiwan") );
        countryCodeMap.insert( "TJK", i18n("Tajikistan") );
        countryCodeMap.insert( "TZA", i18n("Tanzania") );
        countryCodeMap.insert( "THA", i18n("Thailand") );
        countryCodeMap.insert( "TLS", i18n("Timor-Leste") );
        countryCodeMap.insert( "TGO", i18n("Togo") );
        countryCodeMap.insert( "TKL", i18n("Tokelau Islands") );
        countryCodeMap.insert( "TON", i18n("Tonga") );
        countryCodeMap.insert( "TTO", i18n("Trinidad and Tobago") );
        countryCodeMap.insert( "TUN", i18n("Tunisia") );
        countryCodeMap.insert( "TUR", i18n("Turkey") );
        countryCodeMap.insert( "TKM", i18n("Turkmenistan") );
        countryCodeMap.insert( "TCA", i18n("Turks and Caicos Islands") );
        countryCodeMap.insert( "TUV", i18n("Tuvalu") );
        countryCodeMap.insert( "VIR", i18n("US Virgin Islands") );
        countryCodeMap.insert( "UGA", i18n("Uganda") );
        countryCodeMap.insert( "UKR", i18n("Ukraine") );
        countryCodeMap.insert( "ARE", i18n("United Arab Emirates") );
        countryCodeMap.insert( "GBR", i18n("United Kingdom") );
        countryCodeMap.insert( "UMI", i18n("United States Minor Outlying Islands") );
        countryCodeMap.insert( "USA", i18n("United States of America") );
        countryCodeMap.insert( "URY", i18n("Uruguay, Eastern Republic of") );
        countryCodeMap.insert( "UZB", i18n("Uzbekistan") );
        countryCodeMap.insert( "VUT", i18n("Vanuatu") );
        countryCodeMap.insert( "VEN", i18n("Venezuela") );
        countryCodeMap.insert( "VNM", i18n("Viet Nam") );
        countryCodeMap.insert( "WLF", i18n("Wallis and Futuna Islands ") );
        countryCodeMap.insert( "ESH", i18n("Western Sahara") );
        countryCodeMap.insert( "YEM", i18n("Yemen") );
        countryCodeMap.insert( "ZMB", i18n("Zambia") );
        countryCodeMap.insert( "ZWE", i18n("Zimbabwe") );

        // Supplemental IPTC country codes.

        countryCodeMap.insert( "XUN", i18n("United Nations") );
        countryCodeMap.insert( "XEU", i18n("European Union") );
        countryCodeMap.insert( "XSP", i18n("Space") );
        countryCodeMap.insert( "XSE", i18n("At Sea") );
        countryCodeMap.insert( "XIF", i18n("In Flight") );
        countryCodeMap.insert( "XEN", i18n("England") );
        countryCodeMap.insert( "XSC", i18n("Scotland") );
        countryCodeMap.insert( "XNI", i18n("Northern Ireland") );
        countryCodeMap.insert( "XWA", i18n("Wales") );
        countryCodeMap.insert( "PSE", i18n("Palestine") );
        countryCodeMap.insert( "GZA", i18n("Gaza") );
        countryCodeMap.insert( "JRO", i18n("Jericho") );
    }

    typedef QMap<QString, QString> CountryCodeMap; 

    CountryCodeMap    countryCodeMap;

    QComboBox        *countryCB;

    QCheckBox        *objectNameCheck;
    QCheckBox        *cityCheck;
    QCheckBox        *sublocationCheck;
    QCheckBox        *provinceCheck;
    QCheckBox        *locationCheck;
    QCheckBox        *originalTransCheck;

    KLineEdit        *objectNameEdit;
    KLineEdit        *cityEdit;
    KLineEdit        *sublocationEdit;
    KLineEdit        *provinceEdit;
    KLineEdit        *locationEdit;
    KLineEdit        *originalTransEdit;

    MetadataCheckBox *countryCheck;
};

IPTCOrigin::IPTCOrigin(QWidget* parent)
          : QWidget(parent)
{
    d = new IPTCOriginPriv;

    QGridLayout* grid = new QGridLayout(parent, 12, 2, KDialog::spacingHint());

    // IPTC only accept printable Ascii char.
    QRegExp asciiRx("[\x20-\x7F]+$");
    QValidator *asciiValidator = new QRegExpValidator(asciiRx, this);

    // --------------------------------------------------------

    d->objectNameCheck = new QCheckBox(i18n("Object name:"), parent);
    d->objectNameEdit  = new KLineEdit(parent);
    d->objectNameEdit->setValidator(asciiValidator);
    d->objectNameEdit->setMaxLength(64);
    QWhatsThis::add(d->objectNameEdit, i18n("<p>Set here the shorthand reference of content. "
                    "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->locationCheck = new QCheckBox(i18n("Location:"), parent);
    d->locationEdit  = new KLineEdit(parent);
    d->locationEdit->setValidator(asciiValidator);
    d->locationEdit->setMaxLength(64);
    QWhatsThis::add(d->locationEdit, i18n("<p>Set here the full country name referenced by the content. "
                                          "This field is limited to 64 ASCII characters."));


    // --------------------------------------------------------

    d->cityCheck = new QCheckBox(i18n("City:"), parent);
    d->cityEdit  = new KLineEdit(parent);
    d->cityEdit->setValidator(asciiValidator);
    d->cityEdit->setMaxLength(32);
    QWhatsThis::add(d->cityEdit, i18n("<p>Set here the city of content origin. "
                                      "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->sublocationCheck = new QCheckBox(i18n("Sublocation:"), parent);
    d->sublocationEdit  = new KLineEdit(parent);
    d->sublocationEdit->setValidator(asciiValidator);
    d->sublocationEdit->setMaxLength(32);
    QWhatsThis::add(d->sublocationEdit, i18n("<p>Set here the content location within city. "
                                             "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->provinceCheck = new QCheckBox(i18n("State/Province:"), parent);
    d->provinceEdit  = new KLineEdit(parent);
    d->provinceEdit->setValidator(asciiValidator);
    d->provinceEdit->setMaxLength(32);
    QWhatsThis::add(d->provinceEdit, i18n("<p>Set here the Province or State of content origin. "
                                          "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->countryCheck = new MetadataCheckBox(i18n("Country:"), parent);
    d->countryCB    = new QComboBox(false, parent);

    for (IPTCOriginPriv::CountryCodeMap::Iterator it = d->countryCodeMap.begin();
         it != d->countryCodeMap.end(); ++it )
        d->countryCB->insertItem(QString("%1 - %2").arg(it.key()).arg(it.data()));

    d->countryCB->listBox()->sort();

    QWhatsThis::add(d->countryCB, i18n("<p>Select here country name of content origin."));

    // --------------------------------------------------------

    d->originalTransCheck = new QCheckBox(i18n("Original transmission reference:"), parent);
    d->originalTransEdit  = new KLineEdit(parent);
    d->originalTransEdit->setValidator(asciiValidator);
    d->originalTransEdit->setMaxLength(32);
    QWhatsThis::add(d->originalTransEdit, i18n("<p>Set here the location of original content transmission. "
                    "This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    KActiveLabel *note = new KActiveLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters set and limit strings size. "
                 "Use contextual help for details.</b>"), parent);

    // --------------------------------------------------------

    grid->addMultiCellWidget(d->objectNameCheck, 0, 0, 0, 2);
    grid->addMultiCellWidget(d->objectNameEdit, 1, 1, 0, 2);
    grid->addMultiCellWidget(d->locationCheck, 2, 2, 0, 0);
    grid->addMultiCellWidget(d->locationEdit, 2, 2, 1, 2);
    grid->addMultiCellWidget(new KSeparator(Qt::Horizontal, parent), 3, 3, 0, 2);
    grid->addMultiCellWidget(d->cityCheck, 4, 4, 0, 0);
    grid->addMultiCellWidget(d->cityEdit, 4, 4, 1, 2);
    grid->addMultiCellWidget(d->sublocationCheck, 5, 5, 0, 0);
    grid->addMultiCellWidget(d->sublocationEdit, 5, 5, 1, 2);
    grid->addMultiCellWidget(d->provinceCheck, 6, 6, 0, 0);
    grid->addMultiCellWidget(d->provinceEdit, 6, 6, 1, 2);
    grid->addMultiCellWidget(d->countryCheck, 7, 7, 0, 0);
    grid->addMultiCellWidget(d->countryCB, 7, 7, 1, 2);
    grid->addMultiCellWidget(new KSeparator(Qt::Horizontal, parent), 8, 8, 0, 2);
    grid->addMultiCellWidget(d->originalTransCheck, 9, 9, 0, 2);
    grid->addMultiCellWidget(d->originalTransEdit, 10, 10, 0, 2);
    grid->addMultiCellWidget(note, 11, 11, 0, 2);
    grid->setColStretch(2, 10);
    grid->setRowStretch(12, 10);

    // --------------------------------------------------------

    connect(d->objectNameCheck, SIGNAL(toggled(bool)),
            d->objectNameEdit, SLOT(setEnabled(bool)));

    connect(d->cityCheck, SIGNAL(toggled(bool)),
            d->cityEdit, SLOT(setEnabled(bool)));

    connect(d->sublocationCheck, SIGNAL(toggled(bool)),
            d->sublocationEdit, SLOT(setEnabled(bool)));

    connect(d->provinceCheck, SIGNAL(toggled(bool)),
            d->provinceEdit, SLOT(setEnabled(bool)));

    connect(d->countryCheck, SIGNAL(toggled(bool)),
            d->countryCB, SLOT(setEnabled(bool)));

    connect(d->locationCheck, SIGNAL(toggled(bool)),
            d->locationEdit, SLOT(setEnabled(bool)));

    connect(d->originalTransCheck, SIGNAL(toggled(bool)),
            d->originalTransEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->objectNameCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->cityCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->sublocationCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->provinceCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->countryCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->locationCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->originalTransCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->countryCB, SIGNAL(activated(int)),
            this, SIGNAL(signalModified()));

    connect(d->objectNameEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->cityEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->sublocationEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->provinceEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->locationEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));

    connect(d->originalTransEdit, SIGNAL(textChanged(const QString &)),
            this, SIGNAL(signalModified()));
}

IPTCOrigin::~IPTCOrigin()
{
    delete d;
}

void IPTCOrigin::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QString data;

    d->objectNameEdit->clear();
    d->objectNameCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.ObjectName", false);    
    if (!data.isNull())
    {
        d->objectNameEdit->setText(data);
        d->objectNameCheck->setChecked(true);
    }
    d->objectNameEdit->setEnabled(d->objectNameCheck->isChecked());

    d->locationEdit->clear();
    d->locationCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.LocationName", false);    
    if (!data.isNull())
    {
        d->locationEdit->setText(data);
        d->locationCheck->setChecked(true);
    }
    d->locationEdit->setEnabled(d->locationCheck->isChecked());

    d->cityEdit->clear();
    d->cityCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.City", false);    
    if (!data.isNull())
    {
        d->cityEdit->setText(data);
        d->cityCheck->setChecked(true);
    }
    d->cityEdit->setEnabled(d->cityCheck->isChecked());

    d->sublocationEdit->clear();
    d->sublocationCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.SubLocation", false);    
    if (!data.isNull())
    {
        d->sublocationEdit->setText(data);
        d->sublocationCheck->setChecked(true);
    }
    d->sublocationEdit->setEnabled(d->sublocationCheck->isChecked());

    d->provinceEdit->clear();
    d->provinceCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.ProvinceState", false);    
    if (!data.isNull())
    {
        d->provinceEdit->setText(data);
        d->provinceCheck->setChecked(true);
    }
    d->provinceEdit->setEnabled(d->provinceCheck->isChecked());

    d->countryCB->setCurrentItem(0);
    d->countryCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.CountryCode", false);    
    if (!data.isNull())
    {
        int item = -1;
        for (int i = 0 ; i < d->countryCB->count() ; i++)
            if (d->countryCB->text(i).left(3) == data)
                item = i;

        if (item != -1)
        {
            d->countryCB->setCurrentItem(item);
            d->countryCheck->setChecked(true);
        }
        else
            d->countryCheck->setValid(false);
    }
    d->countryCB->setEnabled(d->countryCheck->isChecked());

    d->originalTransEdit->clear();
    d->originalTransCheck->setChecked(false);
    data = exiv2Iface.getIptcTagString("Iptc.Application2.TransmissionReference", false);    
    if (!data.isNull())
    {
        d->originalTransEdit->setText(data);
        d->originalTransCheck->setChecked(true);
    }
    d->originalTransEdit->setEnabled(d->originalTransCheck->isChecked());

    blockSignals(false);
}

void IPTCOrigin::applyMetadata(QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);

    if (d->objectNameCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.ObjectName", d->objectNameEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.ObjectName");

    if (d->locationCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.LocationName", d->locationEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.LocationName");

    if (d->cityCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.City", d->cityEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.City");

    if (d->sublocationCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.SubLocation", d->sublocationEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.SubLocation");

    if (d->provinceCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.ProvinceState", d->provinceEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.ProvinceState");

    if (d->countryCheck->isChecked())
    {
        QString countryName = d->countryCB->currentText().mid(6);
        QString countryCode = d->countryCB->currentText().left(3);
        exiv2Iface.setIptcTagString("Iptc.Application2.CountryCode", countryCode);
        exiv2Iface.setIptcTagString("Iptc.Application2.CountryName", countryName);
    }
    else if (d->countryCheck->isValid())
    {
        exiv2Iface.removeIptcTag("Iptc.Application2.CountryCode");
        exiv2Iface.removeIptcTag("Iptc.Application2.CountryName");
    }

    if (d->originalTransCheck->isChecked())
        exiv2Iface.setIptcTagString("Iptc.Application2.TransmissionReference", d->originalTransEdit->text());
    else
        exiv2Iface.removeIptcTag("Iptc.Application2.TransmissionReference");

    iptcData = exiv2Iface.getIptc();
}

}  // namespace KIPIMetadataEditPlugin

