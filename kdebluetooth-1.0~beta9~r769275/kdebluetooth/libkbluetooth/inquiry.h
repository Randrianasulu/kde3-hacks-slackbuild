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

#ifndef KBT_INQUIRY_H
#define KBT_INQUIRY_H

#include <qstring.h>
#include <qobject.h>
#include <deque>
#include <qguardedptr.h>
#include "deviceaddress.h"
#include <set>

class QSocket;
class QTimer;
class QDateTime;

namespace KBluetooth
{

class DeviceAddress;
class HciSocket;

/** Scans for nearby bluetooth devices.
 * This class provides an asynchronous interface to the
 * inquriy HCI command. To scan for other devices, connect
 * to the signals neighbourFound() and finnished() and call
 * inquiry(). Inquiry results are signalled as soon as they arrive,
 * so the information can be displayed before the whole inquiry
 * process is finnished.
 * Still there should no connections be set up during before
 * the finnished() signal was sent (hmm, is this always true..?)
 */
class Inquiry : public QObject
{
    Q_OBJECT
public:
    /** Constructor.
    @param owner The owning parent object
    */
    Inquiry(HciSocket* socket = NULL, QObject *owner = NULL,
        QString = QString::null);

    /** Destructor. */
    virtual ~Inquiry();

    // General/Unlimited Inquiry Access Code
    static const int GIAC = 0x9E8B33;

    // Limited Dedicated Inquiry Access Code
    static const int LIAC = 0x9E8B00;

    /** Starts the inquiry.
    If if you start inquiry several times without calling clear(),
    you will receive each result only once.
    @param numResponses Number of responses after which the inquiry
    will be stopped. 0 means no limit.
    @param timeout duration of the inquiry in seconds.
    It will be rounded to the next multiple of 1.28 sec,
    with a maximum of 61.44 sec.
    @param lap
    */
    bool inquiry(int numResponses = 0, double timeout = 8.0,
        int lap = GIAC);

    /** Enters periodic inquiry mode.
    An inquiry will be started at a random time in the interval
    between minduration and maxduration.
    @param minduration minimum time between two inquiries. Rounded to
        a multiple of 1.28, (3.84 <= d <= 83884.8)
    @param maxduration maximum time between two inquiries. Rounded to
        a multiple of 1.28, (3.84 <= d <= 83884.8)
    @param inquiryduration duration of the inquiry in seconds.
        It will be rounded to the next multiple of 1.28 sec,
        with a maximum of 61.44 sec.
    @param numResponses Number of responses after which the inquiry
        will be stopped. 0 means no limit.
    @param lap
    */
    void periodicInquiryMode(double minduration, double maxduration,
        double inquiryduration = 8.0, int numResponses = 0,
        int lap = LIAC);

    /** checks if there the inquiry is running currently
    @return true if there is an inquiry running
    which was started by this object.
    @todo possibility to check for "foreign" inquiries. Deal with
    the fact that we can receive foreign inquiry results.
    */
    bool inquiryInProgress();

    /** Synchronous access to inquiry results.
    This function allows to retrieve in results inside a loop instead
    of waiting for signals. While waiting for new results,
    QApplication::processEvents will be called.
    @param bdaddr [out] device address of the found device if
    return value is true, undefined otherwise.
    @return true if another address could be retrieved. false if
    the inquiry has finnished.
    */
    bool nextNeighbour(KBluetooth::DeviceAddress& bdaddr,
        int& deviceClass);

    /** This function returns true after after an inquiry was
    started, results were received and the inquiry ended successfully.
    This can be the case without calling inquiry() at all, because
    results of inquiries started by other programs are also processed. 
    */
    bool isComplete();
      
    /** Resets the list of received results and sets 
    isInquiryComplete() to false.
    */
    void reset();
      
protected:
    /** Called when a new neighbor was found. The default
    implementation emits the neighboutFound signal.
    @param bdaddr the address found.
    */
    virtual void onNeighbourFound(
        const KBluetooth::DeviceAddress& bdaddr, int deviceClass);

    enum ErrorCode {
         InquiryFinnishedTimeout = 0x0100
    };


signals:
    /** Emitted if a new device was found, unless onNeighbourFound()
    is overwritten.
    @param bdaddr the address found
    */
    void neighbourFound(const KBluetooth::DeviceAddress& bdaddr,
        int deviceClass);

    /** Emitted after an inquiry has finnished successfully.
    If the inquiry was canceled, no finnished signals is emitted.
    This signal can be emitted without having called inquiry, since
    other processes may have started an inquiry.
    */
    void finnished();

    /** Emitted instead of finnished if an error occurred after
    calling inquiry() or periodicInquiryMode()
    @param code error code.
    @param message Error message
    */
    void error(int code, QString message);

private:
    struct InquiryInfo {
        DeviceAddress addr;
        int deviceClass;
    };

    std::set<KBluetooth::DeviceAddress> addrCache;
    //double currentTimeout;
    //QByteArray* buf;
    //QSocket* hciSocket;
    QGuardedPtr<HciSocket> socket;
    //QDateTime *startTime;
    QTimer *inquiryTimeoutTimer;
    std::deque<InquiryInfo> infoQueue;
    bool successfullyStarted;
    bool successfullyEnded;
    
private slots:
    void slotInquiryTimeout();
    void slotHciEvent(unsigned char eventCode, QByteArray buf);
};


}
#endif
