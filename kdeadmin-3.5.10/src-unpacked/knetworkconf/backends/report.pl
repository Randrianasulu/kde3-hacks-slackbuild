#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# /* Functions for on-the-fly commentary on a tool's work. */
#
# Copyright (C) 2000-2001 Ximian, Inc.
#
# Authors: Hans Petter Jansson <hpj@ximian.com>
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

# --- Progress printing --- #


$gst_progress_current = 0;  # Compat with old $progress_max use.
$gst_progress_last_percentage = 0;


sub gst_progress
{
  my $prc = $_[0];

  # /* Don't go backwards. */
  $prc = $gst_progress_last_percentage if ($prc < $gst_progress_last_percentage);

  # /* Don't go above 99%. */
  $prc = 99 if ($prc >= 100);

  if ($gst_progress && (int $prc > int $gst_progress_last_percentage))
  {
    &gst_report ("progress", $prc);
    $gst_progress_last_percentage = $prc;
  }
}


sub gst_progress_begin
{
  &gst_progress (0);
}


sub gst_progress_end
{
  &gst_progress (99);
}


sub gst_print_progress  # Compat with old $progress_max use.
{
  my $prc;

  $gst_progress_current++;
  &gst_progress (($gst_progress_current * 100) / $progress_max);
}


# --- Report printing --- #


sub gst_report_begin
{
  my ($tool) = @_;
  
  &gst_report ("begin");
  &gst_report_enter ();
  &gst_progress_begin ();
}


sub gst_report_end
{
  &gst_progress_end ();
  &gst_report_leave ();
  &gst_report ("end");
}


sub gst_report_set_threshold
{
  $gst_report_threshold = $_[0];
}


sub gst_report_enter
{
  # This has been trivialized because it is not working
  # correctly at the moment and is causing some trouble.
  # /* We'll probably replace this with something smarter */
  # (like a detail-level value in the report hash)
  # when the report strings are used at the gui-level again.
#  $gst_report_level ++;
  $gst_report_level = 0;
}


sub gst_report_leave
{
#  $gst_report_level --;
  $gst_report_level = 0;
}


# Escapes a report using the report line format.
sub gst_report_escape
{
  my ($args) = @_;
  my ($arg);

  foreach $arg (@$args)
  {
    $arg =~ s/\\/\\\\/g;
    $arg =~ s/::/\\::/g;
  }
}


$gst_report_level = 0;
$gst_report_started = 0;

# Just to trap these errors with the debugger easily.
sub gst_report_stderr
{
  my ($major, $key, $res) = @_;
  
  print STDERR "$gst_name - $major::${key}::$res";
}

sub gst_report
{
  my (@args) = @_;
  my ($key, $major, $minor, $str, $format, $res);
  my $report_message = &gst_report_table ();

  &gst_report_escape (\@args);

  $key = shift @args;

  if (! (exists $$report_message{$key}))
  {
      &gst_report ("report_minor_unk", $key);
      return;
  }

  ($major, $str) = @{$$report_message{$key}};

  if (! (exists $gst_report_valid_majors{$major}))
  {
      &gst_report ("report_major_unk", $major, join ("::", $key, @args));
      return;
  }

  $gst_report_started = 1 if !$gst_report_started && $key eq "begin" && $major eq "sys";

  # Verbose (--verbose) output is human-readable only.
  $format = "$str\n";
  $res = sprintf ($format, @args);

  if ($gst_do_verbose   ||
      $major eq "error" ||
      $major eq "debug")
  {
    &gst_report_stderr ($major, $key, $res);
  }

  if ($key ne "progress")
  {
    return if ($gst_report_level >= $gst_report_threshold || !$gst_report_started);
  }

  # Report (--report) output is machine-readable.
  if ($gst_do_report)
  {
    print STDOUT join ("::", $major, $key, $str, @args) . "\n";
  }

  &gst_debug_print_indented_string ($gst_report_level, "report $major:$key: $res");
}

# Internal
{
  my $report_table = \%gst_report_message;
  sub gst_report_table
  {
    my $table = shift @_;

    if ($table) # Add
    {
      foreach my $key (keys %$table)
      {
        $$report_table{$key} = $$table{$key} unless exists $$report_table{$key};
      }
    }

    else # Get
    {
      return $report_table;
    }
  }
}

# This disables reporting.
&gst_report_set_threshold (0);

%gst_report_valid_majors = (
    "sys"   => 1,
    "error" => 1,
    "warn"  => 1,
    "info"  => 1,
    "debug" => 1
    );

%gst_report_message =
    (
     "begin"    => ["sys", "Start of work report."],
     "end"      => ["sys", "End of work report."],
     "progress" => ["sys", "%d"],
     "compat"   => ["info", "%s."],

     "report_major_unk"         => ["error", "Unknown major [%s] in report [%s]."],
     "report_minor_unk"         => ["error", "Unknown minor [%s]."],

     "directive_run"            => ["info",  "Running directive [%s] with args [%s]."],
     "directive_unsup"          => ["error", "Directive [%s] not supported."],
     "directive_invalid"        => ["error", "Directive [%s] structure has wrong format."],
     "directive_badargs"        => ["error", "Directive [%s] requires exactly [%s] args in [%s]."],
     "directive_lowargs"        => ["error", "Directive [%s] requires at least [%s] args in [%s]."],

     "platform_unsup"           => ["error", "Your platform [%s] is not supported."],
     "platform_undet"           => ["error", "Unable to determine host platform."],
     "platform_success"         => ["sys",   "Configuring for platform [%s] (%s)."],
     "platform_no_table"        => ["error", "No parse/replace table for platform [%s]."],

     "xml_unexp_tag"            => ["error", "Unexpected tag [%s]."],
     "xml_unexp_arg"            => ["error", "Unexpected argument [%s] to tag [%s]."],

     "file_copy_failed"         => ["debug", "Could not copy file [%s] to [%s]."],
     "file_open_read_failed"    => ["warn",  "Could not open [%s] for reading."],
     "file_open_read_success"   => ["info",  "Reading options from [%s]."],
     "file_open_write_failed"   => ["error", "Failed to write to [%s]."],
     "file_open_write_create"   => ["warn",  "Could not find [%s] for writing. Creating [%s]."],
     "file_open_write_success"  => ["info",  "Writing to [%s]."],
     "file_run_pipe_failed"     => ["warn",  "Failed to pipe command [%s] for reading."],
     "file_run_pipe_success"    => ["info",  "Piping command [%s] for reading."],
     "file_run"                 => ["info",  "Running command [%s]."],
     "file_create_path"         => ["info",  "Directory [%s] created."],
     "file_backup_rotate"       => ["info",  "Backup directory [%s] was rotated."],
     "file_backup_success"      => ["info",  "Saved backup for [%s]."],
     "file_open_filter_failed"  => ["warn",  "No file to patch: [%s]."],
     "file_open_filter_create"  => ["warn",  "Could not find [%s] for patching. Creating [%s]."],
     "file_open_filter_success" => ["info",  "Found [%s]. Patching [%s]."],
     "file_buffer_load"         => ["info",  "Loading file [%s] to buffer."],
     "file_buffer_save"         => ["info",  "Saving buffer to file [%s]."],
     "file_remove"              => ["info",  "Removing file [%s]."],
     "file_locate_tool_success" => ["info",  "Found tool [%s]."],
     "file_locate_tool_failed"  => ["warn",  "Couldn't find tool [%s]."],
     
     "parse_table"              => ["info",  "Parsing option [%s]."],
     "parse_trivial"            => ["info",  "Trivialy passing [%s]."],
     "parse_split"              => ["info",  "Getting option [%s] from [%s]."],
     "parse_split_hash"         => ["info",  "Getting configuration from [%s]."],
     "parse_split_hash_cont"    => ["info",  "Getting configuration from [%s]."],
     "parse_sh"                 => ["info",  "Getting shell option [%s] from [%s]."],
     "parse_kw"                 => ["info",  "Getting keyword [%s] from [%s]."],
     "parse_line_first"         => ["info",  "Getting information from [%s]."],
     "parse_chat"               => ["info",  "Getting chat information from [%s]."],
     "parse_ini"                => ["info",  "Getting option [%s] from [%s], section [%s]."],
     "parse_ifaces_str"         => ["info",  "Getting option [%s] from interface [%s]."],
     "parse_ifaces_kw"          => ["info",  "Getting keyword [%s] from interface [%s]."],
     "parse_ifaces_kw_strange"  => ["warn",  "Keyword for interface [%s] in [%s] had unexpected value."],

     "replace_split"            => ["info",  "Replacing key [%s] in [%s]."],
     "replace_sh"               => ["info",  "Replacing shell var [%s] in [%s]."],
     "replace_kw"               => ["info",  "Replacing keyword [%s] in [%s]."],
     "replace_line_first"       => ["info",  "Replacing contents of file [%s]."],
     "replace_chat"             => ["info",  "Replacing values in [%s]."],
     "replace_ini"              => ["info",  "Replacing variable [%s] in section [%s] of [%s]."],
     "replace_del_ini_sect"     => ["info",  "Removing section [%s] from [%s]."],
     "replace_ifaces_str"       => ["info",  "Replacing option [%s] from interface [%s]."],
     "replace_ifaces_kw"        => ["info",  "Replacing keyword [%s] from interface [%s]."],
     
     "service_status_running"   => ["info",  "Service [%s] is running."],
     "service_status_stopped"   => ["info",  "Service [%s] is stopped."],
     "service_sysv_unsupported" => ["info",  "No SystemV support for platform [%s]."],
     "service_sysv_not_found"   => ["warn",  "Could not find SystemV scripts for service [%s]."],
     "service_sysv_no_runlevel" => ["warn",  "Could not find SystemV runlevel [%s] directory [%s]."],
     "service_sysv_remove_link" => ["info",  "Removed SystemV link [%s]."],
     "service_sysv_add_link"    => ["info",  "Created SystemV link [%s]."],
     "service_sysv_op_unk"      => ["error", "Unknown initd operation [%s]."],
     "service_sysv_op_success"  => ["info",  "Service [%s] %s."],
     "service_sysv_op_failed"   => ["warn",  "Service [%s] could not be %s."],

     "network_dialing_get"      => ["info",  "Loading ISP configurations."],
     "network_iface_active_get" => ["info",  "Finding active interfaces."],
     "network_iface_is_active"  => ["info",  "Checking if interface [%s] is active."],
     "network_hostname_set"     => ["info",  "Setting hostname to [%s]."],
     "network_dialing_set"      => ["info",  "Saving ISP configurations."],
     "network_remove_pap"       => ["info",  "Removing entry [%s] from [%s]."],
     "network_iface_set"        => ["info",  "Configuring interface [%s]."],
     "network_iface_activate"   => ["info",  "Activating interface [%s]."],
     "network_iface_deactivate" => ["info",  "Deactivating interface [%s]."],
     "network_ifaces_set"       => ["info",  "Setting up interfaces."],
     "network_get_pap_passwd"   => ["info",  "Getting PAP/CHAP password for [%s] from [%s]."],
     "network_get_ppp_option"   => ["info",  "Getting option [%s] from [%s]."],
     "network_set_ppp_option"   => ["info",  "Setting option [%s] in [%s]."],
     "network_set_ppp_connect"  => ["info",  "Setting connect option in [%s]."],
     "network_get_ppp_unsup"    => ["info",  "Getting additional options from [%s]."],
     "network_set_ppp_unsup"    => ["info",  "Setting additional options in [%s]."],
     "network_bootproto_unsup"  => ["warn",  "Boot method [%s] for interface [%s] not supported."],
     "network_get_remote"       => ["info",  "Getting remote address for interface [%s]."],
     "network_set_remote"       => ["info",  "Setting remote address for interface [%s]."],
     "network_ensure_lo"        => ["info",  "Ensuring loopback interface configuration."],

     "filesys_mount"            => ["info",  "Mounting [%s] on [%s]."],
     "filesys_mount_failed"     => ["warn",  "Failed to mount [%s] on [%s]."],
     "filesys_unmount"          => ["info",  "Unmounting [%s] from [%s]."],
     "filesys_unmount_failed"   => ["warn",  "Failed to unmount [%s] from [%s]."],

     "boot_lilo_failed"         => ["warn",  "Failed to run lilo."],
     "boot_lilo_success"        => ["info",  "Succesfully executed lilo."],
     "boot_conf_read_failed"    => ["error", "Failed to open boot configuration file [%s]."],
     "boot_grub_convert_failed" => ["error", "Conversion of [%s] failed."],

     "sfdisk_failed"            => ["error", "Could not run sfdisk."],

     "disks_fstab_add"          => ["info",  "Adding [%s] to fstab."],
     "disks_partition_probe"    => ["info",  "Looking for partitions on [%s]."],
     "disks_size_query"         => ["info",  "Querying size of [%s]."],
     "disks_mount"              => ["info",  "Mounting [%s]."],
     "disks_umount"             => ["info",  "Unmounting [%s]."],
     "disks_mount_error"        => ["error", "Could not find mount tools. No mounting done."],
     
     "memory_swap_found"        => ["info",  "Found swap entry [%s]."],
     "memory_swap_probe"        => ["info",  "Looking for swap entries."],
     
     "print_no_printtool"       => ["warn",  "No printtool setup in directory [%s]."],

     "time_timezone_scan"       => ["info",  "Scanning timezones."],
     "time_timezone_cmp"        => ["info",  "Scanning timezones: [%s]."],
     "time_timezone_set"        => ["info",  "Setting timezone as [%s]."],
     "time_localtime_set"       => ["info",  "Setting local time as [%s]."]
    );

1;
