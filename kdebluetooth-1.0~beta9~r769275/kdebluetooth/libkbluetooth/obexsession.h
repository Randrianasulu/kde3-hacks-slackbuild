/*
 *
 *  Session implementation of Obex-Data-Server D-Bus API
 *
 *  Copyright (C) 2007  Tom Patzig <tpatzig@suse.de>
 *
 *
 *  This file is part of libkbluetooth.
 *
 *  libkbluetooth is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libkbluetooth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libkbluetooth; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef OBEXSESSION_H
#define OBEXSESSION_H

#include "dbussignal.h"
#include "dbusfilter.h"

#define INTERFACE_OBEXSESSION "org.openobex.Manager"
namespace KBluetooth
{

class ObexSession : public DBusSignal
{

	Q_OBJECT
	
	public:
		ObexSession(const QString &path, DBusConnection *conn);
		~ObexSession();

	 	 
		void connect();

		void disconnect();
		void close();
		bool isConnected();
		void changeCurrentFolder(const QString &path);
		void changeCurrentFolderBackward();
		void changeCurrentFolderToRoot();
		const QString getCurrentPath();
		void copyRemoteFile(const QString &remote_file, const QString &local_path);
		void createFolder(const QString &folder_name);
	 	void retrieveFolderListing(char** folder_listing);
		void sendFile(const QString &local_path);
		void deleteRemoteFile(const QString &remote_filename);
		bool isBusy();
		void cancel();

	signals:
		
		void cancelled();
		void connected();
		void disconnected();
		void closed();
		void transferStarted(QString filename, QString local_path, int total_bytes);
	 	void transferProgress(int bytes_transferred);
	 	void transferCompleted();
	 	void errorOccurred(QString error_name, QString error_message);

	private:
	 	QString path;
	 	QString match;
		DBusConnection *conn;
		DBusFilter *filter;

		static ObexSession *_session;
		static DBusHandlerResult filterFunction(DBusConnection * conn, DBusMessage *msg, void * data);
	 





};
}
#endif
