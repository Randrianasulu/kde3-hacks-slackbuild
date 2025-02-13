#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# Functions for manipulating system services, like daemons and network.
#
# Copyright (C) 2002 Ximian, Inc.
#
# Authors: Carlos Garnacho Parro <garparr@teleline.es>,
#          Hans Petter Jansson <hpj@ximian.com>,
#          Arturo Espinosa <arturo@ximian.com>
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

$rcd_path;
$initd_path;
$relative_path;

$SCRIPTSDIR = "/usr/share/apps/knetworkconf/backends";
$FILESDIR = "@filesdir@";
if ($SCRIPTSDIR =~ /^@scriptsdir[@]/)
{
    $FILESDIR = "files";
    $SCRIPTSDIR = ".";
    $DOTIN = ".in";
}

use File::Copy;

require "$SCRIPTSDIR/file.pl$DOTIN";
require "$SCRIPTSDIR/report.pl$DOTIN";
require "$SCRIPTSDIR/service-list.pl$DOTIN";

# Where is the SysV subsystem installed?
sub gst_service_sysv_get_paths
{
  my %dist_map =
      (
       # gst_dist => [rc.X dirs location, init.d scripts location, relative path location]
       "redhat-5.2"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "redhat-6.0"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "redhat-6.1"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "redhat-6.2"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "redhat-7.0"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "redhat-7.1"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "redhat-7.2"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "redhat-7.3"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "redhat-8.0"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "redhat-9"     => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "openna-1.0"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],

       "mandrake-7.1" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandrake-7.2" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandrake-9.0" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandrake-9.1" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandrake-9.2" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandrake-10.0" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandrake-10.1" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandrake-10.2" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandriva-2006.0" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandriva-2006.1" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandriva-2007.0" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "mandriva-2007.1" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "yoper-2.2" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],

       "blackpanther-4.0" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],

       "conectiva-9"  => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "conectiva-10" => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],

       "debian-2.2"   => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "debian-3.0"   => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "debian-3.1"   => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "debian-4.0"   => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "debian-5.0"   => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "debian-testing" => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "ubuntu-5.04"  => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "ubuntu-5.10"  => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "ubuntu-6.06"  => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "ubuntu-6.10"  => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "ubuntu-7.04"  => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "ubuntu-7.10"  => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       "ubuntu-8.04"  => ["$gst_prefix/etc", "$gst_prefix/etc/init.d", "../init.d"],
       
       "suse-7.0"     => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d", "../"],
       "suse-9.0"     => ["$gst_prefix/etc/init.d", "$gst_prefix/etc/init.d", "../"],
       "suse-9.1"     => ["$gst_prefix/etc/init.d", "$gst_prefix/etc/init.d", "../"],

       "turbolinux-7.0"   => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],

       "pld-1.0"      => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "pld-1.1"      => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "pld-1.99"     => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],

       "fedora-1"     => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "fedora-2"     => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "fedora-3"     => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "fedora-4"     => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "fedora-5"     => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "rpath"        => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],

       "vine-3.0"     => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "vine-3.1"     => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       "ark"          => ["$gst_prefix/etc/rc.d", "$gst_prefix/etc/rc.d/init.d", "../init.d"],
       );
  my $res;

  $res = $dist_map{$gst_dist};
  &gst_report ("service_sysv_unsupported", $gst_dist) if ($res eq undef);
  return @$res;
}

# Those runlevels that are usually used. Maybe we should add
# the current running runlevel, using the "runlevel" command.
sub gst_service_sysv_get_runlevels
{
  my %dist_map =
      (
       "redhat-5.2"     => [3, 5],
       "redhat-6.0"     => [3, 5],
       "redhat-6.1"     => [3, 5],
       "redhat-6.2"     => [3, 5],
       "redhat-7.0"     => [3, 5],
       "redhat-7.1"     => [3, 5],
       "redhat-7.2"     => [3, 5],
       "redhat-7.3"     => [3, 5],
       "redhat-8.0"     => [3, 5],
       "redhat-9"       => [3, 5],
       "openna-1.0"     => [3, 5],

       "mandrake-7.1"   => [3, 5],
       "mandrake-7.2"   => [3, 5],
       "mandrake-9.0"   => [3, 5],
       "mandrake-9.1"   => [3, 5],
       "mandrake-9.2"   => [3, 5],
       "mandrake-10.0"  => [3, 5],
       "mandrake-10.1"  => [3, 5],
       "mandrake-10.2"  => [3, 5],
       "mandriva-2006.0"  => [3, 5],
       "mandriva-2006.1"  => [3, 5],
       "mandriva-2007.0"  => [3, 5],
       "mandriva-2007.1"  => [3, 5],

       "yoper-2.2"  => [3, 5],

       "blackpanther-4.0" => [3, 5],

       "conectiva-9"    => [3, 5],
       "conectiva-10"   => [3, 5],

       "debian-2.2"     => [2, 3],
       "debian-3.0"     => [2, 3],
       "debian-3.1"     => [2, 3],
       "debian-4.0"     => [2, 3],
       "debian-5.0"     => [2, 3],
       "debian-testing" => [2, 3],
       "ubuntu-5.04"    => [2, 3],
       "ubuntu-5.10"    => [2, 3],
       "ubuntu-6.06"    => [2, 3],
       "ubuntu-6.10"    => [2, 3],
       "ubuntu-7.04"    => [2, 3],
       "ubuntu-7.10"    => [2, 3],
       "ubuntu-8.04"    => [2, 3],
       
       "suse-7.0"       => [3, 5],
       "suse-9.0"       => [3, 5],
       "suse-9.1"       => [3, 5],

       "turbolinux-7.0" => [3, 5],

       "pld-1.0"        => [3, 5],
       "pld-1.1"        => [3, 5],
       "pld-1.99"       => [3, 5],

       "fedora-1"       => [3, 5],
       "fedora-2"       => [3, 5],
       "fedora-3"       => [3, 5],
       "fedora-4"       => [3, 5],
       "fedora-5"       => [3, 5],
       "rpath"          => [3, 5],
       
       "vine-3.0"       => [3, 5],
       "vine-3.1"       => [3, 5],
       "ark"            => [3, 5],
       );
  my $res;

  $res = $dist_map{$gst_dist};
  &gst_report ("service_sysv_unsupported", $gst_dist) if ($res eq undef);
  return @$res;
}

sub gst_service_get_runlevel_roles
{
  my (%dist_map, %runlevels, $desc, $distro);
  %dist_map =
    (
     "redhat-5.2"     => "redhat-5.2",
     "redhat-6.0"     => "redhat-5.2",
     "redhat-6.1"     => "redhat-5.2",
     "redhat-6.2"     => "redhat-5.2",
     "redhat-7.0"     => "redhat-5.2",
     "redhat-7.1"     => "redhat-5.2",
     "redhat-7.2"     => "redhat-5.2",
     "redhat-7.3"     => "redhat-5.2",
     "redhat-8.0"     => "redhat-5.2",
     "redhat-9"       => "redhat-5.2",
     "openna-1.0"     => "redhat-5.2",
     
     "mandrake-7.1"   => "redhat-5.2",
     "mandrake-7.2"   => "redhat-5.2",
     "mandrake-9.0"   => "redhat-5.2",
     "mandrake-9.1"   => "redhat-5.2",
     "mandrake-9.2"   => "redhat-5.2",
     "mandrake-10.0"  => "redhat-5.2",
     "mandrake-10.1"  => "redhat-5.2",
     "mandrake-10.2"  => "redhat-5.2",
     "mandriva-2006.0"  => "redhat-5.2",
     "mandriva-2006.1"  => "redhat-5.2",
     "mandriva-2007.0"  => "redhat-5.2",
     "mandriva-2007.1"  => "redhat-5.2",

     "yoper-2.2"  => "redhat-5.2",
 
     "blackpanther-4.0" => "redhat-5.2",

     "conectiva-9"    => "redhat-5.2",
     "conectiva-10"   => "redhat-5.2",
     
     "debian-2.2"     => "debian-2.2",
     "debian-3.0"     => "debian-2.2",
     "debian-3.1"     => "debian-2.2",
     "debian-4.0"     => "debian-2.2",
     "debian-5.0"     => "debian-2.2",
     "debian-testing" => "debian-2.2",
     "ubuntu-5.04"    => "debian-2.2",
     "ubuntu-5.10"    => "debian-2.2",
     "ubuntu-6.06"    => "debian-2.2",
     "ubuntu-6.10"    => "debian-2.2",
     "ubuntu-7.04"    => "debian-2.2",
     "ubuntu-7.10"    => "debian-2.2",
     "ubuntu-8.04"    => "debian-2.2",
          
     "suse-7.0"       => "redhat-5.2",
     "suse-9.0"       => "redhat-5.2",
     "suse-9.1"       => "redhat-5.2",
     
     "turbolinux-7.0" => "redhat-5.2",
     "pld-1.0"        => "redhat-5.2",
     "pld-1.1"        => "redhat-5.2",
     "pld-1.99"       => "redhat-5.2",
     "fedora-1"       => "redhat-5.2",
     "fedora-2"       => "redhat-5.2",
     "fedora-3"       => "redhat-5.2",
     "fedora-4"       => "redhat-5.2",
     "fedora-5"       => "redhat-5.2",
     "rpath"          => "redhat-5.2",

     "vine-3.0"       => "redhat-5.2",
     "vine-3.1"       => "redhat-5.2",
     "ark"            => "redhat-5.2",

     "slackware-9.1.0" => "slackware-9.1.0",
     "slackware-10.0.0" => "slackware-9.1.0",
     "slackware-10.1.0" => "slackware-9.1.0",
     "slackware-10.2.0" => "slackware-9.1.0",

     "gentoo"         => "gentoo",
     "vlos-1.2"       => "gentoo",

     "archlinux"      => "freebsd-5",
     "freebsd-5"      => "freebsd-5",
     "freebsd-6"      => "freebsd-5",
    );

  %runlevels=
    (
     "redhat-5.2" => {"0" => "HALT",
                      "1" => "RECOVER",
                      "2" => "NONE",
                      "3" => "TEXT",
                      "4" => "NONE",
                      "5" => "GRAPHICAL",
                      "6" => "REBOOT"
                     },
     "debian-2.2" => {"0" => "HALT",
                      "1" => "RECOVER",
                      "2" => "NONE",
                      "3" => "NONE",
                      "4" => "NONE",
                      "5" => "NONE",
                      "6" => "REBOOT"
                     },
     "gentoo"     => {"boot"      => "BOOT",
                      "default"   => "GRAPHICAL",
                      "nonetwork" => "RECOVER"
                     },
     "freebsd-5"  => {"rc" => "GRAPHICAL" },
     "slackware-9.1.0" => {"4" => "GRAPHICAL" }
    );

  $distro = $dist_map{$gst_dist};
  $desc = $runlevels{$distro};

  return $runlevels{$distro};
}

# --- Plain process utilities --- #

# Get owners list (login) of named process.
sub gst_service_proc_get_owners
{
  my ($service) = @_;
  my ($user, $pid, $command);
  my ($fd);
  my (@arr);

  &gst_report_enter ();

  $fd = gst_file_run_pipe_read ("ps acx -o user,pid,command");

  while (<$fd>)
  {
    /(.*)[ \t]+(.*)[ \t]+(.*)/;
    $user    = $1;
    $pid     = $2;
    $command = $3;

    push @arr, $user if ($command eq $service);
  }

  &gst_report_leave ();
  return \@arr;
}

# Stops all instances of a process
sub gst_service_proc_stop_all
{
  my ($service) = @_;

  return &gst_file_run ("killall $service");
}

# Starts instances of a process for a given list of users
sub gst_service_proc_start_all
{
  my ($cmd, $users) = @_;
  my ($fqcmd, $fqsu);

  $fqcmd = &gst_file_get_cmd_path ($cmd);
  $fqsu  = &gst_file_locate_tool  ("su");

  foreach $user (@$users)
  {
    # Can't use gst_file_run_bg here, since it clobbers the quotes.
    system ("$fqsu $user -c \"$fqcmd &\" >/dev/null 2>/dev/null");
  }
}

sub gst_service_sysv_list_dir
{
    my ($path) = @_;
    my ($service, @services);

    foreach $service (<$path/*>)
    {
        if (-x $service)
        {
            $service =~ s/.*\///;
            push @services, $service;
        }
    }

    return \@services;
}

sub gst_service_sysv_list_available
{
    my ($rcd_path, $initd_path);
    
    ($rcd_path, $initd_path) = &gst_service_sysv_get_paths ();

    return &gst_service_sysv_list_dir ($initd_path);
}

# Return 1 or 0: is the service running?
# Depends on the rc script to support the "status" arg.
# Maybe we should do something more portable.
sub gst_service_sysv_get_status
{
  my ($service) = @_;
  my ($rc_path, $initd_path, $res);
  my ($pid);

  &gst_report_enter ();

  # Stolen from RedHat's /etc/rc.d/init.d/functions:status
  # FIXME: portable to other UNIXES?
  $pid = &gst_file_run_backtick ("pidof -o %PPID -x $service");
  chomp $pid;

  if ($pid)
  {
    $res = 1;
    &gst_report ("service_status_running", $service);
  }
  else
  {
    $res = 0;
    &gst_report ("service_status_stopped", $service);
  }
    
#  ($rcd_path, $initd_path) = &gst_service_sysv_get_paths ();
#  $res = 0;
#  
#  if (-f "$initd_path/$service")
#  {
#    $res = &gst_file_run ("$initd_path/$service status")? 0 : 1;
#    &gst_report ("service_status_running", $service) if $res;
#    &gst_report ("service_status_stopped", $service) if !$res;
#  }

  &gst_report_leave ();
  return $res;
}

# If any of the passed services is running, return true.
sub gst_service_sysv_get_status_any
{
  my (@services) = @_;
  my $i;

  foreach $i (@services)
  {
    return 1 if &gst_service_sysv_get_status ($i);
  }

  return 0;
}

# Set start links and remove stop links at the usual runlevels.
# Old start link is removed, in case the priority is different from $pri.
sub gst_service_sysv_set_links_active
{
  my ($pri, $service) = @_;

  foreach $runlevel (&gst_service_sysv_get_runlevels ())
  {
    &gst_service_sysv_remove_link ($runlevel, $service);
    &gst_service_sysv_add_link ($runlevel, "S", $pri, $service);
  }
}

# Set stop links and remove start links at the usual runlevels.
sub gst_service_sysv_set_links_inactive
{
  my ($pri, $service) = @_;

  foreach $runlevel (&gst_service_sysv_get_runlevels ())
  {
    &gst_service_sysv_remove_link ($runlevel, "$service");
    &gst_service_sysv_add_link ($runlevel, "K", $pri, $service);
  }
}

# Set links for active/inactive service at the given priority.
sub gst_service_sysv_set_links
{
  my ($pri, $service, $active) = @_;

  if ($active)
  {
    &gst_service_sysv_set_links_active ($pri, $service);
  }
  else
  {
    &gst_service_sysv_set_links_inactive (100 - $pri, $service);
  }
}



# Start or stop the service, depending on $active. Set
# links accordingly.  $force makes this function use
# start/stop only, without considerations for restart.
# Not to be called from parse/replace tables, due to last $force
# param: use the following two functions instead.
sub gst_service_sysv_set_status_do
{
  my ($priority, $service, $active, $force) = @_;
  my ($arg, $status);

  &gst_service_sysv_set_links ($priority, $service, $active);
  
  $status = &gst_service_sysv_get_status ($service);
  if ($status && !$force)
  {
    # if it's already active and you want it active, restart.
    $arg = $active? "restart" : "stop";
  }
  else
  {
    # normal operation.
    $arg = $active? "start" : "stop";
  }

  return &gst_service_sysv_run_initd_script ($service, $arg);
}

sub gst_service_sysv_set_status
{
  my ($priority, $service, $active) = @_;

  return &gst_service_sysv_set_status_do ($priority, $service, $active, 0);
}

sub gst_service_sysv_force_status
{
  my ($priority, $service, $active) = @_;

  return &gst_service_sysv_set_status_do ($priority, $service, $active, 1);
}

sub gst_service_sysv_install_script
{
  my ($service, $file) = @_;
  my ($res, $rcd_path, $initd_path);

  ($rcd_path, $initd_path) = &gst_service_sysv_get_paths ();

  if (!copy ("$FILESDIR/$file", "$initd_path/$service"))
  {
      &gst_report ("file_copy_failed", "$FILESDIR/$file", "$initd_path/$service");
      return -1;
  }

  chmod (0755, "$initd_path/$service");

  return 0;
}

# THESE ARE THE FUNCTIONS WHICH EXTRACT THE CONFIGURATION FROM THE COMPUTER

# we are going to extract the name of the script
sub gst_service_sysv_get_service_name
{
	my ($service) = @_;
	
	$service =~ s/$initd_path\///;
  
	return $service;
}

# This function gets the state of the service along the runlevels,
# it also returns the average priority
sub gst_service_sysv_get_runlevels_status
{
	my ($service) = @_;
	my ($link);
	my ($runlevel, $action, $priority);
	my (@arr, @ret);
	
	foreach $link (<$rcd_path/rc[0-6].d/[SK][0-9][0-9]$service>)
	{
		$link =~ s/$rcd_path\///;
		$link =~ /rc([0-6])\.d\/([SK])([0-9][0-9]).*/;
		($runlevel,$action,$priority)=($1,$2,$3);
		if ($action eq "S")
		{
      push @arr, { "name"     => $runlevel,
                   "priority" => $priority,
                   "action"   => "start" };
		}
		elsif ($action eq "K")
		{
			push @arr, { "name"     => $runlevel,
                   "priority" => (100 - $priority),
			             "action"   => "stop" };
		}
	}
	
	return undef if (scalar(@arr) eq 0);

	push @ret, { "runlevel" => \@arr };
	return \@ret;
}

# We are going to extract the information of the service
sub gst_service_sysv_get_service_info
{
	my ($service) = @_;
	my ($script, @actions, @runlevels);
	my %hash, $role;

	# Return if it's a directory
	return undef if (-d $service);
	
	# We have to check if the service is executable	
	return undef unless (-x $service);

	$script = &gst_service_sysv_get_service_name ($service);
		
	# We have to check out if the service is in the "forbidden" list
	return undef if (&gst_service_list_service_is_forbidden ($script));

	$runlevels = &gst_service_sysv_get_runlevels_status($script);

	$hash{"script"} = $script;
  $hash{"runlevels"} = $runlevels unless ($runlevels eq undef);
  $hash{"role"} = &gst_service_get_role ($script);

	return \%hash;
}

# This function gets an ordered array of the available services from a SysV system
sub gst_service_sysv_get_services
{
	my ($service);
	my (@arr,%ret);
	
	($rcd_path, $initd_path) = &gst_service_sysv_get_paths ();

	foreach $service (<$initd_path/*>)
	{
		my (%hash);
		$hash = &gst_service_sysv_get_service_info ($service);
		if ($hash ne undef)
		{
      $ret{$service} = $hash;
		}
	}

	return \%ret;
}

# This functions get an ordered array of the available services from a file-rc system
sub gst_service_filerc_get_runlevels_status
{
  my ($start_service, $stop_service, $priority) = @_;
  my (@arr, @ret);

  # we start with the runlevels in which the service starts
  if ($start_service !~ /-/) {
    my (@runlevels);

    @runlevels = split /,/, $start_service;

    foreach $runlevel (@runlevels)
    {
      push @arr, { "name"     => $runlevel,
                   "action"   => "start",
                   "priority" => $priority};
    }
  }

  # now let's go with the runlevels in which the service stops
  if ($stop_service !~ /-/) {
    my (@runlevels);

    @runlevels = split /,/, $stop_service;

    foreach $runlevel (@runlevels)
    {
      push @arr, { "name"     => $runlevel,
                   "action"   => "stop",
                   "priority" => $priority};
    }
  }

  push @ret, {"runlevel" => \@arr};
  return \@ret;
}

sub gst_service_filerc_get_service_info
{
  my ($line, %ret) = @_;
  my %hash;
  my @runlevels;

  if ($line =~ /^([0-9][0-9])[\t ]+([0-9\-S,]+)[\t ]+([0-9\-S,]+)[\t ]+\/etc\/init\.d\/(.*)/)
  {
    $priority = $1;
    $stop_service = $2;
    $start_service = $3;
    $script = $4;

    return undef if (&gst_service_list_service_is_forbidden ($script));

    $hash{"script"} = $script;

    $hash{"runlevels"} = &gst_service_filerc_get_runlevels_status ($start_service, $stop_service, $priority);
    $hash{"role"} = &gst_service_get_role ($script);

    return (\%hash);
  }

  return undef;
}

sub gst_service_filerc_get_services
{
	my ($script);
  my (%ret);
	
  open FILE, "$gst_prefix/etc/runlevel.conf" or return undef;
  while ($line = <FILE>)
  {
    if ($line !~ /^#.*/)
    {
      my (%hash);
      my ($start_service, $stop_service);
      $hash = &gst_service_filerc_get_service_info ($line);

      if ($hash ne undef)
      {
        $script = $$hash{"script"};

        if ($ret{$script} eq undef)
        {
          $ret{$script} = $hash;
        }
        else
        {
          my (@runlevels);

          # We need to mix the runlevels
          @runlevels = $$hash{"runlevels"}[0]{"runlevel"};
          foreach $runlevel (@runlevels)
          {
            push @{$ret{$script}{"runlevels"}[0]{"runlevel"}}, $runlevel;
          }
        }
      }
    }
  }

  return \%ret;
}

# this functions get a list of the services that run on a bsd init
sub gst_service_bsd_get_service_info
{
  my ($service) = @_;
  my ($script);
  my (%hash);
	my (@arr, @rl);

  $script = $service;
  $script =~ s/^.*\///;
  $script =~ s/^rc\.//;

  return undef if (! gst_file_exists ($service));

  return undef if (&gst_service_list_service_is_forbidden ($script));

  $hash {"script"} = $service;

  # we hardcode the fourth runlevel, it's the graphical one
  if ( -x $service)
  {
    push @arr, { "name"   => 4,
                 "action" => "start" };
  }
  else
  {
    push @arr, { "name"   => 4,
                 "action" => "stop" };
  }

	push @rl, { "runlevel" => \@arr };
  
	$hash{"runlevels"} = \@rl;
  $hash{"role"} = &gst_service_get_role ($script);
  
  return \%hash;
}

sub gst_service_bsd_get_services
{
  my (%ret);
  my ($files) = [ "rc.M", "rc.inet2", "rc.4" ];
  my ($file);

  foreach $i (@$files)
  {
    $file = "/etc/rc.d/" . $i;
    $fd = &gst_file_open_read_from_names ($file);

    if (!$fd) {
      &gst_report ("rc_file_read_failed", $file);
      return undef;
    }

    while (<$fd>)
    {
      $line = $_;

      if ($line =~ /^if[ \t]+\[[ \t]+\-x[ \t]([0-9a-zA-Z\/\.\-_]+) .*\]/)
      {
        my (%hash);
        $service = $1;

        $hash = &gst_service_bsd_get_service_info ($service);

        if ($hash ne undef)
        {
          $ret{$service} = $hash;
        }
      }
    }

    gst_file_close ($fd);
  }

  return \%ret;
}

# these functions get a list of the services that run on a gentoo init
sub gst_service_gentoo_get_service_status
{
  my ($script, $runlevel) = @_;
  my ($services) = &gst_service_gentoo_get_services_by_runlevel ($runlevel);

  foreach $i (@$services)
  {
    return 1 if ($i eq $script);
  }

  return 0;
}

sub gst_service_gentoo_get_runlevels
{
  my($raw_output) = gst_file_run_backtick("rc-status -l");
  my(@runlevels) = split(/\n/,$raw_output);
    
  return @runlevels;
}

sub gst_service_gentoo_get_services_by_runlevel
{
  my($runlevel) = @_;
  my($raw_output) = gst_file_run_backtick("rc-status $runlevel");
  my(@raw_lines) = split(/\n/,$raw_output);
  my(@services);
  my($line);

  foreach $line (@raw_lines)
  {
    if ($line !~ /^Runlevel/)
    {
      $line=(split(" ",$line))[0];
	    push(@services,$line);
	  }
  }

  return \@services
}

sub gst_service_gentoo_get_services_list
{
  return &gst_service_sysv_list_dir ("/etc/init.d/");
}

sub gst_service_gentoo_service_exist
{
  my($service) = @_;
  my($services) = &gst_service_gentoo_get_services_list();

  foreach $i (@$services)
  {
    return 1 if ($i =~ /$service/);
  }

  return 0;
}

sub gst_service_gentoo_get_runlevels_by_service
{
  my ($service) = @_;
  my(@runlevels,@services_in_runlevel,@contain_runlevels, $runlevel);
  my ($elem);

  # let's do some caching to improve performance
  if ($gentoo_services_hash eq undef)
  {
    @runlevels = &gst_service_gentoo_get_runlevels ();

    foreach $runlevel (@runlevels)
    {
      $$gentoo_services_hash{$runlevel} = &gst_service_gentoo_get_services_by_runlevel ($runlevel);
    }
  }

  if (&gst_service_gentoo_service_exist($service))
  {
    foreach $runlevel (keys %$gentoo_services_hash)
    {
      $services_in_runlevel = $$gentoo_services_hash {$runlevel};

      foreach $elem (@$services_in_runlevel)
      {
        push (@contain_runlevels, $runlevel) if ($elem eq $service);
      }
    }
  }

  return @contain_runlevels;
}

sub gst_service_gentoo_runlevel_status_by_service
{
  my ($service) = @_;
  my (@arr, @ret);
  my (@runlevels) = &gst_service_gentoo_get_runlevels();
  my (@started) = &gst_service_gentoo_get_runlevels_by_service($service);
  my (%start_runlevels) = map { $started[$_], 1 } 0 .. $#started;

  foreach $runlevel (@runlevels)
  {
    if (defined $start_runlevels{$runlevel})
    {
      push @arr, { "name"   => $runlevel,
                   "action" => "start" };
    }
    else
    {
      push @arr, { "name"   => $runlevel,
                   "action" => "stop" };
    }
  }

  push @ret, { "runlevel" => \@arr };
  return @ret;
}

sub gst_service_gentoo_get_service_info
{
	my ($service) = @_;
	my ($script, @actions, @runlevels);
	my %hash;
	
	# We have to check out if the service is in the "forbidden" list
	return undef if (&gst_service_list_service_is_forbidden ($service));

	my($runlevels) = &gst_service_gentoo_runlevel_status_by_service ($service);

	$hash{"script"} = $service;
	$hash{"runlevels"} = $runlevels unless ($runlevels eq undef);
  $hash{"role"} = &gst_service_get_role ($service);

	return \%hash;
}

sub gst_service_gentoo_get_services
{
  my ($service);
  my (%ret);
  my ($service_list) = &gst_service_gentoo_get_services_list ();

  foreach $service (@$service_list)
  {
    my (%hash);
    $hash = &gst_service_gentoo_get_service_info ($service);

    $ret{$service} = $hash if ($hash ne undef);
  }

  return \%ret;
}

# rcNG functions, mostly for FreeBSD


sub gst_service_archlinux_status_by_service
{
   my ($service) = @_;
   return 1 if( -f "/var/run/daemons/$service");
   return 0;
}

sub gst_service_rcng_status_by_service
{
  my ($service) = @_;
  my ($fd, $line, $active);

  $fd = &gst_file_run_pipe_read ("/etc/rc.d/$service rcvar");

  while (<$fd>)
  {
    $line = $_;

    if ($line =~ /^\$.*=YES$/)
    {
      $active = 1;
      last;
    }
  }

  gst_file_close ($fd);
  return $active;
}

sub gst_service_rcng_get_service_info
{
  my ($service) = @_;
  my ($script, @actions, @runlevels);
  my (%hash, @arr, @rl);

  # We have to check if the service is in the "forbidden" list
  return undef if (&gst_service_list_service_is_forbidden ($service));

  $hash{"script"} = $service;

  if (gst_service_rcng_status_by_service ($service))
  {
    push @arr, { "name"   => "rc",
                 "action" => "start" };
  }
  else
  {
    push @arr, { "name"   => "rc",
                 "action" => "stop" };
  }

  push @rl,  { "runlevel", \@arr };

  $hash {"runlevels"} = \@rl;
  $hash{"role"} = &gst_service_get_role ($service);

  return \%hash;
}

sub gst_service_archlinux_get_service_info
{
  my ($service) = @_;
  my ($script, @actions, @runlevels);
  my (%hash, @arr, @rl);

  # We have to check if the service is in the "forbidden" list
  return undef if (&gst_service_list_service_is_forbidden ($service));

  $hash{"script"} = $service;

  if (&gst_service_archlinux_status_by_service ($service))
  {
    push @arr, { "name"   => "rc",
                 "action" => "start" };
  }
  else
  {
    push @arr, { "name"   => "rc",
                 "action" => "stop" };
  }

  push @rl,  { "runlevel", \@arr };

  $hash {"runlevels"} = \@rl;
  $hash{"role"} = &gst_service_get_role ($service);

  return \%hash;
}

sub gst_service_archlinux_get_services
{
  my ($service);
  my (%ret);

  foreach $service (<$gst_prefix/etc/rc.d/*>)
  {
    my (%hash);
    
    $service =~ s/.*\///;
    $hash = &gst_service_archlinux_get_service_info ($service);

    $ret{$service} = $hash if ($hash ne undef);
  }

  return \%ret;
}

sub gst_service_rcng_get_services
{
  my ($service);
  my (%ret);

  foreach $service (<$gst_prefix/etc/rc.d/*>)
  {
    my (%hash);
    
    $service =~ s/.*\///;
    $hash = &gst_service_rcng_get_service_info ($service);

    $ret{$service} = $hash if ($hash ne undef);
  }

  return \%ret;
}
# SuSE functions, quite similar to SysV, but not equal...
sub gst_service_suse_get_service_info ($service)
{
  my ($service) = @_;
  my (%hash, @arr, @ret);
                                                                                                                                                             
  # We have to check if the service is in the "forbidden" list
  return undef if (&gst_service_list_service_is_forbidden ($service));
                                                                                                                                                             
  $hash{"script"} = $service;

  foreach $link (<$rcd_path/rc[0-9S].d/S[0-9][0-9]$service>)
  {
    $link =~ s/$rcd_path\///;
    $link =~ /rc([0-6])\.d\/S[0-9][0-9].*/;
    $runlevel = $1;

    push @arr, { "name"   => $runlevel,
                 "action" => "start" };
  }

  foreach $link (<$rcd_path/boot.d/S[0-9][0-9]$service>)
  {
    push @arr, {"name"   => "B",
                "action" => "start" };
  }

  if (scalar @arr > 0)
  {
    push @ret, { "runlevel" => \@arr };
    $hash{"runlevels"} = \@ret;
    $hash{"role"} = &gst_service_get_role ($service);
  }

  return \%hash;
}

sub gst_service_suse_get_services
{
  my ($service, %ret);

  ($rcd_path, $initd_path) = &gst_service_sysv_get_paths ();

  foreach $service (<$gst_prefix/etc/init.d/*>)
  {
    my (%hash);

    next if (-d $service || ! -x $service);

    $service =~ s/.*\///;
    $hash = &gst_service_suse_get_service_info ($service);

    $ret{$service} = $hash if ($hash ne undef);
  }

  return \%ret;
}

# generic functions to get the available services
sub gst_get_init_type
{
  if (($gst_dist =~ /debian/) && (stat ("$gst_prefix/etc/runlevel.conf")))
  {
    return "file-rc";
  }
  elsif ($gst_dist =~ /slackware/)
  {
    return "bsd";
  }
  elsif ($gst_dist =~ /freebsd/)
  {
    return "rcng";
  }
  elsif ($gst_dist =~ /archlinux/)
  {
    return "archlinux";
  }
  elsif (($gst_dist =~ /gentoo/) || ($gst_dist =~ /^vlos/))
  {
    return "gentoo";
  }
  elsif ($gst_dist =~ /suse/)
  {
    return "suse";
  }
  else
  {
    return "sysv";
  }
}

sub gst_service_get_services
{
  $type = &gst_get_init_type ();

  return &gst_service_sysv_get_services ()   if ($type eq "sysv");
  return &gst_service_filerc_get_services () if ($type eq "file-rc");
  return &gst_service_bsd_get_services ()    if ($type eq "bsd");
  return &gst_service_gentoo_get_services () if ($type eq "gentoo");
  return &gst_service_rcng_get_services ()   if ($type eq "rcng");
  return &gst_service_suse_get_services ()   if ($type eq "suse");
  return &gst_service_archlinux_get_services() if ($type eq "archlinux");
  return undef;
}


# This function gets the runlevel that is in use
sub gst_service_sysv_get_default_runlevel
{
	my (@arr);
	
	@arr = split / /, `/sbin/runlevel` ;
	$arr[1] =~ s/\n//;
	
	return $arr[1];
}

sub gst_service_get_default_runlevel
{
    my ($type) = &gst_get_init_type ();

    return "default" if ($type eq "gentoo");
    return "rc"      if ($type eq "rcng");
    return "rc"      if ($type eq "archlinux");
    return &gst_service_sysv_get_default_runlevel ();
}


# THESE ARE THE FUNCTIONS WHICH APPLY THE CHANGES MADE TO THE CONFIGURATION OF THE COMPUTER

sub gst_service_sysv_add_link
{
  my ($runlevel, $action, $priority, $service) = @_;
  my ($prio) = sprintf ("%0.2d",$priority);

  symlink ("$relative_path/$service", "$rcd_path/rc$runlevel.d/$action$prio$service");
  
  &gst_report_enter ();
  &gst_report ("service_sysv_add_link", "$rcd_path/rc$runlevel.d/$action$prio$service");
  &gst_report_leave ();
}

sub gst_service_sysv_remove_link
{
  my ($runlevel, $script) = @_;
	
  foreach $link (<$rcd_path/rc$runlevel.d/[SK][0-9][0-9]$script>)
  {
    &gst_report ("service_sysv_remove_link", "$link");
    unlink ("$link");
    &gst_report_leave ();
  }
}


# These are the functions for storing the service settings from XML in SysV
sub gst_service_sysv_set_service
{
  my ($service) = @_;
  my ($script, $priority, $runlevels);
  my ($action);

  ($rcd_path, $initd_path, $relative_path) = &gst_service_sysv_get_paths ();

  $script = $$service{"script"};
  $runlevels = $$service{"runlevels"}[0]{"runlevel"};

  # pass though all the runlevels checking if the service must be started, stopped or removed
  for ($i = 0; $i <= 6; $i++)
  {
    &gst_service_sysv_remove_link ($i, $script);

    $action = undef;
    foreach $j (@$runlevels)
    {
      if ($i == $$j{"name"})
      {
        $found    = 1;
        $action   = $$j{"action"};
        $priority = $$j{"priority"};
        $priority = 50 if (!$priority); #very rough guess
      }
    }

    if ($action ne undef)
    {
      if ($action eq "start")
      {
        &gst_service_sysv_add_link ($i, "S", $priority, $script);
      }
      else
      {
        &gst_service_sysv_add_link ($i, "K", 100 - $priority, $script);
      }
    }
  }
}

sub gst_service_sysv_set_services
{
	my ($services, $runlevel) = @_;

	foreach $i (@$services)
	{
		&gst_service_sysv_set_service($i);
	}
}

sub gst_service_filerc_concat_runlevels
{
  my (@runlevels) = @_;

  $str = join (",", sort (@runlevels));
  return ($str) ? $str : "-";
}

sub gst_service_filerc_set_service
{
  my ($buff, $service) = @_;
  my (%hash, $priority, $line, $str);

  $arr = $$service{"runlevels"}[0]{"runlevel"};

  foreach $i (@$arr)
  {
    $priority = 0 + $$i{"priority"};
    $priority = 50 if ($priority == 0); #very rough guess

    if ($$i{"action"} eq "start")
    {
      $hash{$priority}{"start"} = [] if (!$hash{$priority}{"start"});
      push @{$hash{$priority}{"start"}}, $$i{"name"};
    }
    else
    {
      $hash{$priority}{"stop"} = [] if (!$hash{$priority}{"stop"});
      push @{$hash{$priority}{"stop"}}, $$i{"name"};
    }
  }

  foreach $priority (keys %hash)
  {
    $line  = sprintf ("%0.2d", $priority) . "\t";
    $line .= &gst_service_filerc_concat_runlevels (@{$hash{$priority}{"stop"}}) . "\t";
    $line .= &gst_service_filerc_concat_runlevels (@{$hash{$priority}{"start"}}) . "\t";
    $line .= "/etc/init.d/" . $$service{"script"} . "\n";

    push @$buff, $line;
  }
}

# This is the function for storing the service settings from XML in file-rc
sub gst_service_filerc_set_services
{
  my ($services, $runlevel) = @_;
  my ($buff, $lineno, $line, $file);
  my ($rcd_path, $initd_path, $relative_path) = &gst_service_sysv_get_paths ();

  $file = "$gst_prefix/etc/runlevel.conf";

  $buff = &gst_file_buffer_load ($file);
  &gst_file_buffer_join_lines ($buff);

  $lineno = 0;

  # We prepare the file for storing the configuration, save the initial comments
  # and delete the rest
  while ($$buff[$lineno] =~ /^#.*/)
  {
    $lineno++;
  }

  for ($i = $lineno; $i < scalar (@$buff); $i++)
  {
    $$buff[$i] =~ /.*\/etc\/init\.d\/(.*)/;

    # we need to keep the forbidden services and the services that only start in rcS.d
    if (!gst_service_list_service_is_forbidden ($1))
    {
      delete $$buff[$i];
    }
  }

  # Now we append the services
  foreach $service (@$services)
  {
    &gst_service_filerc_set_service ($buff, $service);
  }

  @$buff = sort @$buff;

  push @$buff, "\n";
  &gst_file_buffer_clean ($buff);
  &gst_file_buffer_save ($buff, $file);
}

sub gst_service_bsd_set_services
{
  my ($services, $runlevel) = @_;
  my ($script, $runlevels);

	foreach $service (@$services)
	{
    $script = $$service{"script"};
    $runlevels = $$service{"runlevels"}[0]{"runlevel"}[0];

    $action = $$runlevels {"action"};

    if ($action eq "start")
    {
      &gst_file_run ("chmod ugo+x $script");
    }
    else
    {
      &gst_file_run ("chmod ugo-x $script");
    }
  }
}

sub gst_service_gentoo_set_services
{
  my ($services, $runlevel) = @_;
  my ($action);

  foreach $service (@$services)
  {
    $script = $$service{"script"};
    $arr = $$service{"runlevels"}[0]{"runlevel"};

    foreach $i (@$arr)
    {
      $action = $$i{"action"};
      $rl = $$i{"name"};

      if ( $action eq "start")
      {
        &gst_file_run ("rc-update add $script $rl");
      }
      elsif ($action eq "stop")
      {
        &gst_file_run ("rc-update del $script $rl");
      }
    }
  }
}

sub gst_service_rcng_set_status
{
  my ($service, $action) = @_;
  my ($fd, $key, $res);
  my ($default_rcconf) = "/etc/defaults/rc.conf";
  my ($rcconf) = "/etc/rc.conf";

  if (&gst_file_exists ("/etc/rc.d/$service"))
  {
    $fd = &gst_file_run_pipe_read ("/etc/rc.d/$service rcvar");

    while (<$fd>)
    {
      if (/^\$(.*)=.*$/)
      {
        # to avoid cluttering rc.conf with duplicated data,
        # we first look in the defaults/rc.conf for the key
        $key = $1;
        $res = &gst_parse_sh_bool ($default_rcconf, $key);

        if ($res == $action)
        {
          &gst_replace_sh ($rcconf, $key);
        }
        else
        {
          &gst_replace_sh_bool ($rcconf, $key, "YES", "NO", $action);
        }
      }
    }

    &gst_file_close ($fd);
  }
  elsif (&gst_file_exists ("/usr/local/etc/rc.d/$service.sh"))
  {
    if ($action)
    {
      gst_file_copy ("/usr/local/etc/rc.d/$service.sh.sample",
                     "/usr/local/etc/rc.d/$service.sh");
    }
    else
    {
      gst_file_remove ("/usr/local/etc/rc.d/$service.sh");
    }
  }
}

sub gst_service_rcng_set_services
{
  my ($services, $runlevel) = @_;
  my ($action, $runlevels, $script);

  foreach $service (@$services)
  {
    $script = $$service {"script"};
    $runlevels = $$service{"runlevels"}[0]{"runlevel"}[0];
    $action = ($$runlevels {"action"} eq "start")? 1 : 0;

    &gst_service_rcng_set_status ($script, $action);
  }
}

sub gst_service_suse_set_services
{
  my ($services, $runlevel) = @_;
  my ($action, $runlevels, $script, $rllist);

  foreach $service (@$services)
  {
    $script = $$service{"script"};
    $runlevels = $$service{"runlevels"}[0]{"runlevel"};
    $rllist = "";

    &gst_file_run ("insserv -r $script");

    foreach $rl (@$runlevels)
    {
      if ($$rl{"action"} eq "start")
      {
        $rllist .= $$rl{"name"} . ",";
      }
    }

    if ($rllist ne "")
    {
      $rllist =~ s/,$//;

      &gst_file_run ("insserv $script,start=$rllist");
    }
  }
}


sub gst_service_archlinux_set_status
{
    my ($script, $active) = @_;
    my $rcconf = '/etc/rc.conf';
    open DATA, "$rcconf";
    my @rcconflines = <DATA>;
    close (DATA);
    open DATAOUT, ">", "$rcconf";
    if(($active) && (! -f "/var/run/daemons/$script"))
    {
    	&gst_service_archlinux_run_script($script,"start");
    }
    if((!$active) && ( -f "/var/run/daemons/$script"))
    {
    	&gst_service_archlinux_run_script($script,"start");
    }
    
   
    foreach my $line (@rcconflines)
    {
      if($line =~ /\DAEMONS=/)
      {
        if($line =~ m/$script/)
        {
          if(!$active)
          {
            $line =~ s/$script //;
          }
        }
        else
        {
          if($active)
          {
            $line =~ s/network/network $script/g;
          }
        }
     }
     print DATAOUT "$line";
    }

    close (DATAOUT);    
}

sub gst_service_archlinux_set_services
{
  my ($services, $runlevel) = @_;
  my ($action, $runlevels, $script);
  foreach $service (@$services)
  {
    $script = $$service {"script"};
    $runlevels = $$service{"runlevels"}[0]{"runlevel"}[0];
    $action = ($$runlevels {"action"} eq "start")? 1 : 0;
    &gst_service_archlinux_set_status ($script, $action);
  }
}

sub gst_service_set_services
{
  my ($services, $runlevel) = @_;
  
  $type = &gst_get_init_type ();
  &gst_service_sysv_set_services   ($services, $runlevel) if ($type eq "sysv");
  &gst_service_filerc_set_services ($services, $runlevel) if ($type eq "file-rc");
  &gst_service_bsd_set_services    ($services, $runlevel) if ($type eq "bsd");
  &gst_service_gentoo_set_services ($services, $runlevel) if ($type eq "gentoo");
  &gst_service_rcng_set_services   ($services, $runlevel) if ($type eq "rcng");
  &gst_service_suse_set_services   ($services, $runlevel) if ($type eq "suse");
  &gst_service_archlinux_set_services ($services, $runlevel) if ($type eq "archlinux");
}

sub gst_service_set_conf
{
  my ($hash) = @_;
  my ($services, $runlevel);

  return unless $hash;
  $services = $$hash{"services"}[0]{"service"};
  return unless $services;
  $runlevel = $$hash{"runlevel"};
  return unless $runlevel;

  &gst_service_set_services($services, $runlevel);
}

# stuff for checking whether service is running
sub gst_service_debian_get_status
{
  my ($service) = @_;
  my ($rcd_path, $initd_path) = &gst_service_sysv_get_paths ();
  my ($output, $pidfile);

  $output = `grep "\/var\/run\/.*\.pid" $initd_path\/$service`;

  if ($output =~ /.*(\/var\/run\/.*\.pid).*/ )
  {
    $pidfile = $1;
    $pidval = `cat $pidfile`;

    return 0 if $pidval eq "";

    $pid = `ps h $pidval`;

    if ($pid eq "")
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }

  return undef;
}

sub gst_service_redhat_get_status
{
  my ($service) = @_;
  my ($rcd_path, $initd_path) = &gst_service_sysv_get_paths ();

  if (-f "/var/lock/subsys/$service")
  {
    return 1;
  }

  return 0;
}

sub gst_service_gentoo_get_status
{
  my ($service) = @_;

  $line = `/etc/init.d/$service status`;

  return 1 if ($line =~ /started/);
  return 0;
}

sub gst_service_rcng_get_status
{
  my ($service) = @_;

  $line = gst_file_run_backtick ("/etc/rc.d/$service forcestatus");
  return 1 if ($line =~ /pid [0-9]*/);

  # hacky as hell, we need to check services in /usr/local/etc/rc.d
  # and there's no standard way to check they're running
  return 1 if (-f "/var/run/$service.pid");

  # we give up, the service isn't running
  return 0;
}
sub gst_service_archlinux_get_status
{
	my ($service) = @_;
	return 1 if( -f "/var/run/daemons/$service");
	return 0;
}

sub gst_service_suse_get_status
{
  my ($service) = @_;

  $line = gst_file_run_backtick ("/etc/init.d/$service status");
  return 1 if ($line =~ /running/);
  return 0;
}

# returns true if the service is already running
sub gst_service_get_status
{
  my ($service) = @_;
  my %dist_map =
      (
       "debian-2.2"   => \&gst_service_debian_get_status,
       "debian-3.0"   => \&gst_service_debian_get_status,
       "debian-3.1"   => \&gst_service_debian_get_status,
       "debian-4.0"   => \&gst_service_debian_get_status,
       "debian-5.0"   => \&gst_service_debian_get_status,
       "debian-testing" => \&gst_service_debian_get_status,
       "ubuntu-5.04"  => \&gst_service_debian_get_status,
       "ubuntu-5.10"  => \&gst_service_debian_get_status,
       "ubuntu-6.06"  => \&gst_service_debian_get_status,
       "ubuntu-6.10"  => \&gst_service_debian_get_status,
       "ubuntu-7.04"  => \&gst_service_debian_get_status,
       "ubuntu-7.10"  => \&gst_service_debian_get_status,
       "ubuntu-8.04"  => \&gst_service_debian_get_status,
              
       "redhat-5.2"   => \&gst_service_redhat_get_status,
       "redhat-6.0"   => \&gst_service_redhat_get_status,
       "redhat-6.1"   => \&gst_service_redhat_get_status,
       "redhat-6.2"   => \&gst_service_redhat_get_status,
       "redhat-7.0"   => \&gst_service_redhat_get_status,
       "redhat-7.1"   => \&gst_service_redhat_get_status,
       "redhat-7.2"   => \&gst_service_redhat_get_status,
       "redhat-7.3"   => \&gst_service_redhat_get_status,
       "redhat-8.0"   => \&gst_service_redhat_get_status,
       "redhat-9"     => \&gst_service_redhat_get_status,
       "mandrake-7.2" => \&gst_service_redhat_get_status,
       "fedora-1"     => \&gst_service_redhat_get_status,
       "fedora-2"     => \&gst_service_redhat_get_status,
       "fedora-3"     => \&gst_service_redhat_get_status,
       "fedora-4"     => \&gst_service_redhat_get_status,
       "fedora-5"     => \&gst_service_redhat_get_status,

       "rpath"        => \&gst_service_redhat_get_status,

       "suse-9.0"     => \&gst_service_suse_get_status,
       "suse-9.1"     => \&gst_service_suse_get_status,
       "archlinux"    => \&gst_service_archlinux_get_status,
       "gentoo"       => \&gst_service_gentoo_get_status,
       "vlos-1.2"     => \&gst_service_gentoo_get_status,

       "freebsd-5"    => \&gst_service_rcng_get_status,
       "freebsd-6"    => \&gst_service_rcng_get_status,
      );
  my $proc;

  $proc = $dist_map {$gst_dist};

  return undef if ($proc eq undef);

  return &$proc ($service);
}

# Functions to run a service
sub gst_service_sysv_run_initd_script
{
  my ($service, $arg) = @_;
  my ($rc_path, $initd_path);
  my $str;
  my %map =
      ("restart" => "restarted",
       "stop" => "stopped",
       "start" => "started");

  &gst_report_enter ();
  
  if (!exists $map{$arg})
  {
    &gst_report ("service_sysv_op_unk", $arg);
    &gst_report_leave ();
    return -1;
  }

  $str = $map{$arg};

  ($rcd_path, $initd_path) = &gst_service_sysv_get_paths ();

  if (-f "$initd_path/$service")
  {
    if (!&gst_file_run ("$initd_path/$service $arg"))
    {
      &gst_report ("service_sysv_op_success", $service, $str);
      &gst_report_leave ();
      return 0;
    }
  }
  
  &gst_report ("service_sysv_op_failed", $service, $str);
  &gst_report_leave ();
  return -1;
}

sub gst_service_bsd_run_script
{
  my ($service, $arg) = @_;
  my ($chmod) = 0;

  return if (!&gst_file_exists ($service));

  # if it's not executable then chmod it
  if (!((stat ($service))[2] & (S_IXUSR || S_IXGRP || S_IXOTH)))
  {
    $chmod = 1;
    &gst_file_run ("chmod ugo+x $service");
  }
  
  &gst_file_run ("$service $arg");

  # return it to it's normal state
  if ($chmod)
  {
    &gst_file_run ("chmod ugo-x $service");
  }
}

sub gst_service_gentoo_run_script
{
  my ($service, $arg) = @_;
  my ($option);

  my %map =
    ("stop" => "stopped",
     "start" => "started"
    );

  &gst_report_enter ();

  if (!exists $map{$arg})
  {
    &gst_report ("service_sysv_op_unk", $arg);
    &gst_report_leave ();
    return -1;
  }

  if (&gst_service_gentoo_service_exist ($service))
  {
    if (!&gst_file_run ("/etc/init.d/$service $arg"))
    {
      &gst_report ("service_sysv_op_success", $service, $str);
      &gst_report_leave ();
	    return 0;
	  }
  }

  &gst_report ("service_sysv_op_failed", $service, $str);
  &gst_report_leave ();
  return -1;
}


sub gst_service_rcng_run_script
{
  my ($service, $arg) = @_;
  my ($farg);

  my %map =
    ("stop"  => "forcestop",
     "start" => "forcestart"
    );

  &gst_report_enter ();

  if (!exists $map{$arg})
  {
    &gst_report ("service_sysv_op_unk", $arg);
    &gst_report_leave ();
    return -1;
  }

  $farg = $map {$arg};

  if (!&gst_file_run ("/etc/rc.d/$service $farg"))
  {
    &gst_report ("service_sysv_op_success", $service, $str);
    &gst_report_leave ();
    return 0;
  }

  &gst_report ("service_sysv_op_failed", $service, $str);
  &gst_report_leave ();
  return -1;
}

sub gst_service_archlinux_run_script
{
  my ($service, $arg) = @_;
  my ($farg);

  my %map =
    ("stop"  => "stop",
     "start" => "start"
    );

  &gst_report_enter ();

  if (!exists $map{$arg})
  {
    &gst_report ("service_sysv_op_unk", $arg);
    &gst_report_leave ();
    return -1;
  }

  $farg = $map {$arg};

  if (!&gst_file_run ("/etc/rc.d/$service $farg"))
  {
    &gst_report ("service_sysv_op_success", $service, $str);
    &gst_report_leave ();
    return 0;
  }

  &gst_report ("service_sysv_op_failed", $service, $str);
  &gst_report_leave ();
  return -1;
}
sub gst_service_run_script
{
  my ($service, $arg) = @_;
  my ($proc, $type);
  my %map =
      (
       "file-rc" => \&gst_service_sysv_run_initd_script,
       "sysv"    => \&gst_service_sysv_run_initd_script,
       "bsd"     => \&gst_service_bsd_run_script,
       "gentoo"  => \&gst_service_gentoo_run_script,
       "rcng"    => \&gst_service_rcng_run_script,
       "suse"    => \&gst_service_sysv_run_initd_script,
       "archlinux" => \&gst_service_archlinux_run_script,
      );

  $type = &gst_get_init_type ();

  $proc = $map {$type};

  &$proc ($service, $arg);
}

# functions to know if a service will be installed
sub gst_service_sysv_installed
{
  my ($service) = @_;
  my ($res, $rcd_path, $initd_path);

  &gst_report_enter ();
  
  ($rcd_path, $initd_path) = &gst_service_sysv_get_paths ();

  $res = 1;
  if (! -f "$initd_path/$service")
  {
    $res = 0;
    &gst_report ("service_sysv_not_found", $service);
  }

  &gst_report_leave ();
  return $res;
}

sub gst_service_bsd_installed
{
  my ($service) = @_;

  return 1 if ( -f "$service");
  return 0;
}

sub gst_service_gentoo_installed
{
  my ($service) = @_;

  return 1 if ( -f "/etc/init.d/$service");
  return 0;
}

sub gst_service_rcng_installed
{
  my ($service) = @_;

  return 1 if ( -f "/etc/rc.d/$service");
  return 1 if ( -f "/usr/local/etc/rc.d/$service.sh.sample");
  return 0;
}

sub gst_service_installed
{
  my ($service) = @_;
  my ($type);
  $type = &gst_get_init_type ();

  return &gst_service_sysv_installed ($service) if (($type eq "sysv") || ($type eq "file-rc") || ($type eq "suse"));
  return &gst_service_bsd_installed ($service) if ($type eq "bsd");
  return &gst_service_gentoo_installed ($service) if ($type eq "gentoo");
  return &gst_service_rcng_installed ($service) if ($type eq "rcng");
  return &gst_service_rcng_installed ($service) if ($type eq "archlinux");
  return 0;
}

sub gst_service_list_any_installed
{
  my @service = @_;
  my $res;

  $res = 0;
  
  foreach $serv (@service)
  {
    if (gst_service_installed ($serv))
    {
      $res = 1;
    }
  }

  return $res;
}

sub gst_service_bsd_set_status
{
  my ($script, $active) = @_;
  my (@arr);

  if ($active)
  {
    &gst_file_run ("chmod ugo+x $script");
    &gst_service_run_script ($script, "start");
  }
  else
  {
    &gst_service_run_script ($script, "stop");
    &gst_file_run ("chmod ugo-x $script");
  }
}

sub gst_service_gentoo_set_status
{
  my ($script, $force_now, $active) = @_;
  my (@arr);

  if ($active)
  {
    &gst_file_run ("rc-update add $script default");
    &gst_file_run ("/etc/init.d/$script start") if ($force_now == 1);
  }
  else
  {
    &gst_file_run ("rc-update del $script default");
    &gst_file_run ("/etc/init.d/$script stop") if ($force_now == 1);
  }
}

sub gst_service_suse_set_status
{
  my ($script, $active) = @_;
  my (@runlevels, $rllist);
  my ($rcd_path, $initd_path);
  my ($rl);

  ($rcd_path, $initd_path) = &gst_service_sysv_get_paths ();
  @runlevels = &gst_service_sysv_get_runlevels ();

  if ($active)
  {
    $rllist = join ",", @runlevels;
    &gst_file_run ("insserv $script,start=$rllist");
    &gst_service_run_script ($script, "start");
  }
  else
  {
    # to remove a service from a few runlevels we need to run
    # insserv -r and then insserv blah,start=x,y,z
    foreach $link (<$rcd_path/rc[0-9S].d/S[0-9][0-9]$script>)
    {
      $link =~ s/$rcd_path\///;
      $link =~ /rc([0-9S])\.d\/S[0-9][0-9].*/;
      $rllist .= "$1,";
    }

    foreach $link (<$rcd_path/boot.d/S[0-9][0-9]$service>)
    {
      $rllist .= "B,";
    }

    # remove the default runlevels from the list
    foreach $runlevel (@runlevels)
    {
      $rllist =~ s/$runlevel,//;
    }

    $rllist =~ s/,$//;

    &gst_file_run ("insserv -r $script");

    if ($rllist ne "")
    {
      &gst_file_run ("insserv $script,start=$rllist");
    }

    &gst_service_run_script ($script, "stop");
  }
}
