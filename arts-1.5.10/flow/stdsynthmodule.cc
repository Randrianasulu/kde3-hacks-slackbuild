    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include "stdsynthmodule.h"
#include "gslschedule.h"
#include "audiosubsys.h"
#include "flowsystem.h"
#include "debug.h"

using namespace Arts;
using namespace std;

StdSynthModule::StdSynthModule()
{
	// TODO: maybe this should be handled over some other mechanism

	samplingRate = AudioSubSystem::the()->samplingRate();
	samplingRateFloat = (float)samplingRate;
}

void StdSynthModule::streamInit()
{
}

void StdSynthModule::streamStart()
{
}

void StdSynthModule::streamEnd()
{
}

void StdSynthModule::start()
{
	_node()->start();
}

void StdSynthModule::stop()
{
	_node()->stop();
}

AutoSuspendState StdSynthModule::autoSuspend()
{
	return asNoSuspend;
}

unsigned long StdSynthModule::inputConnectionCount(const std::string& port)
{
	StdScheduleNode *xnode = (StdScheduleNode *)
		_node()->cast("StdScheduleNode");
	arts_return_val_if_fail(xnode, 0);

	return xnode->inputConnectionCount(port);
}

unsigned long StdSynthModule::outputConnectionCount(const std::string& port)
{
	StdScheduleNode *xnode = (StdScheduleNode *)
		_node()->cast("StdScheduleNode");
	arts_return_val_if_fail(xnode, 0);

	return xnode->outputConnectionCount(port);
}

bool StdSynthModule::connectionCountChanged()
{
	StdScheduleNode *xnode = (StdScheduleNode *)
		_node()->cast("StdScheduleNode");
	arts_return_val_if_fail(xnode, 0);

	return xnode->connectionCountChanged();
}
