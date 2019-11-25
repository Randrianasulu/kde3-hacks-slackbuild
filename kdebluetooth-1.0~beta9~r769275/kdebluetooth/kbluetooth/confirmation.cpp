//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kbluetoothd@schaettgen.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "confirmation.h"
#include <kconfig.h>
#include <kmessagebox.h>
#include <kdialogbase.h>
#include <kbookmarkmenu.h>
#include <kbookmarkmanager.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <kdebug.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kglobal.h>
#include <qcombobox.h>
#include "confirmationdlgbase.h"

using namespace KBluetooth;

Confirmation::Confirmation(KConfig* c,QString configGroup) :
    configGroup(configGroup)
{   
    config = c;
    config->setGroup(configGroup);
    policies.append("ask");
    policies.append("allow");
    policies.append("deny");
    defaultPolicy = policies[0];
}

bool Confirmation::askForPermission(QString devicename,
    KBluetooth::DeviceAddress deviceaddress, QString servicename)
{
    kdDebug() << "CONFIG: " << config->groupList() << endl;
    QString policy = getPolicy(deviceaddress, servicename);

    if (policy == "allow") {
        kdDebug() << i18n("Kbluetoothd: access to %1 from %2: ALLOW")
            .arg(servicename).arg(QString(deviceaddress)) << endl;
        return true;
    }
    else if (policy == "deny") {
        kdDebug() << i18n("Kbluetoothd: access to %1 from %2: DENY")
            .arg(servicename).arg(QString(deviceaddress)) << endl;
        return false;
    }
    else if (policy == "ask") {
        kdDebug() << i18n("Kbluetoothd: access to %1 from %2: ASK")
            .arg(servicename).arg(QString(deviceaddress)) << endl;
        KDialogBase dlg(NULL, "connectionconfirmdlg", true,
            "KDE Bluetooth Framework", KDialogBase::Ok | KDialogBase::Cancel);
        ConfirmationDlgBase *mainWidget = new ConfirmationDlgBase(&dlg);
        mainWidget->messageLabel->setText(mainWidget->messageLabel->text()
            .arg(devicename).arg(servicename));

        mainWidget->logoLabel->setPixmap(KGlobal::iconLoader()->loadIcon(
            "kdebluetooth", KIcon::Desktop, 48));
        mainWidget->policyComboBox->insertStringList(policies);
        const int noChangeItemIndex = 0;
        mainWidget->policyComboBox->insertItem(i18n("Unchanged"), noChangeItemIndex);
        
        QString bookmarkURL = QString("sdp://[%1]/").arg(deviceaddress);
        QString bookmarkName = QString("%1 - %2")
            .arg(devicename).arg(servicename);
        
        KConfig* cfg = KGlobal::config();
        cfg->setGroup("Confirmation");
//        mainWidget->bookmarkBox->setChecked(
  //          cfg->readBoolEntry("bookmarkDevice", true));
            
        mainWidget->setMinimumSize(mainWidget->sizeHint());
        dlg.setMainWidget(mainWidget);
	mainWidget->setActiveWindow();
	mainWidget->raise();	
	
        dlg.setButtonOK(KGuiItem(i18n("Accept"), "connect_established"));
        bool ret;
        if (dlg.exec() == QDialog::Accepted) {
            ret = true;
            //bool setBookmark = mainWidget->bookmarkBox->isChecked();
            //cfg->writeEntry("bookmarkDevice", setBookmark);
            //if (setBookmark) {
            //    saveBookmark(bookmarkURL, bookmarkName);
            //}
            //cfg->sync();
        }
        else {
            ret = false;
        }
        kdDebug() << "Exists_Entry: " << config->readListEntry("ruleAddrList").size() << endl;
        if ((mainWidget->policyComboBox->currentItem() != noChangeItemIndex)) {
//            KMessageBox::information(NULL, i18n("You can change \
the confirmation settings again using the Bluetooth services applet in \
the Control Center."), "KDE Bluetooth Framework", "showChangeConfirmationMessage");
            ConnectionRule rule;
            rule.addr = deviceaddress;
            rule.service = servicename;
            rule.policy = mainWidget->policyComboBox->currentText();
            insertRule(0, rule);
        }
                    
        return ret;
    }
    return false;
}

void Confirmation::saveBookmark(const QString &url, const QString& name)
{
    KBookmarkManager* kbm = KBookmarkManager::userBookmarksManager();
    KBookmarkGroup startGroup = kbm->root();
    
    QValueList<KBookmark> list;
    list.push_back(startGroup);
    KBookmarkGroup firstFoundGroup = startGroup;
    bool foundGroup = false;
    bool found = false;
    while (list.size() > 0) {
        KBookmark bm = list.front();
        //kdDebug() << "Found bookmark " << bm.text() 
        //    << " url: " << bm.url().url() << endl;
        list.pop_front();
        if (bm.isGroup()) {
            KBookmarkGroup bmg = bm.toGroup();
            for (KBookmark b = bmg.first(); !b.isNull(); b = bmg.next(b)) {
                list.push_back(b);
            }
        }
        else {
            if (bm.url().url().lower().startsWith("sdp://") && !foundGroup) {
                kdDebug() << "Found group!" << endl;
                foundGroup = true;
                firstFoundGroup = bm.parentGroup();
            }
            if (bm.url().url().lower() == url.lower()) {
                kdDebug() << "Found url" << endl;
                found = true;
            }
        }
    } 
    if (found == false) {
        kbm->addBookmarkDialog(url, name, firstFoundGroup.groupAddress());
        kbm->emitChanged( firstFoundGroup );
    }
}

QString Confirmation::getPolicy(DeviceAddress addr, QString service)
{
    int numRules = getNumRules();
    kdDebug() << "NUMRules: " << numRules << endl;
    for (int n=0; n<numRules; ++n) {
        ConnectionRule rule = getRule(n);
        bool addrMatch = false;
        if (rule.addr == addr || rule.addr == QString(DeviceAddress::any)) {
            addrMatch = true;
        }        
        bool serviceMatch = false;
        if (rule.service == service || rule.service == "*") {
            serviceMatch = true;
        }
        if (addrMatch && serviceMatch) return rule.policy;
    }
    return defaultPolicy;
}

int Confirmation::getNumRules()
{
    config->setGroup(configGroup);
    return config->readListEntry("ruleAddrList").size();
}

Confirmation::ConnectionRule Confirmation::getRule(int n)
{
    config->setGroup(configGroup);
    ConnectionRule rule;
    QStringList addrList = config->readListEntry("ruleAddrList");
    QStringList serviceList = config->readListEntry("ruleServiceList");
    QStringList policyList = config->readListEntry("rulePolicyList");
    if (n<int(addrList.size()) && n<int(serviceList.size()) && 
        n<int(policyList.size()) && n>=0) 
    {
        rule.addr = DeviceAddress(addrList[n]);
        rule.service = serviceList[n];
        rule.policy = policyList[n];
        if (policies.contains(rule.policy) == false) {
            rule.policy = defaultPolicy;
        }
    }
    return rule;
}

bool Confirmation::insertRule(int n, const ConnectionRule& rule)
{
    config->setGroup(configGroup);
    QStringList addrList = config->readListEntry("ruleAddrList");
    QStringList serviceList = config->readListEntry("ruleServiceList");
    QStringList policyList = config->readListEntry("rulePolicyList");
    addrList.insert(addrList.at(n), QString(rule.addr));
    serviceList.insert(serviceList.at(n), rule.service);
    if (policies.contains(rule.policy)) {
        policyList.insert(policyList.at(n), rule.policy);
    }
    else {
        policyList.insert(policyList.at(n), defaultPolicy);
    }
    config->writeEntry("ruleAddrList", addrList);
    config->writeEntry("ruleServiceList", serviceList);
    config->writeEntry("rulePolicyList", policyList);
    return true;
}

bool Confirmation::deleteRule(int n)
{
    config->setGroup(configGroup);
    QStringList addrList = config->readListEntry("ruleAddrList");
    QStringList serviceList = config->readListEntry("ruleServiceList");
    QStringList policyList = config->readListEntry("rulePolicyList");
    if (n<int(addrList.size()) && n<int(serviceList.size()) && 
        n<int(policyList.size()) && n>=0) 
    {
        addrList.remove(addrList.at(n));
        serviceList.remove(serviceList.at(n));
        policyList.remove(policyList.at(n));
        config->writeEntry("ruleAddrList", addrList);
        config->writeEntry("ruleServiceList", serviceList);
        config->writeEntry("rulePolicyList", policyList);
        return true;
    }
    else {
        return false;
    }
}

QStringList Confirmation::getPolicies()
{
    return policies;
}
