/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KAMIXCONFIG_H
#define KAMIXCONFIG_H

#include <kconfigskeleton.h>
#include <qfont.h>
#include <qcolor.h>
#include <qrect.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/**
@author Stefano Rivoir
*/
class KAMConfig : public KConfigSkeleton
{
public:

    static KAMConfig *self();
    
    bool allowLoadLevels;
    bool alwaysSystray;
    bool debugInfo;
    long defaultCard;
    bool hideInactive;
    bool itemsFrame;
    QFont lblFont;
    bool loadLevels;
    bool midButtonMutes;
    QColor osdBackcolor;
    QFont osdFont;
    QColor osdForecolor;
    long osdPosition;
    long osdPositionX;
    long osdPositionY;
    bool restoreToSystray;
    bool restoreWindow;
    bool saveLevels;
    bool showAlternativeShortcutConfig;
    bool showBal;
    bool showFrameAroundGroups;
    bool showMeter, showMute, showOSD, showOSDExtern, showTicks;
    long viewType;
    long volumeMode;
    
    QRect windowGeometry;
    QPoint windowPosition;
    bool trayWarning;
    QString volUpShortcut;
    QString volDnShortcut;

#ifdef VUMETER
    bool useVumeter;
#endif
    
    const QStringList captureGroups ( const QString& sect );
    const QStringList playbackGroups ( const QString& sect );
    void setPlaybackGroups ( const QString&, const QStringList& );
    void setCaptureGroups ( const QString&, const QStringList& );
    
    bool splitPbChannels ( const QString& sect, int num );
    bool splitCpChannels ( const QString& sect, int num );
    void setSplitPbChannels ( const QString& sect, int num, bool val );
    void setSplitCpChannels ( const QString& sect, int num, bool val );
    
    bool showPlayback ( const QString& sect, int num );
    bool showCapture ( const QString& sect, int num );
    long playbackOrder ( const QString& sect, int num );
    long captureOrder  ( const QString& sect, int num );
    long cardHidden ( const QString& sect );
    
    void setShowPlayback ( const QString& sect, int num, bool val );
    void setShowCapture ( const QString& sect, int num, bool val );
    void setCustomName ( const QString& sect, int num, const QString& val );
    void setPlaybackOrder ( const QString& sect, int num, long val );
    void setCaptureOrder  ( const QString& sect, int num, long val );
    void setPlaybackGroup ( const QString& sect, int num, long val );
    void setCaptureGroup  ( const QString& sect, int num, long val );
    
    bool rawBool ( const QString& sect, const QString& key, bool def, int num=-1 );
    long rawLong ( const QString& sect, const QString& key, long def, int num=-1 );
    const QString rawString ( const QString& sect, const QString& key, const QString& def, int num=-1 );
    void setRawBool ( const QString& sect, const QString& key, bool val, int num=-1 );
    void setRawLong ( const QString& sect, const QString& key, long val, int num=-1 );
    void setRawList ( const QString& sect, const QString& key, const QStringList &val, int num=-1 );
    void setRawString ( const QString& sect, const QString& key, const QString& val, int num=-1 );

    void setPreset ( const QString&, const QString& );
    void delPreset ( const QString& );
    
private:
    KAMConfig();
    ~KAMConfig();
    
    static KAMConfig *_me;

};

#endif
