/***************************************************************************
                          sender.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2003 by Simone Gotti
    email                : simone.gotti@email.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sender.h"

#include <libkobex/kobexclient.h>

#include <qlabel.h>
#include <qlayout.h>
#include <kdeversion.h>

using namespace KBluetooth;

Sender::Sender(DeviceAddress bda, int channel,
    KFileItemList* fileList, QWidget *parent, const char *name)
    : QWidget(parent, name), error(false) {

  this->obex=obex;
  this->fileList=*fileList;

  topLayout = new QVBoxLayout(this);

  label = new QLabel(this);
  label->show();
  topLayout->addWidget(label);

  bar = new KProgress(this);
  bar->show();
  topLayout->addWidget(bar);



  // Create client to perform push and connect signals
  client = new ObexClient(bda, channel, this);

  connect(client, SIGNAL(connectionError(const QString&)), this, SIGNAL(connectionError(const QString&)));

  connect(client, SIGNAL(sendError(const QString&)), this, SLOT(slotSendError(const QString&)));
  connect(client, SIGNAL(info(const QString&)), this, SLOT(slotClientInfo(const QString &)));
  connect(client, SIGNAL(fileSended()), this, SLOT(slotFileSended()));


  connect(client, SIGNAL(totalSize(int)), bar, SLOT(setTotalSteps(int)));
  connect(client, SIGNAL(progress(int)), bar, SLOT(setValue(int)));

}

Sender::~Sender() {}

void Sender::send() {
  KFileItem *item;

  if((item = nextFile())) {
    currentFileItem = item;
    sendFile(item->url());
  } else {
    client->stopConnection();
    deleteLater();
  }
}

void Sender::sendFile(KURL url) {


  if(!client->connected())
    client->startConnection();

  // Setup Progress bar
  bar->setProgress(0);

  // Start client
  client->sendFile(url.path());

}

KFileItem* Sender::nextFile() {

  KFileItem *item, *sentItem;
  bool Ok;

  item = fileList.first();
  while(item) {
    Ok = TRUE;
    sentItem = sentList.first();
    while(sentItem) {
      if(item == sentItem)
        Ok = FALSE;
      sentItem = sentList.next();
    }
    if(Ok) return item;
    else item = fileList.next();
  }
  return 0;
}


void Sender::slotClientInfo(const QString& msg) {
  label->setText(msg);
}

void Sender::slotSendError(const QString& msg) {
  // Remember the files already transfered
  sentList.append(currentFileItem);

  emit(sendError(currentFileItem, msg));

}

void Sender::slotFileSended() {
  // Remember the files already transfered
  sentList.append(currentFileItem);

  emit(fileSended(currentFileItem));

}

#include "sender.moc"


