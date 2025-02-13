#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# parse.pl: Common parsing stuff for the ximian-setup-tools backends.
#
# Copyright (C) 2000-2001 Ximian, Inc.
#
# Authors: Hans Petter Jansson <hpj@ximian.com>
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


$SCRIPTSDIR = "/usr/share/apps/knetworkconf/backends";
if ($SCRIPTSDIR =~ /^@scriptsdir[@]/){

    $SCRIPTSDIR = ".";
    $DOTIN = ".in";
}

require "$SCRIPTSDIR/util.pl$DOTIN";
require "$SCRIPTSDIR/file.pl$DOTIN";
require "$SCRIPTSDIR/debug.pl$DOTIN";


# The concept of keyword (kw) here is a key, normaly in its own line, whose
# boolean representation is its own existence.

# Every final parsing function to be used by a table must handle one key
# at a time, but maybe parse several values from there and return a
# ref to array or hash.
#
# Always return a scalar. If you need to return an array or a hash,
# return a ref to it.

# First some helper functions for the whole process.
# Expand substrings of the form #$substr# to the $value in
# the string or recursively in the array $strarr.

sub gst_parse_expand
{
  my ($strarr, $substr, $value) = @_;

  if (ref $strarr eq "ARRAY")
  {
    my ($i);
    
    $strarr = [ @$strarr ];
    foreach $i (@$strarr)
    {
      $i = &gst_parse_expand ($i, $substr, $value);
    }

    return $strarr;
  }
  
  $strarr =~ s/\#$substr\#/$value/;
  return $strarr;
}

sub gst_parse_replace_hash_values
{
  my ($cp, $hash) = @_;
  my ($j, $replace_key, $value);

  foreach $j (@$cp)
  {
    while ($j =~ /%([^%]*)%/)
    {
      $replace_key = $1;
      if (exists $$hash{$replace_key}) 
      {
        $value = $$hash{$replace_key};
        if (ref $value)
        {
          $j = $value;
        }
        else
        {
          $j =~ s/%$replace_key%/$value/g;
        }
      }
      else
      {
        &gst_debug_print_line ("Warning: gst_parse_replace_hash_values: key $replace_key doesn't exist.");
        return 0;
      }
    }
  }

  return 1;
}

sub gst_parse_replace_files
{
  my ($values, $fn_hash) = @_;
  my @ret;

  return () if $values eq undef;
  $values = [$values] if !ref $values;

  foreach $i (@$values)
  {
    if (exists $$fn_hash{$i})
    {
      push @ret, $$fn_hash{$i};
    }
    else
    {
      push @ret, $i;
    }
  }

  return @ret;
}

# Additional abstraction: parse table entries can have
# arrays inside. The parsing proc will be ran with every
# combination that the arrays provide. Ex:
# ["user", \&gst_parse_foo, [0, 1], [2, 3] ] will parse
# using the combinatory of [0, 1]x[2, 3] until a result
# ne undef is given. Check RedHat 7.2's network parse table
# for further enlightenment.
sub gst_parse_run_entry
{
  my ($hash, $key, $proc, $cp) = @_;
  my ($ncp, $i, $j, $res);

  $ncp = [@$cp];
  for ($i = 0; $i < scalar (@$cp); $i ++)
  {
    if (ref $$cp[$i] eq "ARRAY")
    {
      foreach $j (@{$$cp[$i]})
      {
        $$ncp[$i] = $j;
        $res = &gst_parse_run_entry ($hash, $key, $proc, $ncp);
        return $res if $res ne undef;
      }
      return undef;
    }
  }

  # OK, the given entry didn't have any array refs in it...
  
  return undef if (!&gst_parse_replace_hash_values ($cp, $hash));

  &gst_report_enter ();
  &gst_report ("parse_table", "$key");
  &gst_report_leave ();
  
  $$hash{$key} = &$proc (@$cp);
  return $$hash{$key};
}

# OK, this is the good stuff:

# gst_parse_from_table takes a file mapping and a parse table.
#
# The functions in the replace tables, most of which are coded in
# this file, receive the mapped files of the first argument, and then
# a set of values.

# The value the parse function returns is set into a hash,
# using as key the first item of the parse table entry. This is done
# only if the $hash{$key} is empty, which allows us to try with
# several parse methods to try to get a value, where our parse functions
# can return undef if they failed to get the requested value.
#
# A ref to the hash with all the fetched values is returned.
sub gst_parse_from_table
{
  my ($fn, $table) = @_;
  my %hash;
  my ($key, $proc, @param);
  my ($i, @cp, @files);

  foreach $i (@$table)
  {
    @cp = @$i;
    $key = shift (@cp);

    if ($hash{$key} eq undef)
    {
      $proc = shift (@cp);
      @files = &gst_parse_replace_files (shift (@cp), $fn);

      # Don't unshift the resulting files if none were given.
      unshift @cp, @files if (scalar @files) > 0;

      &gst_parse_run_entry (\%hash, $key, $proc, \@cp);
    }
  }

  foreach $i (keys (%hash))
  {
    delete $hash{$i} if ($hash{$i} eq undef);
  }
  
  return \%hash;
}

# Just return the passed values. If there's just
# one value, the value. If more, a reference to an
# array with the values.
sub gst_parse_trivial
{
  my (@res) = @_;

  &gst_report_enter ();
  &gst_report ("parse_trivial", "@res");
  &gst_report_leave ();

  return $res[0] if (scalar @res) <= 1;
  return \@res;
}

# Try to read a line from $fd and remove any leading or
# trailing white spaces. Return ref to read $line or
# -1 if eof.
sub gst_parse_chomp_line_std
{
  my ($fd) = @_;
  my $line;

  $line = <$fd>;
  return -1 if !$line;

  chomp $line;
  $line =~ s/^[ \t]+//;
  $line =~ s/[ \t]+$//;

  return \$line;
}

# Assuming $line is a line read from a shell file,
# remove comments.
sub gst_parse_process_sh_line
{
  my ($line) = @_;
  my ($pline);

  # This will put escaped hashes out of danger.
  # But only inside valid quotes!
  while ($line =~ /([^\"\']*[\"\'][^\#\"\']*)(\#?)([^\"\']*[\"\'])/g)
  {
      $pline .= $1;
      $pline .= "__hash__" if ($2 ne undef);
      $pline .= $3;
  }

  # The line may not match the regexp above,
  $pline = $line if ($pline eq undef);

  $pline =~ s/\\\#/\\__hash__/g;

  # Nuke everything after a hash and bye bye trailing spaces.
  $pline =~ s/[ \t]*\#.*//;

  # Let escaped hashes come back home.
  $pline =~ s/__hash__/\#/g;

  return $pline;
}

# Same as gst_parse_chomp_line_std, but apply
# the sh line processing before returning.
# -1 if eof, ref to read $line if success.
sub gst_parse_chomp_line_hash_comment
{
  my ($fd) = @_;
  my $line;

  $line = &gst_parse_chomp_line_std ($fd);
  return -1 if $line == -1;

  $line = &gst_parse_process_sh_line ($$line);
  return \$line;
}

# Get an sh line, and remove the export keyword, if any.
sub gst_parse_chomp_line_sh_export
{
  my ($fd) = @_;
  my $line;

  $line = &gst_parse_chomp_line_hash_comment ($fd);
  return -1 if $line == -1;

  $line = $$line;

  $line =~ s/^export //;

  return \$line;
}

# Parse a $file, wich is assumed to have a column-based format, with $re matching field separators
# and one record per line. Search for $key, and return either a scalar with the first ocurrence,
# or an array with all the found ocurrences.
sub gst_parse_split_ref
{
  my ($file, $key, $re, $all, $line_read_proc) = @_;
  my ($fd, @line, @res);

  &gst_report_enter ();
  &gst_report ("parse_split", $key, $file);

  $proc = $line_read_proc? $line_read_proc : \&gst_parse_chomp_line_std;
  
  $fd = &gst_file_open_read_from_names ($file);
  $all = 0 if !$fd;

  while (($line = &$proc ($fd)) != -1)
  {
    $line = $$line;
    next if $line eq "";

    @line = split ($re, $line, 2);

    if (shift (@line) =~ "^$key\$")
    {
      if ($all) {
        push @res, $line[0];
      }
      else
      {
        &gst_report_leave ();
        &gst_file_close ($fd);
        return \$line[0];
      }
    }
  }

  &gst_report_leave ();
  &gst_file_close ($fd);
  return \@res if ($all);
  return -1;
}

sub gst_parse_split
{
  my $res;

  # Don't pass @_ like this anywhere. This is bad practice.
  $res = &gst_parse_split_ref (@_);

  return $$res if ref $res eq "SCALAR";
  return @$res if ref $res eq "ARRAY";
  return undef;
}

# This gives meaning to the $all flag of gst_parse_split, and returns a reference to the array, which
# is what we want. (ie search a.com\nsearch b.com\nsearch c.com)
sub gst_parse_split_all
{
  my ($file, $key, $re, $line_read_proc) = @_;
  my @a;

  @a = &gst_parse_split ($file, $key, $re, 1, $line_read_proc);

  return \@a;
}

# Same, but use the hash_comment routine for line analysis.
sub gst_parse_split_all_hash_comment
{
  my ($file, $key, $re) = @_;

  return &gst_parse_split_all ($file, $key, $re, \&gst_parse_chomp_line_hash_comment);
}

# Make the elements of the resulting array unique.
sub gst_parse_split_all_unique_hash_comment
{
  my ($file, $key, $re) = @_;
  my ($arr, @res);
  my (%hash, $i);

  $arr = &gst_parse_split_all ($file, $key, $re, \&gst_parse_chomp_line_hash_comment);

  foreach $i (@$arr)
  {
    next if exists $hash{$i};
    $hash{$i} = 1;
    push @res, $i;
  }

  return \@res;
}

sub gst_parse_split_all_array_with_pos
{
  my ($file, $key, $pos, $re, $sep, $line_read_proc) = @_;
  my ($arr, @s, @ret, $i);

  $arr = &gst_parse_split_all ($file, $key, $re, $line_read_proc);

  foreach $i (@$arr)
  {
    @s = split ($sep, $i);
    push @ret, @s[0];
  }

  return \@ret;
}

# Same, but for $all = 0. (ie nameserver 10.0.0.1)
sub gst_parse_split_first_str
{
  my ($file, $key, $re, $line_read_proc) = @_;

  return &gst_parse_split ($file, $key, $re, 0, $line_read_proc);
}

# Interpret the result as a boolean. (ie multi on)
sub gst_parse_split_first_bool
{
  my ($file, $key, $re, $line_read_proc) = @_;
  my $ret;

  $ret = &gst_parse_split_first_str ($file, $key, $re, $line_read_proc);

  return undef if ($ret eq undef);
  return (&gst_util_read_boolean ($ret)? 1: 0);
}

# After getting the first field, split the result with $sep matching separators. (ie order hosts,bind)
sub gst_parse_split_first_array
{
  my ($file, $key, $re, $sep, $line_read_proc) = @_;
  my @ret;

  @ret = split ($sep, &gst_parse_split ($file, $key, $re, 0, $line_read_proc));

  return \@ret;
}

sub gst_parse_split_first_array_pos
{
  my ($file, $key, $pos, $re, $sep, $line_read_proc) = @_;
  my (@ret);

  @ret = split ($sep, &gst_parse_split ($file, $key, $re, 0, $line_read_proc));
  return $ret[$pos];
}

# Do an gst_parse_split_first_array and then make
# the array elements unique. This is to fix broken
# searchdomain entries in /etc/resolv.conf, for example.
sub gst_parse_split_first_array_unique
{
  my ($file, $key, $re, $sep, $line_read_proc) = @_;
  my (@arr, @res);
  my (%hash, $i);
  
  @arr = split ($sep, &gst_parse_split ($file, $key, $re, 0, $line_read_proc));

  foreach $i (@arr)
  {
    next if exists $hash{$i};
    $hash{$i} = 1;
    push @res, $i;
  }

  return \@res;
}

# For all keys in $file, sepparated from its values
# by $key_re, sepparate its values using $value_re
# and assign to a newly created hash. Use ONLY when
# you don't know what keys you are going to parse
# (i.e. /etc/hosts). Any other application will not
# be very portable and should be avoided.
sub gst_parse_split_hash
{
  my ($file, $key_re, $value_re) = @_;
  my ($fd, @line, %res, $key);

  &gst_report_enter ();
  &gst_report ("parse_split_hash", $file);
  
  $fd = &gst_file_open_read_from_names ($file);
  
  while (<$fd>)
  {
    chomp;
    s/^[ \t]+//;
    s/[ \t]+$//;
    s/\#.*$//;
    next if (/^$/);
    @line = split ($key_re, $_, 2);

    $key = shift (@line);
    push @{$res{$key}}, split ($value_re, $line[0]);
  }

  &gst_file_close ($fd);
  &gst_report_leave ();
  return undef if (scalar keys (%res) == 0);
  return \%res;
}

# Same as above, but join lines that end with '\'.
sub gst_parse_split_hash_with_continuation
{
  my ($file, $key_re, $value_re) = @_;
  my ($fd, $l, @line, %res, $key);

  &gst_report_enter ();
  &gst_report ("parse_split_hash_cont", $file);
  
  $fd = &gst_file_open_read_from_names ($file);
  
  while (($l = &gst_parse_ini_line_read ($fd)) != -1)
  {
    $_ = $$l;
    chomp;
    s/^[ \t]+//;
    s/[ \t]+$//;
    s/\#.*$//;
    next if (/^$/);
    @line = split ($key_re, $_, 2);

    $key = shift (@line);
    $res{$key} = [ split ($value_re, $line[0]) ];
  }

  &gst_file_close ($fd);
  &gst_report_leave ();
  return undef if (scalar keys (%res) == 0);
  return \%res;
}

# Remove escape sequences in a shell value.
sub gst_parse_shell_unescape
{
  my $ret = $_[0];

  # Quote shell special chars.
  $ret =~ s/\\\"/\\_/g;
  $ret =~ s/\"//g;
  $ret =~ s/\\_/\"/g;
  $ret =~ s/\\\'/\\_/g;
  $ret =~ s/\'//g;
  $ret =~ s/\\_/\'/g;
  $ret =~ s/\\(.)/$1/g;

  return $ret;
}

# unescape (escape (x)) == x
sub gst_parse_shell_escape
{
  my ($value) = @_;
  
  $value =~ s/([\"\`\$\\])/\\$1/g;
  $value = "\"$value\"" if ($value =~ /[ \t\'&|*?\[\]\{\}\{\}<>]/);

  return $value;
}

# For files which are a list of /bin/sh shell variable declarations. (ie GATEWAY=10.10.10.1)
sub gst_parse_sh
{
  my ($file, $key) = @_;
  my $ret;

  &gst_report_enter ();
  &gst_report ("parse_sh", $key, $file);
  $ret = &gst_parse_split_first_str ($file, $key, "[ \t]*=[ \t]*",
                                     \&gst_parse_chomp_line_hash_comment);
  &gst_report_leave ();

  return &gst_parse_shell_unescape ($ret);
}

# Same, but interpret the returning value as a bool. (ie NETWORKING=yes)
sub gst_parse_sh_bool
{
  my ($file, $key) = @_;
  my $ret;

  $ret = &gst_parse_sh ($file, $key);

  return undef if ($ret eq undef);
  return (&gst_util_read_boolean ($ret)? 1: 0);
}

# Get an sh value and then split with $re, returning ref to resulting array.
sub gst_parse_sh_split
{
  my ($file, $key, $re) = @_;
  my (@ret, $val);

  $val = &gst_parse_sh ($file, $key);
  @ret = split ($re, $val);

  return \@ret;
}

# Get a fully qualified hostname from a $key shell var in $file
# and extract the hostname from there. e.g.: suse70's /etc/rc.config's FQHOSTNAME.
sub gst_parse_sh_get_hostname
{
  my ($file, $key) = @_;
  my ($val);

  $val = &gst_parse_sh_split ($file, $key, "\\.");

  return $$val[0];
}

# Get a fully qualified hostname from a $key shell var in $file
# and extract the domain from there. e.g.: suse70's /etc/rc.config's FQHOSTNAME.
sub gst_parse_sh_get_domain
{
  my ($file, $key) = @_;
  my ($val);

  $val = &gst_parse_sh_split ($file, $key, "\\.");

  return join ".", @$val[1..$#$val];
}

# For files which are a list of /bin/sh shell variable exports. (eg export GATEWAY=10.10.10.1)
sub gst_parse_sh_export
{
  my ($file, $key) = @_;
  my $ret;

  &gst_report_enter ();
  &gst_report ("parse_sh", $key, $file);
  $ret = &gst_parse_split_first_str ($file, $key, "[ \t]*=[ \t]*",
                                     \&gst_parse_chomp_line_sh_export);
  &gst_report_leave ();

  return &gst_parse_shell_unescape ($ret);
}

# Same, but interpret the returing value as a bool. (ie export NETWORKING=yes)
sub gst_parse_sh_export_bool
{
  my ($file, $key) = @_;
  my $ret;

  $ret = &gst_parse_sh_export ($file, $key);

  return undef if ($ret eq undef);
  return (&gst_util_read_boolean ($ret)? 1: 0);
}

# Same, but accepting a regexp and returning the value between the paren operator
sub gst_parse_sh_re
{
  my ($file, $key, $re) = @_;
  my $ret;

  $ret = &gst_parse_sh ($file, $key);

  $ret =~ /$re/i;
  return $1;
}


# Search for $keyword in $file, delimited by $re (default " ") or EOL.
# If keyword exists, return 1, else 0.
sub gst_parse_kw
{
  my ($file, $keyword, $re, $line_read_proc) = @_;
  my $res;

  &gst_report_enter ();
  &gst_report ("parse_kw", $keyword, $file);
  
  if (! -f "$gst_prefix/$file")
  {
    &gst_report_enter ();
    &gst_report ("file_open_read_failed", $file);
    &gst_report_leave ();
    &gst_report_leave ();
    return undef;
  }
  
  $re = " " if $re eq undef;
  $res = &gst_parse_split_ref ($file, $keyword, $re, 0, $line_read_proc);

  &gst_report_leave ();
  return 0 if $res == -1;
  return 1;
}

# A file containing the desired value in its first line. (ie /etc/hostname)
sub gst_parse_line_first
{
  my ($file) = @_;
  my ($fd, $res);

  &gst_report_enter ();
  &gst_report ("parse_line_first", $file);
  $fd = &gst_file_open_read_from_names ($file);
  &gst_report_leave ();
  
  return undef if !$fd;

  chomp ($res = <$fd>);
  &gst_file_close ($fd);
  return $res;
}

# parse a chat file, searching for an entry that matches $re.
# $re must have one paren operator (ie "^atd[^0-9]*([0-9, -]+)").
sub gst_parse_chat
{
  my ($file, $re) = @_;
  my ($fd, $found);

  &gst_report_enter ();
  &gst_report ("parse_chat", $file);
  $fd = &gst_file_open_read_from_names ("$file");
  &gst_report_leave ();
  return undef if !$fd;

  while (<$fd>)
  {
    # We'll be emptying $_ as we "scan".
    chomp;
    while ($_ ne "")
    {
      # If it uses quotes. FIXME: Assuming they surround the whole string.
      if (/^\'/)
      {
        s/\'([^\']*)\' ?//;
        $found = $1;
      }
      else
      {
        s/([^ \t]*) ?//;
        $found = $1;
      }
      
      # If it looks like what we're looking for, return what matched the parens.
      if ($found =~ /$re/i)
      {
        &gst_file_close ($fd);
        return $1;
      }
    }
  }
  
  &gst_file_close ($fd);
  # Oops: not found.
  return undef;
}

# Clean an ini line of comments and leading or
# trailing spaces.
sub gst_parse_ini_line_clean
{
  $_ = $_[0];
  
  chomp;
  s/\#.*//;
  s/;.*//;
  s/^[ \t]+//;
  s/[ \t]+$//;

  return $_;
}

# Read an ini line, which may have to be joined
# with the next one if it ends with '\'.
sub gst_parse_ini_line_read
{
  my $fd = $_[0];
  my $l;

  $l = <$fd>;
  return -1 if ($l eq undef);
  
  $l = &gst_parse_ini_line_clean ($l);
  while ($l =~ /\\$/)
  {
    $l =~ s/\\$//;
    $l .= &gst_parse_ini_line_clean (scalar <$fd>);
  }

  return \$l;
}

# Return an array of all found sections in $file.
sub gst_parse_ini_sections
{
  my ($file) = @_;
  my (@sections, $line);

  $fd = &gst_file_open_read_from_names ($file);
  
  while (($line = &gst_parse_ini_line_read ($fd)) != -1)
  {
    $_ = $$line;
    next if (/^$/);
    push @sections, $1 if (/\[([^\]]+)\]/i);
  }

  &gst_file_close ($fd);

  return @sections;
}

# Get the value of a $var in a $section from $file.
sub gst_parse_ini
{
  my ($file, $section, $var) = @_;
  my ($fd, $res, $line);
  my $found_section_flag = 0;

  &gst_report_enter ();
  &gst_report ("parse_ini", $var, $file, $section);
  $fd = &gst_file_open_read_from_names ($file);
  &gst_report_leave ();
  $res = undef;
  
  while (($line = &gst_parse_ini_line_read ($fd)) != -1)
  {
    $_ = $$line;
    next if (/^$/);

    if (/\[$section\]/i)
    {
      $found_section_flag = 1;
      next;
    }

    if ($found_section_flag)
    {
      if (/^$var[ \t]*=/i)
      {
        s/^$var[ \t]*=[ \t]*//i;
        $res = $_;
        last;
      }
      elsif (/\[\S+\]/i)
      {
        last;
      }
    }
  }

  &gst_file_close ($fd);

  return $res;
}

# Same, but treat value as bool and return 1/0.
sub gst_parse_ini_bool
{
  my ($file, $section, $var) = @_;
  my $ret;
  
  $ret = &gst_parse_ini ($file, $section, $var);
  
  return undef if ($ret eq undef);
  return (&gst_util_read_boolean ($ret)? 1: 0);
}

sub gst_parse_cap_line_clean
{
  $_ = $_[0];

  chomp;
  s/^[ \t]*\#.*//;
  s/;.*//;
  s/^[ \t]+//;
  s/[ \t]+$//;

  return $_; 
}

sub gst_parse_cap_line_read
{
  my $fd = $_[0];
  my $l;

  $l = <$fd>;
  return -1 if ($l eq undef);
  
  $l = &gst_parse_cap_line_clean ($l);
  while ($l =~ /\\$/)
  {
    $l =~ s/\\$//;
    $l .= &gst_parse_cap_line_clean (scalar <$fd>);
  }

  return \$l;
}

sub gst_parse_cap_sections
{
  my ($file) = @_;
  my (@sections, $line);

  $fd = &gst_file_open_read_from_names ($file);
  
  while (($line = &gst_parse_cap_line_read ($fd)) != -1)
  {
    $_ = $$line;
    next if (/^$/);
    push @sections, $1 if (/^([^:|]+)/i);
  }

  &gst_file_close ($fd);
  return @sections;
}

sub gst_parse_cap
{
  my ($file, $section, $var) = @_;
  my ($fd, $res, $line);
  my $found_section_flag = 0;

  $fd = &gst_file_open_read_from_names ($file);
  $res = undef;
  
  while (($line = &gst_parse_ini_line_read ($fd)) != -1)
  {
    $_ = $$line;
    next if (/^$/);
    if (/^$section[:|]/i)
    {
      $found_section_flag = 1;
    }

    if ($found_section_flag && /:$var\#/i)
    {
      $_ =~ /:$var\#([^:]*)/;
      $res = $1;
      last;
    }
#    if ($found_section_flag && /:$var[#=]/i)
#    {
#      $_ =~ /:$var[#=]([^:]*)/;
#      $res = $1;
#      last;
#    }
  }

  &gst_file_close ($fd);
  return $res;
}

sub gst_parse_cap_bool
{
  my ($file, $section, $var) = @_;
  my ($fd, $res, $line);
  my $found_section_flag = 0;

  $fd = &gst_file_open_read_from_names ($file);
  $res = 0;
  
  while (($line = &gst_parse_ini_line_read ($fd)) != -1)
  {
    $_ = $$line;
    next if (/^$/);
    if (/^$section[:|]/i)
    {
      $found_section_flag = 1;
    }

    if ($found_section_flag && /:$var[:\#=]/i)
    {
      $res = 1;
      last;
    }
  }

  &gst_file_close ($fd);
  return $res;
}

# Load a printcap file to buffer, join \ lines and split them back up into a
# 'one option, printtool comment or section name per line' format.
sub gst_parse_printcap_buffer_load
{
  my ($file) = @_;
  my ($inbuf, @outbuf);

  $inbuf = &gst_file_buffer_load ($file);
  &gst_file_buffer_join_lines ($inbuf);

  for $i (@$inbuf)
  {
    my ($comment) = ("");

    chomp $i;
    $comment = $1 if $i =~ s/^([ \t]*[\#].*)//;

    if ($i ne "")
    {
      my @line = split /:/, $i;

      if ($i =~ /^[a-z0-9]+/i)
      {
        push @outbuf, ($line [0] . ":\n");
        shift @line;
      }

      for $elem (@line)
      {
        $elem =~ s/^[ \t]//;
        $elem =~ s/[ \t]$//;
        if ($elem ne "")
        {
          push @outbuf, ("\t:$elem:\n");
        }
      }
    }
    elsif ($comment ne "")
    {
      push @outbuf, ($comment . "\n");
    }
    else
    {
      push @outbuf, "\n";
    }
  }

  return \@outbuf;
}

# Find next printer definition, returning (printtool-comment-lineno, stanza-name-lineno).
sub gst_parse_printcap_get_next_stanza
{
  my ($buf, $line_no) = @_;
  my ($last_printtool_line) = (-1);

  while ($line_no <= $#$buf)
  {
    if ($$buf [$line_no] =~ /^\#\#PRINTTOOL3\#\#/)
    {
      $last_printtool_line = $line_no;
    }
    elsif ($$buf [$line_no] =~ /^[a-z0-9]+/i)
    {
      return ($last_printtool_line, $line_no);
    }

    $line_no++;
  }

  return (-1, -1);
}

# Find next printer option.
sub gst_parse_printcap_get_next_option
{
  my ($buf, $line_no) = @_;

  while ($line_no <= $#$buf)
  {
    if ($$buf [$line_no] =~ /^\#\#PRINTTOOL3\#\#/ ||
        $$buf [$line_no] =~ /^[a-z0-9]+/i)
    {
      last;
    }

    if ($$buf [$line_no] =~ /^\t:/)
    {
      return $line_no;
    }

    $line_no++;
  }

  return -1;
}

sub gst_parse_printcap_parse_stanza
{
  my ($stanza) = @_;
  my ($key);

  $key = $1 if $stanza =~ /^([a-z0-9]+)/i;
  return $key;
}

sub gst_parse_printcap_parse_option
{
  my ($option) = @_;
  my ($key, $value);

  $key   = $1 if $option =~ /^\t:([a-z0-9]+)/i;
  $value = $1 if $option =~ /^\t:[a-z0-9]+[\#=]([a-z0-9\/_-]*)/i;
  return ($key, $value);
}

# Locate stanza line for $printer in $buf, starting at $line_no.
sub gst_parse_printcap_find_stanza
{
  my ($buf, $line_no, $printer) = @_;
  my ($printtool_line_no, $found_printer);

  while ((($printtool_line_no, $line_no) = &gst_parse_printcap_get_next_stanza ($buf, $line_no)))
  {
    if ($line_no == -1) { last; }

    $found_printer = &gst_parse_printcap_parse_stanza ($$buf [$line_no]);
    return ($printtool_line_no, $line_no) if ($found_printer eq $printer);
    $line_no++;
  }

  return (-1, -1);
}

# Search buffer for option with key $key, starting
# at $line_no position. Return line number, or -1 if not found.
sub gst_parse_printcap_find_option
{
  my ($buf, $line_no, $key) = @_;
  my $found_key;

  while (($line_no = &gst_parse_printcap_get_next_option ($buf, $line_no)) != -1)
  {
    ($found_key) = &gst_parse_printcap_parse_option ($$buf [$line_no]);
    return $line_no if ($found_key eq $key);
    $line_no++;
  }

  return -1;
}

# High-level API.
sub gst_parse_printcap
{
  my ($file, $section, $var) = @_;
  my ($printtool_line_no, $stanza_line_no, $option_line_no);
  my ($buf);
  my ($key, $value);

  $buf = &gst_parse_printcap_buffer_load ($file);

  ($printtool_line_no, $stanza_line_no) = &gst_parse_printcap_find_stanza ($buf, 0, $section);
  return undef if ($stanza_line_no == -1);

  $option_line_no = &gst_parse_printcap_find_option ($buf, $stanza_line_no + 1, $var);
  return undef if ($option_line_no == -1);

  ($key, $value) = &gst_parse_printcap_parse_option ($$buf [$option_line_no]);
  return $value;
}

# High-level API.
sub gst_parse_printcap_bool
{
  my ($file, $section, $var) = @_;
  my ($printtool_line_no, $stanza_line_no, $option_line_no);
  my ($buf);
  my ($key, $value);

  $buf = &gst_parse_printcap_buffer_load ($file);

  ($printtool_line_no, $stanza_line_no) = &gst_parse_printcap_find_stanza ($buf, 0, $section);
  return 0 if ($stanza_line_no == -1);

  $option_line_no = &gst_parse_printcap_find_option ($buf, $stanza_line_no + 1, $var);
  return 0 if ($option_line_no == -1);

  return 1;
}

# Debian interfaces(5) states that files starting with # are comments.
# Also, leading and trailing spaces are ignored.
sub gst_parse_interfaces_line_clean
{
  $_ = $_[0];
  
  chomp;
  s/^[ \t]+//;
  s/^\#.*//;
  s/[ \t]+$//;

  return $_;
}

# interfaces(5) also states that \ line continuation is possible.
sub gst_parse_interfaces_line_read
{
  my $fd = $_[0];
  my $l;

  $l = <$fd>;
  return -1 if ($l eq undef);
  
  $l = &gst_parse_interfaces_line_clean ($l);
  while ($l =~ /\\$/)
  {
    $l =~ s/\\$//;
    $l .= &gst_parse_interfaces_line_clean (scalar <$fd>);
  }

  return \$l;
}

# Read lines until a stanza, a line starting with $stanza_type is found.
# Return ref to an array with the stanza params split.
sub gst_parse_interfaces_get_next_stanza
{
  my ($fd, $stanza_type) = @_;
  my $line;

  while (($line = &gst_parse_interfaces_line_read ($fd)) != -1)
  {
    $_ = $$line;
    if (/^$stanza_type[ \t]+[^ \t]/)
    {
      s/^$stanza_type[ \t]+//;
      return [ split ("[ \t]+", $_) ];
    }
  }

  return -1;
}

# Read lines until a line not recognized as a stanza is
# found, and split in a "tuple" of key/value.
sub gst_parse_interfaces_get_next_option
{
  my $fd = $_[0];
  my $line;

  while (($line = &gst_parse_interfaces_line_read ($fd)) != -1)
  {
    $_ = $$line;
    next if /^$/;
    
    return [ split ("[ \t]+", $_, 2) ] if (!/^iface[ \t]/);
    return -1;
  }

  return -1;
}

# Get all stanzas from file. Return array.
sub gst_parse_interfaces_stanzas
{
  my ($file, $stanza_type) = @_;
  my ($fd, @res);

  $fd = &gst_file_open_read_from_names ($file);
  $res = undef;
  
  while (($_ = &gst_parse_interfaces_get_next_stanza ($fd, $stanza_type)) != -1)
  {
    push @res, $_;
  }

  &gst_file_close ($fd);

  return @res;
}

# Find stanza for $iface in $file, and return
# tuple for option with $key. Return -1 if unexisting.
sub gst_parse_interfaces_option_tuple
{
  my ($file, $iface, $key, $all) = @_;
  my ($fd, @res);

  $fd = &gst_file_open_read_from_names ($file);

  while (($stanza = &gst_parse_interfaces_get_next_stanza ($fd, "iface")) != -1)
  {
    if ($$stanza[0] eq $iface)
    {
      while (($tuple = &gst_parse_interfaces_get_next_option ($fd)) != -1)
      {
        if ($$tuple[0] =~ /$key/)
        {
          return $tuple if !$all;
          push @res, $tuple;
        }
      }

      return -1 if !$all;
    }
  }

  return @res if $all;
  return -1;
}

# Go get option $kw for $iface stanza. If found,
# return 1 (true), else, false.
sub gst_parse_interfaces_option_kw
{
  my ($file, $iface, $kw) = @_;
  my $tuple;

  &gst_report_enter ();
  &gst_report ("parse_ifaces_kw", $kw, $file);
  $tuple = &gst_parse_interfaces_option_tuple ($file, $iface, $kw);
  &gst_report_leave ();

  if ($tuple != -1)
  {
    &gst_report ("parse_ifaces_kw_strange", $iface, $file) if ($$tuple[1] ne "");

    return 1;
  }

  return 0;
}

# For such keywords as noauto, whose existence means
# a false value.
sub gst_parse_interfaces_option_kw_not
{
  my ($file, $iface, $kw) = @_;
  
  return &gst_parse_interfaces_option_kw ($file, $iface, $kw)? 0 : 1;
}

# Go get option $key for $iface in $file and return value.
sub gst_parse_interfaces_option_str
{
  my ($file, $iface, $key) = @_;
  my $tuple;

  &gst_report_enter ();
  &gst_report ("parse_ifaces_str", $kw, $file);
  $tuple = &gst_parse_interfaces_option_tuple ($file, $iface, $key);
  &gst_report_leave ();

  if ($tuple != -1)
  {
    return $$tuple[1];
  }

  return undef;
}


# Implementing pump(8) pump.conf file format parser.
# May be useful for dhcpd too.
sub gst_parse_pump_get_next_option
{
  my ($fd) = @_;
  my $line;

  while (($line = &gst_parse_interfaces_line_read ($fd)) != -1)
  {
    $line = $$line;
    if ($line ne "")
    {
      return [ split ("[ \t]+", $line, 2) ];
    }
  }

  return -1;
}

sub gst_parse_pump_get_device
{
  my ($fd, $iface) = @_;
  my ($opt);
  
  while (($opt = &gst_parse_pump_get_next_option ($fd)) != -1)
  {
    if ($$opt[0] eq "device")
    {
      $$opt[1] =~ s/[ \t]*\{//;
      return 1 if $$opt[1] eq $iface;
    }
  }

  return 0;
}

sub gst_parse_pump_get_iface_option_ref
{
  my ($file, $iface, $key) = @_;
  my ($fd, $opt, $ret);

  $fd = &gst_file_open_read_from_names ($file);

  if (&gst_parse_pump_get_device ($fd, $iface))
  {
    while (($opt = &gst_parse_pump_get_next_option ($fd)) != -1)
    {
      if ($$opt[0] eq $key)
      {
        $ret = &gst_parse_shell_unescape ($$opt[1]);
        return \$ret;
      }
      
      return -1 if ($$opt[0] eq "}");
    }
  }

  return -1;
}

sub gst_parse_pump_get_iface_kw
{
  my ($file, $iface, $key) = @_;
  my ($ret);

  return 1 if &gst_parse_pump_get_iface_option_ref ($file, $iface, $key) != -1;
  return 0;
}

sub gst_parse_pump_get_iface_kw_not
{
  my ($file, $iface, $key) = @_;

  return 0 if &gst_parse_pump_get_iface_option_ref ($file, $iface, $key) != -1;
  return 1;
}

# Read a variable out of an XML document. The varpath is the '/'-separated path to the
# XML tag. If the name of a property is passed, that property of the leaf tag is read,
# otherwise the tag's PCDATA.
sub gst_parse_xml
{
  my ($file, $varpath, $property) = @_;
  my ($model, $branch);

  ($model) = &gst_xml_model_scan ($file);
  $branch  = &gst_xml_model_find ($model, $varpath);

  if ($branch)
  {
    return &gst_xml_model_get_attribute ($branch, $property) if $property ne "";
    return &gst_xml_model_get_pcdata ($branch);
  }

  return undef;
}

sub gst_parse_xml_child_names
{
  my ($file, $varpath) = @_;
  my ($model, $branch, @children);

  ($model) = &gst_xml_model_scan ($file);
  $branch  = &gst_xml_model_find ($model, $varpath);

  if (!$branch) { return @children; }

  my @list = @$branch;
  shift @list;  # Attributes

  while (@list)
  {
    if ($list [0] ne "__unparsed__" && $list [0] ne "0")
    {
      push @children, shift @list;
    }
    else
    {
      shift @list;
    }

    shift @list;
  }

  return @children;
}

sub gst_parse_alchemist
{
  my ($file, $varpath) = @_;

  $varpath = "/adm_context/datatree/" . $varpath;
  return &gst_parse_xml ($file, $varpath, "VALUE");
}

sub gst_parse_alchemist_print
{
  my ($file, $printer, $varpath) = @_;

  $varpath = "printconf/print_queues/" . $printer . "/" . $varpath;
  return &gst_parse_alchemist ($file, $varpath);
}

sub gst_parse_alchemist_print_option
{
  my ($file, $printer, $name) = @_;
  my ($varpath, $model, $branch, $fd, $options, $option);

  ($model) = &gst_xml_model_scan ($file);
  $branch  = &gst_xml_model_find ($model, "/adm_context/datatree/printconf/print_queues/" . $printer .
                                  "/filter_data/foomatic_defaults");

  return undef if (!$branch);

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

  return undef if (!$option);

  my $node = &gst_xml_model_find ($option, "default");
  return undef if (!$node);

  return &gst_xml_model_get_attribute ($node, "VALUE");
}

# extracts hostname from a fully qualified hostname
# contained in a file
sub gst_parse_fq_hostname
{
  my ($file) = @_;
  my ($ret);

  $ret = &gst_parse_line_first ($file);
  $ret =~ s/\..*//; #remove domain

  return $ret;
}

# extracts domain from a fully qualified hostname
# contained in a file
sub gst_parse_fq_domain
{
  my ($file) = @_;
  my ($ret);

  $ret = &gst_parse_line_first ($file);
  $ret =~ s/^[^\.]*\.//;

  return $ret;
}

sub gst_parse_rcinet1conf
{
  my ($file, $iface, $kw) = @_;
  my ($line);

  $iface =~ s/eth//;

  #we must double escape those []
  $line = "$kw\\[$iface\\]";

  return &gst_parse_sh ($file, $line);
}

sub gst_parse_rcinet1conf_bool
{
  my ($file, $iface, $kw) = @_;
  my ($ret);

  $ret = &gst_parse_rcinet1conf ($file, $iface, $kw);
  
  return undef if ($ret eq undef);
  return (&gst_util_read_boolean ($ret)? 1: 0);
}

sub gst_parse_wireless_opts
{
  my ($file, $iface, $proc, $kw) = @_;
  my $ifaces = &$proc ();
  my $found = 0;
  my $search = 1;
  my $val = "";
  my $fd;
  
  foreach $i (@$ifaces)
  {
    $found = 1 if ($iface eq $i);
  }

  return undef if (!$found);

  $fd = &gst_file_open_read_from_names ($file);
  while (<$fd>)
  {
    $line = $_;

    if ($line =~ /^case/)
    {
      # we don't want to search inside the case
      $search = 0;
    }
    elsif ($line =~ /^esac/)
    {
      # continue searching
      $search = 1;
    }
    elsif (($line =~ /^[ \t]*$kw/ ) && ($search))
    {
      $line =~ s/.*=//;

      if ($line =~ /"(.*)"/)
      {
        $line = $1;
      }

      $val = $line;
    }
  }

  &gst_file_close ($fd);
  return $val;
}

# function for parsing /etc/start_if.$iface files in FreeBSD
sub gst_parse_startif
{
  my ($file, $regex) = @_;
  my ($fd, $line, $val);

  $fd  = &gst_file_open_read_from_names ($file);
  $val = undef;

  return undef if ($fd eq undef);

  while (<$fd>)
  {
    chomp;

    # ignore comments
    next if (/^\#/);
                 
    if (/$regex/)
    {
      $val = $1;
    }
  }

  # remove double quote
  if ($val =~ /\"(.*)\"/)
  {
    $val = $1;
  }

  return $val;
}

# functions for parsing /etc/ppp/ppp.conf sections in FreeBSD
sub gst_parse_pppconf_find_next_stanza
{
  my ($buff, $line_no) = @_;

  $line_no = 0 if ($line_no eq undef);

  while ($$buff[$line_no] ne undef)
  {
    if ($$buff[$line_no] !~ /^[\#\n]/)
    {
      return $line_no if ($$buff[$line_no] =~ /^[^ \t]+/);
    }

    $line_no++;
  }

  return -1;
}

sub gst_parse_pppconf_find_stanza
{
  my ($buff, $section) = @_;
  my ($line_no) = 0;
  
  while (($line_no = &gst_parse_pppconf_find_next_stanza ($buff, $line_no)) != -1)
  {
    return $line_no if ($$buff[$line_no] =~ /^$section\:/);
    $line_no++;
  }

  return -1;
}

sub gst_parse_pppconf_common
{
  my ($file, $section, $key) = @_;
  my ($fd, $val);

  $fd = &gst_file_open_read_from_names ($file);
  return undef if ($fd eq undef);

  $val = undef;

  # First of all, we must find the line where the section begins
  while (<$fd>)
  {
    chomp;
    last if (/^$section\:[ \t]*/);
  }

  while (<$fd>)
  {
    chomp;

    # read until the next section arrives
    last if (/^[^ \t]/);

    next if (/^\#/);

    if (/^[ \t]+(add|set|enable|disable)[ \t]+$key/)
    {
      $val = $_;
      last;
    }
  }

  # this is done because commands can be multiline
  while (<$fd>)
  {
    last if (/^[^ \t]/);
    last if ($val !~ /\\$/);

    s/^[ \t]*/ /;
    $val =~ s/\\$//;
    $val .= $_;
  }

  &gst_file_close ($fd);

  if ($val eq undef)
  {
    return undef if ($section eq "default");
    return &gst_parse_pppconf_common ($file, "default", $key);
  }
  else
  {
    $val =~ s/\#[^\#]*$//;
    $val =~ s/[ \t]*$//;
    $val =~ s/^[ \t]*//;
    return $val;
  }
}

sub gst_parse_pppconf
{
  my ($file, $section, $key) = @_;
  my ($val);

  $val = &gst_parse_pppconf_common ($file, $section, $key);

  if ($val =~ /$key[ \t]+(.+)/)
  {
    return $1;
  }
}

sub gst_parse_pppconf_bool
{
  my ($file, $section, $key) = @_;
  my ($val);

  $val = &gst_parse_pppconf_common ($file, $section, $key);

  return 1 if ($val ne undef);
  return 0;
}

sub gst_parse_pppconf_re
{
  my ($file, $section, $key, $re) = @_;
  my ($val);

  $val = &gst_parse_pppconf_common ($file, $section, $key);

  if ($val =~ /$re/i)
  {
    return $1;
  }
}

sub gst_parse_confd_net
{
  my ($file, $key) = @_;
  my ($str, $contents, $i);

  $contents = &gst_file_buffer_load ($file);

  for ($i = 0; $i <= scalar (@$contents); $i++)
  {
    # search for key
    if ($$contents[$i] =~ /^$key[ \t]*=[ \t]*\(/)
    {
      # contents can be multiline,
      # just get the first value
      do {
        $$contents[$i] =~ /\"([^\"]*)\"/;
        $str = $1;
        $i++;
      } while (!$str);
    }
  }

  return $str;
}

sub gst_parse_confd_net_re
{
  my ($file, $key, $re) = @_;
  my ($str);

  $str = &gst_parse_confd_net ($file, $key);

  if ($str =~ /$re/i)
  {
    return $1;
  }
}
