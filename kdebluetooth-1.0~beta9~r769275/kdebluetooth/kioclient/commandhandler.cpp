/***************************************************************************
 *   Copyright (C) 2004 by Fred Schaettgen <kdebluetooth@schaettgen.de>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "commandhandler.h"
#include <kcmdlineargs.h>
#include <kapplication.h>
#include <klocale.h>
#include <iostream>
#include <kdebug.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <kio/job.h>

CommandHandler::CommandHandler(KCmdLineArgs *args)
{
    this->iterate = false;
    this->args = args;
    returnValue = 0;
    if (args->count() < 2) {
        exitPrintUsage(i18n("Arguments missing."));
    }
    origCommand = args->arg(0);
    for (int n=1; n<args->count()-1; ++n) {
        sources.append(args->arg(n));
    }
    target = args->arg(args->count()-1);
    targets = sources;
    targets.append(target);
    
    if (args->isSet("outfile")) {
        outFile.setName(args->getOption("outfile"));
        outFile.open(IO_WriteOnly);
    }
    else {
        // Open stdout
        outFile.open(IO_WriteOnly, 1);
    }

    if (args->isSet("infile")) {
        inFile.setName(args->getOption("infile"));
        inFile.open(IO_ReadOnly);
    }
    else {
        // Open stdin
        inFile.open(IO_ReadOnly, 0);
    }
        
    showProgressWindow = args->isSet("progresswindow");
    overwrite = args->isSet("overwrite");
}


CommandHandler::~CommandHandler()
{
}

void CommandHandler::start()
{
    if (origCommand == "ls") {
        command = "ls";
        list(target);
    }
    else if (origCommand == "get" || origCommand == "cat" || origCommand == "read") {
        command = "get";
        get(target);
    }
    else if (origCommand == "put" || origCommand == "write") {
        command = "put";
        put(target);
    }
    else if (origCommand == "mkdir") {
        command = "mkdir";
        mkdir(target);
    }
    else if (origCommand == "rmdir") {
        command = "rmdir";
        rmdir(target);
    }
    else if (origCommand == "rm" || origCommand == "del") {
        command = "del";
        del(targets);
    }
    else if (origCommand == "cp" || origCommand == "copy") {
        command = "copy";
        copy(sources, target);
    }
    else {
        command = origCommand;
        exitPrintUsage(i18n("unknown command: %1").arg(command));
    }
}

void CommandHandler::commonConnect(KIO::Job* job)
{
    connect(job, SIGNAL(infoMessage(KIO::Job*,const QString&)),
        this, SLOT(slotInfoMessage(KIO::Job*,const QString&)));
    connect(job, SIGNAL(percent (KIO::Job*, unsigned long)),
        this, SLOT(slotPercent(KIO::Job*, unsigned long)));
    connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slotFinished(KIO::Job*)));
}

KURL::List CommandHandler::urlList(const QStringList& sources)
{
    KURL::List ret;
    for (size_t n=0; n < sources.size(); ++n) {
        ret.append(KURL(sources[n]));
    }
    return ret;
}

// Commands ---------------------------------------------------

void CommandHandler::list(const QString& target)
{
    bool showHidden = true;
    KIO::ListJob* job = KIO::listDir(KURL(target), showProgressWindow, showHidden);
    this->job = job;
    commonConnect(job);
    connect(job, SIGNAL(entries(KIO::Job*, const KIO::UDSEntryList&)),
        this, SLOT(slotEntries(KIO::Job*, const KIO::UDSEntryList&)));
}

void CommandHandler::get(const QString& target)
{
    bool reload = false;
    KIO::TransferJob* job = KIO::get(KURL(target), reload, showProgressWindow);
    this->job = job;
    commonConnect(job);
    connect(job, SIGNAL(data(KIO::Job*,const QByteArray&)),
        this, SLOT(slotData(KIO::Job*,const QByteArray&)));
}

void CommandHandler::put(const QString& target)
{
    int permissions = -1;
    bool resume = false;
    KIO::TransferJob* job = KIO::put(KURL(target), permissions, 
        overwrite, resume, showProgressWindow);
    this->job = job;
    commonConnect(job);
    connect(job, SIGNAL(dataReq(KIO::Job*,QByteArray&)),
        this, SLOT(slotDataReq(KIO::Job*,QByteArray&)));
}

void CommandHandler::mkdir(const QString& target)
{
    int permissions = -1;
    KIO::SimpleJob* job = KIO::mkdir(KURL(target), permissions);
    this->job = job;
    commonConnect(job);    
}

void CommandHandler::rmdir(const QString& target)
{
    KIO::SimpleJob* job = KIO::rmdir(KURL(target));
    this->job = job;
    commonConnect(job);    
}

void CommandHandler::del(const QStringList& targets)
{
    bool shred = false;
    KIO::DeleteJob* job = KIO::del(urlList(targets), shred, showProgressWindow);
    this->job = job;
    commonConnect(job);    
}

void CommandHandler::copy(const QStringList& sources, const QString& target)
{
    KIO::CopyJob *job = KIO::copy(urlList(sources), KURL(target),
       showProgressWindow);
    this->job = job;
    commonConnect(job);
}

// Signal handlers --------------------------------------------

void CommandHandler::slotEntries(KIO::Job* /*job*/, const KIO::UDSEntryList& list)
{
    if (command == "ls") {
        for (size_t n=0; n<list.size(); ++n) {
            KIO::UDSEntry entry = list[n];
            
            QDateTime date = QDateTime::currentDateTime();
            QString user = "n/a";
            QString iconName = "unknown";
            QString group = "n/a";
            QString extra = "n/a";
            QString name = "n/a";
            QDateTime mTime = QDateTime::currentDateTime();
            QDateTime aTime = QDateTime::currentDateTime();
            QDateTime cTime = QDateTime::currentDateTime();
            int fileType = 0;
            QString linkDest = "n/a";
            QString url = "n/a";
            QString mimeType = "n/a";
            QString guessedMimeType = "n/a";
            QString xmlProperties = "n/a";
            long long size = -1;
            int access = 0;
            
            for (size_t m=0; m<entry.size(); ++m) {
                KIO::UDSAtom atom = entry[m];
                switch (atom.m_uds) {
                case KIO::UDS_TIME:
                    date.setTime_t(atom.m_long); break;
                case KIO::UDS_SIZE:
                    size = atom.m_long; break;
                case KIO::UDS_USER:
                    user = atom.m_str; break;
                case KIO::UDS_ICON_NAME:
                    iconName = atom.m_str; break;
                case KIO::UDS_GROUP:
                    group = atom.m_str; break;
                case KIO::UDS_EXTRA:
                    extra = atom.m_str; break;
                case KIO::UDS_NAME:
                    name = atom.m_str; break;
                case KIO::UDS_ACCESS:
                    access = atom.m_long; break;
                case KIO::UDS_MODIFICATION_TIME:
                    mTime.setTime_t(atom.m_long); break;
                case KIO::UDS_ACCESS_TIME:
                    aTime.setTime_t(atom.m_long); break;
                case KIO::UDS_CREATION_TIME:
                    cTime.setTime_t(atom.m_long); break;
                case KIO::UDS_FILE_TYPE:
                    fileType = atom.m_long; break;
                case KIO::UDS_LINK_DEST:
                    linkDest = atom.m_str; break;
                case KIO::UDS_URL:
                    url = atom.m_str; break;
                case KIO::UDS_MIME_TYPE:
                    mimeType = atom.m_str; break;
                case KIO::UDS_GUESSED_MIME_TYPE:
                    guessedMimeType = atom.m_str; break;
                case KIO::UDS_XML_PROPERTIES:
                    xmlProperties = atom.m_str; break;                    
                };
            }
            
            if (args->isSet("access")) {
                std::cout << QString::number(access,8).local8Bit() << " ";
            }
            if (args->isSet("filetype")) {
                std::cout << QString::number(fileType).rightJustify(4).local8Bit() << " ";
            }
            if (args->isSet("user")) {
                std::cout << user.rightJustify(8).local8Bit() << " ";
            }
            if (args->isSet("group")) {
                std::cout << group.rightJustify(8).local8Bit() << " ";
            }
            if (args->isSet("size")) {
                std::cout << QString::number(size).rightJustify(9).local8Bit() << " ";
            }
            if (args->isSet("date")) {
                std::cout << date.toString("yyyy-MM-dd hh:mm:ss").local8Bit() << " ";
            }
            if (args->isSet("mtime")) {
                std::cout << mTime.toString("yyyy-MM-dd hh:mm:ss").local8Bit() << " ";
            }
            if (args->isSet("atime")) {
                std::cout << aTime.toString("yyyy-MM-dd hh:mm:ss").local8Bit() << " ";
            }
            if (args->isSet("ctime")) {
                std::cout << cTime.toString("yyyy-MM-dd hh:mm:ss").local8Bit() << " ";
            }
            if (args->isSet("iconName")) {
                std::cout << iconName.local8Bit() << " ";
            }
            if (args->isSet("mimetype")) {
                std::cout << mimeType.local8Bit() << " ";
            }
            if (args->isSet("guessedmimetype")) {
                std::cout << guessedMimeType.local8Bit() << " ";
            }
            if (args->isSet("linkdest")) {
                std::cout << linkDest.local8Bit() << " ";
            }
            if (args->isSet("url")) {
                std::cout << url.local8Bit() << " ";
            }
            if (args->isSet("name")) {
                std::cout << name.local8Bit() << " ";
            }
            if (args->isSet("xmlproperties")) {
                std::cout << xmlProperties.local8Bit() << " ";
            }
            if (args->isSet("extra")) {
                std::cout << extra.local8Bit() << " ";
            }
            std::cout << std::endl;
        }
    }
}

void CommandHandler::slotData(KIO::Job* /*job*/, const QByteArray &data)
{
    outFile.writeBlock(data);
}

void CommandHandler::slotDataReq(KIO::Job* /*job*/, QByteArray &data)
{
    data.resize(65536);
    data.resize(inFile.readBlock(data.data(), data.size()));
}

void CommandHandler::slotInfoMessage(KIO::Job* /*job*/,const QString& msg)
{
    if (msg != lastMessage && args->isSet("messages")) {
        std::cerr << " >" << msg.local8Bit() << std::endl;
        lastMessage = msg;
    }
}

void CommandHandler::slotPercent(KIO::Job* /*job*/, unsigned long /*percent*/)
{

}

void CommandHandler::slotFinished(KIO::Job* job)
{
    if (job->error() != 0) {
        std::cerr << job->errorString().local8Bit() << std::endl;
        iterate = false;
    } 
    else {
        if (iterate) {
            start();
        }
    }
    
    if (!iterate) {
        KApplication::kApplication()->exit(job->error());
    }
}

void CommandHandler::exitPrintUsage(const QString& message)
{
    std::cout << message.local8Bit() << std::endl;
    KCmdLineArgs::usage();
}


#include "commandhandler.moc"
