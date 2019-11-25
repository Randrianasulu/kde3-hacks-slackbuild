/***************************************************************************
 *   Copyright (C) 2003 by Stefano Rivoir                                  *
 *   s.rivoir@gts.it                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KMIXERITEM_H
#define KMIXERITEM_H

#include "alsa/asoundlib.h"
#include <qstring.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qwidget.h>

#include "kchannel.h"
#include "kmixconst.h"
#include "kmixer.h"

#include "config.h"

class KMixer;
class KItemsGroup;

#define KAMIX_ALL_CHANNELS 65535
#define KAMIX_NO_DB        65535
/**
 * @short This class represents a single item of the mixer
 * @author Stefano Rivoir <s.rivoir@gts.it>
 */
class KMixerItem : public QWidget
{
Q_OBJECT
private:

    QString aName, cName;
    long minPVolume, maxPVolume;
    long minCVolume, maxCVolume;
    long minPdB, maxPdB;
    long minCdB, maxCdB;
    bool hPSwitch, hCSwitch, hCoSwitch;
    bool hPSwitchJoined, hCSwitchJoined;
    bool hPVolume, hCVolume, hCoVolume;
    bool hPEnum, hCEnum;
    
public:

    /**
      @brief Default ctor
      @param e A pointer to the ALSA mixer structure
      @param m A pointer to the parent mixer
      @param idd The id this item will have
    */ 
    KMixerItem( snd_mixer_elem_t *e,KMixer *m, int idd );

    /**
      @brief Default dtor
    */
    ~KMixerItem();

    snd_mixer_elem_t *aelem;
    int		id;

    KMixer      *mix;
    void	setAlsaName ( const QString& );
    void	setCustomName ( const QString& );
    
    const QString& name();
    const QString& alsaName();
    const QString& customName();
    
    void        mixerChanged();
    
    QPtrList<KChannel> pbChannels;
    QPtrList<KChannel> cpChannels;
    QPtrList<KChannel> *channels ( KMixConst::Sense );
    
    QStringList        valueNames;

    bool	isActive;
    int		pbGroup;
    int		cpGroup;
    
    // Capabilities
    bool	canPlayback;
    bool	canCapture;
    bool	hasEnum( KMixConst::Sense );
    bool	hasSwitch ( KMixConst::Sense );
    bool	hasSwitchJoined ( KMixConst::Sense );
    bool	hasVolume ( KMixConst::Sense );
    bool	mono ( KMixConst::Sense );
    bool	hasCaptureCapabilities();
    bool	hasPlaybackCapabilities();
    
    // Limits
    long        minVolume(KMixConst::Sense);
    long        maxVolume(KMixConst::Sense);

    int		muted ( KMixConst::Sense, unsigned int chan );
    long        volume ( KMixConst::Sense, unsigned int chan=KAMIX_ALL_CHANNELS );
#ifdef HAS_SND_DECIBEL
    double	dB( KMixConst::Sense, unsigned int chan=KAMIX_ALL_CHANNELS );
#endif
    long	percent ( KMixConst::Sense, unsigned int chan=KAMIX_ALL_CHANNELS );
    
    int         enumValue();
    
    bool	initialized;
    
public slots:
    void	toggleMuted ( KMixConst::Sense, unsigned int chan=KAMIX_ALL_CHANNELS );
    void        setVolume ( KMixConst::Sense, long val,   unsigned int chan=KAMIX_ALL_CHANNELS );
    void        setMuted  ( KMixConst::Sense, int muted, unsigned int chan=KAMIX_ALL_CHANNELS );
    void        setEnumValue ( int en );
    
signals:
    void	changed(KMixerItem*);

};

#endif
