//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "serviceselectionwidget.h"
#include "deviceaddress.h"
#include "devicemimeconverter.h"
#include <kdialogbase.h>
#include <klistview.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kpixmapeffect.h>
#include <algorithm>
#include <qtimer.h>
#include <qpushbutton.h>
#include <ksqueezedtextlabel.h>
#include <kdebug.h>
#include <kmessagebox.h>

namespace KBluetooth {


ServiceSelectionWidget::ServiceSelectionWidget(QWidget* parent,
    QStringList uuids, bool bInitFromCache, bool bStartScan, bool autoSelect) :
    ServiceSelectionWidgetBase(parent),
    bInitFromCache(bInitFromCache),
    uuids(uuids),
	autoSelect(autoSelect)
{
    statusLabel->setText("");
    serviceDiscovery = new ServiceDiscovery(this, uuids, 
        QString("ServiceDiscoveryCache-")+uuids.join("-"));
    connect(serviceDiscovery, SIGNAL(update()), this, SLOT(slotServiceUpdate()));
    connect(serviceDiscovery, SIGNAL(finished()), this, SLOT(slotInquiryFinished()));

    connect(serviceListBox, SIGNAL(selectionChanged(QListBoxItem*)), this,
        SLOT(slotSelectionChanged(QListBoxItem*)));
    init();
    connect(searchButton, SIGNAL(clicked()), this, SLOT(slotSearch()));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClear()));
    searchButton->setEnabled(false);
    if (bStartScan) {
        slotSearch();
    }
}

void ServiceSelectionWidget::clearSelection()
{
	serviceListBox->clearSelection();
}

const ServiceSelectionWidget::DeviceIcons& ServiceSelectionWidget::getIcons(QString mimetype)
{
    if (mimeToIconMap.find(mimetype) == mimeToIconMap.end()) {
        DeviceIcons icons;
        QString iconName = DeviceClassMimeConverter::mimeTypeToIcon(mimetype);
        KPixmap icon = KGlobal::iconLoader()->loadIcon(
            iconName, KIcon::Small, 16);
        
        icons.verifiedServiceIcon = icon;
        KPixmapEffect::intensity(icons.verifiedServiceIcon, +0.3);
        
        icons.cachedServiceIcon = icon;
        KPixmapEffect::intensity(icons.cachedServiceIcon, -0.3);
        KPixmapEffect::toGray(icons.cachedServiceIcon);
        
        icons.inquiriedServiceIcon = icon;
        KPixmapEffect::intensity(icons.inquiriedServiceIcon, -0.1);
        
        mimeToIconMap[mimetype] = icons;
    }
    return mimeToIconMap[mimetype];
}


ServiceSelectionWidget::~ServiceSelectionWidget()
{

}

void ServiceSelectionWidget::initFromCache()
{
    bInitFromCache = true;
    init();
}

void ServiceSelectionWidget::init()
{
    serviceListBox->clear();
}

void ServiceSelectionWidget::scan()
{
    initFromCache();
    searchButton->setEnabled(false);
    statusLabel->setText(i18n("Searching..."));
    serviceDiscovery->inquiry();
}

DeviceAddress ServiceSelectionWidget::selectedAddress()
{
    if (serviceListBox->currentItem() >= 0) {
        serviceInfos[serviceListBox->currentItem()]->use();
        return serviceInfos[serviceListBox->currentItem()]->address();
    }
    else {
        return DeviceAddress::invalid;
    }
}

uint ServiceSelectionWidget::selectedChannel()
{
    if (serviceListBox->currentItem() >= 0) {
        serviceInfos[serviceListBox->currentItem()]->use();
        return serviceInfos[serviceListBox->currentItem()]->rfcommChannel();
    }
    else {
        return 0;
    }
}

void ServiceSelectionWidget::slotInquiryFinished()
{
    // When the inquiry is finnished we simply start it again
    // after a short period of time.
    //QTimer::singleShot(1000, serviceDiscovery, SLOT(inquiry()));
    searchButton->setEnabled(true);
    statusLabel->setText("");
}

void ServiceSelectionWidget::slotServiceUpdate()
{
    // Save the address and service name of the currently selected item
    int selectedItem = serviceListBox->index(serviceListBox->selectedItem());
    DeviceAddress selectedAddress;
    QString selectedServiceName;
    if (selectedItem > -1) {
        selectedAddress = serviceInfos[selectedItem]->address();
        selectedServiceName = serviceInfos[selectedItem]->serviceName();
    }
    kdDebug() << "Item " << selectedItem << endl;
    // sort and filter the service list according to our filter/sort rules
    serviceInfos = this->filter(this->sort(serviceDiscovery->getServices()));

    init();

    ServiceDiscovery::ServiceInfoVector::iterator it;
    for (it = serviceInfos.begin(); it != serviceInfos.end(); ++it) {
        KPixmap selectedPixmap;
        DeviceIcons icons = getIcons(DeviceClassMimeConverter::classToMimeType(
            (*it)->deviceClass()));
        
        if ((*it)->isChannelVerified()) {
            selectedPixmap = icons.verifiedServiceIcon;
        }
        else if ((*it)->isAddressVerified()) {
            selectedPixmap = icons.inquiriedServiceIcon;
        }
        else {
            selectedPixmap = icons.cachedServiceIcon;
        }
        //selectedPixmap = DeviceClassMimeConverter::classToIcon(
        //    (*it)->deviceClass()).pixmap(QIconSet::Small, QIconSet::Normal);
        QListBoxPixmap* item = new QListBoxPixmap(selectedPixmap,
            QString("%1 - %2").arg((*it)->deviceName()).arg((*it)->serviceName()));
        serviceListBox->insertItem(item);

        // Restore the selection
        if (selectedItem > -1) {
            if ((*it)->serviceName() == selectedServiceName &&
                (*it)->address() == selectedAddress) {
                serviceListBox->setSelected(item, true);
                serviceListBox->setCurrentItem(item);
                serviceListBox->ensureCurrentVisible();
            }
        }
    }

    // if nothing was selected, we select the first item
	if(autoSelect) {
    	if (selectedItem == -1 && serviceListBox->count() > 0) {
        	serviceListBox->setSelected(0, true);
    	}
	}
}


bool ServiceSelectionWidget::showSelectionDialog(QWidget* parent, QStringList uuids,
        KBluetooth::DeviceAddress &address, int &rfcommChannel)
{
    KDialogBase dialog(parent, "serviceselectiondlg", true,
        i18n("Bluetooth Service Selection"), KDialogBase::Ok|KDialogBase::Cancel);
    ServiceSelectionWidget *widget = new ServiceSelectionWidget(
        &dialog, uuids);
    dialog.setMainWidget(widget);
    connect(widget->serviceListBox, SIGNAL(returnPressed(QListBoxItem*)),
        &dialog, SLOT(accept()));
    widget->scan();
    bool ret;
    if (dialog.exec() == QDialog::Accepted) {
        address = widget->selectedAddress();
        rfcommChannel = widget->selectedChannel();
        ret = true;
    }
    else {
        ret = false;
    }
    delete widget;
    return ret;
}

ServiceSelectionWidget::DefaultPredicate::DefaultPredicate(ServiceSelectionWidget* owner) :
    owner(owner)
{

}

bool ServiceSelectionWidget::DefaultPredicate::operator()(
    const ServiceDiscovery::ServiceInfo* a,
    const ServiceDiscovery::ServiceInfo* b)
{
    return owner->lessThan(a, b);
}

ServiceDiscovery::ServiceInfoVector ServiceSelectionWidget::sort(
    const ServiceDiscovery::ServiceInfoVector& services)
{
    ServiceDiscovery::ServiceInfoVector ret = services;
    DefaultPredicate defaultPredicate(this);
    std::sort(ret.begin(), ret.end(), defaultPredicate);
    return ret;
}


bool ServiceSelectionWidget::lessThan(
    const ServiceDiscovery::ServiceInfo* a,
    const ServiceDiscovery::ServiceInfo* b)
{
    // This is the default sorting criteria.
    // The first condition is the most relevant one.
    // If both services are equal regarding that condition,
    // the following conditions are evaluated.

    // Unverified services go down in the list
    if (a->isAddressVerified() != b->isAddressVerified()) {
        kdDebug() << "Sort criteria 'addressVerified': "
            << a->isAddressVerified() << " < " << b->isAddressVerified() << endl;
        return a->isAddressVerified();
    }
    // sort by date of last usage
    if (a->lastUsed() != b->lastUsed()) {
        kdDebug() << "Sort criteria 'lastUsed': "
            << a->lastUsed().toString() << " > " << b->lastUsed().toString() << endl;
        return a->lastUsed() > b->lastUsed();
    }
    // Sort by last seen date
    if (a->lastSeen() != b->lastSeen()) {
        kdDebug() << "Sort criteria 'lastSeen': "
            << a->lastSeen().toString() << " > " << b->lastSeen().toString() << endl;
        return a->lastSeen() > b->lastSeen();
    }
    kdDebug() << "Sort criteria 'none'" << endl;
    return false;
}

void ServiceSelectionWidget::slotSelectionChanged(QListBoxItem*)
{
    emit serviceChanged (selectedAddress(), selectedChannel());
}

ServiceDiscovery::ServiceInfoVector ServiceSelectionWidget::filter(
    const ServiceDiscovery::ServiceInfoVector& services)
{
    const int maxLRU = 5;
    // Filter out all completely unverified services,
    // except for the first <maxLRU> of them.
    ServiceDiscovery::ServiceInfoVector ret;
    ServiceDiscovery::ServiceInfoVector::const_iterator it;
    int lruCount = 0;
    for (it = services.begin(); it != services.end(); ++it) {
        ServiceDiscovery::ServiceInfo* info = *it;
        if (info->isAddressVerified() == true) {
            ret.push_back(info);
        }
        else if (lruCount<maxLRU) {
            ret.push_back(info);
            ++lruCount;
        }
    }
    return ret;
}


void ServiceSelectionWidget::slotSearch()
{
    scan();
}

void ServiceSelectionWidget::slotClear()
{
    if (KMessageBox::questionYesNo(this, i18n("Clear the device cache?")) == 
        KMessageBox::Yes) 
    {
        serviceDiscovery->clearCache();
        initFromCache();
    }
}

}
#include "serviceselectionwidget.moc"
