/* ============================================================
 * File  : sinks.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2007-01-14
 *
 * Copyright 2007 by Colin Guthrie <kde@colin.guthr.ie>
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
 * ============================================================ */

#include <qstring.h>

#include <qwidget.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdeversion.h>
#if KDE_IS_VERSION(3,2,0)
#include <kwallet.h>
#endif

#include "sinks.h"
#include "sink.h"
#include "sinkfactory.h"

namespace KIPISyncPlugin
{

SinkQListViewItem::SinkQListViewItem(Sink* pSink, QListView* pParent)
  : QListViewItem(pParent, pSink->Name(), pSink->Type()),
    mpSink(pSink)
{
}

Sink* SinkQListViewItem::GetSink()
{
  return mpSink;
}

void SinkQListViewItem::Refresh()
{
  setText(0, mpSink->Name());
  setText(1, mpSink->Type());
}



Sinks::Sinks()
 : mpWallet(NULL),
   mMaxSinkId(0)
{
  KWallet::Wallet* p_wallet = NULL;
#if KDE_IS_VERSION(3,2,0)
  mpWallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),
                                         kapp->activeWindow()->winId(),
                                         KWallet::Wallet::Synchronous);
  if (!mpWallet)
  {
    kdWarning() << "Failed to open kwallet" << endl;
  }
  else
  {
    if (!mpWallet->hasFolder("KIPISyncPlugin"))
    {
      if (!mpWallet->createFolder("KIPISyncPlugin"))
        kdWarning() << "Failed to create kwallet folder" << endl;
    }

    if (!mpWallet->setFolder("KIPISyncPlugin"))
      kdWarning() << "Failed to set kwallet folder" << endl;
    else
      p_wallet = mpWallet;
  }
#endif

  // Read config
  KConfig config("kipirc");
  config.setGroup("Sync Settings");
  QValueList<int> sink_ids = config.readIntListEntry("Sinks");

  config.setGroup("Sync Sinks");
  QString name, type;
  for (QValueList<int>::Iterator it = sink_ids.begin(); it != sink_ids.end(); ++it)
  {
    unsigned int sink_id = (*it);

    if (sink_id > mMaxSinkId)
      mMaxSinkId = sink_id;

    type = config.readEntry(QString("Type%1").arg(sink_id));
    name = config.readEntry(QString("Name%1").arg(sink_id));
    Sink* p_sink = SinkFactory::Create(type, sink_id, name, &config, p_wallet);
    if (p_sink)
      mSinks.append(p_sink);
  }
}


Sinks::~Sinks()
{
  if (mpWallet)
    delete mpWallet;

  // Todo: clear up mSinks
}


/// @todo Abstract this to per-sink-type load
void Sinks::Load()
{
  static bool bln_loaded = false;
  if (bln_loaded) return;
  bln_loaded = true;
}


Sink* Sinks::Add(QString type, QString name)
{
  Sink* p_sink = SinkFactory::Create(type, ++mMaxSinkId, name, NULL, NULL);
  // This actually needs to be a call to sync factory creation... pass in the new SinkId
  mSinks.append(p_sink);
  return p_sink;
}

void Sinks::Remove(Sink* pSink)
{
  mSinks.remove(pSink);

  // Slight cosmetic thing for sink numbering.
  if (mSinks.isEmpty())
    mMaxSinkId = 0;
}


/// @todo Abstract this to per-sink-type save
void Sinks::Save()
{
  QValueList<int> sink_ids;
  KConfig config("kipirc");
  config.deleteGroup("Sync Sinks");
  config.setGroup("Sync Sinks");

  KWallet::Wallet* p_wallet = NULL;
  if (mpWallet)
  {
    if (mpWallet->hasFolder("KIPISyncPlugin"))
    {
      if (!mpWallet->removeFolder("KIPISyncPlugin"))
        kdWarning() << "Failed to clear kwallet folder" << endl;
    }
    if (!mpWallet->createFolder("KIPISyncPlugin"))
      kdWarning() << "Failed to create kwallet folder" << endl;

    if (!mpWallet->setFolder("KIPISyncPlugin"))
      kdWarning() << "Failed to set kwallet folder" << endl;
    else
      p_wallet = mpWallet;
  }

  for (SinkPtrList::iterator it = mSinks.begin(); it != mSinks.end(); ++it)
  {
    Sink* p_sink = (*it);
    p_sink->Save(&config, p_wallet);
    sink_ids.append(p_sink->SinkId());
  }

  config.setGroup("Sync Settings");
  config.writeEntry("Sinks", sink_ids);
}

void Sinks::asQListView(QListView* pListView)
{
  Load();

  pListView->clear();
  for (SinkPtrList::iterator it = mSinks.begin(); it != mSinks.end(); ++it)
  {
    //(*it)->asQListViewItem(pListView);
  }
}

}
