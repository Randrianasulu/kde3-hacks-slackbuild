/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage JPEGLossLess plugin 
 *               actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2004-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
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

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// Qt includes.

#include <qstring.h>
#include <qthread.h>
#include <qstringlist.h>

// KDE includes.

#include <kurl.h>

// LibKipi includes.

#include <libkipi/interface.h>

// Local includes.

#include "actions.h"
#include "mtqueue.h"

class QObject;

namespace KIPIJPEGLossLessPlugin
{

class ActionThread : public QThread
{
public:

    ActionThread( KIPI::Interface* interface, QObject *parent);
    ~ActionThread();

    void rotate(const KURL::List& urlList, RotateAction val);
    void flip(const KURL::List& urlList, FlipAction val);
    void convert2grayscale(const KURL::List& urlList);
    void cancel();

protected:

    void run();

private:

    struct Task_ 
    {
        QString      filePath;
        Action       action;
        RotateAction rotAction;
        FlipAction   flipAction;
    };

    typedef struct Task_ Task;

    QObject         *m_parent;

    MTQueue<Task>    m_taskQueue;

    KIPI::Interface *m_interface;
};

}  // NameSpace KIPIJPEGLossLessPlugin

#endif /* ACTIONTHREAD_H */
