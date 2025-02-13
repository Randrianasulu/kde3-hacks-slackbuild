#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# Functions for file manipulation. Find, open, read, write, backup, etc.
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

use File::Path;
use File::Copy;
use File::Temp;
use Carp;

$SCRIPTSDIR = "/usr/share/apps/knetworkconf/backends";
$FILESDIR = "@filesdir@";
if ($SCRIPTSDIR =~ /^@scriptsdir[@]/)
{
    $FILESDIR = "files";
    $SCRIPTSDIR = ".";
    $DOTIN = ".in";
}

require "$SCRIPTSDIR/general.pl$DOTIN";
require "$SCRIPTSDIR/report.pl$DOTIN";


$GST_FILE_READ  = 1;
$GST_FILE_WRITE = 2;


# --- File operations --- #


sub gst_file_get_base_path
{
  my $path = "/var/cache/setup-tool-backends";
  chmod (0755, $path);
  return $path;
}


sub gst_file_get_tmp_path
{
  return (&gst_file_get_base_path () . "/tmp");
}


sub gst_file_get_backup_path
{
  return (&gst_file_get_base_path () . "/backup");
}


sub gst_file_get_debug_path
{
  return (&gst_file_get_base_path (). "/debug");
}


sub gst_file_get_data_path
{
  my $path = &gst_file_get_base_path (). "/data";
  chmod (0755, $path);
  return $path;
}


# Give a command, and it will put in C locale, some sane PATH values and find
# the program to run in the path. Redirects stderr to null.
sub get_cmd_path
{
  my ($cmd) = @_;
  my ($tool_name, @argline, $command, $tool_path);
  
  ($tool_name, @argline) = split("[ \t]+", $cmd);

  $tool_path = &gst_file_locate_tool ($tool_name);
  return -1 if ($tool_path eq "");


  $command = "$tool_path @argline";
  $command =~ s/\"/\\\"/g;

  return $command;
}

sub gst_file_get_cmd_path
{
   my ($cmd) = @_;

   my $command = &get_cmd_path ($cmd);
   return ("LC_ALL=C PATH=\$PATH:/sbin:/usr/sbin $command 2> /dev/null");
}

# necessary for some programs that output info through stderr
sub gst_file_get_cmd_path_with_stderr
{
   my ($cmd) = @_;

   my $command = &get_cmd_path ($cmd);
   return ("LC_ALL=C PATH=\$PATH:/sbin:/usr/sbin $command 2>&1");
}


sub gst_file_create_path
{
  my ($path, $perms) = @_;
  $prems = $perms || 0770;
  my @pelem;
  
  $path =~ tr/\///s;
  @pelem = split(/\//, $path); # 'a/b/c/d/' -> 'a', 'b', 'c', 'd', ''

  for ($path = ""; @pelem; shift @pelem)
  {
    $path = "$path$pelem[0]";
    mkdir($path, $perms);
    $path = "$path/";
  }

  &gst_report_enter ();
  &gst_report ("file_create_path", $_[0]);
  &gst_report_leave ();
}


sub gst_file_create_path_for_file
{
  my ($path, $perms) = @_;
  $prems = $perms || 0770;
  my @pelem;
  
  $path =~ tr/\///s;
  @pelem = split(/\//, $path); # 'a/b/c/d/' -> 'a', 'b', 'c', 'd', ''
    
  for ($path = ""; @pelem; shift @pelem)
  {
    if ($pelem[1] ne "")
    {
      $path = "$path$pelem[0]";
      mkdir($path, $perms);
      $path = "$path/";
    }
  }

  &gst_report_enter ();
  &gst_report ("file_create_path", $_[0]);
  &gst_report_leave ();
}


$gst_file_backup_dir_rotation_was_made = 0;

# If this is the first backup created by this tool on this invocation,
# rotate the backup directories and create a new, empty one.
sub gst_file_backup_rotate_dirs
{
  my $backup_tool_dir = $_[0];
  
  &gst_report_enter ();
  
  if (!$gst_file_backup_dir_rotation_was_made)
  {
    my $i;

    $gst_file_backup_dir_rotation_was_made = 1;
    if (-e "$backup_tool_dir/9")
    {
      if (-s "$backup_tool_dir/9")
      {
        unlink ("$backup_tool_dir/9");
      }
      else
      {
        &gst_file_rmtree ("$backup_tool_dir/9");
      }
    }

    for ($i = 8; $i; $i--)
    {
      if (stat ("$backup_tool_dir/$i"))
      {
        move ("$backup_tool_dir/$i", "$backup_tool_dir/" . ($i+1));
      }
    }

    if (!stat ("$backup_tool_dir/First"))
    {
      &gst_file_create_path ("$backup_tool_dir/First");
      &gst_file_run ("ln -s First $backup_tool_dir/1");
    }
    else
    {
      &gst_file_create_path_for_file ("$backup_tool_dir/1/");
    }

    &gst_report ("file_backup_rotate", $backup_tool_dir);
  }
  
  &gst_report_enter ();
}

sub gst_file_backup
{
  my $backup_file = $_[0];
  my $backup_tool_dir;

  &gst_report_enter ();
  
  $backup_tool_dir = &gst_file_get_backup_path () . "/$gst_name/";

  &gst_file_backup_rotate_dirs ($backup_tool_dir);
  
  # If the file hasn't already been backed up on this invocation, copy the
  # file to the backup directory.

  if (!stat ("$backup_tool_dir/1/$backup_file"))
  {
    &gst_file_create_path_for_file ("$backup_tool_dir/1/$backup_file");
    copy ($backup_file, "$backup_tool_dir/1/$backup_file");
    &gst_report ("file_backup_success", $backup_tool_dir);
  }
  
  &gst_report_leave ();
}

# Return 1/0 depending on file existance.
sub gst_file_exists
{
  my ($file) = @_;

  return (-f "$gst_prefix/$file")? 1: 0;
}

sub gst_file_open_read_from_names
{
  local *FILE;
  my $fname = "";

  &gst_report_enter ();
  
  foreach $name (@_)
  {
    if (open (FILE, "$gst_prefix/$name"))
    {
      $fname = $name;
      last;
    }
  }
  
  (my $fullname = "$gst_prefix/$fname") =~ tr/\//\//s;  # '//' -> '/'	

  if ($fname eq "") 
  { 
    &gst_report ("file_open_read_failed", "@_");
    return undef;
  }

  &gst_report ("file_open_read_success", $fullname);
  &gst_report_leave ();

  return *FILE;
}


sub gst_file_open_write_from_names
{
  local *FILE;
  my $name;
  my $fullname;

  &gst_report_enter ();
    
  # Find out where it lives.
    
  foreach $elem (@_) { if (stat($elem) ne "") { $name = $elem; last; } }
    
  if ($name eq "")
  {
    $name = $_[0];
    (my $fullname = "$gst_prefix/$name") =~ tr/\//\//s;
    &gst_report ("file_open_write_create", "@_", $fullname);
  }
  else
  {
    (my $fullname = "$gst_prefix/$name") =~ tr/\//\//s;
    &gst_report ("file_open_write_success", $fullname);
  }
    
  ($name = "$gst_prefix/$name") =~ tr/\//\//s;  # '//' -> '/' 
  &gst_file_create_path_for_file ($name);
    
  # Make a backup if the file already exists - if the user specified a prefix,
  # it might not.
    
  if (stat ($name))
  {
    &gst_file_backup ($name);
  }

  &gst_report_leave ();
  
  # Truncate and return filehandle.

  if (!open (FILE, ">$name"))
  {
    &gst_report ("file_open_write_failed",  $name);
    return undef;
  }

  return *FILE;
}

sub gst_file_open_filter_write_from_names
{
  local *INFILE;
  local *OUTFILE;
  my ($filename, $name, $elem);

  &gst_report_enter ();

  # Find out where it lives.

  foreach $coin (@_)
  {
    if (-e $coin) { $name = $coin; last; }
  }

  if (! -e $name)
  {
    # If we couldn't locate the file, and have no prefix, give up.

    # If we have a prefix, but couldn't locate the file relative to '/',
    # take the first name in the array and let that be created in $prefix.

    if ($prefix eq "")
    {
      &gst_report ("file_open_filter_failed", "@_");
      return(0, 0);
    }
    else
    {
      $name = $_[0];
      (my $fullname = "$gst_prefix/$name") =~ tr/\//\//s;
      &gst_report ("file_open_filter_create", "@_", $fullname);
    }
  }
  else
  {
    (my $fullname = "$gst_prefix/$name") =~ tr/\//\//s;
    &gst_report ("file_open_filter_success", $name, $fullname);
  }

  ($filename) = $name =~ /.*\/(.+)$/;
  ($name = "$gst_prefix/$name") =~ tr/\//\//s;  # '//' -> '/' 
  &gst_file_create_path_for_file ($name);

  # Make a backup if the file already exists - if the user specified a prefix,
  # it might not.

  if (-e $name)
  {
    &gst_file_backup ($name);
  }

  # Return filehandles. Make a copy to use as filter input. It might be
  # invalid (no source file), in which case the caller should just write to
  # OUTFILE without bothering with INFILE filtering.

  my $tmp_path = &gst_file_get_tmp_path ();

  &gst_file_create_path ("$tmp_path");
  unlink ("$tmp_path/$gst_name-$filename");
  copy ($name, "$tmp_path/$gst_name-$filename");

  open (INFILE, "$tmp_path/$gst_name-$filename");

  if (!open (OUTFILE, ">$name"))
  {
    &gst_report ("file_open_filter_failed", $name);
    return;
  }
    
  &gst_report_leave ();

  return (*INFILE, *OUTFILE);
}


sub gst_file_open_write_compressed
{
  local *FILE;
  my ($name, $fullname, $gzip);

  $gzip = &gst_file_locate_tool ("gzip");
  return undef if (!$gzip);

  &gst_report_enter ();
    
  # Find out where it lives.
    
  foreach $elem (@_) { if (stat($elem) ne "") { $name = $elem; last; } }
    
  if ($name eq "")
  {
    $name = $_[0];
    (my $fullname = "$gst_prefix/$name") =~ tr/\//\//s;
    &gst_report ("file_open_write_create", "@_", $fullname);
  }
  else
  {
    (my $fullname = "$gst_prefix/$name") =~ tr/\//\//s;
    &gst_report ("file_open_write_success", $fullname);
  }
    
  ($name = "$gst_prefix/$name") =~ tr/\//\//s;  # '//' -> '/' 
  &gst_file_create_path_for_file ($name);
    
  # Make a backup if the file already exists - if the user specified a prefix,
  # it might not.
    
  if (stat ($name))
  {
    &gst_file_backup ($name);
  }

  &gst_report_leave ();
  
  # Truncate and return filehandle.

  if (!open (FILE, "| $gzip -c > $name"))
  {
    &gst_report ("file_open_write_failed",  $name);
    return;
  }

  return *FILE;
}


sub gst_file_run_pipe
{
  my ($cmd, $mode_mask, $stderr) = @_;
  my ($command);
  local *PIPE;

  $mode_mask = $GST_FILE_READ if $mode_mask eq undef;

  &gst_report_enter ();
  
  if ($stderr)
  {
    $command = &gst_file_get_cmd_path_with_stderr ($cmd);
  }
  else
  {
    $command = &gst_file_get_cmd_path ($cmd);
  }
  
  if ($command == -1)
  {
    &gst_report ("file_run_pipe_failed", $command);
    &gst_report_leave ();
    return undef;
  }

  $command .= " |"        if $mode_mask & $GST_FILE_READ;
  $command = "| $command > /dev/null" if $mode_mask & $GST_FILE_WRITE;

  open PIPE, $command;
  &gst_report ("file_run_pipe_success", $command);
  &gst_report_leave ();
  return *PIPE;
}


sub gst_file_run_pipe_read
{
  my ($cmd) = @_;

  return &gst_file_run_pipe ($cmd, $GST_FILE_READ);
}

sub gst_file_run_pipe_read_with_stderr
{
   my ($cmd) = @_;

   return &gst_file_run_pipe ($cmd, $GST_FILE_READ, 1);
}

sub gst_file_run_pipe_write
{
  my ($cmd) = @_;

  return &gst_file_run_pipe ($cmd, $GST_FILE_WRITE);
}


sub gst_file_run_backtick
{
  my ($cmd, $stderr) = @_;
  my ($fd, $res);

  if ($stderr)
  {
    $fd = &gst_file_run_pipe_read_with_stderr ($cmd);
  }
  else
  {
    $fd = &gst_file_run_pipe_read ($cmd);
  }

  $res = join ('', <$fd>);
  &gst_file_close ($fd);

  return $res;
}


sub gst_file_close
{
  my ($fd) = @_;

  close $fd if (ref \$fd eq "GLOB");
}


sub gst_file_remove
{
  my ($name) = @_;

  &gst_report_enter ();
  &gst_report ("file_remove", $name);

  if (stat ($name))
  {
    &gst_file_backup ($name);
  }

  unlink $name;
  &gst_report_leave ();
}

sub gst_file_rmtree
{
  my($roots, $verbose, $safe) = @_;
  my(@files);
  my($count) = 0;
  $verbose ||= 0;
  $safe ||= 0;

  if ( defined($roots) && length($roots) ) {
    $roots = [$roots] unless ref $roots;
  }
  else {
    carp "No root path(s) specified\n";
    return 0;
  }

  my($root);
  foreach $root (@{$roots}) {
    $root =~ s#/\z##;
    (undef, undef, my $rp) = lstat $root or next;
    $rp &= 07777;	# don't forget setuid, setgid, sticky bits
    
    if ( -d $root ) { # $root used to be _, which is a bug.
                      # this is why we are replicating this function.
      
	    # notabene: 0777 is for making readable in the first place,
	    # it's also intended to change it to writable in case we have
	    # to recurse in which case we are better than rm -rf for 
	    # subtrees with strange permissions
	    chmod(0777, ($Is_VMS ? VMS::Filespec::fileify($root) : $root))
          or carp "Can't make directory $root read+writeable: $!"
              unless $safe;

      local *DIR;
	    if (opendir DIR, $root) {
        @files = readdir DIR;
        closedir DIR;
	    }
	    else {
        carp "Can't read $root: $!";
        @files = ();
	    }

	    # Deleting large numbers of files from VMS Files-11 filesystems
	    # is faster if done in reverse ASCIIbetical order 
	    @files = reverse @files if $Is_VMS;
	    ($root = VMS::Filespec::unixify($root)) =~ s#\.dir\z## if $Is_VMS;
	    @files = map("$root/$_", grep $_!~/^\.{1,2}\z/s,@files);
	    $count += &gst_file_rmtree(\@files,$verbose,$safe);
	    if ($safe &&
          ($Is_VMS ? !&VMS::Filespec::candelete($root) : !-w $root)) {
        print "skipped $root\n" if $verbose;
        next;
	    }
	    chmod 0777, $root
          or carp "Can't make directory $root writeable: $!"
              if $force_writeable;
	    print "rmdir $root\n" if $verbose;
	    if (rmdir $root) {
        ++$count;
	    }
	    else {
        carp "Can't remove directory $root: $!";
        chmod($rp, ($Is_VMS ? VMS::Filespec::fileify($root) : $root))
            or carp("and can't restore permissions to "
                    . sprintf("0%o",$rp) . "\n");
	    }
    }
    else { 
	    if ($safe &&
          ($Is_VMS ? !&VMS::Filespec::candelete($root)
           : !(-l $root || -w $root)))
	    {
        print "skipped $root\n" if $verbose;
        next;
	    }
	    chmod 0666, $root
          or carp "Can't make file $root writeable: $!"
              if $force_writeable;
	    print "unlink $root\n" if $verbose;
	    # delete all versions under VMS
	    for (;;) {
        unless (unlink $root) {
          carp "Can't unlink file $root: $!";
          if ($force_writeable) {
            chmod $rp, $root
                or carp("and can't restore permissions to "
                        . sprintf("0%o",$rp) . "\n");
          }
          last;
        }
        ++$count;
        last unless $Is_VMS && lstat $root;
	    }
    }
  }

  $count;
}

# --- Buffer operations --- #


# Open $file and put it into @buffer, for in-line editting.
# \@buffer on success, undef on error.

sub gst_file_buffer_load
{
  my ($file) = @_;
  my @buffer;
  my $fd;

  &gst_report_enter ();
  &gst_report ("file_buffer_load", $file);

  $fd = &gst_file_open_read_from_names ($file);
  return [] unless $fd;

  @buffer = (<$fd>);

  &gst_report_leave ();

  return \@buffer;
}

# Same with an already open fd.
sub gst_file_buffer_load_fd
{
  my ($fd) = @_;
  my (@buffer);
  
  &gst_report_enter ();
  &gst_report ("file_buffer_load", $file);

  @buffer = (<$fd>);

  &gst_report_leave ();

  return \@buffer;
}

# Take a $buffer and save it in $file. -1 is error, 0 success.

sub gst_file_buffer_save
{
  my ($buffer, $file) = @_;
  my ($fd, $i);

  &gst_report_enter ();
  &gst_report ("file_buffer_save", $file);

  foreach $i (@$buffer)
  {
    &gst_debug_print_string ("|" . $i);
  }

  $fd = &gst_file_open_write_from_names ($file);
  return -1 if !$fd;

  if (@$buffer < 1)
  {
    # We want to write single line.
    # Print only if $buffer is NOT a reference (it'll print ARRAY(0x412493) for example).
    print $fd $buffer if (!ref ($buffer));
  }

  else
  {
    # Let's print array
    
    foreach $i (@$buffer)
    {
      print $fd $i;
    }
  }

  &gst_file_close ($fd);

  &gst_report_leave ();
  
  return 0;
}


# Erase all empty string elements from the $buffer.

sub gst_file_buffer_clean
{
  my $buffer = $_[0];
  my $i;

  for ($i = 0; $i <= $#$buffer; $i++)
  {
    splice (@$buffer, $i, 1) if $$buffer[$i] eq "";
  }
}


sub gst_file_buffer_join_lines
{
  my $buffer = $_[0];
  my $i;

  for ($i = 0; $i <= $#$buffer; $i++)
  {
    while ($$buffer[$i] =~ /\\$/)
    {
      chomp $$buffer[$i];
      chop $$buffer[$i];
      $$buffer[$i] .= $$buffer[$i + 1];
      splice (@$buffer, $i + 1, 1);
    }
  }
}


# --- Command-line utilities --- #


# &gst_file_run (<command line>)
#
# Assumes the first word on the command line is the command-line utility
# to run, and tries to locate it, replacing it with its full path. The path
# is cached in a hash, to avoid searching for it repeatedly. Output
# redirection is appended, to make the utility perfectly silent. The
# preprocessed command line is run, and its exit value is returned.
#
# Example: "mkswap /dev/hda3" -> 'PATH=$PATH:/sbin:/usr/sbin /sbin/mkswap /dev/hda3 2>/dev/null >/dev/null'.

sub gst_file_run
{
  my ($cmd, $background) = @_;
  my ($command, $tool_name, $tool_path, @argline);

  &gst_report_enter ();

  $command = &gst_file_get_cmd_path ($cmd);
  return -1 if $command == -1;
  $command .= " > /dev/null";
  $command .= " &" if $background;

  &gst_report ("file_run", $command);
  &gst_report_leave ();

  # As documented in perlfunc, divide by 256.
  return (system ($command) / 256);
}

sub gst_file_run_bg
{
  my ($cmd) = @_;

  return &gst_file_run ($cmd, 1);
}

# &gst_file_locate_tool
#
# Tries to locate a command-line utility from a set of built-in paths
# and a set of user paths (found in the environment). The path (or a negative
# entry) is cached in a hash, to avoid searching for it repeatedly.

@gst_builtin_paths = ( "/sbin", "/usr/sbin", "/usr/local/sbin",
                       "/bin", "/usr/bin", "/usr/local/bin" );

%gst_tool_paths = ();

sub gst_file_locate_tool
{
  my ($tool) = @_;
  my $found = "";
  my @user_paths;

  # We don't search absolute paths. Arturo.
  if ($tool =~ /^\//)
  {
    if (! (-x $tool))
    {
      &gst_report ("file_locate_tool_failed", $tool);
      return "";
    }
    
    return $tool;
  }

  &gst_report_enter ();
  
  $found = $gst_tool_paths{$tool};
  if ($found eq "0")
  {
    # Negative cache hit. At this point, the failure has already been reported
    # once.
    return "";
  }

  if ($found eq "")
  {
    # Nothing found in cache. Look for real.

    # Extract user paths to try.

    @user_paths = ($ENV{PATH} =~ /([^:]+):/mg);

    # Try user paths.

    foreach $path (@user_paths)
    {
      if (-x "$path/$tool" || -u "$path/$tool") { $found = "$path/$tool"; last; }
    }

    if (!$found)
    {
      # Try builtin paths.
      foreach $path (@gst_builtin_paths)
      {
        if (-x "$path/$tool" || -u "$path/$tool") { $found = "$path/$tool"; last; }
      }
    }

    # Report success/failure and update cache.

    if ($found)
    {
      $gst_tool_paths{$tool} = $found;
      &gst_report ("file_locate_tool_success", $tool);
    }
    else
    {
      $gst_tool_paths{$tool} = "0";
      &gst_report ("file_locate_tool_failed", $tool);
    }
  }
  
  &gst_report_leave ();
  
  return ($found);
}

sub gst_file_tool_installed
{
  my ($tool) = @_;
  
  $tool = &gst_file_locate_tool ($tool);
  return 0 if $tool eq "";
  return 1;
}

sub gst_file_copy
{
  my ($orig, $dest) = @_;

  return if (!gst_file_exists ($orig));
  copy ("$gst_prefix/$orig", "$gst_prefix/$dest");
}

sub gst_file_get_temp_name
{
  my ($prefix) = @_;

  return mktemp ($prefix);
}

sub gst_file_copy_from_stock
{
  my ($orig, $dest) = @_;

  if (!copy ("$FILESDIR/$orig", $dest))
  {
    &gst_report ("file_copy_failed", "$FILESDIR/$orig", $dest);
    return -1;
  }

  return 0;
}

1;
