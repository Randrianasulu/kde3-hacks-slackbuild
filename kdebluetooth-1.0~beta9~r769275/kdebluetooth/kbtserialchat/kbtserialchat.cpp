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

#include "kbtserialchat.h"

#include <qlineedit.h>
#include <qapplication.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <libkbluetooth/serviceselectionwidget.h>
#include <libkbluetoothd/mruservices.h>

using namespace KBluetooth;

MainDialog::MainDialog()
        : MainDialogBase(0, "kbtserialchat")
{
    DeviceAddress addr;
    int channel;
    QStringList uuids("0x1101");
    if (ServiceSelectionWidget::showSelectionDialog(this,
        uuids, addr, channel) == true) {

        socket = new RfcommSocket(this, "rfcommsocket");
        socket->connectToHost(addr, channel);

        connect(sendButton, SIGNAL(clicked()),
            this, SLOT(slotSendCommand()));
        connect(socket, SIGNAL(readyRead()),
            this, SLOT(slotSocketReadyRead()));
        connect(socket, SIGNAL(error(int)),
            this, SLOT(slotSocketError(int)));
        connect(socket, SIGNAL(connectionClosed()),
            this, SLOT(slotSocketConnectionClosed()));

        QString text = textEdit->text();
        textEdit->setText(text + QString("<br>Connected to %1 on channel %2.").
            arg(QString(addr)).arg(channel));
        saveMru(addr, channel);
    }
    else {
        QApplication::exit(1);
    }
}


MainDialog::MainDialog(DeviceAddress addr, int channel)
        : MainDialogBase( 0, "kbtserialchat" )
{
    socket = new RfcommSocket(this, "rfcommsocket");
    socket->connectToHost(addr, channel);

    connect(sendButton, SIGNAL(clicked()),
        this, SLOT(slotSendCommand()));
    connect(socket, SIGNAL(readyRead()),
        this, SLOT(slotSocketReadyRead()));
    connect(socket, SIGNAL(error(int)),
        this, SLOT(slotSocketError(int)));
    connect(socket, SIGNAL(connectionClosed()),
        this, SLOT(slotSocketConnectionClosed()));

    QString text = textEdit->text();
    textEdit->setText(text + QString("<br>Connected to %1 on channel %2.").
        arg(QString(addr)).arg(channel));
    saveMru(addr, channel);
}

MainDialog::MainDialog(int s, QString addr, QString name)
        : MainDialogBase( 0, "kbtserialchat" )
{
    socket = new RfcommSocket(this, "rfcommsocket");
    socket->setSocket(s);

    connect(sendButton, SIGNAL(clicked()),
        this, SLOT(slotSendCommand()));
    connect(socket, SIGNAL(readyRead()),
        this, SLOT(slotSocketReadyRead()));
    connect(socket, SIGNAL(error(int)),
        this, SLOT(slotSocketError()));
    connect(socket, SIGNAL(connectionClosed()),
        this, SLOT(slotSocketConnectionClosed()));

    QString text = textEdit->text();
    textEdit->setText(text + QString("<br>Incoming connection from %1 (%2).")
        .arg(name).arg(addr));
}


void MainDialog::slotSendCommand()
{
    QString s = lineEdit->text() + "\r\n";
    QCString buf(s.utf8());
    socket->writeBlock((const char*)buf, buf.size());
    QString text = textEdit->text();
    textEdit->setText(text + "<br><font color=#008800>" +
        s + "</font>");
    lineEdit->setText("");
    textEdit->scrollToBottom();
}

void MainDialog::slotSocketReadyRead()
{
    const int len = socket->size();
    if (len > 0) {
        char *buf = new char[len];
        socket->readBlock(buf, len);
        QCString cs(buf, len);
        delete []buf;
        QString text = textEdit->text();
        textEdit->setText(text + "<br><font color=#880000>" +
            QString::fromUtf8(cs) + "</font>");
        textEdit->scrollToBottom();
    }
}

void MainDialog::slotSocketError(int errnum)
{
    KMessageBox::information(NULL,
        QString("Connection error (%1).").arg(errnum),
        "kbtserialchat");
    sendButton->setEnabled(false);
    lineEdit->setEnabled(false);
}

void MainDialog::slotSocketConnectionClosed()
{
    KMessageBox::information(NULL,
        "Connection closed.",
        "kbtserialchat");
    sendButton->setEnabled(false);
    lineEdit->setEnabled(false);
}

void MainDialog::saveMru(KBluetooth::DeviceAddress addr, int channel)
{    
    KBluetoothd::MRUServices::add(
    QStringList("kbtserialchat") << QString("sdp://[%1]/params?rfcommchannel=%2")
        .arg(QString(addr)).arg(channel), addr);
}

MainDialog::~MainDialog()
{
}

#include "kbtserialchat.moc"
