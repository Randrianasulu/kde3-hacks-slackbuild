/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-09
 * Description : Multithread queue description class 
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
 * 
 * ============================================================ */

#ifndef MTQUEUE_H
#define MTQUEUE_H

// Qt includes.

#include <qptrqueue.h>
#include <qmutex.h>

namespace KIPIRawConverterPlugin
{

template<class Type> class MTQueue
{

public:

    MTQueue()
    {
        m_queue.setAutoDelete(true);
    }
    
    ~MTQueue()
    {
        flush();
    }
    
    bool isEmpty()
    {
        m_mutex.lock();
        bool empty = m_queue.isEmpty();
        m_mutex.unlock();
        return empty;
    }
    
    void flush()
    {
        m_mutex.lock();
        m_queue.clear();
        m_mutex.unlock();
    }
    
    void enqueue(Type * t)
    {
        m_mutex.lock();
        m_queue.enqueue(t);
        m_mutex.unlock();
    }
    
    Type * dequeue()
    {
        m_mutex.lock();
        Type * i = m_queue.dequeue();
        m_mutex.unlock();
        return i;
    }

private:

  QPtrQueue<Type> m_queue;
  QMutex          m_mutex;
};

}  // NameSpace KIPIRawConverterPlugin

#endif // MTQUEUE_H
