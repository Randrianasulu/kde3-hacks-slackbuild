/*
    This file is part of kio_obex.

    Copyright (c) 2003 Mathias Froehlich <Mathias.Froehlich@web.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef OBEX_H
#define OBEX_H

#include <qobject.h>
#include <qstring.h>
#include <qcstring.h>
#include <qdom.h>
#include <qmap.h>

#include <kurl.h>
#include <kio/global.h>
#include <kio/slavebase.h>
#include <kconfig.h>
#include <libkbluetooth/deviceaddress.h>

using namespace KIO;

class QObexClient;

class ObexProtocol : public QObject, public KIO::SlaveBase
{
  Q_OBJECT
public:
  /**
     Required constructor.
   */
  ObexProtocol(const QCString &pool_socket, const QCString &app_socket,
	       KConfig* hc);
  
  /**
     Required d'tor.
   */
  ~ObexProtocol();

  /**
     Get a file from the device.
   */
  void get( const KURL& );

  /**
     Stat a file on the device. Stats are cached for some time, but not longer
     than the next disconnect.
   */
  void stat( const KURL& );

  /**
     Get the folder listing. Also fills the listing cache for a later
     stat/listing.
   */
  void listDir( const KURL& );
  
  /**
     Creates the given directory. File modes are not accessible through
     this protocol.
   */
  void mkdir( const KURL&, int );

  /**
     Delete file on the device.
   */
  void del( const KURL&, bool );

  /**
     Put a file onto the device. File permissions are not accessible through
     that protocol. Ignore them.
   */
  void put( const KURL&, int, bool, bool );

  /**
     Open a connection oriented connection.
   */
  void openConnection();

  /**
     Close a connection oriented connection.
   */
  void closeConnection();

  /**
     It understands 4 commands.
     'd' means disconnect. Used internally by the disconnect timer.
     'sb' means search bluetooth devices
     'si' means search irda devices
     'sa' means search all devices, bluetooth and irda
     All serach commands return their data by MetaData. Available Keys are:
      OBEX-DISCOVERED-DEVICES contains the number of available devices.
     If the above integer is > 0 there are also the following three keys for
     each device.
      OBEX-DEVICE-<devnum>-TRANSPORT is either irda or bluetooth
      OBEX-DEVICE-<devnum>-NAME is the name of the device.
      OBEX-DEVICE-<devnum>-ADDRESS is the hardware address of the device.
     Where 0 <= <devnum> < OBEX-DISCOVERED-DEVICES.
     Its intentional use is in kaddressbook to beam vcards ober bluetooth/irda.
     First discover devices using this special command, then send the vcard to
     the device using put.
  */
  void special( const QByteArray& );

  /**
     Is called whenever the host part of the url is changed.
   */
  void setHost( const QString&, int, const QString&, const QString& );

public slots:
  /**
     Callbacks from the obex client code.
   */
  void slotResponse( const QObexObject& );
  void slotAuthenticationRequired( const QString&, bool );
  void slotData( const QValueList<QByteArray>&, bool* );
  void slotDataReq( QByteArray&, size_t, bool* );
  void slotAborted( const QObexObject& );
  void slotError( QObexClient::Error );

private:
  /**
     Internal state tracking.
   */
  enum CommandType {
    Idle,
    Connect,
    Disconnect,
    Get,
    GetFolderListing,
    Stat,
    Put,
    MkDir,
    Delete
  };
  CommandType mCommandType;

  /**
     Pointer to the obex client class.
  */
  QObexClient* mClient;

  /**
     Data structure for cached directory listings and stats.
   */
  struct CacheValue {
    CacheValue( const QDateTime& t, const UDSEntry& ue )
      : time( t ), data( ue ) {}
    CacheValue( const UDSEntry& ue )
      : time( QDateTime::currentDateTime() ), data( ue ) {}
    CacheValue() {}
    QDateTime time;
    UDSEntry data;
  };
  /**
     The cache itself.
   */
  QMap<QString,CacheValue> mUDSEntryCache;
  /**
     The maximum time a cache entry is valid.
   */
  enum { UDSEntryCacheTimeout = 10 };

  /**
     Internal helper function
   */
  void sendError( int );

  /**
     The protocol required that we step into the directories. Do this here.
     Also trace where we are.
   */
  bool changeWorkingDirectory( const QString& to );
  /**
     Store the current working directory.
   */
  QString mCwd;

  /**
     Generate fake UDS values when the device does not send these.
     Take the owner of the kioslave process.
   */
  QString userName;
  QString groupName;

  /**
     Some devices return too restricted permissions for directories. Add fake
     permissions to files/directories so that konquerror can step
     into those directories.
   */
  long mOverridePerm;

  /**
     Record the arguments to the last setHost call and compare ...
     May be this is not required ... FIXME
   */
  QString mCurrentHost;
  int mCurrentPort;
  QString mCurrentUser;
  QString mCurrentPass;

  /**
     Authentication ...
   */
  bool mCachedAuthIsRead;
  KIO::AuthInfo mAuthInfo;

  /**
     Helper functions.
   */
  bool connectClientIfRequired();
  void startDisconnectTimer();
  enum { DefaultDisconnectTimeout = 1 };
  int mDisconnectTimeout;
  bool mConnectionOriented;

  /**
     Temporary buffer for several things.
  */
  QByteArray mDataBuffer;

  /**
     Progress handling.
     True if we have a totalsize value.
  */
  bool mHaveTotalsize;
  /**
     The actually transfered file size
  */
  KIO::filesize_t mProcessedSize;

  /**
     True if we have already chosen a mime type for the current file.
  */
  bool mHaveMimeType;

  /**
     Returns the cached UDSEntry for the given url.
     Reads the corresponding folder listing and caches all entries in
     this directory.
  */
  UDSEntry getCachedStat( const KURL& );

  /**
     Some helper function for parsing the xml directory listings
  */
  UDSEntry createUDSEntry( const QDomElement& );
  time_t stringToTime_t( QString );

  /**
     Connection parameter handling.
     Map where the connection parameters are stored.
  */
  QMap<QString,QString> mParamMap;
  /**
     Query for the existence of parameter.
  */
  bool hasParam( const QString& );
  /**
     Query for parameter.
  */
  QString getParam( const QString& );

  /**
   * Adds an entry to kbluetoothd's mru list
   */
  void saveMru(KBluetooth::DeviceAddress addr, int channel);

};

#endif
