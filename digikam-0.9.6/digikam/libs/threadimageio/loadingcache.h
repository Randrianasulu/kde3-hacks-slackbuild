/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-01-11
 * Description : shared image loading and caching
 *
 * Copyright (C) 2005-2007 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef LOADING_CACHE_H
#define LOADING_CACHE_H

#include <qptrlist.h>
#include <qcache.h>
#include <qdict.h>
#include <qmutex.h>

#include "dimg.h"
#include "loadsavethread.h"

namespace Digikam
{

class LoadingProcessListener
{
public:

    virtual bool querySendNotifyEvent() = 0;
    virtual QObject *eventReceiver() = 0;
    virtual LoadSaveThread::AccessMode accessMode() = 0;

};

class LoadingProcess
{
public:

    virtual bool completed() = 0;
    virtual QString filePath() = 0;
    virtual QString cacheKey() = 0;
    virtual void addListener(LoadingProcessListener *listener) = 0;
    virtual void removeListener(LoadingProcessListener *listener) = 0;
    virtual void notifyNewLoadingProcess(LoadingProcess *process, LoadingDescription description) = 0;

};

class LoadingCachePriv;

class LoadingCache : public QObject
{

    Q_OBJECT

public:

    static LoadingCache *cache();
    static void cleanUp();
    ~LoadingCache();

    // all functions shall only be called when a CacheLock is held
    class CacheLock
    {
    public:
        CacheLock(LoadingCache *cache);
        ~CacheLock();
        void wakeAll();
        void timedWait();
    private:
        LoadingCache *m_cache;
    };

    // Retrieves an image for the given string from the cache,
    // or 0 if no image is found.
    DImg *retrieveImage(const QString &cacheKey);
    // Returns whether the given DImg fits in the cache.
    bool isCacheable(const DImg *img);
    // Put image into for given string into the cache.
    // Returns true if image has been put in the cache, false otherwise.
    // Ownership of the DImg instance is passed to the cache.
    // When it cannot be put in the cache it is deleted.
    // The third parameter specifies a file path that will be watched.
    // If this file changes, the object will be removed from the cache.
    bool putImage(const QString &cacheKey, DImg *img, const QString &filePath);
    void removeImage(const QString &cacheKey);
    void removeImages();

    // Find the loading process for given cacheKey, or 0 if not found
    LoadingProcess *retrieveLoadingProcess(const QString &cacheKey);
    // Add a loading process to the list. Only one loading process
    // for the same cache key is registered at a time.
    void addLoadingProcess(LoadingProcess *process);
    // Remove loading process for given cache key
    void removeLoadingProcess(LoadingProcess *process);
    // Notify all currently registered loading processes
    void notifyNewLoadingProcess(LoadingProcess *process, LoadingDescription description);

    void setCacheSize(int megabytes);

protected:

    virtual void customEvent (QCustomEvent *event);

private slots:

    void slotFileDirty(const QString &path);

private:

    static LoadingCache *m_instance;

    LoadingCache();

    friend class CacheLock;
    LoadingCachePriv *d;

};

}   // namespace Digikam

#endif
