//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2004 by Fred Schaettgen <kdebluetooth@schaettgen.de>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <qobject.h>
#include <qstringlist.h>
#include <kio/jobclasses.h>
#include <qfile.h>
#include <kurl.h>

class KCmdLineArgs;
namespace KIO {
    class Job;
}

class CommandHandler : public QObject
{
Q_OBJECT
public:
    CommandHandler(KCmdLineArgs *args);
    ~CommandHandler();
    void start();
    
private:
    KURL::List urlList(const QStringList& list);
    void exitPrintUsage(const QString& message);
    void list(const QString& target);
    void get(const QString& target);
    void put(const QString& target);
    void mkdir(const QString& target);
    void rmdir(const QString& target);
    void del(const QStringList& targets);
    void copy(const QStringList& sources, const QString& target);
    void commonConnect(KIO::Job* job);
    
    QString command;
    QString origCommand;
    QString target;
    QStringList targets;
    QStringList sources;
    KIO::Job* job;     
    int returnValue;
    KCmdLineArgs *args;
    QFile outFile;
    QFile inFile;
    bool showProgressWindow;
    QString lastMessage;
    bool overwrite;
    bool iterate;
               
private slots:
    void slotFinished(KIO::Job *job);
    void slotEntries(KIO::Job* job, const KIO::UDSEntryList& list);
    void slotData(KIO::Job *, const QByteArray &data);
    void slotDataReq(KIO::Job *, QByteArray &data);
    void slotInfoMessage(KIO::Job* job,const QString& msg);
    void slotPercent(KIO::Job* job, unsigned long percent);
};

#endif
