/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kamconfig.h"

KAMConfig *KAMConfig::_me = 0;

KAMConfig::KAMConfig()
 : KConfigSkeleton()
{

    setCurrentGroup ( "General" );
    
#ifdef VUMETER
    addItemBool ( "UseVUMeter", useVumeter, true );
#endif

    addItemBool ( "AllowLoadLevels", allowLoadLevels, false );
    addItemBool ( "AlwaysSystray", alwaysSystray, false );
    addItemBool ( "Debug", debugInfo, false );
    addItemLong ( "DefaultCard", defaultCard, 0 );
    addItemBool ( "HideInactive", hideInactive, true );
    addItemBool ( "ItemsFrame", itemsFrame, false );
    addItemFont ( "LBLFont", lblFont, QFont("Arial", 8, QFont::Bold) );
    addItemBool ( "LoadLevels", loadLevels, false );
    addItemBool ( "MidButtonMutes", midButtonMutes, false );
    addItemColor( "OSDBackColor", osdBackcolor, QColor(Qt::blue) );
    addItemFont ( "OSDFont", osdFont, QFont("Arial", 8, QFont::Bold) );
    addItemColor( "OSDForeColor", osdForecolor, QColor(Qt::yellow) );
    addItemLong ( "OSDPosition", osdPosition, 0 );
    addItemLong ( "OSDPositionX", osdPositionX, 10 );
    addItemLong ( "OSDPositionY", osdPositionY, 10 );
    addItemBool ( "RestoreToSystray", restoreToSystray, false );
    addItemBool ( "RestoreWindow", restoreWindow, true );
    addItemBool ( "SaveLevels", saveLevels, false );
    addItemBool ( "ShowBal", showBal, false );
    addItemBool ( "ShowFrameAroundGroups", showFrameAroundGroups, true );
    addItemBool ( "ShowMeter", showMeter, true );
    addItemBool ( "ShowMute", showMute, true );
    addItemBool ( "ShowOSD", showOSD, true ); 
    addItemBool ( "ShowOSDExtern", showOSDExtern, true ); 
    addItemBool ( "ShowTicks", showTicks, true );
    addItemLong ( "ViewType", viewType, 0 );
    addItemLong ( "VolumeMode", volumeMode, 0 );
    addItemRect ( "WindowGeometry", windowGeometry, QRect(0,0,0,0) );
    addItemPoint( "WindowPosition", windowPosition, QPoint ( 10, 10 ) );
    addItemBool ( "TrayWarning", trayWarning, true );
    addItemString( "VolUpShortcut", volUpShortcut, "" );
    addItemString( "VolDnShortcut", volDnShortcut, "" );
    
}

KAMConfig::~KAMConfig()
{
}

const QStringList KAMConfig::playbackGroups ( const QString& sect ) {
    config()->setGroup ( sect );
    return ( config()->readListEntry ( "PlaybackGroups", ',' ) );
}

const QStringList KAMConfig::captureGroups ( const QString& sect ) {
    config()->setGroup ( sect );
    return ( config()->readListEntry ( "CaptureGroups", ',' ) );
}

bool KAMConfig::splitPbChannels( const QString& sect, int num ) {
    return rawBool ( sect, "SplitPbChannels", false, num );
}

bool KAMConfig::splitCpChannels( const QString& sect, int num ) {
    return rawBool ( sect, "SplitCpChannels", false, num );
}

void KAMConfig::setSplitCpChannels( const QString& sect, int num, bool val ) {
    setRawBool ( sect, "SplitCpChannels", val, num );
}

void KAMConfig::setSplitPbChannels( const QString& sect, int num, bool val ) {
    setRawBool ( sect, "SplitPbChannels", val, num );
}

bool KAMConfig::showPlayback ( const QString& sect, int num ) {
    return rawBool ( sect, "ShowPlayback", true, num );
}

bool KAMConfig::showCapture ( const QString& sect, int num ) {
    return rawBool ( sect, "ShowCapture", true, num );
}

void KAMConfig::setShowCapture( const QString& sect, int num, bool val ) {
    setRawBool ( sect, "ShowCapture", val, num );
}

void KAMConfig::setShowPlayback( const QString& sect, int num, bool val ) {
    setRawBool ( sect, "ShowPlayback", val, num );
}

void KAMConfig::setPlaybackOrder( const QString& sect, int num, long val ) {
    setRawLong( sect, "PlaybackOrder", val, num );
}

void KAMConfig::setPlaybackGroup( const QString& sect, int num, long val ) {
    setRawLong( sect, "PlaybackGroup", val, num );
}

void KAMConfig::setCaptureOrder( const QString& sect, int num, long val ) {
    setRawLong( sect, "CaptureOrder", val, num );
}

void KAMConfig::setCaptureGroup( const QString& sect, int num, long val ) {
    setRawLong( sect, "CaptureGroup", val, num );
}

void KAMConfig::setCustomName ( const QString& sect, int num, const QString& val) {
    setRawString ( sect, "CustomName", val, num );
}

long KAMConfig::playbackOrder( const QString& sect, int num ) {
    return rawLong( sect, "PlaybackOrder", -1, num );
}

long KAMConfig::captureOrder( const QString& sect, int num ) {
    return rawLong( sect, "CaptureOrder", -1, num );
}

long KAMConfig::cardHidden ( const QString& sect ) {
    return rawLong( sect, "HideCard", 0, -1 );
}

bool KAMConfig::rawBool( const QString &sect, const QString &key, bool def, int num ) {
    config()->setGroup ( sect );
    if ( num == -1 )
        return config()->readBoolEntry ( key, def );
    else {
        QString s1, s2;
	s2.sprintf ( "%d", num );
	s1 = key + "_" + s2;
	return config()->readBoolEntry ( s1, def );
    }
}

long KAMConfig::rawLong( const QString &sect, const QString &key, long def, int num ) {
    config()->setGroup ( sect );
    if ( num == -1 )
        return config()->readNumEntry ( key, def );
    else {
        QString s1, s2;
	s2.sprintf ( "%d", num );
	s1 = key + "_" + s2;
	return config()->readNumEntry ( s1, def );
    }
}

const QString KAMConfig::rawString( const QString &sect, const QString &key, const QString &def, int num ) {
    config()->setGroup ( sect );
    if ( num == -1 ) {
        return config()->readEntry ( key, def );
    } else {
        QString s1, s2;
	s2.sprintf ( "%d", num );
	s1 = key + "_" + s2;
	return config()->readEntry ( s1, def );
    }
}

void KAMConfig::setRawBool ( const QString &sect, const QString &key, bool val, int num ) {
    config()->setGroup ( sect );
    if ( num == -1 )
        return config()->writeEntry ( key, val );
    else {
        QString s1, s2;
	s2.sprintf ( "%d", num );
	s1 = key + "_" + s2;
	return config()->writeEntry ( s1, val );
    }
}

void KAMConfig::setRawLong ( const QString &sect, const QString &key, long val, int num ) {
    config()->setGroup ( sect );
    if ( num == -1 )
        return config()->writeEntry ( key, val );
    else {
        QString s1, s2;
	s2.sprintf ( "%d", num );
	s1 = key + "_" + s2;
	return config()->writeEntry ( s1, val );
    }
}

void KAMConfig::setRawString ( const QString &sect, const QString &key, const QString& val, int num ) {
    config()->setGroup ( sect );
    if ( num == -1 )
        return config()->writeEntry ( key, val );
    else {
        QString s1, s2;
	s2.sprintf ( "%d", num );
	s1 = key + "_" + s2;
	return config()->writeEntry ( s1, val );
    }
}

void KAMConfig::setRawList ( const QString &sect, const QString &key, const QStringList &val, int num ) {
    config()->setGroup ( sect );
    if ( num == -1 )
        return config()->writeEntry ( key, val );
    else {
        QString s1, s2;
	s2.sprintf ( "%d", num );
	s1 = key + "_" + s2;
	return config()->writeEntry ( s1, val );
    }
}

void KAMConfig::setCaptureGroups ( const QString& sect, const QStringList& groups ) {
    setRawList ( sect, "CaptureGroups", groups );
}

void KAMConfig::setPlaybackGroups ( const QString& sect, const QStringList& groups ) {
    setRawList ( sect, "PlaybackGroups", groups );
}

void KAMConfig::setPreset ( const QString& pset, const QString& val ) {
    config()->setGroup ( "Presets" );
    config()->writeEntry ( pset, val );
}

void KAMConfig::delPreset ( const QString& pset ) {
    config()->setGroup ( "Presets" );
    config()->deleteEntry ( pset );
}

KAMConfig* KAMConfig::self() {
    if ( !_me )
        _me = new KAMConfig();
    return _me;
}

