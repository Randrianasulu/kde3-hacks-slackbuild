#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# XML printing, scanning and parsing.
#
# Copyright (C) 2000-2001 Ximian, Inc.
#
# Authors: Hans Petter Jansson <hpj@ximian.com>
#          Arturo Espinosa <arturo@ximian.com>
#          Kenneth Christiansen <kenneth@gnu.org>
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
require "$SCRIPTSDIR/general.pl$DOTIN";
require "$SCRIPTSDIR/debug.pl$DOTIN";

use Text::ParseWords;

$has_encode = 0;
if (eval "require Encode") {
  Encode->import ();
  $has_encode = 1;
}

# --- XML print formatting  --- #


# &gst_xml_enter: Call after entering a block. Increases indent level.
# &gst_xml_leave: Call before leaving a block. Decreases indent level.
# &gst_xml_print_indent: Call before printing a line. Indents to current level. 
# &gst_xml_print_vspace: Ensures there is a vertical space of one and only one line.
# &gst_xml_print: Indent, then print all arguments. Just for sugar.


my $gst_indent_level = 0;
my $gst_have_vspace = 0;

my @gst_xml_stack;

sub gst_xml_print_comment # (comment text)
{
  my ($comment) = @_;

  &gst_xml_print_line ("<!-- $comment -->") if $comment;
}


sub gst_xml_print_begin
{
  my ($name) = @_;

  $name = $gst_name if !$name;

  &gst_xml_print_string ("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n");
  &gst_xml_print_string ("<!DOCTYPE $name []>\n\n");
  &gst_xml_print_string ("<$name>\n");

  &gst_xml_enter ();

  &gst_xml_print_vspace ();
}


# The frontend expects exactly this string. Don not alter.
sub gst_xml_print_request_end
{
  print "\n<!-- GST: end of request -->\n";
}


sub gst_xml_print_end
{
  my ($name) = @_;

  $name = $gst_name if !$name;
  
  &gst_xml_leave ();

  &gst_xml_print_vspace ();
  &gst_xml_print_string ("</$name>\n");
}


sub gst_xml_enter
{
  $gst_indent_level += 2;
}


sub gst_xml_leave
{
  $gst_indent_level -= 2;
}


sub gst_xml_print_string
{
  if ($has_encode)
  {
    if (&decode_utf8 ($_[0]) eq undef)
    {
      # we first decode the string, if it's not
      # utf-8 (returns undef), then encode it
      print "" . &encode_utf8 ($_[0]);
      return;
    }
  }

  # It could not encode the string, write it as is
  print $_[0];
}

sub gst_xml_format_indent
{
  $gst_have_vspace = 0;
  return " " x $gst_indent_level;
}

sub gst_xml_print_indent
{
  &gst_xml_print_string(&gst_xml_format_indent ());
}

sub gst_xml_print_vspace
{
  if (not $gst_have_vspace)
  {
    &gst_xml_print_string ("\n");
    $gst_have_vspace = 1;
  }
}


sub gst_xml_print_line
{
  my $line;
  $line = join ("", @_);
  $line =~ tr/\n//d;

  &gst_xml_print_indent ();
  &gst_xml_print_string ($line . "\n");
}

sub gst_xml_format_pcdata # (name, pcdata)
{
  my ($name, $pcdata) = @_;
  return "<$name>$pcdata</$name>" if defined ($name) || defined ($pcdata);
}

sub gst_xml_print_pcdata # (name, pcdata)
{
  my ($name, $pcdata) = @_;
  my $line = &gst_xml_format_pcdata ($name, $pcdata);

  &gst_xml_print_line ($line) if $line;
}

sub gst_xml_format_state_tag
{
  my ($name, $state) = @_;
  my $boolean = &gst_print_boolean_truefalse ($state);

  return "<$name state='$boolean'/>";
}

sub gst_xml_print_state_tag
{
  my ($name, $state) = @_;
  my $state_tag = &gst_xml_format_state_tag ($name, $state);

  &gst_xml_print_line ($state_tag); 
}

# Pass a hash and the keys whose items are scalars. Print <key>val</key>.
sub gst_xml_print_scalars
{
  my ($h, @scalar_keys) = @_;
  my ($i, $val);

  @scalar_keys = sort @scalar_keys;

  while ($i = shift @scalar_keys)
  {
    $val = &gst_xml_quote ($$h{$i});
    &gst_xml_print_line ("<$i>$val</$i>\n") if exists $$h{$i};
  }
  
}

# Print the @$array using <$tag>val</$tag> foreach val in the array.
# Actually lets print_structure do that now. Just print sequentially
# the given elements, using as $tag as the surrounding tags.
sub gst_xml_print_array
{
  my ($array, $tag) = @_;
  my ($i, $val);

  return if (scalar @$array <= 0);

  &gst_xml_print_vspace ();
  foreach $i (@$array)
  {
    &gst_xml_print_structure ($i, $tag);
  }
}

# Pass a hash and the keys whose items are arrays. Print <key>val</key> foreach val
# in the array at hash{key}
sub gst_xml_print_arrays
{
  my ($h, @array_keys) = @_;
  my ($i, $j, $val);
  
  foreach $i (sort @array_keys)
  {
    &gst_xml_print_array ($$h{$i}, $i) if (exists $$h{$i})
  }
}

# Pass a hash, create a parent tag $tag and print <key>val</key> for every
# value pair in the hash. If structure refs are found, these are recursively
# printed with print_structure.
sub gst_xml_print_hash
{
  my ($hash, $tag) = @_;
  my ($j, $val);
  
  &gst_xml_print_vspace ();
  if (defined $tag) {
      &gst_xml_print_line ("<$tag>\n");
      &gst_xml_enter ();
  }
  
  foreach $j (sort keys (%$hash))
  {
    &gst_xml_print_structure ($$hash{$j}, $j);
  }

  if (defined $tag) {
      &gst_xml_leave ();
      &gst_xml_print_line ("</$tag>\n");
  }
}

# Call the corresponding function depending on the reference
# type of $x. If just a scalar, print <$tag>$x</$tag>.
sub gst_xml_print_structure
{
  my ($x, $tag) = @_;

  if (ref $x eq "ARRAY") { &gst_xml_print_array ($x, $tag); }
  elsif (ref $x eq "HASH")  { &gst_xml_print_hash  ($x, $tag); }
  else
  {
    &gst_xml_print_line ("<$tag>" . &gst_xml_quote ($x) . "</$tag>\n");
  }
}

# Treats hash as an array: doesn't use the keys as tags for its
# elements, but the given tag.
sub gst_xml_print_hash_hash
{
  my ($h, $tag) = @_;
  my $i;
  
  foreach $i (sort keys %$h)
  {
    &gst_xml_print_hash ($$h{$i}, $tag);
  } 
}


sub gst_xml_container_enter # (name)
{
  my ($container) = @_;

  ## gst_xml_stack is not my, as it is defined at top,
  ## so it is global
  push @gst_xml_stack, $container;

  &gst_xml_print_line ("<$container>");
  &gst_xml_enter();
}


sub gst_xml_container_leave
{
  ## checks if there is a start tag
  if ($#gst_xml_stack >= 0)
  { 
    my $current_container = pop @gst_xml_stack;

    &gst_xml_leave ();
    &gst_xml_print_line ("</$current_container>");
  }
}


sub gst_xml_print_container # (name, @strings)
{
  my ($name, @strings) = @_;

  if (@strings) {
    &gst_xml_container_enter ($name);
    foreach $tag (@strings) {
      &gst_xml_print_line ("$tag");
    }
    &gst_xml_container_leave ();
  }
} 

# --- XML printing from in-memory model --- #

sub gst_xml_model_print_attributes
{
  my ($tree) = @_;
  my ($attrs, $string);

  $attrs = @$tree [0];

  for $attr (keys %$attrs)
  {
    $string .= " " . $attr . "=\"" . $$attrs{$attr} . "\"";
  }

  return $string;
}

sub gst_xml_model_print_recurse
{
  my ($tree, $indent) = @_;
  my ($string);

  my @children = @$tree;
  shift @children;  # Attributes

  while (@children)
  {
    my $branch = $children [1];

    if ($children [0] eq "__unparsed__")
    {
      $string .= "<" . $children [1] . ">";
    }
    elsif ($children [0] eq "0")
    {
      $string .= $children [1];
    }
    elsif (@$branch == 1)  # Empty tag.
    {
      $string .= "<" . $children [0] . &gst_xml_model_print_attributes ($branch) . "/>";
    }
    else
    {
      $string .= "<" . $children [0] . &gst_xml_model_print_attributes ($branch) . ">";
      $string .= &gst_xml_model_print_recurse ($branch);
      $string .= "</" . $children [0] . ">";
    }

    shift @children;
    shift @children;
  }

#  if ($branch)
#  {
#    return &gst_xml_get_attribute ($branch, $property) if $property ne "";
#    return &gst_xml_get_pcdata ($branch);
#  }

  return $string;
}

sub gst_xml_model_print
{
  my ($tree) = @_;
  my ($string);

  $string = &gst_xml_model_print_recurse ($tree);
  chomp $string;
  $string .= "\n";

  return $string;
}

# --- XML scanning --- #


# This code tries to replace XML::Parser scanning from stdin in tree mode.

sub gst_xml_scan_make_kid_array
{
  my $line = shift;
  my (%hash, @sublist, @attr, @list);

  # Remove < and />
  $line =~ s/^[ \t]*<//;
  $line =~ s/[\/]?>[ \t]*$//;

  my @list = &quotewords ('[ \t]+', 1, $line);
  shift @list; # push tag name.

  foreach (@list)
  {
    my @tmp = split '[ \t]*=[ \t]*';
    if (scalar @tmp == 2 && $tmp[1] =~ s/[\"\']//g)
    {
      push @attr, @tmp;
    }
  }

  %hash = @attr;

  push (@sublist, \%hash);
  return \@sublist;
}


sub gst_xml_scan_recurse
{
  my ($gst_xml_scan_list, $list_arg) = @_;
  my @list;
  if ($list_arg ne undef) { @list = $$list_arg[0]; }
  
  while (@$gst_xml_scan_list)
  {
    $el = shift @$gst_xml_scan_list;

    if (($el eq "") || $el =~ /^\<[!?].*\>$/s) { next; }  # Empty strings, PI and DTD must go.
    if ($el =~ /^\<.*\/\>$/s)  # Empty.
    {
      $el =~ /^\<([a-zA-Z0-9_-]+).*\/\>$/s;
      push (@list, $1);
      push (@list, &gst_xml_scan_make_kid_array ($el));
    }
    elsif ($el =~ /^\<\/.*\>$/s)  # End.
    {
      last;
    }
    elsif ($el =~ /^\<.*\>$/s)  # Start.
    {
      $el =~ /^\<([a-zA-Z0-9_-]+).*\>$/s;
      push (@list, $1);
      $sublist = &gst_xml_scan_make_kid_array ($el);
      push (@list, &gst_xml_scan_recurse ($gst_xml_scan_list, $sublist));
      next;
    }
    elsif ($el ne "")  # PCDATA.
    {
      push (@list, 0);
      push (@list, "$el");
    }
  }

  return \@list;
}


sub gst_xml_read_file
{
  my ($file) = @_;
  my ($doc, $i);
  local *INPUT_FILE;
  
  open INPUT_FILE, $file;
  $doc .= $i while ($i = <INPUT_FILE>);
  close INPUT_FILE;

  return $doc;
}


sub gst_xml_read_compressed_file
{
  my ($file) = @_;
  my ($doc, $i, $fd);

  $fd = &gst_file_run_pipe_read ("gunzip -c $file");
  return undef if $fd eq undef;
  $doc .= $i while ($i = <$fd>);
  &gst_file_close ($fd);

  if (length ($doc) < 4)  # Allow for one blank line from gzip, '\n\r'.
  {
    $doc = undef;
  }

  return $doc;
}


sub gst_xml_read_stdin
{
  my ($i, $doc);
  
  do {
    $i = <STDIN>;

    if ($i ne undef)
    {
      $doc .=$i;
    }
  } while (! ($i =~ /^<!-- GST: end of request -->$/));

  return $doc;
}

# (file, tool) If no file specified, reads stdin.
# file could also contain xml document.
# If tool is an gst_tool, stores the read buffer in
# $$tool{"xml_doc"}.
sub gst_xml_scan  
{
  my ($file, $tool) = @_;
  my ($doc, @tree, @gst_xml_scan_list);

  $file = $gst_input_file unless $file;

  if ($file && stat ($file))
  {
    $doc = &gst_xml_read_file ($file);
  }
  elsif ($file)
  {
    $doc = $file;
  }
  else
  {
    $doc = &gst_xml_read_stdin ();
  }

  # remove any blank or carriage return at the beginning of the xml
  $doc =~ s/^[ \n]*//;

  &gst_debug_print_log_to_file ("in.xml", $doc);

  $$tool{"xml_doc"} = $doc if (&gst_is_tool ($tool));

  @gst_xml_scan_list = ($doc =~ /([^\<]*)(\<[^\>]*\>)[ \t\n\r]*/mg); # pcdata, tag, pcdata, tag, ...
  $tree = &gst_xml_scan_recurse (\@gst_xml_scan_list);

  return $tree;
}

# XML scanning that preserves more exact attributes of the scanned XML.

sub gst_xml_model_scan_recurse
{
  my @list;
  if (@_) { @list = $_[0]->[0]; }
  
  while (@gst_xml_scan_list)
  {
    $el = $gst_xml_scan_list[0]; shift @gst_xml_scan_list;

    if ($el eq "")  # Empty strings.
    {
      next;
    }
    elsif ($el =~ /^\<[!?].*\>$/s)  # PI and DTD.
    {
      $el =~ /^\<([^\>]+)\>$/s;
      push (@list, "__unparsed__");
      push (@list, $1);
    }
    elsif ($el =~ /^\<.*\/\>$/s)  # Empty.
    {
      $el =~ /^\<([a-zA-Z0-9_-]+).*\/\>$/s;
      push (@list, $1);
      push (@list, &gst_xml_scan_make_kid_array ($el));
    }
    elsif ($el =~ /^\<\/.*\>$/s)  # End.
    {
      last;
    }
    elsif ($el =~ /^\<.*\>$/s)  # Start.
    {
      $el =~ /^\<([a-zA-Z0-9_-]+).*\>$/s;
      push (@list, $1);
      $sublist = &gst_xml_scan_make_kid_array ($el);
      push (@list, &gst_xml_model_scan_recurse ($sublist));
      next;
    }
    elsif ($el ne "")  # PCDATA.
    {
      push (@list, 0);
      push (@list, "$el");
    }
  }

  return \@list;
}

sub gst_xml_model_scan  # (file) If no file specified, reads stdin.
{
  my ($file) = @_;
  my ($doc, $tree, $compressed);

  $file = $gst_input_file if $file eq undef;

  if ($file)
  {
    $doc = &gst_xml_read_compressed_file ($file);
    if (!$doc)
    {
      $doc = &gst_xml_read_file ($file);
      $compressed = 0;
    }
    else
    {
      $compressed = 1;
    }
  }
  else
  {
    return undef, 0;
  }

  @gst_xml_scan_list = ($doc =~ /([^\<]*)(\<[^\>]*\>)/mg); # pcdata, tag, pcdata, tag, ...
  $tree = &gst_xml_model_scan_recurse;

  return $tree, $compressed;
}

sub gst_xml_model_save
{
  my ($model, $file, $compressed) = @_;
  my $fd;

  if ($compressed == 1)
  {
    $fd = &gst_file_open_write_compressed ($file);
  }
  else
  {
    $fd = &gst_file_open_write_from_names ($file);
  }

  if ($fd == -1) { return -1; }

  print $fd &gst_xml_model_print ($model);
  &gst_file_close ($fd);

  return 0;
}

# Quote/unquote.

@gst_xml_entities = ( "&lt;", '<', "&gt;", '>', "&apos;", '\'', "&quot;", '"', "&amp;", '&' );


sub gst_xml_quote
{
  my $in = $_[0];
  my $out = "";
  my @xe;
  my $joined = 0;
  
  my @clist = split (//, $in);
  
  while (@clist)
  {
    # Find character and join its entity equivalent.
    # If none found, simply join the character.
	
    $joined = 0;		# Cumbersome.
    
    for (@xe = @gst_xml_entities; @xe && !$joined; )
    {
      if ($xe [1] eq $clist [0]) { $out = join ('', $out, $xe [0]); $joined = 1; }
      shift @xe; shift @xe;
    }
	
    if (!$joined) { $out = join ('', $out, $clist [0]); }
    shift @clist;
  }
  
  return $out;
}


sub gst_xml_unquote
{
  my $ret = $_[0];
  my $i;

  #print STDERR "INI U: $ret\n";
  
  for ($i = 0; $gst_xml_entities[$i] ne undef; $i += 2)
  {
    $ret =~ s/$gst_xml_entities[$i]/$gst_xml_entities[$i + 1]/g;
  }

  while ($ret =~ /&#([0-9]+);/)
  {
    $num = $1;
    $c = chr ($num);
    $ret =~ s/&#$num;/$c/g;
  }

  #print STDERR "END U: $ret\n";
  
  return $ret;
}


# --- XML parsing --- #


sub gst_xml_get_pcdata
{
  my $tree = $_[0];
  my $retval;
  
  shift @$tree;  # Skip attributes.
  
  while (@$tree)
  {
    if ($$tree[0] == 0)
    {
      $retval = &gst_xml_unquote ($$tree[1]);
      &gst_debug_print_line ("gst_xml_get_pcdata: $retval");
      return ($retval);
    }
    
    shift @$tree;
    shift @$tree;
  }

  return "";
}

# Compresses node into a word and returns it.

sub gst_xml_get_word
{
  my $tree = $_[0];
  my $retval;

  $retval = &gst_xml_get_pcdata ($tree);
  $retval =~ tr/ \n\r\t\f//d;
  return $retval;
}


# Compresses node into a size and returns it.

sub gst_xml_get_size
{
  my $tree = $_[0];
  my $retval;

  $retval = &gst_xml_get_word ($tree);
  if ($retval =~ /Mb$/)
  {
    $retval =~ tr/ Mb//d; 
    $retval *= 1024;
  }

  return $retval;
}


# Replaces misc. whitespace with spaces and returns text.

sub gst_xml_get_text
{
  my $tree = $_[0];
  my $retval;

  $retval = &gst_xml_get_pcdata ($tree);
  my $type = ref ($retval);

  if (!$type) { $retval =~ tr/\n\r\t\f/    /; }  

  return $retval;
}

sub gst_xml_get_attribute
{
  my ($tree, $attr) = @_;

  return $$tree[0]->{$attr};
}

sub gst_xml_get_state
{
  my ($tree) = @_;

  # Check attribute; 'yes', 'true', 'no', 'false'.
  return &gst_util_read_boolean ($$tree[0]->{state});
}

# XML model operations.

# Locate a node from the branch leading up to it.
sub gst_xml_model_find
{
  my ($model, $varpath) = @_;
  my ($branch, @path);

  $branch = $model;
  @path   = split /\//, $varpath;

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
        last;
      }

      shift @children;
      shift @children;
    }

    last if ($branch == undef);
  }

  return $branch;
}

# Add a branch to another branch. Allows duplicates.
sub gst_xml_model_add
{
  my ($model, $varpath, $addpath) = @_;
  my ($branch, @path);

  @path = split /\//, $addpath;
  $branch = &gst_xml_model_find ($model, $varpath);
  if ($branch == undef)
  {
    return -1;
  }

  for $elem (@path)
  {
    my %hash;
    my @list = ();

    push @list, \%hash;

    push @$branch, $elem;
    push @$branch, \@list;

    $branch = \@list;
  }

  return 0;
}

# Ensure a branch exists, by extending the branch with given elements, if needed.
sub gst_xml_model_ensure
{
  my ($model, $varpath) = @_;
  my ($branch, @path);

  $branch = $model;
  @path   = split /\//, $varpath;

  for $elem (@path)
  {
    next if ($elem eq "");

    my @children = @$branch;
    my $parent_branch = $branch;

    shift @children;  # Attributes
    $branch = undef;

    while (@children)
    {
      if ($children [0] eq $elem)
      {
        shift @children;
        $branch = shift @children;
        last;
      }

      shift @children;
      shift @children;
    }

    if ($branch == undef)
    {
      my %hash;
      my @list = ();

      $branch = \@list;

      push @list, \%hash;

      push @$parent_branch, $elem;
      push @$parent_branch, $branch;
    }
  }

  return $branch;
}

sub gst_xml_model_remove
{
  my ($model, $varpath, $tag) = @_;
  my ($branch, $i);

  @path = split /\//, $addpath;
  $branch = &gst_xml_model_find ($model, $varpath);
  if ($branch == undef)
  {
    return -1;
  }

  for ($i = 1; $i < @$branch; $i += 2)
  {
    if (@$branch [$i] eq $tag)
    {
      @$branch = (@$branch [0 .. $i - 1], @$branch [$i + 2 .. @$branch - 1]);
      return 0;
    }
  }

  return -1;
}

sub gst_xml_model_get_children
{
  my ($branch) = @_;
  my (@children);

  if (!$branch) { return \@children; }

  for ($i = 1; $i < @$branch; $i += 2)
  {
    if (@$branch [$i] ne "__unparsed__" && @$branch [$i] ne "0")
    {
      push @children, @$branch [$i + 1];
    }
  }

  return \@children;
}

sub gst_xml_model_get_pcdata
{
  my ($branch) = @_;
  my ($i);

  for ($i = 1; $i < @$branch; $i += 2)
  {
    if ($$branch [$i] == 0)
    {
      my $retval = &gst_xml_unquote ($$branch [$i + 1]);
      return ($retval);
    }
  }

  return "";
}

sub gst_xml_model_set_pcdata
{
  my ($branch, $pcdata) = @_;

  @$branch = (@$branch [0]);

  $$branch [1] = 0;
  $$branch [2] = &gst_xml_quote ($pcdata);
}

sub gst_xml_model_get_attribute
{
  my ($branch, $attr) = @_;

  return $$branch[0]->{$attr};
}

sub gst_xml_model_set_attribute
{
  my ($branch, $attr, $value) = @_;

  return $$branch[0]->{$attr} = $value;
}

1;
