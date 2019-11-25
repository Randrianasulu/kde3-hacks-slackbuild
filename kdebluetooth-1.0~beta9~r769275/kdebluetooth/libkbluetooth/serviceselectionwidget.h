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

#ifndef KBLUETOOTHSERVICESELECTIONWIDGET_H
#define KBLUETOOTHSERVICESELECTIONWIDGET_H

#include <qwidget.h>
#include <qstringlist.h>
#include "servicediscovery.h"
#include "serviceselectionwidgetbase.h"
#include <kpixmap.h>
#include <map>

namespace KBluetooth {

class DeviceAddress;

/**
@author Fred Schaettgen
*/
class ServiceSelectionWidget : public ServiceSelectionWidgetBase
{
Q_OBJECT
public:
    ServiceSelectionWidget(QWidget* parent, QStringList uuids,
        bool initFromCache = true, bool startScan = true, bool autoSelect = true);
    ~ServiceSelectionWidget();
    DeviceAddress selectedAddress();
    uint selectedChannel();

    /** Displays a modal service selection dialog.
     * @param parent The parent Widget
     * @param uuids The uuids that must appear in sdp record for the displayed services.
     * @param address Output parameter for the address of the selected service
     * @param rfcommChannel Output parameter the the channel of the selected service
     * @return true if the user clicked OK, false otherwise
     * @note If the return value is false, the value of the output parameters is undefined.
     */
    static bool showSelectionDialog(QWidget* parent, QStringList uuids,
        KBluetooth::DeviceAddress &address, int &rfcommChannel);

	void clearSelection();
		
signals:
	void serviceChanged (KBluetooth::DeviceAddress, uint);
	
public slots:
    void initFromCache();
    void scan();
protected:
    void init();
    virtual ServiceDiscovery::ServiceInfoVector filter(
        const ServiceDiscovery::ServiceInfoVector& services);
public: //gcc 2.95.4 HACK
    virtual bool lessThan(const ServiceDiscovery::ServiceInfo* a,
        const ServiceDiscovery::ServiceInfo* b);
protected:  //gcc 2.95.4 HACK
    ServiceDiscovery* serviceDiscovery;
    bool bInitFromCache;
    QStringList uuids;
    ServiceDiscovery::ServiceInfoVector serviceInfos;
private:
    class DefaultPredicate {
    private:
        ServiceSelectionWidget* owner;
    public:
        DefaultPredicate(ServiceSelectionWidget* owner);
        bool operator()(const ServiceDiscovery::ServiceInfo* a,
            const ServiceDiscovery::ServiceInfo* b);
    };
    ServiceDiscovery::ServiceInfoVector sort(
        const ServiceDiscovery::ServiceInfoVector& services);
    
    struct DeviceIcons {
        KPixmap verifiedServiceIcon;
        KPixmap cachedServiceIcon;
        KPixmap inquiriedServiceIcon;
    };
    std::map<QString, DeviceIcons> mimeToIconMap;
    const DeviceIcons& getIcons(QString mimetype);
    //void loadIcons();

    bool autoSelect;
    //KPixmap cachedServiceIcon;
    //KPixmap inquiriedServiceIcon;
    //KPixmap verifiedServiceIcon;
private slots:
    void slotInquiryFinished();
    void slotServiceUpdate();
    void slotSelectionChanged(QListBoxItem*);
    void slotSearch();
    void slotClear();
};

}

#endif
