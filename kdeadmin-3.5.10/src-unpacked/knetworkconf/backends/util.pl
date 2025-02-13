#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# Utility functions.
#
# Copyright (C) 2000-2001 Ximian, Inc.
#
# Authors: Hans Petter Jansson <hpj@ximian.com>
#          Arturo Espinosa <arturo@ximian.com>
#          Michael Vogt <mvo@debian.org> - Debian 2.[2|3] support.
#          David Lee Ludwig <davidl@wpi.edu> - Debian 2.[2|3] support.
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


# --- Utilities for strings, arrays and other data structures --- #


$SCRIPTSDIR = "/usr/share/apps/knetworkconf/backends";
if ($SCRIPTSDIR =~ /^@scriptsdir[@]/)
{
    $SCRIPTSDIR = ".";
    $DOTIN = ".in";
}

sub gst_max
{
  return ($_[0] > $_[1])? $_[0]: $_[1];
}

# Boolean <-> strings conversion.

sub gst_util_read_boolean
{
  my ($v) = @_;
    
  return 1 if ($v =~ "true" ||
               $v =~ "yes"  ||
               $v =~ "YES"  ||
               $v =~ "on"   ||
               $v eq "1");
  return 0;
}


sub gst_print_boolean_yesno
{
  if ($_[0] == 1) { return "yes"; }
  return "no";
}


sub gst_print_boolean_truefalse
{
  if ($_[0] == 1) { return "true"; }
  return "false";
}


sub gst_print_boolean_onoff
{
  if ($_[0] == 1) { return "on"; }
  return "off";
}


# Pushes a list to an array, only if it's not already in there.
# I'm sure there's a smarter way to do this. Should only be used for small
# lists, as it's O(N^2). Larger lists with unique members should use a hash.

sub gst_push_unique
{
  my $arr = $_[0];
  my $found;
  my $i;

  # Go through all elements in pushed list.

  for ($i = 1; $_[$i]; $i++)
  {
    # Compare against all elements in destination array.
	
    $found = "";
    for $elem (@$arr)
    {
      if ($elem eq $_[$i]) { $found = $elem; last; }
    }
	
    if ($found eq "") { push (@$arr, $_[$i]); }
  }
}


# Merges scr array into dest array.
sub gst_arr_merge
{
  my ($dest, $src) = @_;
  my (%h, $i);

  foreach $i (@$a, @$b)
  {
    $h{$i} = 1;
  }

  @$a = keys %h;
  return $a;
}

# Given an array and a pattern, it returns the index of the
# array that contains it
sub gst_array_find_index
{
    my($arrayRef, $pattern) = @_;
    my(@array)              = @{$arrayRef};
    my($numElements)        = scalar(@array);
    my(@indexes)            = (0..$numElements);
    my(@elements);
    
    @elements = grep @{$arrayRef}[$_] =~ /$pattern/, @indexes;
    return(wantarray ? @elements : $elements[0]);
}

    

sub gst_ignore_line
{
  if (($_[0] =~ /^[ \t]*\#/) || ($_[0] =~ /^[ \t\n\r]*$/)) { return 1; }
  return 0;
}


# &gst_item_is_in_list
#
# Given:
#   * A scalar value.
#   * An array.
# this function will return 1 if the scalar value is in the array, 0 otherwise.

sub gst_item_is_in_list
{
  my ($value, @arr) = @_;
  my ($item);

  foreach $item (@arr)
  {
    return 1 if $value eq $item;
  }

  return 0;
}


# Recursively compare a structure made of nested arrays and hashes, diving
# into references, if necessary. Circular references will cause a loop.
# Watch it: arrays must have elements in the same order to be equal.
sub gst_util_struct_eq
{
  my ($a1, $a2) = @_;
  my ($type1, $type2);
  my (@keys1, @keys2);
  my ($elem1, $elem2);
  my $i;

  $type1 = ref $a1;
  $type2 = ref $a2;
  
  return 0 if $type1 != $type2;
  return 1 if $a1 eq $a2;
  return 0 if (!$type1); # Scalars
  
  if ($type1 eq "SCALAR") {
    return 0 if $$a1 ne $$a2;
  }
  elsif ($type1 eq "ARRAY")
  {
    return 0 if $#$a1 != $#$a2;

    for ($i = 0; $i <= $#$a1; $i++)
    {
      return 0 if !&gst_util_struct_eq ($$a1[$i], $$a2[$i]);
    }
  }
  elsif ($type1 eq "HASH") {
    @keys1 = sort keys (%$a1);
    @keys2 = sort keys (%$a2);

    return 0 if !&gst_util_struct_eq (\@keys1, \@keys2);
    foreach $i (@keys1)
    {
      return 0 if !&gst_util_struct_eq ($$a1{$i}, $$a2{$i});
    }
  }
  else
  {
    return 0;
  }
    
  return 1;
}


# &gst_get_key_for_subkeys
#
# Given:
#   * A hash-table with its values containing references to other hash-tables,
#     which are called "sub-hash-tables".
#   * A list of possible keys (stored as strings), called the "match_list".
# this method will look through the "sub-keys" (the keys of each
# sub-hash-table) seeing if one of them matches up with an item in the
# match_list.  If so, the key will be returned.

sub gst_get_key_for_subkeys
{
  my %hash = %{$_[0]};
  my @match_list = @{$_[1]};

  foreach $key (keys (%hash))
  {
    my %subhash = %{$hash{$key}};
    foreach $item (@match_list)
    {
      if ($subhash{$item} ne "") { return $key; }
    }
  }

  return "";
}


# &gst_get_key_for_subkey_and_subvalues
#
# Given:
#   * A hash-table with its values containing references to other hash-tables,
#     which are called "sub-hash-tables".  These sub-hash-tables contain
#     "sub-keys" with associated "sub-values".
#   * A sub-key, called the "match_key".
#   * A list of possible sub-values, called the "match_list".
# this function will look through each sub-hash-table looking for an entry
# whose:
#   * sub-key equals match_key.
#   * sub-key associated sub-value is contained in the match_list.

sub gst_get_key_for_subkey_and_subvalues
{
  my %hash = %{$_[0]};
  my $key;
  my $match_key = $_[1];
  my @match_list = @{$_[2]};

  foreach $key (keys (%hash))
  {
    my %subhash = %{$hash{$key}};
    my $subvalue = $subhash{$match_key};

    if ($subvalue eq "") { next; }

    foreach $item (@match_list)
    {
      if ($item eq $subvalue) { return $key; }
    }
  }

  return "";
}


# --- IP calculation --- #


# &gst_ip_calc_network (<IP>, <netmask>)
#
# Calculates the network address and returns it as a string.

sub gst_ip_calc_network
{
  my @ip_reg1;
  my @ip_reg2;

  @ip_reg1 = ($_[0] =~ /([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)/);
  @ip_reg2 = ($_[1] =~ /([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)/);

  $ip_reg1[0] = ($ip_reg1[0] * 1) & ($ip_reg2[0] * 1);
  $ip_reg1[1] = ($ip_reg1[1] * 1) & ($ip_reg2[1] * 1);
  $ip_reg1[2] = ($ip_reg1[2] * 1) & ($ip_reg2[2] * 1);
  $ip_reg1[3] = ($ip_reg1[3] * 1) & ($ip_reg2[3] * 1);

  return join ('.', @ip_reg1);
}


# &gst_ip_calc_network (<IP>, <netmask>)
#
# Calculates the broadcast address and returns it as a string.

sub gst_ip_calc_broadcast
{
  my @ip_reg1;
  my @ip_reg2;
  
  @ip_reg1 = ($_[0] =~ /([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)/);
  @ip_reg2 = ($_[1] =~ /([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)/);
 
  @ip_reg1 = ($cf_hostip =~ /([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)/);

  $ip_reg1[0] = ($ip_reg1[0] * 1) | (~($ip_reg2[0] * 1) & 255);
  $ip_reg1[1] = ($ip_reg1[1] * 1) | (~($ip_reg2[1] * 1) & 255);
  $ip_reg1[2] = ($ip_reg1[2] * 1) | (~($ip_reg2[2] * 1) & 255);
  $ip_reg1[3] = ($ip_reg1[3] * 1) | (~($ip_reg2[3] * 1) & 255);
  
  return join ('.', @ip_reg1);
}

# Forks a process, running $proc with @args in the child, and
# printing the returned value of $proc in the pipe. Parent
# returns a structure with useful data about the process.
sub gst_process_fork
{
  my ($proc, @args) = @_;
  my $pid;
  local *PARENT_RDR;
  local *CHILD_WTR;
  
  pipe (PARENT_RDR, CHILD_WTR);
  
  $pid = fork ();
  if ($pid)
  {
    # Parent
    close CHILD_WTR;
    return {"pid" => $pid, "fd" => *PARENT_RDR, "fileno" => fileno (*PARENT_RDR)};
  }
  else
  {
    my $ret;
    close PARENT_RDR;
    # Child
    $ret = &$proc (@args);
    my $type = ref ($ret);

    if (!$type)
    {
      print CHILD_WTR $ret;
    }
    elsif ($type eq 'ARRAY')
    {
      print CHILD_WTR "$_\n" foreach (@$ret);
    }

    close CHILD_WTR;
    exit (0);
  }
}


# Close pipe, kill process, wait for it to finish.
sub gst_process_kill
{
  my ($proc) = @_;
  
  &gst_file_close ($$proc{"fd"});
  kill 2, $$proc{"pid"};
  waitpid ($$proc{"pid"}, undef);
}


# Populate a bitmap of the used file descriptors.
sub gst_process_list_build_fd_bitmap
{
  my ($procs) = @_;
  my ($bits, $proc);
  
  foreach $proc (@$procs)
  {
    vec ($bits, $$proc{"fileno"}, 1) = 1;
  }
  
  return $bits;
}


# Receives a seconds timeout (may be float) and a ref to
# a list of processes (each returned by gst_fork_process), and
# set the "ready" key to true in all the procs that are ready
# to return values, false otherwise. Returns time left before
# timeout.
sub gst_process_list_check_ready
{
  my ($timeout, $procs) = @_;
  my ($bits, $bitsleft, $bitsready, $timestamp, $timeleft);

  $procs = [ $procs ] if ref ($procs) ne 'ARRAY';
  $bits = &gst_process_list_build_fd_bitmap ($procs);
  
  # Check with timeout which descriptors are ready with info.
  $timeout = undef if $timeout == 0;
  $timeleft = $timeout;
  $bitsleft = $bits;
  while (($timeout eq undef) || ($timeleft > 0))
  {
    $timestamp = time;
    select ($bitsleft, undef, undef, $timeleft);
    $timeleft -= time - $timestamp if $timeout ne undef;
    
    $bitsready |= $bitsleft;
    $bitsleft = $bits & (~$bitsready);
    last if $bitsready eq $bits;
  }
  $bits = $bitsready;

  # For every process, set "ready" key to 1/0 depending on
  # its file descriptor bit.
  foreach $proc (@$procs)
  {
    $$proc{"ready"} = (ord ($bits) & (1 << $$proc{"fileno"}))? 1 : 0;
  }

  return $timeleft;
}


sub gst_process_result_collect
{
  my ($proc, $func, @args) = @_;
  my ($value, $tmp, $lines);

  if ($$proc{"ready"})
  {
    my @list;

    $lines .= $tmp while (sysread ($$proc{"fd"}, $tmp, 4096));
    goto PROC_KILL unless $lines;
    if ($lines =~ /\n/)
    {
      @list = split ("\n", $lines);
    }
    else
    {
      push @list, $line;
    }

    $value = &$func (\@list, @args);
  }

 PROC_KILL:
  &gst_process_kill ($proc);

  return $value;
}


1;
