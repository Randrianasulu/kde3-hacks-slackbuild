#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# Determine the platform we're running on.
#
# Copyright (C) 2000-2001 Ximian, Inc.
#
# Authors: Arturo Espinosa <arturo@ximian.com>
#          Hans Petter Jansson <hpj@ximian.com>
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


# --- System guessing --- #


$SCRIPTSDIR = "/usr/share/apps/knetworkconf/backends";
if ($SCRIPTSDIR =~ /^@scriptsdir[@]/)
{
    $SCRIPTSDIR = ".";
    $DOTIN = ".in";
}

require "$SCRIPTSDIR/parse.pl$DOTIN";
require "$SCRIPTSDIR/xml.pl$DOTIN";


$PLATFORM_INFO = {
  "debian-2.2"      => "Debian GNU/Linux 2.2 Potato",
  "debian-3.0"      => "Debian GNU/Linux 3.0 Woody",
  "debian-3.1"      => "Debian GNU/Linux 3.1 Sarge",
  "debian-4.0"      => "Debian GNU/Linux 4.0 Etch",
  "debian-5.0"      => "Debian GNU/Linux 5.0 Lenny",
  "debian-testing"  => "Debian GNU/Linux Testing/Unstable",
  "ubuntu-5.04"     => "Kubuntu 5.04, Hoary Hedgehog",
  "ubuntu-5.10"     => "Kubuntu 5.10, Breezy Badger",
  "ubuntu-6.06"     => "Kubuntu 6.06, Dapper Drake",
  "ubuntu-6.10"     => "Kubuntu 6.10, Edgy Eft",
  "ubuntu-7.04"     => "Kubuntu 7.04, Feisty Fawn",
  "ubuntu-7.10"     => "Kubuntu 7.10, Gutsy Gibbon",
  "ubuntu-8.04"     => "Kubuntu 8.04, Hardy Heron",
  "redhat-5.2"      => "Red Hat Linux 5.2 Apollo",
  "redhat-6.0"      => "Red Hat Linux 6.0 Hedwig",
  "redhat-6.1"      => "Red Hat Linux 6.1 Cartman",
  "redhat-6.2"      => "Red Hat Linux 6.2 Zoot",
  "redhat-7.0"      => "Red Hat Linux 7.0 Guinness",
  "redhat-7.1"      => "Red Hat Linux 7.1 Seawolf",
  "redhat-7.2"      => "Red Hat Linux 7.2 Enigma",
  "redhat-7.3"      => "Red Hat Linux 7.3 Valhalla",
  "redhat-8.0"      => "Red Hat Linux 8.0 Psyche",
  "redhat-9"        => "Red Hat Linux 9.0 Shrike",
  "openna-1.0"      => "OpenNA Linux 1.0 VSLC",
  "mandrake-7.1"    => "Linux Mandrake 7.1",
  "mandrake-7.2"    => "Linux Mandrake 7.2 Odyssey",
  "mandrake-8.0"    => "Linux Mandrake 8.0 Traktopel",
  "mandrake-9.0"    => "Linux Mandrake 9.0 Dolphin",
  "mandrake-9.1"    => "Linux Mandrake 9.1 Bamboo",
  "mandrake-9.2"    => "Linux Mandrake 9.2 FiveStar",
  "mandrake-10.0"   => "Linux Mandrake 10.0",
  "mandrake-10.1"   => "Linux Mandrake 10.1",
  "mandrake-10.2"   => "Linux Mandrake 2005 LE",
  "mandriva-2006.0" => "Mandriva Linux 2006.0",
  "mandriva-2006.1" => "Mandriva Linux 2006.1",
  "mandriva-2007.0" => "Mandriva Linux 2007.0",
  "mandriva-2007.1" => "Mandriva Linux 2007.1",
  "yoper-2.2"       => "Yoper Linux 2.2",
  "blackpanther-4.0" => "Black Panther OS 4.0",
  "conectiva-9"     => "Conectiva Linux 9",
  "conectiva-10"    => "Conectiva Linux 10",
  "suse-7.0"        => "SuSE Linux 7.0",
  "suse-9.0"        => "SuSE Linux 9.0",
  "suse-9.1"        => "SuSE Linux 9.1",
  "turbolinux-7.0"  => "Turbolinux 7.0",
  "slackware-8.0.0" => "Slackware 8.0.0",
  "slackware-8.1"   => "Slackware 8.1",
  "slackware-9.0.0" => "Slackware 9.0.0",
  "slackware-9.1.0" => "Slackware 9.1.0",
  "slackware-10.0.0" => "Slackware 10.0.0",
  "slackware-10.1.0" => "Slackware 10.1.0",
  "slackware-10.2.0" => "Slackware 10.2.0",
  "freebsd-4"       => "FreeBSD 4",
  "freebsd-5"       => "FreeBSD 5",
  "freebsd-6"       => "FreeBSD 6",
  "gentoo"          => "Gentoo Linux",
  "vlos-1.2"        => "Vida Linux OS 1.2",
  "archlinux"       => "Arch Linux",
  "pld-1.0"         => "PLD 1.0 Ra",
  "pld-1.1"         => "PLD 1.1 Ra",
  "pld-1.99"        => "PLD 1.99 Ac-pre",
  "vine-3.0"        => "Vine Linux 3.0",
  "vine-3.1"        => "Vine Linux 3.1",
  "fedora-1"        => "Fedora Core 1 (Yarrow)",
  "fedora-2"        => "Fedora Core 2 (Tettnang)",
  "fedora-3"        => "Fedora Core 3 (Heidelberg)",
  "fedora-4"        => "Fedora Core 4 (Stentz)",
  "fedora-5"        => "Fedora Core 5 (Bordeaux)",
  "rpath"           => "rPath Linux",
  "ark"             => "Ark Linux",
};
  
sub check_lsb
{
  my ($ver, $dist);
#  my %vermap =
#      ("3.0" => "woody");
  
  my %distmap =
      ("Debian" => "debian"),
      ("Mandrake" => "mandrake"),
      ("Conectiva" => "conectiva"),
      ("Blackpanther" => "blackpanther");

  # gst_prefix not required here: parse already does that for us.
  $dist = lc (&gst_parse_sh ("/etc/lsb-release", "DISTRIB_ID"));
  $ver = lc (&gst_parse_sh ("/etc/lsb-release", "DISTRIB_RELEASE"));
  
#  $ver = $vermap{$ver} if exists $vermap{$ver};
  $dist = $distmap{$dist} if exists $dirmap{$dir};

  return -1 if ($dist eq "") || ($ver eq "");
  return "$dist-$ver";
}

sub check_debian
{
  my ($ver, $i);
  my %vermap =
      ("testing/unstable" => "testing",
       "lenny/sid"        => "testing",
       "3.1"              => "sarge",
       "4.0"              => "etch",
       "5.0"              => "lenny");

  open DEBIAN, "$gst_prefix/etc/debian_version" or return -1;
  chomp ($ver = <DEBIAN>);
  close DEBIAN;

  #if $ver is not found, we will assume it is testing
  if($ver and exists $vermap{$ver}) {
    $ver = $vermap{$ver};
  } else {
    $ver = "testing";
  }

  return "debian-$ver";
}


sub check_redhat
{
  open RELEASE, "$gst_prefix/etc/redhat-release" or return -1;
  while (<RELEASE>)
  {
    chomp;
    if (/^Red Hat Linux.*\s+([0-9.]+)\s+.*/)
    {
      close RELEASE;
      return "redhat-$1";
    }
  }
  close RELEASE;
  return -1;
}

sub check_openna
{
  open OPENNA, "$gst_prefix/etc/openna-release" or return -1;
  while (<OPENNA>)
  {
    chomp;
    if (/^OpenNA*/)
    {
      close OPENNA;
      return "openna-$1";
    }
  }
  close OPENNA;
  return -1;
}

sub check_caldera
{
  open INSTALLED, "$gst_prefix/etc/.installed" or return -1;
  while (<INSTALLED>)
  {
    chomp;
    if (/^OpenLinux-(.*)-.*/)
    {
      close INSTALLED;
      return "caldera-$1";
    }
  }
  close INSTALLED;
  return -1;
}


sub check_suse
{
  open RELEASE, "$gst_prefix/etc/SuSE-release" or return -1;
  while (<RELEASE>)
  {
    chomp;
    if (/^VERSION\s*=\s*(\S+)/)
    {
      close RELEASE;
      return "suse-$1";
    }
  }
  close RELEASE;
  return -1;
}

sub check_mandrake
{
  open MANDRAKE, "$gst_prefix/etc/mandrake-release" or return -1;
  while (<MANDRAKE>)
  {
    $ver = $_;
    chomp ($ver);
    if ($ver =~ /^Linux Mandrake release (\S+)/)
    {
      close MANDRAKE;
      return "mandrake-$1";
    }
    elsif ($ver =~ /^Mandrake( L|l)inux release ([\d\.]+)/i)
    {
      close MANDRAKE;
      return "mandrake-$2";
    }
  }
  close MANDRAKE;
  return -1;
}

sub check_mandriva
{
  open MANDRIVA, "$gst_prefix/etc/mandriva-release" or return -1;
  while (<MANDRIVA>)
  {
    $ver = $_;
    chomp ($ver);
    if ($ver =~ /^Linux Mandriva release (\S+)/)
    {
      close MANDRIVA;
      return "mandriva-$1";
    }
    elsif ($ver =~ /^Mandriva( L|l)inux release ([\d\.]+)/i)
    {
      close MANDRIVA;
      return "mandriva-$2";
    }
  }
  close MANDRIVA;
  return -1;
}

sub check_yoper
{
   open YOPER, "$gst_prefix/etc/yoper-release" or return -1;
   while (<YOPER>)
   {
     $ver = $_;
     chomp ($ver);
     if ($ver =~ m/Yoper (\S+)/)
     {
       close YOPER;
       # find the first digit of our release
       $mystring= ~m/(\d)/;
       #store it in $fdigit
       $fdigit= $1;
       # the end of the release is marked with -2 so find the -
       $end = index($ver,"-");
       $start = index($ver,$fdigit);
       # extract the substring into $newver
       $newver= substr($ver,$start,$end-$start);
       print $newver;
       return "yoper-$newver";
     }
   }
   close YOPER;
   return -1;
}

sub check_blackpanther
{
  open BLACKPANTHER, "$gst_prefix/etc/blackPanther-release" or return -1;

  while (<BLACKPANTHER>)
  {
    $ver = $_;
    chomp ($ver);
    if ($ver =~ /^Linux Black Panther release (\S+)/)
    {
      close BLACKPANTHER;
      return "blackPanther-$1";    
    }
    elsif ($ver =~ /^Black Panther ( L|l)inux release ([\d\.]+)/i)
    {
      close BLACKPANTHER;
      return "blackPanther-$2";     
    }
  }

  close BLACKPANTHER;
  return -1;
}

sub check_fedora
{
    open FEDORA, "$gst_prefix/etc/fedora-release" or return -1;
    while (<FEDORA>)
    {
      $ver = $_;
      chomp ($ver);

      if ($ver =~ /^Fedora Core release (\S+)/)
      {
        close FEDORA;
        return "fedora-$1";
      }
    }

    close FEDORA;
    return -1;
}

sub check_rpath
{
  open RPATH, "$gst_prefix/etc/distro-release" or return -1;

  while (<RPATH>)
  {
    $ver = $_;
    chomp ($ver);

    if ($ver =~ /^rPath Linux/)
    {
      close RPATH;
      return "rpath";
    }
    if ($ver =~ /Foresight/)
    {
      close RPATH;
      return "rpath";
    }
  }

  close RPATH;
  return -1;
}

sub check_conectiva
{
  open RELEASE, "$gst_prefix/etc/conectiva-release" or return -1;

  while (<RELEASE>)
 	{
    chomp;

    if (/^Conectiva Linux (\S+)/)
 		{
      close RELEASE;
 			return "conectiva-$1";
 		}
 	}

  close RELEASE;
 	return -1;
}

sub check_turbolinux
{
  open RELEASE, "$gst_prefix/etc/turbolinux-release" or return -1;
  while (<RELEASE>)
  {
    chomp;
    if (/^Turbolinux\s.*\s([0-9.]+)\s.*/)
    {
      close RELEASE;
      return "turbolinux-$1";
    }
  }
  close RELEASE;
  return -1;
}

sub check_slackware
{
  open RELEASE, "$gst_prefix/etc/slackware-version" or return -1;
  while (<RELEASE>)
  {
    chomp;
    if (/^Slackware ([0-9.]+)/)
    {
        close RELEASE;
        return "slackware-$1";
    }
  }
  close RELEASE;
  return -1;
}

sub check_gentoo
{
  return "gentoo" if stat ("$gst_prefix/usr/portage");
  return -1;
}

sub check_vlos
{
  open RELEASE, "$gst_prefix/etc/vlos-release" or return -1;
  while (<RELEASE>)
  {
    chomp;
    if (/^VLOS.*\s+([0-9.]+)/)
    {
      close RELEASE;
      return "vlos-$1";
    }
  }
  close RELEASE;
  return -1;
}

sub check_archlinux
{
  # Since Arch stores gnome in /opt/gnome, use full dir
  return "archlinux" if stat ("/etc/arch-release");
  return -1;
}

sub check_linuxppc
{
  open RELEASE, "$gst_prefix/etc/redhat-release" or return -1;
  while (<RELEASE>)
  {
    chomp;
    if (/^LinuxPPC\s+(\S+)/)
    {
      close RELEASE;
      return "linuxppc-$1";
    }
  }
  close RELEASE;
  return -1;
}

sub check_pld
{
  open RELEASE, "$gst_prefix/etc/pld-release" or return -1;
  while (<RELEASE>)
  {
    chomp;
               if(/^([0-9.]+) PLD Linux/)
    {
      close RELEASE;
      return "pld-$1";
    }
  }
  close RELEASE;
  return -1;
}

sub check_vine
{
  open RELEASE, "$gst_prefix/etc/vine-release" or return -1;
  while (<RELEASE>)
  {
    chomp;
    if(/^Vine Linux ([0-9.]+)\s+.*/)
    {
      close RELEASE;
      return "vine-$1";
    }
  }
  close RELEASE;
  return -1;
}

sub check_ark
{
  open ARK, "$gst_prefix/etc/ark-release" or return -1;
  while (<ARK>)
  {
    $ver = $_;
    chomp ($ver);

    if ($ver =~ /^Ark Linux/)
    {
      close ARK;
      return "ark";
    }
  }

  close ARK;
  return -1;
}

sub check_freebsd
{
  my ($sysctl_cmd, @output);

  $sysctl_cmd = &gst_file_locate_tool ("sysctl");
  @output = (readpipe("$sysctl_cmd -n kern.version"));
  foreach (@output)
  {
    chomp;
    if (/^FreeBSD\s([0-9]+)\.\S+.*/)
    {
      return "freebsd-$1";
    }
  }
  return -1;
}

sub check_solaris
{
  my ($fd, $dist);

  #
  # The file /etc/release is present for solaris-2.6
  # solaris 2.5 does not have the file.  Solaris-7.0 and 8.0 have not
  # been checked
  #
  # uname output
  # Solaris 2.5: 5.5(.1)
  # Solaris 2.6: 5.6
  # Solaris 7:   unknown, assume 7.0
  # Solaris 8:   unknown, assume 8.0
  #
  $fd = &gst_file_run_pipe_read ("uname -r");
  return -1 if $fd eq undef;
  chomp ($dist = <$fd>);
  &gst_file_close ($fd);

  if ($dist =~ /^5\.(\d)/) { return "solaris-2.$1" }
  else { if ($dist =~ /^([78])\.\d/) { return "solaris-$1.0" } }
  return -1;
}

sub gst_platform_get_system
{
  my ($tool) = @_;

  # get the output of 'uname -s', it returns the system we are running
  $$tool{"system"} = &gst_file_run_backtick ("uname -s");
  chomp ($$tool{"system"});
}

sub gst_platform_guess
{
  my ($tool) = @_;

  my %check = (
    # Red Hat check must run after Vine, Mandrake and Fedora, and Mandrake after BlackPanther
    "Linux" => [ \&check_lsb, \&check_debian, \&check_caldera, \&check_suse, \&check_blackpanther, \&check_vine,
                 \&check_fedora, \&check_mandrake, \&check_mandriva, \&check_conectiva, \&check_linuxppc, \&check_redhat, \&check_openna,
                 \&check_turbolinux, \&check_slackware, \&check_vlos, \&check_gentoo, \&check_pld, \&check_rpath, \&check_archlinux, \&check_ark ],
    "FreeBSD" => [ \&check_freebsd ],
    "SunOS"    => [ \&check_solaris ]
               );
  my $plat;

  # Fool-the-backend hack.
  if (exists $ENV{"GST_DIST"})
  {
    $$tool{"platform"} = $gst_dist = $ENV{"GST_DIST"};
    return;
  }

  foreach $plat (keys %check)
  {
    if ($$tool{"system"} =~ /$plat/)
    {
      my ($check, $dist);
      
      foreach $check (@{$check{$plat}})
      {
        $dist = &$check ();
        if ($dist != -1)
        {
          $$tool{"platform"} = $gst_dist = $dist;
          return;
        }
      }
    }
  }

  $$tool{"platform"} = $gst_dist = "unknown";
}


# gst_platform_ensure_supported
#
# Takes a list of supported platforms and sees if the one detected is found in
# this list. If not, will report a list of supported platforms and fail.

sub gst_platform_ensure_supported
{
  my ($tool, @supported) = @_;

  $$tool{"platforms"} = [ @supported ];

  foreach $platform (@supported)
  {
    if ($platform eq $$tool{"platform"})
    {
      &gst_report ("platform_success", $platform, $$PLATFORM_INFO{$platform});
      return;
    }
  }

  # Not supported.
  if (exists $$tool{"platform"})
  {
    &gst_report ("platform_unsup", $$tool{"platform"});
  }
  else
  {
    &gst_report ("platform_undet");
  }
}


# A directive handler that sets the currently selected platform.
sub gst_platform_set
{
  my ($tool, $platform) = @_;
  my ($p);

  foreach $p (@{ $$tool{"platforms"}})
  {
    if ($p eq $platform)
    {
      $$tool{"platform"} = $gst_dist = $platform;
      &gst_report ("platform_success", $platform, $$PLATFORM_INFO{$platform});
      &gst_report_end ();
      return;
    }
  }

  &gst_report ("platform_unsup", $platform);
  &gst_report_end ();
}

sub gst_platform_print_list
{
  my ($platforms) = @_;
  my ($platform, $name);

  &gst_xml_print_begin ("platforms");
  foreach $platform (sort @$platforms)
  {
    $name = $$PLATFORM_INFO{$platform};

    &gst_xml_container_enter ("platform");
    &gst_xml_print_line ("<key>$platform</key>");
    &gst_xml_print_line ("<name>$name</name>");
    &gst_xml_container_leave ();
  }
  &gst_xml_print_end ("platforms");
}

sub gst_platform_list
{
  my ($tool) = @_;

  &gst_report_end ();
  &gst_platform_print_list ($$tool{"platforms"});
}

1;
