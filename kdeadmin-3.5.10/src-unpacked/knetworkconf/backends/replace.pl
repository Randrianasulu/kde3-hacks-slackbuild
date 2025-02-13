#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# replace.pl: Common in-line replacing stuff for the ximian-setup-tools backends.
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


$SCRIPTSDIR = "/usr/share/apps/knetworkconf/backends";
if ($SCRIPTSDIR =~ /^@scriptsdir[@]/)
{
    $SCRIPTSDIR = ".";
    $DOTIN = ".in";
}

require "$SCRIPTSDIR/util.pl$DOTIN";
require "$SCRIPTSDIR/file.pl$DOTIN";
require "$SCRIPTSDIR/parse.pl$DOTIN";


# General rules: all replacing is in-line. Respect unsupported values, comments
# and as many spacing as possible.

# The concept of keyword (kw) here is a key, normaly in its own line, whose
# boolean representation is its own existence.

# A $re is a regular expression. In most functions here, regular expressions
# are converted to simple separators, by using gst_replace_regexp_to_separator.
# This makes it easier to convert a parse table into a replace table.

# Every final replacing function to be used by a table must handle one key
# at a time, but may replace several values from there.
#
# Return 0 for success, and -1 for failure.
#
# Most of these functions have a parsing counterpart. The convention is
# that parse becomes replace and split becomes join:
# gst_parse_split_first_str -> gst_replace_join_first_str

# Additional abstraction: replace table entries can have
# arrays inside. The replace proc will be ran with every
# combination that the arrays provide. Ex:
# ["user", \&gst_replace_foo, [0, 1], [2, 3] ] will replace
# using all possibilities in the combinatory of [0, 1]x[2, 3].
# Check RedHat 7.2's network replace table for further
# enlightenment.
sub gst_replace_run_entry
{
  my ($values_hash, $key, $proc, $cp, $value) = @_;
  my ($ncp, $i, $j, $res);

  $ncp = [@$cp];
  for ($i = 0; $i < scalar (@$cp); $i ++)
  {
      if (ref $$cp[$i] eq "ARRAY")
      {
          foreach $j (@{$$cp[$i]})
          {
              $$ncp[$i] = $j;
              $res = -1 if &gst_replace_run_entry ($values_hash, $key, $proc, $ncp, $value);
          }
          return $res;
      }
  }
  
  # OK, the given entry didn't have any array refs in it...
  
  &gst_debug_print_line ("gst_replace_from_table: $key");
  return -1 if (!&gst_parse_replace_hash_values ($ncp, $values_hash));
  push (@$ncp, $$values_hash{$key}) unless $key eq "_always_";
  $res = -1 if &$proc (@$ncp);
  return $res;
}

# gst_replace_from_table takes a file mapping, a replace table, a hash
# of values, probably made from XML parsing, and whose keys are
# the same keys the table handles.
#
# Table entries whose keys are not present in the values_hash
# will not be processed. More than one entry may process the same key.
#
# The functions in the replace tables, most of which are coded in
# this file, receive the mapped files of the first argument, and then
# a set of values. The last argument is the value of the $values_hash
# for the corresponding key of the entry.
sub gst_replace_from_table
{
  my ($fn, $table, $values_hash, $old_hash) = @_;
  my ($key, $proc, @param);
  my ($i, @cp, @files, $res);

  $$fn{"OLD_HASH"} = $old_hash;
  
  foreach $i (@$table)
  {
    @cp = @$i;
    $key = shift (@cp);

    $proc = shift (@cp);
    @files = &gst_parse_replace_files (shift (@cp), $fn);
    unshift @cp, @files if (scalar @files) > 0;

    if ((exists $$values_hash{$key}) or ($key eq "_always_"))
    {
      $res = &gst_replace_run_entry ($values_hash, $key, $proc, \@cp, $$values_hash{$key});
    }
    elsif ((!exists $$values_hash{$key}) && (exists $$old_hash{$key}))
    {
      # we need to remove all the instances of the known variables that doesn't exist in the XML
      $res = &gst_replace_run_entry ($values_hash, $key, $proc, \@cp, undef);
    }
  }

  return $res;
}

# Wacky function that tries to create a field separator from a regular expression.
# Doesn't work with all possible regular expressions: just with the ones we are working with.
sub gst_replace_regexp_to_separator
{
  $_ = $_[0];

  s/\[([^^])([^\]])[^\]]*\]/$1/g;
  s/\+//g;
  s/\$//g;
  s/[^\*]\*//g;

  return $_;
}

sub is_array_ref
{
  my $val;

  return 1 if (ref ($val) eq "ARRAY");
  return 0 if (ref ($val) eq undef);

  &gst_debug_print_line ("is_array_ref: We shouldn't be here!");

  return 0;
}

sub set_value
{
  my ($key, $val, $re) = @_;
  
  return $key . &gst_replace_regexp_to_separator ($re) . $val;
}

# Edit a $file, wich is assumed to have a column-based format, with $re matching field separators
# and one record per line. Search for lines with the corresponding $key.
# The last arguments can be any number of standard strings.
sub gst_replace_split
{
  my ($file, $key, $re, @value) = @_;
  my ($fd, @line, @res);
  my ($buff, $i);
  my ($pre_space, $post_comment);
  my ($line_key, $val, $ret);

  &gst_report_enter ();
  &gst_report ("replace_split", $key, $file);

  $buff = &gst_file_buffer_load ($file);
  
  foreach $i (@$buff)
  {
    $pre_space = $post_comment = "";

    chomp $i;
    $pre_space    = $1 if $i =~ s/^([ \t]+)//;
    $post_comment = $1 if $i =~ s/([ \t]*\#.*)//;
    
    if ($i ne "")
    {
      @line = split ($re, $i, 2);
      $line_key = shift (@line);

      # found the key?
      if ($line_key eq $key)
      {
        shift (@value) while ($value[0] eq "" && (scalar @value) > 0);

        if ((scalar @value) == 0)
        {
          $i = "";
          next;
        }

        $val = shift (@value);

        chomp $val;
        $i = &set_value ($key, $val, $re);
      }
    }

    $i = $pre_space . $i . $post_comment . "\n";
  }

  foreach $i (@value)
  {
    push (@$buff, &set_value ($key, $i, $re) . "\n") if ($i ne "");
  }

  &gst_file_buffer_clean ($buff);
  $ret = &gst_file_buffer_save ($buff, $file);
  &gst_report_leave ();
  return $ret;
}

# Replace all key/values in file with those in @$value,
# deleting exceeding ones and appending those required.
sub gst_replace_join_all
{
  my ($file, $key, $re, $value) = @_;

  return &gst_replace_split ($file, $key, $re, @$value);
}

# Find first $key value and replace with $value. Append if not found.
sub gst_replace_join_first_str
{
  my ($file, $key, $re, $value) = @_;

  return &gst_replace_split ($file, $key, $re, $value);
}

# Treat value as a bool value, using val_off and val_on as corresponding
# boolean representations.
sub gst_replace_join_first_bool
{
  my ($file, $key, $re, $val_on, $val_off, $value) = @_;

  # Fixme: on and off should be a parameter.
  $value = ($value == 1)? $val_on: $val_off;
  
  return &gst_replace_split ($file, $key, $re, $value);
}

# Find first key in file, and set array join as value.
sub gst_replace_join_first_array
{
  my ($file, $key, $re1, $re2, $value) = @_;

  return &gst_replace_split ($file, $key, $re1, join (&gst_replace_regexp_to_separator ($re2), @$value));
}

# Escape $value in /bin/sh way, find/append key and set escaped value.
sub gst_replace_sh
{
  my ($file, $key, $value) = @_;
  my $ret;

  $value = &gst_parse_shell_escape ($value);

  &gst_report_enter ();
  &gst_report ("replace_sh", $key, $file);

  # This will expunge the whole var if the value is empty.
  if ($value eq "")
  {
    $ret = &gst_replace_split ($file, $key, "[ \t]*=[ \t]*");
  }
  else
  {
    $ret = &gst_replace_split ($file, $key, "[ \t]*=[ \t]*", $value);
  }
  
  &gst_report_leave ();
  return $ret;
}

# Escape $value in /bin/sh way, find/append key and set escaped value, make sure line har 
sub gst_replace_sh_export
{
  my ($file, $key, $value) = @_;
  my $ret;

  $value = &gst_parse_shell_escape ($value);

  # This will expunge the whole var if the value is empty.

  # FIXME: Just adding "export " works for the case I need, though it doesn't
  # handle arbitraty whitespace. Something should be written to replace gst_replace_split()
  # here.

  if ($value eq "")
  {
    $ret = &gst_replace_split ($file, "export " . $key, "[ \t]*=[ \t]*");
  }
  else
  {
    $ret = &gst_replace_split ($file, "export " . $key, "[ \t]*=[ \t]*", $value);
  }
  
  return $ret;
}

# Treat value as a yes/no bool, replace in shell style.
# val_true and val_false have default yes/no values.
# use &gst_replace_sh_bool (file, key, value) if defaults are desired.
sub gst_replace_sh_bool
{
  my ($file, $key, $val_true, $val_false, $value) = @_;

  # default value magic.
  if ($val_false eq undef)
  {
      $value = $val_true;
      $val_true = undef;
  }

  $val_true  = "yes" unless $val_true;
  $val_false = "no"  unless $val_false;

  $value = ($value == 1)? $val_true: $val_false;
  
  return &gst_replace_sh ($file, $key, $value);
}

# Treat value as a yes/no bool, replace in export... shell style.
sub gst_replace_sh_export_bool
{
  my ($file, $key, $val_true, $val_false, $value) = @_;

  # default value magic.
  if ($val_false eq undef)
  {
      $value = $val_true;
      $val_true = undef;
  }

  $val_true  = "yes" unless $val_true;
  $val_false = "no"  unless $val_false;

  $value = ($value == 1)? $val_true: $val_false;
  
  return &gst_replace_sh_export ($file, $key, $value);
}

# Get a fully qualified hostname from a $key shell var in $file
# and set the hostname part. e.g.: suse70's /etc/rc.config's FQHOSTNAME.
sub gst_replace_sh_set_hostname
{
  my ($file, $key, $value) = @_;
  my ($domain);

  $domain = &gst_parse_sh_get_domain ($file, $key);
  return &gst_replace_sh ($file, $key, "$value.$domain");
}

# Get a fully qualified hostname from a $key shell var in $file
# and set the domain part. e.g.: suse70's /etc/rc.config's FQHOSTNAME.
sub gst_replace_sh_set_domain
{
  my ($file, $key, $value) = @_;
  my ($hostname);

  $hostname = &gst_parse_sh_get_hostname ($file, $key);
  return &gst_replace_sh ($file, $key, "$hostname.$value");
}

# Join the array pointed by $value with the corresponding $re separator
# and assign that to the $key shell variable in $file.
sub gst_replace_sh_join
{
  my ($file, $key, $re, $value) = @_;

  return &gst_replace_sh ($file, $key,
                          join (&gst_replace_regexp_to_separator ($re), @$value));
}

# replace a regexp with $value
sub gst_replace_sh_re
{
  my ($file, $key, $re, $value) = @_;
  my ($val);

  $val = &gst_parse_sh ($file, $key);

  if ($val =~ /$re/)
  {
    $val =~ s/$re/$value/;
  }
  else
  {
    $val .= $value;
  }

  $val = '"' . $val . '"' if ($val !~ /^\".*\"$/);

  return &gst_replace_split ($file, $key, "[ \t]*=[ \t]*", $val)
}

# Quick trick to set a keyword $key in $file. (think /etc/lilo.conf keywords).
sub gst_replace_kw
{
  my ($file, $key, $value) = @_;
  my $ret;

  &gst_report_enter ();
  &gst_report ("replace_kw", $key, $file);
  $ret = &gst_replace_split ($file, $key, "\$", ($value)? "\n" : "");
  &gst_report_leave ();
  return $ret;
}

# The kind of $file whose $value is its first line contents.
# (/etc/hostname)
sub gst_replace_line_first
{
  my ($file, $value) = @_;
  my $fd;

  &gst_report_enter ();
  &gst_report ("replace_line_first", $file);
  $fd = &gst_file_open_write_from_names ($file);
  &gst_report_leave ();
  return -1 if !$fd;

  print $fd "$value\n";
  &gst_file_close ($fd);
  
  return 0;
}

# For every key in %$value, replace/append the corresponding key/value pair.
# The separator for $re1 
sub gst_replace_join_hash
{
  my ($file, $re1, $re2, $value) = @_;
  my ($i, $res, $tmp, $val);
  my ($oldhash, %merge);

  $oldhash = &gst_parse_split_hash ($file, $re1, $re2);
  foreach $i (keys (%$value), keys (%$oldhash))
  {
    $merge{$i} = 1;
  }

  $res = 0;
  
  foreach $i (keys (%merge))
  {
    if (exists $$value{$i})
    {
      $val = join (&gst_replace_regexp_to_separator ($re2), @{$$value{$i}});
      $tmp = &gst_replace_split ($file, $i, $re1, $val);
    }
    else
    {
      # This deletes the entry.
      $tmp = &gst_replace_split ($file, $i, $re1);
    }
    $res = $tmp if !$res;
  }

  return $res;
}

# Find $re matching send string and replace parenthesyzed
# part of $re with $value. FIXME: apply meeks' more general impl.
sub gst_replace_chat
{
  my ($file, $re, $value) = @_;
  my ($buff, $i, $bak, $found, $substr, $ret);

  &gst_report_enter ();
  &gst_report ("replace_chat", $file);
  $buff = &gst_file_buffer_load ($file);

  SCAN: foreach $i (@$buff)
  {
    $bak = "";
    $found = "";
    my ($quoted);
    chomp $i;

    while ($i ne "")
    {
	 # If it uses quotes. FIXME: Assuming they surround the whole string.
	 if ($i =~ /^\'/)
	 {
	   $i =~ s/\'([^\']*)\' ?//;
	   $found = $1;
	   $quoted = 1;
	 }
	 else
	 {
	   $i =~ s/([^ \t]*) ?//;
	   $found = $1;
	   $quoted = 0;
	 }
	 
	 # If it looks like what we're looking for,
	 # substitute what is in parens with value.
	 if ($found =~ /$re/i)
	 {
	   $substr = $1;
	   $found =~ s/$substr/$value/i;

	   if ($quoted == 1)
	   {
	     $i = $bak . "\'$found\' " . $i . "\n";
	   }
	   else
	   {
	     $i = $bak . "$found " . $i . "\n";
	   }

	   last SCAN;
	 }

	 if ($quoted == 1)
	 {
	   $bak .= "\'$found\'";
	 }
	 else
	 {
	   $bak .= "$found";
	 }

	 $bak .= " " if $bak ne "";
    }
    
    $i = $bak . "\n";
  }

  $ret = &gst_file_buffer_save ($buff, $file);
  &gst_report_leave ();
  return $ret;
}

# Find/append $section in ini $file and replace/append
# $var = $value pair. FIXME: should reimplement with
# interfaces style. This is too large.
sub gst_replace_ini
{
  my ($file, $section, $var, $value) = @_;
  my ($buff, $i, $found_flag, $ret);
  my ($pre_space, $post_comment, $sec_save);

  &gst_report_enter ();
  &gst_report ("replace_ini", $var, $section, $file);

  $buff = &gst_file_buffer_load ($file);

  &gst_file_buffer_join_lines ($buff);
  $found_flag = 0;
  
  foreach $i (@$buff)
  {
    $pre_space = $post_comment = "";
    
    chomp $i;
    $pre_space = $1 if $i =~ s/^([ \t]+)//;
    $post_comment = $1 if $i =~ s/([ \t]*[\#;].*)//;
    
    if ($i ne "")
    {
      if ($i =~ /\[$section\]/i)
      {
        $i =~ s/(\[$section\][ \t]*)//i;
        $sec_save = $1;
        $found_flag = 1;
      }

      if ($found_flag)
      {
        if ($i =~ /\[[^\]]+\]/)
        {
          $i = "$var = $value\n$i" if ($value ne "");
          $found_flag = 2;
        }
        
        if ($i =~ /^$var[ \t]*=/i)
        {
          if ($value ne "")
          {
            $i =~ s/^($var[ \t]*=[ \t]*).*/$1$value/i;
          }
          else
          {
            $i = "";
          }
          $found_flag = 2;
        }
      }
    }
    
    if ($found_flag && $sec_save ne "")
    {
      $i = $sec_save . $i;
      $sec_save = "";
    }
    
    $i = $pre_space . $i . $post_comment . "\n";
    last if $found_flag == 2;
  }

  push @$buff, "\n[$section]\n" if (!$found_flag);
  push @$buff, "$var = $value\n" if ($found_flag < 2 && $value ne "");

  &gst_file_buffer_clean ($buff);
  $ret = &gst_file_buffer_save ($buff, $file);
  &gst_report_leave ();
  return $ret;
}

# Well, removes a $section from an ini type $file.
sub gst_replace_remove_ini_section
{
  my ($file, $section) = @_;
  my ($buff, $i, $found_flag, $ret);
  my ($pre_space, $post_comment, $sec_save);

  &gst_report_enter ();
  &gst_report ("replace_del_ini_sect", $section, $file);

  $buff = &gst_file_buffer_load ($file);

  &gst_file_buffer_join_lines ($buff);
  $found_flag = 0;

  foreach $i (@$buff)
  {
    $pre_space = $post_comment = "";

    chomp $i;
    $pre_space = $1 if $i =~ s/^([ \t]+)//;
    $post_comment = $1 if $i =~ s/([ \t]*[\#;].*)//;
    
    if ($i ne "")
    {
      if ($i =~ /\[$section\]/i)
      {
        $i =~ s/(\[$section\][ \t]*)//i;
        $found_flag = 1;
      }
      elsif ($found_flag && $i =~ /\[.+\]/i)
      {
        $i = $pre_space . $i . $post_comment . "\n";
        last;
      }
    }

    if ($found_flag)
    {
      if ($post_comment =~ /^[ \t]*$/)
      {
        $i = "";
      }
      else
      {
        $i = $post_comment . "\n";
      }
    }
    else
    {
      $i = $pre_space . $i . $post_comment . "\n";
    }
  }

  &gst_file_buffer_clean ($buff);
  $ret = &gst_file_buffer_save ($buff, $file);
  &gst_report_leave ();
  return $ret;
}

# Removes a $var in $section of a ini type $file.
sub gst_replace_remove_ini_var
{
  my ($file, $section, $var) = @_;
  &gst_replace_ini ($file, $section, $var, "");
}

# Replace using boolean $value with a yes/no representation,
# ini style.
sub gst_replace_ini_bool
{
  my ($file, $section, $var, $value) = @_;

  $value = ($value == 1)? "yes": "no";

  return &gst_replace_ini ($file, $section, $var, $value);
}

# *cap replacement methods.
#sub gst_replace_cap
#{
#  my ($file, $section, $var, $value) = @_;
#  my ($buff, $i, $found_flag, $ret);
#  my ($pre_space, $post_comment, $sec_save);
#
#  $buff = &gst_file_buffer_load ($file);
##  &gst_file_buffer_join_lines ($buff);
#  $found_flag = 0;
#
#  foreach $i (@$buff)
#  {
#    $pre_space = $post_comment = "";
#    
#    chomp $i;
#    $pre_space = $1    if $i =~ s/^([ \t]+)//;
#    $post_comment = $1 if $i =~ s/^([ \t]*[\#].*)//;
#    
#    if ($i ne "")
#    {
#      if ($i =~ /^$section[|:]/i)
#      {
#        $i =~ s/^($section)//i;
#        $sec_save = $1;
#        $found_flag = 1;
#      }
#
#      if ($found_flag)
#      {
#        if ($i =~ /^[a-z0-9]+[|:]/)
#        {
#          $i = "\t:$var=$value:\n$i";
#          $found_flag = 2;
#        }
#
#        if ($found_flag && $i =~ /^:$var[=:]/i)
#        {
#          if ($value ne "")
#          {
#            $i =~ s/^(:$var)[^:]*/$1=$value/i;
#          }
#          else
#          {
#            $i = "";
#          }
#          $found_flag = 2;
#        }
#      }
#    }
#
#    if ($found_flag && $sec_save ne "")
#    {
#      $i = $sec_save . $i;
#      $sec_save = "";
#    }
#    
#    $i = $pre_space . $i . $post_comment . "\n";
#    last if $found_flag == 2;
#  }
#
#  push @$buff, "\n$section:\\\n" if (!$found_flag);
#  push @$buff, "\t:$var=$value:\n" if ($found_flag < 2 && $value ne "");
#  
#  &gst_file_buffer_clean ($buff);
#  $ret = &gst_file_buffer_save ($buff, $file);
#  return $ret;
#}

sub gst_replace_remove_cap_section
{
  my ($file, $section) = @_;
  my ($buff, $i, $found_flag, $ret);
  my ($pre_space, $post_comment, $sec_save);

  $buff = &gst_file_buffer_load ($file);
  $found_flag = 0;

  foreach $i (@$buff)
  {
    $pre_space = $post_comment = "";

    chomp $i;
    $pre_space = $1    if $i =~ s/^([ \t]+)//;
    $post_comment = $1 if $i =~ s/^([ \t]*[\#].*)//;

    if ($i ne "")
    {
      if ($i =~ /^$section[|:]/i)
      {
        $i = "";
        $found_flag = 1;
      }
      elsif ($found_flag && $i =~ /^[a-z0-9]+[|:]/i)
      {
        $i = $pre_space . $i . $post_comment . "\n";
        last;
      }
    }

    if ($found_flag)
    {
      if ($post_comment =~ /^[ \t]*$/)
      {
        $i = "";
      }
      else
      {
        $i = $post_comment . "\n";
      }
    }
    else
    {
      $i = $pre_space . $i . $post_comment . "\n";
    }
  }

  &gst_file_buffer_clean ($buff);
  $ret = &gst_file_buffer_save ($buff, $file);
  return $ret;
}

# Save a printcap buffer to file. This doesn't do any extra processing for now,
# but it may do so in the future.
sub gst_replace_printcap_buffer_save
{
  my ($file, $buf) = @_;
  my $ret;

  &gst_file_buffer_clean ($buf);
  $ret = &gst_file_buffer_save ($buf, $file);
  return $ret;
}

sub gst_replace_printcap_print_stanza
{
  my ($stanza) = @_;
  return $stanza . ":\n";
}

sub gst_replace_printcap_print_option
{
  my ($option, $type, $value) = @_;
  return "\t:" . $option . $type . $value . ":\n";
}

sub gst_replace_printcap_add_stanza
{
  my ($buf, $stanza) = @_;

  push @$buf, "\n";
  push @$buf, "##PRINTTOOL3## LOCAL unknown NAxNA {} Unknown Default {}\n";
  push @$buf, &gst_replace_printcap_print_stanza ($stanza);

  return ($#$buf - 1, $#$buf);
}

sub gst_replace_printcap_add_option_slot
{
  my ($buf, $stanza_line_no) = @_;
  my (@buf_tail);

  @buf_tail = splice (@$buf, $stanza_line_no + 1);
  push @$buf, "\t:NEW_OPTION:\n";
  push @$buf, @buf_tail;

  return $stanza_line_no + 1;
}

sub gst_replace_printcap_remove_stanza_from_buf
{
  my ($buf, $printtool_line_no, $stanza_line_no) = @_;
  my ($next_printtool_line_no, $next_stanza_line_no);
  my ($splice_start, $splice_end);

  ($next_printtool_line_no, $next_stanza_line_no) =
    &gst_parse_printcap_get_next_stanza ($buf, $stanza_line_no + 1);

  if ($printtool_line_no != -1)
  {
    $splice_start = $printtool_line_no;
  }
  else
  {
    $splice_start = $stanza_line_no;
  }

  if ($next_printtool_line_no != -1)
  {
    $splice_end = $next_printtool_line_no;
  }
  else
  {
    $splice_end = $next_stanza_line_no;
  }

  if ($splice_end != -1)
  {
    splice (@$buf, $splice_start, $splice_end - $splice_start);
  }
  else
  {
    splice (@$buf, $splice_start);
  }
}

sub gst_replace_printcap_remove_option_slot
{
  my ($buf, $option_line_no) = @_;
  splice (@$buf, $option_line_no, 1);
}

# High-level API.
sub gst_replace_printcap_remove_printer
{
  my ($file, $printer) = @_;
  my ($buf, $printtool_line_no, $stanza_line_no);

  $buf = &gst_parse_printcap_buffer_load ($file);

  ($printtool_line_no, $stanza_line_no) = &gst_parse_printcap_find_stanza ($buf, 0, $printer);
  &gst_replace_printcap_remove_stanza_from_buf ($buf, $printtool_line_no, $stanza_line_no);

  $ret = &gst_replace_printcap_buffer_save ($file, $buf);
  return $ret;
}

# High-level API.
sub gst_replace_printcap
{
  my ($file, $section, $var, $type, $value) = @_;
  my ($printtool_line_no, $stanza_line_no, $option_line_no);
  my ($buf, $ret);

  $buf = &gst_parse_printcap_buffer_load ($file);

  ($printtool_line_no, $stanza_line_no) = &gst_parse_printcap_find_stanza ($buf, 0, $section);
  if ($stanza_line_no == -1)
  {
    ($printtool_line_no, $stanza_line_no) = &gst_replace_printcap_add_stanza ($buf, $section);
  }

  $option_line_no = &gst_parse_printcap_find_option ($buf, $stanza_line_no + 1, $var);
  if ($option_line_no == -1)
  {
    $option_line_no = &gst_replace_printcap_add_option_slot ($buf, $stanza_line_no);
  }

  if ($type ne "")
  {
    $$buf [$option_line_no] = "\t:" . $var . $type . $value . ":\n";
  }
  elsif ($value == 1)
  {
    $$buf [$option_line_no] = "\t:" . $var . ":\n";
  }
  else
  {
    &gst_replace_printcap_remove_option_slot ($buf, $option_line_no);
  }

  $ret = &gst_replace_printcap_buffer_save ($file, $buf);
  return $ret;
}

# Debian /etc/network/interfaces in-line replacing methods.

# From loaded buffer, starting at $line_no, find next debian
# interfaces format stanza. Return array ref with all stanza args.
# -1 if not found.
# NOTE: $line_no is a scalar ref. and gives the position of next stanza.
sub gst_replace_interfaces_get_next_stanza
{
  my ($buff, $line_no, $stanza_type) = @_;
  my ($i, $line);

  while ($$line_no < (scalar @$buff))
  {
    $_ = $$buff[$$line_no];
    $_ = &gst_parse_interfaces_line_clean ($_);

    if (/^$stanza_type[ \t]+[^ \t]/)
    {
      s/^$stanza_type[ \t]+//;
      return [ split ("[ \t]+", $_) ];
    }
    $$line_no ++;
  }

  return -1;
}

sub gst_replace_interfaces_line_is_stanza
{
  my ($line) = @_;

  return 1 if $line =~ /^(iface|auto|mapping)[ \t]+[^ \t]/;
  return 0;
}

# Scan for next option. An option is something that is
# not a stanza. Return key/value tuple ref, -1 if not found.
# $$line_no will contain position.
sub gst_replace_interfaces_get_next_option
{
  my ($buff, $line_no) = @_;
  my ($i, $line, $empty_lines);

  $empty_lines = 0;
  
  while ($$line_no < (scalar @$buff))
  {
    $_ = $$buff[$$line_no];
    $_ = &gst_parse_interfaces_line_clean ($_);

    if (!/^$/)
    {
      return [ split ("[ \t]+", $_, 2) ] if (! &gst_replace_interfaces_line_is_stanza ($_));
      $$line_no -= $empty_lines;
      return -1;
    }
    else
    {
      $empty_lines ++;
    }
    
    $$line_no ++;
  }

  $$line_no -= $empty_lines;
  return -1;
}

# Search buffer for option with key $key, starting
# at $$line_no position. Return 1/0 found result.
# $$line_no will show position.
sub gst_replace_interfaces_option_locate
{
  my ($buff, $line_no, $key) = @_;
  my $option;

  while (($option = &gst_replace_interfaces_get_next_option ($buff, $line_no)) != -1)
  {
    return 1 if ($$option[0] eq $key);
    $$line_no ++;
  }
  
  return 0;
}

# Locate stanza line for $iface in $buff, starting at $$line_no.
sub gst_replace_interfaces_next_stanza_locate
{
  my ($buff, $line_no) = @_;

  return &gst_replace_interfaces_get_next_stanza ($buff, \$$line_no, "(iface|auto|mapping)");
}

sub gst_replace_interfaces_iface_stanza_locate
{
  my ($buff, $line_no, $iface) = @_;

  return &gst_replace_interfaces_generic_stanza_locate ($buff, \$$line_no, $iface, "iface");
}

sub gst_replace_interfaces_auto_stanza_locate
{
  my ($buff, $line_no, $iface) = @_;

  return &gst_replace_interfaces_generic_stanza_locate ($buff, \$$line_no, $iface, "auto");
}

sub gst_replace_interfaces_generic_stanza_locate
{
  my ($buff, $line_no, $iface, $stanza_name) = @_;
  my $stanza;

  while (($stanza = &gst_replace_interfaces_get_next_stanza ($buff, \$$line_no, $stanza_name)) != -1)
  {
    return 1 if ($$stanza[0] eq $iface);
    $$line_no++;
  }

  return 0;
}

# Create a Debian Woody stanza, type auto, with the requested
# @ifaces as values.
sub gst_replace_interfaces_auto_stanza_create
{
  my ($buff, @ifaces) = @_;
  my ($count);
  
  push @$buff, "\n" if ($$buff[$count] ne "");
  push @$buff, "auto " . join (" ", @ifaces) . "\n";
}

# Append a stanza for $iface to buffer.
sub gst_replace_interfaces_iface_stanza_create
{
  my ($buff, $iface) = @_;
  my ($count);

  $count = $#$buff;
  push @$buff, "\n" if ($$buff[$count] ne "");
  push @$buff, "iface $iface inet static\n";
}

# Delete $iface stanza and all its option lines.
sub gst_replace_interfaces_iface_stanza_delete
{
  my ($file, $iface) = @_;
  my ($buff, $line_no, $line_end, $stanza);

  $buff = &gst_file_buffer_load ($file);
  &gst_file_buffer_join_lines ($buff);
  $line_no = 0;

  return -1 if (!&gst_replace_interfaces_iface_stanza_locate ($buff, \$line_no, $iface));
  $line_end = $line_no + 1;
  &gst_replace_interfaces_next_stanza_locate ($buff, \$line_end);

  while ($line_no < $line_end)
  {
    delete $$buff[$line_no];
    $line_no++;
  }
  
  $line_no = 0;
  if (&gst_replace_interfaces_auto_stanza_locate ($buff, \$line_no, $iface))
  {
    $line_end = $line_no + 1;
    &gst_replace_interfaces_next_stanza_locate ($buff, \$line_end);

    while ($line_no < $line_end)
    {
      delete $$buff[$line_no];
      $line_no++;
    }
  }
  
  &gst_file_buffer_clean ($buff);
  return &gst_file_buffer_save ($buff, $file);
}

# Find $iface stanza line and replace $pos value (ie the method).
sub gst_replace_interfaces_stanza_value
{
  my ($file, $iface, $pos, $value) = @_;
  my ($buff, $line_no, $stanza);
  my ($pre_space, $line, $line_arr);

  $buff = &gst_file_buffer_load ($file);
  &gst_file_buffer_join_lines ($buff);
  $line_no = 0;

  if (!&gst_replace_interfaces_iface_stanza_locate ($buff, \$line_no, $iface))
  {
    $line_no = 0;
    &gst_replace_interfaces_iface_stanza_create ($buff, $iface);
    &gst_replace_interfaces_iface_stanza_locate ($buff, \$line_no, $iface);
  }

  $line = $$buff[$line_no];
  chomp $line;
  $pre_space = $1 if $line =~ s/^([ \t]+)//;
  $line =~ s/^iface[ \t]+//;
  @line_arr = split ("[ \t]+", $line);
  $line_arr[$pos] = $value;
  $$buff[$line_no] = $pre_space . "iface " . join (' ', @line_arr) . "\n";

  &gst_file_buffer_clean ($buff);
  return &gst_file_buffer_save ($buff, $file);
}

# Find/append $key option in $iface stanza and set $value.
sub gst_replace_interfaces_option_str
{
  my ($file, $iface, $key, $value) = @_;
  my ($buff, $line_no, $stanza, $ret);
  my ($pre_space, $line, $line_arr);

  &gst_report_enter ();
  &gst_report ("replace_ifaces_str", $key, $iface);
  
  $buff = &gst_file_buffer_load ($file);
  &gst_file_buffer_join_lines ($buff);
  $line_no = 0;

  if (!&gst_replace_interfaces_iface_stanza_locate ($buff, \$line_no, $iface))
  {
    $line_no = 0;
    &gst_replace_interfaces_iface_stanza_create ($buff, $iface);
    &gst_replace_interfaces_iface_stanza_locate ($buff, \$line_no, $iface);
  }

  $line_no++;

  if (&gst_replace_interfaces_option_locate ($buff, \$line_no, $key))
  {
    if ($value eq "") # Delete option if value is empty.
    {
      $$buff[$line_no] = "";
    }
    else
    {
      chomp $$buff[$line_no];
      $$buff[$line_no] =~ s/^([ \t]*$key[ \t]).*/$1/;
    }
  }
  elsif ($value ne "")
  {
    $line_no --;
    chomp $$buff[$line_no];
    $$buff[$line_no] =~ s/^([ \t]*)(.*)/$1$2\n$1$key /;
  }

  $$buff[$line_no] .= $value . "\n" if $value ne "";
  
  &gst_file_buffer_clean ($buff);
  $ret = &gst_file_buffer_save ($buff, $file);
  &gst_report_leave ();
  return $ret;
}

# $key option is keyword. $value says if it should exist or not.
sub gst_replace_interfaces_option_kw
{
  my ($file, $iface, $key, $value) = @_;

  return &gst_replace_interfaces_option_str ($file, $iface, $key, $value? " ": "");
}

# !$value says if keyword should exist or not (ie noauto).
sub gst_replace_interfaces_option_kw_not
{
  my ($file, $iface, $key, $value) = @_;

  return &gst_replace_interfaces_option_kw ($file, $iface, $key, !$value);
}


# Implementing pump(8) pump.conf file format replacer.
# May be useful for dhcpd too.

# Try to find the next option, returning an array ref
# with the found key and the rest of the options in
# two items, or -1 if not found.
sub gst_replace_pump_get_next_option
{
  my ($buff, $line_no) = @_;

  while ($$line_no < (scalar @$buff))
  {
    $_ = $$buff[$$line_no];
    $_ = &gst_parse_interfaces_line_clean ($_);
    if ($_ ne "")
    {
      return [ split ("[ \t]+", $_, 2) ];
    }
    
    $$line_no ++;
  }

  return -1;
}

# Iterate with get_next_option, starting at $line_no
# until the option with $key is found, or eof.
# Return 0/1 as found.
sub gst_replace_pump_option_locate
{
  my ($buff, $line_no, $key) = @_;
  my ($opt);
  
  while (($opt = &gst_replace_pump_get_next_option ($buff, $line_no)) != -1)
  {
    return 1 if $$opt[0] eq $key;
    return 0 if $$opt[0] eq "}";

    $$line_no ++;
  }
  
  return 0;
}

# Try to find a "device" option whose interface is $iface,
# starting at $$line_no. Return 0/1 as found.
sub gst_replace_pump_get_device
{
  my ($buff, $line_no, $iface) = @_;
  my ($opt);

  while (($opt = &gst_replace_pump_get_next_option ($buff, $line_no)) != -1)
  {
    if ($$opt[0] eq "device")
    {
      $$opt[1] =~ s/[ \t]*\{//;
      return 1 if $$opt[1] eq $iface;
    }

    $$line_no ++;
  }

  return 0;
}

# Add a device entry for $iface at the end of $buff.
sub gst_replace_pump_add_device
{
  my ($buff, $iface) = @_;

  push @$buff, "\n";
  push @$buff, "device $iface {\n";
  push @$buff, "\t\n";
  push @$buff, "}\n";
}

# Find a "device" section for $iface and
# replace/add/delete the $key option inside the section.
sub gst_replace_pump_iface_option_str
{
  my ($file, $iface, $key, $value) = @_;
  my ($line_no, $ret);

  $buff = &gst_file_buffer_load ($file);
  $line_no = 0;

  if (!&gst_replace_pump_get_device ($buff, \$line_no, $iface))
  {
    $line_no = 0;
    &gst_replace_pump_add_device ($buff, $iface);
    &gst_replace_pump_get_device ($buff, \$line_no, $iface);
  }

  $line_no ++;

  if (&gst_replace_pump_option_locate ($buff, \$line_no, $key))
  {
    if ($value eq "")
    {
      $$buff[$line_no] = "";
    }
    else
    {
      chomp $$buff[$line_no];
      $$buff[$line_no] =~ s/^([ \t]*$key[ \t]).*/$1/;
    }
  }
  elsif ($value ne "")
  {
    $line_no --;
    chomp $$buff[$line_no];
    $$buff[$line_no] =~ s/^([ \t]*)(.*)/$1$2\n$1$key /;
  }

  if ($value ne "")
  {
    $value =~ s/^[ \t]+//;
    $value =~ s/[ \t]+$//;
    $$buff[$line_no] .= &gst_parse_shell_escape ($value) . "\n";
  }

  &gst_file_buffer_clean ($buff);
  $ret = &gst_file_buffer_save ($buff, $file);
  &gst_report_leave ();
  return $ret;
}

# Same as function above, except $key is a keyword.
sub gst_replace_pump_iface_kw
{
  my ($file, $iface, $key, $value) = @_;

  return &gst_replace_pump_iface_option_str ($file, $iface, $key, $value? " ": "");
}

# Same, but use the negative of $value (i.e. nodns)
sub gst_replace_pump_iface_kw_not
{
  my ($file, $iface, $key, $value) = @_;

  return &gst_replace_pump_iface_kw ($file, $iface, $key, !$value);
}

sub gst_replace_xml_pcdata
{
  my ($file, $varpath, $data) = @_;
  my ($model, $branch, $fd, $compressed);

  ($model, $compressed) = &gst_xml_model_scan ($file);
  $branch = &gst_xml_model_ensure ($model, $varpath);

  &gst_xml_model_set_pcdata ($branch, $data);

  return &gst_xml_model_save ($model, $file, $compressed);
}

sub gst_replace_xml_attribute
{
  my ($file, $varpath, $attr, $value) = @_;
  my ($model, $branch, $fd, $compressed);

  ($model, $compressed) = &gst_xml_model_scan ($file);
  $branch = &gst_xml_model_ensure ($model, $varpath);

  &gst_xml_model_set_attribute ($branch, $attr, $value);

  return &gst_xml_model_save ($model, $file, $compressed);
}

sub gst_replace_xml_pcdata_with_type
{
  my ($file, $varpath, $type, $data) = @_;
  my ($model, $branch, $fd, $compressed);

  ($model, $compressed) = &gst_xml_model_scan ($file);
  $branch = &gst_xml_model_ensure ($model, $varpath);

  &gst_xml_model_set_pcdata ($branch, $data);
  &gst_xml_model_set_attribute ($branch, "TYPE", $type);

  return &gst_xml_model_save ($model, $file, $compressed);
}

sub gst_replace_xml_attribute_with_type
{
  my ($file, $varpath, $attr, $type, $value) = @_;
  my ($model, $branch, $fd, $compressed);

  ($model, $compressed) = &gst_xml_model_scan ($file);
  $branch = &gst_xml_model_ensure ($model, $varpath);

  &gst_xml_model_set_attribute ($branch, $attr, $value);
  &gst_xml_model_set_attribute ($branch, "TYPE", $type);

  return &gst_xml_model_save ($model, $file, $compressed);
}

sub gst_replace_alchemist_ensure_list_types
{
  my ($model, $varpath, $setpath) = @_;
  my ($branch, @path);

  $branch = &gst_xml_model_find ($model, $varpath);
  @path   = split /\//, $setpath;

  # NOTE: The following could be done with a depth-iterator callback from a func
  # similar to gst_xml_model_find ().

  for $elem (@path)
  {
    next if ($elem eq "");
    my @children = @$branch;
    shift @children;  # Attributes
    $branch = undef;

    while (@children)
    {
      if ($children [0] eq $elem)
      {
        shift @children;
        $branch = shift @children;
        &gst_xml_model_set_attribute ($branch, "TYPE", "LIST");
        last;
      }

      shift @children;
      shift @children;
    }

    last if ($branch == undef);
  }
}

sub gst_replace_alchemist
{
  my ($file, $varpath, $type, $value) = @_;
  my ($fullpath, $model, $branch, $fd, $compressed);

  $fullpath = "/adm_context/datatree/" . $varpath;
  ($model, $compressed) = &gst_xml_model_scan ($file);
  $branch = &gst_xml_model_ensure ($model, $fullpath);
  &gst_replace_alchemist_ensure_list_types ($model, "/adm_context/datatree/", $varpath);

  &gst_xml_model_set_attribute ($branch, "VALUE", $value);
  &gst_xml_model_set_attribute ($branch, "TYPE",  $type);

  return &gst_xml_model_save ($model, $file, $compressed);
}

sub gst_replace_alchemist_print
{
  my ($file, $printer, $varpath, $type, $value) = @_;
  my ($fullpath, $model, $branch, $fd, $compressed);

  $fullpath = "/adm_context/datatree/printconf/print_queues/" . $printer . "/" . $varpath;
  ($model, $compressed) = &gst_xml_model_scan ($file);

  $branch = &gst_xml_model_ensure ($model, $fullpath);

  &gst_replace_alchemist_ensure_list_types ($model, "/adm_context/datatree/",
                                            "printconf/print_queues/" . $printer . "/" . $varpath);

  &gst_xml_model_set_attribute ($branch, "VALUE", $value);
  &gst_xml_model_set_attribute ($branch, "TYPE",  $type);

  $branch = &gst_xml_model_find ($model, "/adm_context/datatree/printconf/print_queues/" . $printer);
  &gst_xml_model_set_attribute ($branch, "ATOMIC", "TRUE");

  return &gst_xml_model_save ($model, $file, $compressed);
}

# This could be split up.
sub gst_replace_alchemist_print_option
{
  my ($file, $printer, $name, $type, $value) = @_;
  my ($varpath, $model, $branch, $fd, $compressed, $options, $option);

  ($model, $compressed) = &gst_xml_model_scan ($file);
  $branch = &gst_xml_model_ensure ($model, "/adm_context/datatree/printconf/print_queues/" . $printer .
                                   "/filter_data/foomatic_defaults");
  &gst_replace_alchemist_ensure_list_types ($model, "/adm_context/datatree/", "printconf/print_queues/" .
                                            $printer . "/filter_data/foomatic_defaults");
  &gst_xml_model_set_attribute ($branch, "ANONYMOUS", "TRUE");

  # See if option is already defined.

  $options = &gst_xml_model_get_children ($branch);

  foreach $o (@$options)
  {
    my $opt_node = &gst_xml_model_find ($o, "name");
    next if (!$opt_node);

    if (&gst_xml_model_get_attribute ($opt_node, "VALUE") eq $name)
    {
      $option = $o;
      last;
    }
  }

  # If not, create node for it.

  if (!$option)
  {
    $option = &gst_xml_model_add ($branch, "", $option_default);
    &gst_xml_model_set_attribute ($option, "TYPE", "LIST");
  }

  # Set the option attributes.

  my $node = &gst_xml_model_ensure ($option, "name");
  &gst_xml_model_set_attribute ($node, "TYPE", "STRING");
  &gst_xml_model_set_attribute ($node, "VALUE", $name);

  $node = &gst_xml_model_ensure ($option, "type");
  &gst_xml_model_set_attribute ($node, "TYPE", "STRING");
  &gst_xml_model_set_attribute ($node, "VALUE", $type);

  $node = &gst_xml_model_ensure ($option, "default");
  &gst_xml_model_set_attribute ($node, "TYPE", "STRING");
  &gst_xml_model_set_attribute ($node, "VALUE", $value);

  return &gst_xml_model_save ($model, $file, $compressed);
}

sub gst_replace_fq_hostname
{
  my ($file, $hostname, $domain) = @_;

  if ($domain eq undef)
  {
    return &gst_replace_line_first ($file, "$hostname");
  }
  else
  {
    return &gst_replace_line_first ($file, "$hostname.$domain");
  }
}

sub gst_replace_rcinet1conf
{
  my ($file, $iface, $kw, $val) = @_;
  my ($line);

  $iface =~ s/eth//;
  $line = "$kw\[$iface\]";

  $val = "\"$val\"" if ($val ne undef);

  return &gst_replace_split ($file, $line, "[ \t]*=[ \t]*", $val);
}

sub gst_replace_rcinet1conf_global
{
  my ($file, $kw, $val) = @_;

  $val = "\"$val\"";

  return &gst_replace_split ($file, $kw, "[ \t]*=[ \t]*", $val)
}

sub gst_replace_wireless_opts
{
  my ($file, $iface, $proc, $kw, $value) = @_;
  my $ifaces = &$proc ();
  my $found = 0;
  my $search = 1;
  my $buff;
  
  foreach $i (@$ifaces)
  {
    $found = 1 if ($iface eq $i);
  }

  $buff = &gst_file_buffer_load ($file);

  foreach $i (@$buff)
  {
    if (/^case/)
    {
      # we don't want to search inside the case
      $search = 0;
    }
    elsif (/^esac/)
    {
      # we want to continue searching
      $search = 1;
    }
    if ((/^[ \t]*$kw/) && ($search))
    {
      $_ = "$kw=\"$value\"";
      $found = 1;
    }
  }

  if (!$found)
  {
    push @$buff, "$kw=\"$value\"";
  }

  &gst_file_buffer_clean ($buff);
  return &gst_file_buffer_save ($buff, $file);
}

# Functions for replacing in FreeBSD's /etc/ppp/ppp.conf
sub gst_replace_pppconf_common
{
  my ($pppconf, $section, $key, $string) = @_;
  my ($buff, $line_no, $end_line_no, $i, $found);

  $buff = &gst_file_buffer_load ($pppconf);

  $line_no = &gst_parse_pppconf_find_stanza ($buff, $section);

  if ($line_no ne -1)
  {
    # The stanza exists
    $line_no++;

    $end_line_no = &gst_parse_pppconf_find_next_stanza ($buff, $line_no);
    $end_line_no = scalar @$buff + 1 if ($end_line_no == -1);
    $end_line_no--;

    for ($i = $line_no; $i <= $end_line_no; $i++)
    {
      if ($$buff[$i] =~ /[ \t]+$key/)
      {
        if ($string ne undef)
        {
          $$buff[$i] = " $string\n";
          $found = 1;
        }
        else
        {
          delete $$buff[$i];
        }
      }
    }

    if ($found != 1)
    {
      $$buff[$end_line_no] .= " $string\n" if ($string ne undef);
    }
  }
  else
  {
    if ($string ne undef)
    {
      push @$buff, "$section:\n";
      push @$buff, " $string\n";
    }
  }

  &gst_file_buffer_clean ($buff);
  return &gst_file_buffer_save ($buff, $pppconf);
}

sub gst_replace_pppconf
{
  my ($pppconf, $section, $key, $value) = @_;
  &gst_replace_pppconf_common ($pppconf, $section, $key, "set $key $value");
}

sub gst_replace_pppconf_bool
{
  my ($pppconf, $section, $key, $value) = @_;
  &gst_replace_pppconf_common ($pppconf, $section, $key,
                               ($value == 1)? "enable $key" : "disable $key");
}

sub gst_replace_confd_net_re
{
  my ($file, $key, $re, $value) = @_;
  my ($str, $contents, $i, $found, $done);

  $found = $done = 0;
  $contents = &gst_file_buffer_load ($file);

  for ($i = 0; $i <= scalar (@$contents); $i++)
  {
    # search for key
    if ($$contents[$i] =~ /^$key[ \t]*=[ \t]*\(/)
    {
      $found = 1;

      do {
        if ($$contents[$i] =~ /\"([^\"]*)\"/)
        {
          $str = $1;

          if ($str =~ /$re/)
          {
            $str =~ s/$re/$value/;
          }
          else
          {
            $str .= $value;
          }

          $$contents[$i] =~ s/\"([^\"]*)\"/\"$str\"/;
          $done = 1;
        }

        $i++;
      } while (!$done);
    }
  }

  if (!$found)
  {
    push @$contents, "$key=(\"$value\")\n";
  }

  return &gst_file_buffer_save ($contents, $file);
}

sub gst_replace_confd_net
{
  my ($file, $key, $value) = @_;

  return &gst_replace_confd_net_re ($file, $key, ".*", $value);
}
