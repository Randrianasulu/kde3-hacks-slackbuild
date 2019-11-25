/***************************************************************************
                          sender.h  -  description
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

#ifndef _SENDER_H
#define _SENDER_H

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <libkbluetooth/deviceaddress.h>

#include <qstring.h>

#include <kfileitem.h>
#include <kprogress.h>
#include <kurl.h>

class Obex;
class ObexClient;
class KURL;
class QVBoxLayout;

class Sender : public QWidget {
  Q_OBJECT

public:
  Sender(KBluetooth::DeviceAddress bda, int channel,
    KFileItemList* fileList, QWidget *parent=0, const char *name=0);
  ~Sender();
  void send();
	
protected slots:
  void slotClientInfo(const QString&);
  void slotSendError(const QString&);
  void slotFileSended();

signals:
  void fileSended(KFileItem*);
  void sendError(KFileItem*, const QString&);
  void connectionError(const QString&);

protected:
  void sendFile(KURL url);
  KFileItem* nextFile();

  bool error;
	KFileItemList fileList;
	KFileItemList sentList;
	KFileItem* currentFileItem;

	QVBoxLayout* topLayout;
  KProgress* bar;
  QLabel* label;

	Obex* obex;
  ObexClient *client;
};

#endif // _SENDER_H_
