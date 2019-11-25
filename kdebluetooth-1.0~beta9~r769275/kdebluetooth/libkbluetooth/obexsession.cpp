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

#include "obexsession.h"

namespace KBluetooth
{
ObexSession* ObexSession::_session = NULL;

ObexSession::ObexSession(const QString &path, DBusConnection *conn)
	: DBusSignal(OBEX_SERVICE, path, INTERFACE_OBEXSESSION, conn), path(path), conn(conn)
{
	_session = this;
	match = QString("type='signal',interface='" INTERFACE_OBEXSESSION "', path='%1',sender='" OBEX_SERVICE "'").arg(path);

	filter = new DBusFilter(conn);
	filter->addFilter(filterFunction);
	filter->addMatch(match);
}

ObexSession::~ObexSession()
{
	kdDebug() << k_funcinfo << endl;

	filter->removeFilter(filterFunction);
	filter->removeMatch(match);
	delete filter;
}

void ObexSession::connect()
{
	sendString("Connect", DBUS_TYPE_INVALID);
}

void ObexSession::disconnect()
{
	sendString("Disconnect", DBUS_TYPE_INVALID);
}

void ObexSession::close()
{
	sendString("Close", DBUS_TYPE_INVALID);
}

bool ObexSession::isConnected()
{
	return getBoolean("IsConnected", DBUS_TYPE_INVALID);
}

void ObexSession::changeCurrentFolder(const QString &path)
{
	QCString _pat = path.latin1();
    	const char* _path = _pat.data();

	sendString("ChangeCurrentFolder", DBUS_TYPE_STRING, &_path,  DBUS_TYPE_INVALID);
}

void ObexSession::changeCurrentFolderBackward()
{
	sendString("ChangeCurrentFolderBackward",DBUS_TYPE_INVALID);
}

void ObexSession::changeCurrentFolderToRoot()
{
	sendString("ChangeCurrentFolderToRoot",DBUS_TYPE_INVALID);
}

const QString ObexSession::getCurrentPath()
{
	return getString("GetCurrentPath",DBUS_TYPE_INVALID);
}

void ObexSession::copyRemoteFile(const QString &remote_file, const QString &local_path)
{
	QCString _rem = remote_file.latin1();
    	const char* _remote_file = _rem.data();

	QCString _loc = local_path.latin1();
    	const char* _local_path = _loc.data();

	sendString("CopyRemoteFile", DBUS_TYPE_STRING, &_remote_file, DBUS_TYPE_STRING, &_local_path, DBUS_TYPE_INVALID);
}

void ObexSession::createFolder(const QString &folder_name)
{
	QCString _fol = folder_name.latin1();
    	const char* _folder_name = _fol.data();

	sendString("CreateFolder", DBUS_TYPE_STRING, &_folder_name,  DBUS_TYPE_INVALID);
}

void ObexSession::retrieveFolderListing(char** folder_listing)
{
//	sendString("RetrieveFolderListing", DBUS_TYPE_ARRAY_AS_STRING, &folder_listing,  DBUS_TYPE_INVALID);
}

void ObexSession::sendFile(const QString &local_path)
{
	QCString _loc = local_path.latin1();
    	const char* _local_path = _loc.data();

	sendString("SendFile", DBUS_TYPE_STRING, &_local_path,  DBUS_TYPE_INVALID);
}

void ObexSession::deleteRemoteFile(const QString &remote_filename)
{
	QCString _rem = remote_filename.latin1();
    	const char* _remote_filename = _rem.data();

	sendString("DeleteRemoteFile", DBUS_TYPE_STRING, &_remote_filename,  DBUS_TYPE_INVALID);
}

bool ObexSession::isBusy()
{
	return getBoolean("IsBusy", DBUS_TYPE_INVALID);
}

void ObexSession::cancel()
{
	sendString("Cancel", DBUS_TYPE_INVALID);
}


DBusHandlerResult ObexSession::filterFunction(DBusConnection * /*conn*/, DBusMessage *msg, void  * /*data*/)
{

	DBusError error;
	const char *member = dbus_message_get_member(msg);
	const char *arg = NULL;
	QString qarg = QString::null;

	dbus_error_init(&error);

	if (!dbus_message_has_interface(msg, INTERFACE_OBEXSESSION))
	        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!dbus_message_has_path(msg, OBEX_PATH))
	        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (!member)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (dbus_error_is_set(&error)) {
		kdDebug() << __func__ << " (" << member << "): " << error.message << endl;
		dbus_error_free(&error);
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
	
	kdDebug() << "Obex-Session Signal: " << member << " Argument: " << qarg << endl;

	if (!strcmp("Cancelled", member)) {
		_session->emit cancelled();
	} else if (!strcmp("Connected", member)) {
		_session->emit connected();
	} else if (!strcmp("Disconnected", member)) {
		_session->emit disconnected();
	} else if (!strcmp("Closed", member)) {
		_session->emit closed();
	} else if (!strcmp("TransferStarted", member)) {
		const char *local_path;
		Q_UINT32 total_bytes;
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &qarg, DBUS_TYPE_STRING, &local_path, DBUS_TYPE_UINT32, &total_bytes, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		QString _local_path = QString::fromUtf8(local_path);
		_session->emit transferStarted(qarg,_local_path,total_bytes);
	} else if (!strcmp("TransferProgress", member)) {
		Q_UINT32 bytes_transferred;
		dbus_message_get_args(msg, NULL, DBUS_TYPE_UINT32, &bytes_transferred, DBUS_TYPE_INVALID);
		_session->emit transferProgress(bytes_transferred);
	} else if (!strcmp("TransferCompleted", member)) {
		_session->emit transferCompleted();
	} else if (!strcmp("ErrorOccurred", member)) {
		const char *error_message;
		dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &qarg, DBUS_TYPE_STRING, &error_message, DBUS_TYPE_INVALID);
		qarg = QString::fromUtf8(arg);
		QString _error_message = QString::fromUtf8(error_message);
		_session->emit errorOccurred(qarg,_error_message);
	}  else {
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}

}

#include "obexsession.moc"
