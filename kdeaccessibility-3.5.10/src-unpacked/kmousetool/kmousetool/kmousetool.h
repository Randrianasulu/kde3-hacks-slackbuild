/***************************************************************************
                          kmousetool.h  -  description
                             -------------------
    begin                : Sun Jan 20 23:27:58 PST 2002
    copyright            : (C) 2002-2003 by Jeff Roush
    email                : jeff@mousetool.com
    copyright            : (C) 2003 by Olaf Schmidt
    email                : ojschmidt@kde.org
    copyright            : (C) 2003 by Gunnar Schmi Dt
    email                : gunnar@schmi-dt.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KMOUSETOOL_H
#define KMOUSETOOL_H

#include <qdir.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "version.h"

#include <kapplication.h>
#include <ksystemtray.h>
#include <qwidget.h>
#include "mtstroke.h"
#include "kmousetoolui.h"

class QLineEdit;
class QLabel;
class QCheckBox;
class QPushButton;
class KAudioPlayer;
class KAboutApplication;
class KMouseToolTray;


/**
 * KMouseTool is the base class of the project
 *
 * It is the main widget for the dialog
 *
 */

class KMouseTool : public KMouseToolUI
{
	Q_OBJECT

private:
	MTStroke stroke;

	// boolean flags to keep track of program states
	int mouse_is_down;
	int continue_timer;
	int tick_count;
	int dwell_time;
	int drag_time;
	int max_ticks;
	int min_movement;
	bool smart_drag_on;
	bool playSound;
	bool mousetool_is_running;
	bool mousetool_just_started;
	bool moving;
	bool strokesEnabled;

	QString autostartdirname;
	QString rcfilename;
	QString appfilename;
	QString	mSoundFileName;
	KAudioPlayer *mplayer;
	KMouseToolTray *trayIcon;

	KAboutApplication *aboutDlg;

	/**
	 * Initialize all variables
	 */
	void init_vars();

	/**
	 * Take care of details of playing .wav file
	 *
	 * Currently uses KAudioPlayer::play(), which has an annoying delay.
	 *
	 * The solution seems to be to use MCOP, but I haven't been able to get that to work yet.
	 */
	void playTickSound();

	/**
	 * Load state of program from "kmousetool.rc" in the user's local .kde folder,
	 *
	 */
	void loadOptions();

	/**
	 * Save state of program under the user's local .kde folder,
	 * in a file named "kmousetool.rc"
	 *
	 */
    void saveOptions();

	/**
	 * This function changes text on button depending on
	 * state of time (either "start", or "stop").
	 **/
	void updateStartStopText();

	/**
	 * Returns true if the current values in the settings window are different
	 * from the settings currently used
	 **/
	bool newSettings();

	/**
	 * Returns true if the current values in the settings window are identical
	 * with the default settings
	 **/
	bool defaultSettings();

	/**
	 * Resets the values in the settings window to the settings currently used
	 **/
	void resetSettings();

	/**
	 * Sets the values in the settings window to the default settings
	 **/
	void setDefaultSettings();

	/**
	 * Applies the current values in the settings window
	 **/
	bool applySettings();

	bool isAutostart();
	void setAutostart (bool start);

public slots:
	/**
	 * This slot is called whenever a value in the settings window was changed.
	 * It enabled and disables the three buttons "Defaults", "Reset" and "Apply".
	 **/
	void settingsChanged();

	void startStopSelected();

	void defaultSelected();
	void resetSelected();
	void applySelected();

	void helpSelected();
	void closeSelected();
	void quitSelected();

	void aboutSelected();
	void configureSelected();

public:
	/**
	 * This function runs the show; it is called once every
	 * 1/10 second.
	 *
	 * It checks to see if SmartDrag is on, and compares the
	 * current mouse position to its previous position to see
	 * whether to send a down click, and up click, or wait.
	 */
	void timerEvent (QTimerEvent *e);

	/**
	 * This generates a normal click event --
	 * down, up, or down-up, depending on smart-drag settings and current state
	 */
	void normalClick();

	/**
	 *  construtor
	 */
	KMouseTool(QWidget* parent=0, const char *name=0);

	/** destructor */
	~KMouseTool();
};

class KMouseToolTray : public KSystemTray {
        Q_OBJECT
	int startStopId;
public:
	KMouseToolTray (QWidget *parent=0, const char *name=0);
	~KMouseToolTray();

	void updateStartStopText (bool mousetool_is_running);
signals:
	void startStopSelected();
	void configureSelected();
	void aboutSelected();
	void helpSelected();
};
#endif
