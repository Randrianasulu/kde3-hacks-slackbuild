#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# Common process stuff for the setup tools backends.
#
# Copyright (C) 2000-2001 Ximian, Inc.
#
# Authors: Carlos Garnacho Parro  <carlosg@gnome.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

$SCRIPTSDIR = "/usr/share/apps/knetworkconf/backends";
if ($SCRIPTSDIR =~ /^@scriptsdir[@]/)
{
  $SCRIPTSDIR = ".";
  $DOTIN = ".in";
}

require "$SCRIPTSDIR/general.pl$DOTIN";
require "$SCRIPTSDIR/file.pl$DOTIN";

sub gst_process_kill_by_pidfile
{
  my ($pidfile) = @_;
  my ($buf, $pid);

  if ($pidfile !~ /^\//)
  {
    # add default pidfiles path if it isn't full path
    $pidfile = "/var/run/" . $pidfile;
  }

  if (&gst_file_exists ($pidfile))
  {
    $buf = &gst_file_buffer_load ($pidfile);
    $pid = $$buf[0];

    &gst_file_run ("kill -9 $pid");
  }
}

1;
