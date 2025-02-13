#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# Functions for hacker debug.
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

use File::Path;
use File::Copy;

$SCRIPTSDIR = "/usr/share/apps/knetworkconf/backends";
if ($SCRIPTSDIR =~ /^@scriptsdir[@]/)
{
    $SCRIPTSDIR = ".";
    $DOTIN = ".in";
}

require "$SCRIPTSDIR/general.pl$DOTIN";
require "$SCRIPTSDIR/file.pl$DOTIN";


%gst_debug_fd_hash = ();


sub gst_debug_open_output_file
{
  local *FILE;
  my $debug_path = &gst_file_get_debug_path () . "/$gst_name/1/$_[0]";

  if (!exists $gst_debug_fd_hash{$debug_path})
  {
    &gst_debug_rotate_try ();
    open (FILE, ">>$debug_path");
    $gst_debug_fd_hash{$debug_path} = *FILE;
  }
  
  return $gst_debug_fd_hash{$debug_path};
}

sub gst_debug_close_all
{
    my ($file, @files);

    @files = keys %gst_debug_fd_hash;
    foreach $file (@files)
    {
        &gst_file_close ($gst_debug_fd_hash{$file});
        delete $gst_debug_fd_hash{$file};
    }
}

sub gst_debug_print_string_to_file
{
  my $debug_file;

  $debug_file = &gst_debug_open_output_file ($_[0]);
  print $debug_file $_[1];
}

sub gst_debug_print_log_to_file
{
  my ($file, $doc) = @_;
  my (@buff, $line, $fd);

  $fd = &gst_debug_open_output_file ($file);
  
  @buff = split ("\n", $doc);
  foreach $line (@buff)
  {
    print $fd "$line\n";
  }
}


sub gst_debug_print_string
{
  if ($gst_debug) { print STDERR $_[0]; }
  &gst_debug_print_string_to_file ("debug", $_[0]);
}


sub gst_debug_print_line
{
  &gst_debug_print_string ($_[0] . "\n");
}


sub gst_debug_print_indent
{
  my $indent = $_[0];
  my $indent_string = "";
  
  $indent_string = "  " x $indent;
  &gst_debug_print_string ($indent_string);
}


sub gst_debug_print_indented_string
{
  my ($indent, @string) = @_;

  &gst_debug_print_indent ($indent);
  &gst_debug_print_string (@string);
}


sub gst_debug_print_indented_line
{
  my $indent = shift @_;
  my @line = @_;

  &gst_debug_print_indent ($indent);
  &gst_debug_print_line (@line);
}


sub gst_debug_print_struct
{
  foreach $i (@_)
  {
    &gst_debug_print_struct_r (0, 0, $i);
  }
}


sub gst_debug_print_struct_r
{
  my ($indent) = $_[0];
  my $is_hash_value = $_[1];
  my $a = $_[2];
  my $type;
  my @keys;
  my $elem;
  my $i;

  $type = ref $a;

  if (!$is_hash_value) { &gst_debug_print_indent ($indent); }

  if ($type eq "SCALAR")
  {
    &gst_debug_print_line ($$a);
  }
  elsif ($type eq "ARRAY")
  {
    &gst_debug_print_line ("[ARRAY]");

    for ($i = 0; $i <= $#$a; $i++)
    {
      &gst_debug_print_struct_r ($indent + 1, 0, $$a[$i]);
    }
  }
  elsif ($type eq "HASH")
  {
    @keys = sort keys (%$a);

    &gst_debug_print_line ("[HASH]");

    foreach $i (@keys)
    {
      &gst_debug_print_indented_string ($indent + !$is_hash_value, $i . " -> ");
      &gst_debug_print_struct_r ($indent + !$is_hash_value + 1, 1, $$a{$i});
    }
  }
  else
  {
    &gst_debug_print_line ($a);
  }
}


$gst_debug_dir_rotation_was_made = 0;

sub gst_debug_rotate_try
{

  my $debug_file = $_[0];
  my $debug_tool_dir = &gst_file_get_debug_path () . "/$gst_name";

  # If this is the first debug created by this tool on this invocation,
  # rotate the debug directories and create a new, empty one.

  if (!$gst_debug_dir_rotation_was_made)
  {
    my $i;

    $gst_debug_dir_rotation_was_made = 1;

    if (-e "$debug_tool_dir/9")
    {
      &gst_file_rmtree ("$debug_tool_dir/9", 0, 1);
    }

    for ($i = 8; $i; $i--)
    {
      if (-e "$debug_tool_dir/$i")
      {
        move ("$debug_tool_dir/$i", "$debug_tool_dir/" . ($i + 1));
      }
    }
    
    &gst_file_create_path ("$debug_tool_dir/1");
  }
}

1;
