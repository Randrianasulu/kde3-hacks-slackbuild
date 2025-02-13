#!/usr/bin/env perl
#-*- Mode: perl; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-

# Common network stuff for the ximian-setup-tools backends.
#
# Copyright (C) 2000-2001 Ximian, Inc.
#
# Authors: Hans Petter Jansson <hpj@ximian.com>
#          Arturo Espinosa <arturo@ximian.com>
#          Michael Vogt <mvo@debian.org> - Debian 2.[2|3] support.
#          David Lee Ludwig <davidl@wpi.edu> - Debian 2.[2|3] support.
#          Grzegorz Golawski <grzegol@pld-linux.org> - PLD support
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

use Socket;

$SCRIPTSDIR = "/usr/share/apps/knetworkconf/backends";
if ($SCRIPTSDIR =~ /^@scriptsdir[@]/)
{
  $SCRIPTSDIR = ".";
  $DOTIN = ".in";
}

require "$SCRIPTSDIR/general.pl$DOTIN";
require "$SCRIPTSDIR/file.pl$DOTIN";
require "$SCRIPTSDIR/parse.pl$DOTIN";
require "$SCRIPTSDIR/replace.pl$DOTIN";
require "$SCRIPTSDIR/service.pl$DOTIN";
require "$SCRIPTSDIR/util.pl$DOTIN";
require "$SCRIPTSDIR/xml.pl$DOTIN";
require "$SCRIPTSDIR/process.pl$DOTIN";


# Shared XML printing function
sub network_xml_print_statichost
{
  my ($h) = $_[0];
  my ($statichost, $i, $j, $val);

  &gst_xml_print_vspace ();
  foreach $i (keys %{$$h{"statichost"}})
  {
    $statichost = $ {$$h{"statichost"}}{$i};
    &gst_xml_container_enter ("statichost");
    $val = &gst_xml_quote ($i);
    &gst_xml_print_pcdata ("ip", $val);
    foreach $j (@$statichost)
    {
      $val = &gst_xml_quote ($j);
      &gst_xml_print_pcdata ("alias", $val);
    }
    &gst_xml_container_leave ("statichost");
  }
}

sub gst_network_get_linux_wireless_ifaces
{
  my ($fd, $line);
  my (@ifaces, $command);

  $command = &gst_file_get_cmd_path ("iwconfig");
  open $fd, "$command |";
  return @ifaces if $fd eq undef;

  while (<$fd>)
  {
    if (/^([a-zA-Z0-9]+)[\t ].*$/)
    {
      push @ifaces, $1;
    }
  }

  &gst_file_close ($fd);

  &gst_report_leave ();
  return \@ifaces;
}

sub gst_network_get_freebsd_wireless_ifaces
{
  my ($fd, $line, $iface);
  my (@ifaces, $command);

  $command = &gst_file_get_cmd_path ("iwconfig");
  open $fd, "$command |";
  return @ifaces if $fd eq undef;

  while (<$fd>)
  {
    if (/^([a-zA-Z]+[0-9]+):/)
    {
      $iface = $1;
    }

    if (/media:.*wireless.*/i)
    {
      push @ifaces, $iface;
    }
  }

  &gst_file_close ($fd);
  &gst_report_leave ();

  return \@ifaces;
}

# Returns an array with the wireless devices found
sub gst_network_get_wireless_ifaces
{
  my ($plat) = $$tool{"system"};
    
  return &gst_network_get_linux_wireless_ifaces   if ($plat eq "Linux");
  return &gst_network_get_freebsd_wireless_ifaces if ($plat eq "FreeBSD");
}

# set of functions for enabling an interface
sub gst_network_config_wireless
{
  my ($hash, $dev, $command_iwconfig, $command_ifconfig) = @_;
  my ($essid, $key, $command);

  $essid = $$hash{"configuration"}{"essid"};
  $key   = $$hash{"configuration"}{"key"};
  $key_type = $$hash{"configuration"}{"key_type"};

  if ($essid)
  {
    $command  = $command_iwconfig if ($$tool{"system"} eq "Linux");
    $command  = $command_ifconfig if ($$tool{"system"} eq "FreeBSD");

    $command .= " $dev";
    $command .= " essid '$essid'" if ($essid);

    if ($key)
    {
      $key = &gst_network_get_full_key ($key, $key_type);
      $command .= " key '$key'";
    }
    else
    {
      $command .= " key off";
    }

    &gst_file_run ($command);
  }
}

sub gst_network_enable_iface
{
  my ($hash, $dev, $command_ifconfig) = @_;
  my ($address, $netmask, $bootproto, $remote_address);

  $address        = $$hash{"configuration"}{"address"};
  $netmask        = $$hash{"configuration"}{"netmask"};
  $bootproto      = $$hash{"configuration"}{"bootproto"};
  $remote_address = $$hash{"configuration"}{"remote_address"};

  if ($bootproto eq "dhcp")
  {
    if (&gst_file_locate_tool ("dhclient3"))
    {
      $command = "dhclient3 -pf /var/run/dhclient.$dev.pid $dev";
    }
    elsif (&gst_file_locate_tool ("dhclient"))
    {
      $command = "dhclient -pf /var/run/dhclient.$dev.pid -e $dev";
    }
    else
    {
      $command = "dhcpcd -n $dev";
    }
  }
  else
  {
    $command  = $command_ifconfig;
    $command .= " $dev";
    $command .= " $address" if ($address);
    $command .= " netmask $netmask" if ($netmask);
    $command .= " dstaddr $remote_address" if ($remote_address);
    $command .= " up";
  }

  return &gst_file_run ($command);
}

sub gst_network_get_chat_file
{
  my ($hash) = @_;
  my ($volume, $phone_number, $external_line, $dial_command);
  my ($dial, $tmp_file, $filename, $arr);

  $volume        = $$hash{"configuration"}{"volume"};
  $phone_number  = $$hash{"configuration"}{"phone_number"};
  $external_line = $$hash{"configuration"}{"external_line"};
  $dial_command  = $$hash{"configuration"}{"dial_command"};

  $filename = gst_file_get_temp_name ("network-admin-chat-script-XXXXXXXXX");
  $tmp_file = "/tmp/$filename";

  $dial  = $dial_command;
  $dial .= $external_line . "W" if ($external_line);
  $dial .= $phone_number;

  &gst_network_create_pppscript ($tmp_file);
  chmod (0600, $tmp_file);
  &gst_replace_chat ($tmp_file, "atd[tp][0-9wW]+", $dial);
  &gst_network_set_modem_volume ($tmp_file, $volume);


  return $tmp_file;
}

sub gst_network_enable_modem
{
  my ($hash, $dev, $command_pppd, $command_chat) = @_;
  my ($config, $chat_file, $command, $ret);

  $config    = $$hash{"configuration"};
  $chat_file = &gst_network_get_chat_file ($hash);

  $command  = $command_pppd;
  $command .= " " . $$config{"serial_port"};
  $command .= " debug" if ($$config{"debug"});
  $command .= ($$config{"noauth"}) ? " noauth" : " auth";
  $command .= ($$config{"persist"}) ? " persist" : " nopersist";
  $command .= ($$config{"serial_hwctl"}) ? " crtscts" : " nocrtscts";
  $command .= ($$config{"set_default_gw"}) ? " defaultroute" : " nodefaultroute";
  $command .= " usepeerdns" if ($$config{"update_dns"});
  $command .= " user " . $$config{"login"};
  $command .= " connect \'$command_chat -v -f $chat_file\'";

  $ret = &gst_file_run ($command);
  
  return $ret;
}

sub gst_network_enable_isdn
{
  my ($hash, $dev, $command_pppd) = @_;
  my ($config, $command);

  $config = $$hash{"configuration"};

  $command = $command_pppd;
  $command .= " debug" if ($$config{"debug"});
  $command .= ($$config{"noauth"}) ? " noauth" : " auth";
  $command .= ($$config{"persist"}) ? " persist" : " nopersist";
  $command .= ($$config{"serial_hwctl"}) ? " crtscts" : " nocrtscts";
  $command .= ($$config{"set_default_gw"}) ? " defaultroute" : " nodefaultroute";
  $command .= " usepeerdns" if ($$config{"update_dns"});
  $command .= " sync plugin userpass.so ipcp-accept-local ipcp-accept-remote";
  $command .= " plugin capiplugin.so";
  $command .= " /dev/null";
  $command .= " user " . $$config{"login"};
  $command .= " number " . $$config{"phone_number"};

  return &gst_file_run ($command);
}

sub drop_dhcp_connection
{
  my ($dev) = @_;
  &gst_process_kill_by_pidfile ("dhclient.$dev.pid");
  &gst_process_kill_by_pidfile ("/etc/dhcpc/dhcpcd-$dev.pid");
}

sub drop_pppd_connection
{
  my ($dev) = @_;
  &gst_process_kill_by_pidfile ("$dev.pid");
}

sub gst_network_enable_iface_with_config
{
  my ($hash) = @_;
  my ($command_ifconfig, $command_iwconfig);
  my ($command_pppd, $command_chat);
  my ($dev, $command, $ret);

  $command_ifconfig = "ifconfig";
  $command_iwconfig = "iwconfig";
  $command_pppd     = "pppd";
  $command_chat     = gst_file_locate_tool ("chat");

  $dev = $$hash{"dev"};

  $command = "$command_ifconfig $dev down";
  $ret = &gst_file_run ($command);

  &drop_dhcp_connection ($dev);
  &drop_pppd_connection ($dev);

  if (exists ($$hash{"configuration"}))
  {
    if (exists ($$hash{"configuration"}{"phone_number"}))
    {
      $login = $$hash{"configuration"}{"login"};
      $pw = $$hash{"configuration"}{"password"};

      &gst_network_set_pap_passwd ("/etc/ppp/pap-secrets", $login, $pw); 
      &gst_network_set_pap_passwd ("/etc/ppp/chap-secrets", $login, $pw);

      if (&gst_network_check_pppd_plugin ("capiplugin"))
      {
        $ret = &gst_network_enable_isdn ($hash, $dev, $command_pppd);
      }
      else
      {
        $ret = &gst_network_enable_modem ($hash, $dev, $command_pppd, $command_chat);
      }
    }
    else
    {
      # if it's a wireless interface, put first the essid
      &gst_network_config_wireless ($hash, $dev, $command_iwconfig, $command_ifconfig);
      $ret = &gst_network_enable_iface ($hash, $dev, $command_ifconfig);
    }
  }

  return $ret;
}

# Tries to detect modem in a closed list of devices
sub gst_network_autodetect_modem
{
  my (@arr);

  if ($$tool{"system"} eq "Linux")
  {
    @arr = ("/dev/modem", "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3");

    $pid = `pidof pppd 2>/dev/null`;

    if ($pid =~ /\d/) {
      return;
    }
  }
  elsif ($$tool{"system"} eq "FreeBSD")
  {
    @arr = ("/dev/modem", "/dev/cuaa0", "/dev/cuaa1", "/dev/cuaa2", "/dev/cuaa3");
  }
  
  foreach $tty (@arr) {
    $temp = `pppd lcp-max-configure 1 nodetach noauth nocrtscts $tty connect \"chat -t1 \'\' AT OK\" 2>/dev/null`;

    if ($temp =~ /established/) {
      return $tty;
    }
  }
}

# Parse <interface>...</interface>. Put here so it can be shared with internetsharing.
sub gst_network_xml_parse_interface_config
{
  my ($tree) = @_;
  my (%config);

  shift @$tree;

  while ($elem = shift @$tree)
  {
    $config{$elem} = &gst_xml_get_pcdata (shift @$tree);
  }

  return \%config;
}

sub gst_network_xml_parse_interface
{
  my ($tree, $interface) = @_;
  my (%hash, $aux, $dev, $elem);

  shift @$tree;

  while ($elem = shift @$tree)
  {
    if ($elem eq "configuration")
    {
      $aux = &gst_network_xml_parse_interface_config (shift @$tree);
      $hash{$elem} = $aux if ($aux ne undef);
    }
    else
    {
      $hash{$elem} = &gst_xml_get_pcdata (shift @$tree);
    }
  }

  if (($hash{"configuration"} ne undef) &&
      ($hash{"configuration"}{"file"} ne undef))
  {
    $dev = $hash{"configuration"}{"file"};
  }
  else
  {
    $dev = &gst_network_get_file (\%hash);
    $hash{"configuration"}{"file"} = $dev if ($hash{"configuration"} ne undef);
  }

  $$interface{$dev} = \%hash;
  return \%hash;
}

# Expects something like "132.248.10.2" and returns 4 bytes.
sub gst_network_ipv4_str2vec
{
  my ($str) = @_;

  return pack ('C4', split ('\.', $str));
}

# Converse
sub gst_network_ipv4_vec2str
{
  my ($vec) = @_;

  return join ('.', unpack ('C4', $vec));
}

# Gets the subnet, in bitmap.
sub gst_network_ipv4_calc_subnet_vec
{
  my ($addr, $mask) = @_;
  my ($addrvec, $maskvec);

  $addrvec = &gst_network_ipv4_str2vec ($addr);
  $maskvec = &gst_network_ipv4_str2vec ($mask);

  return $addrvec & $maskvec;
}

# What you're looking for.
sub gst_network_ipv4_calc_subnet
{
  my ($addr, $mask) = @_;
    
  return &gst_network_ipv4_vec2str (&gst_network_ipv4_calc_subnet_vec);
}

# The broadcast, bitmap.
sub gst_network_ipv4_calc_bcast_vec
{
  my ($addr, $mask) = @_;
    
  $addrvec = &gst_network_ipv4_str2vec ($addr);
  $maskvec = &gst_network_ipv4_str2vec ($mask);

  return $addrvec | ~$maskvec;
}

# And this returning the quad-dot notation.
sub gst_network_ipv4_calc_bcast
{
  my ($addr, $mask) = @_;

  return &gst_network_ipv4_vec2str (&gst_network_ipv4_calc_bcast_vec);
}


# Selects a ping command, which must ping just twice, with
# one second interval, returning numeric IPs to the given
# broadcast address.
sub gst_network_get_broadcast_ping_cmd
{
  my ($bcast) = @_;
  my ($tool_ping);

  my %cmd_map =
      (
       "debian-2.2" => "ping -c 2 -i 1 -n $bcast",
       "redhat-6.2" => "ping -c 2 -i 1 -n -b $bcast"
       );
  my %dist_map =
      (
       "redhat-6.0"   => "redhat-6.2",
       "redhat-6.1"   => "redhat-6.2",
       "redhat-6.2"   => "redhat-6.2",
       "redhat-7.0"   => "redhat-6.2",
       "redhat-7.1"   => "redhat-6.2",
       "redhat-7.2"   => "redhat-6.2",
       "redhat-8.0"   => "redhat-6.2",
       "redhat-9"     => "redhat-6.2",
       "openna-1.0"   => "redhat-6.2",
       "debian-2.2"   => "debian-2.2",
       "debian-3.0"   => "debian-2.2",
       "debian-3.1"   => "debian-2.2",
       "debian-4.0"   => "debian-2.2",
       "debian-5.0"   => "debian-2.2",
       "debian-testing" => "debian-2.2",
       "ubuntu-5.04"  => "debian-2.2",
       "ubuntu-5.10"  => "debian-2.2",
       "ubuntu-6.06"  => "debian-2.2",
       "ubuntu-6.10"  => "debian-2.2",
       "ubuntu-7.04"  => "debian-2.2",
       "ubuntu-7.10"  => "debian-2.2",
       "ubuntu-8.04"  => "debian-2.2",
       "mandrake-7.1" => "debian-2.2",
       "mandrake-7.2" => "debian-2.2",
       "mandrake-9.0" => "debian-2.2",
       "mandrake-9.1" => "debian-2.2",
       "mandrake-9.2" => "debian-2.2",
       "mandrake-10.0" => "debian-2.2",
       "mandrake-10.1" => "redhat-6.2",
       "mandrake-10.2" => "redhat-6.2",
       "mandriva-2006.0" => "redhat-6.2",
       "mandriva-2006.1" => "redhat-6.2",
       "mandriva-2007.0" => "redhat-6.2",
       "mandriva-2007.1" => "redhat-6.2",
       "yoper-2.2"       => "redhat-6.2",
       "blackpanther-4.0" => "debian-2.2",
       "suse-7.0"     => "debian-2.2",
       "suse-9.0"     => "redhat-6.2",
       "suse-9.1"     => "redhat-6.2",
       "pld-1.0"      => "redhat-6.2",
       "pld-1.1"      => "redhat-6.2",
       "pld-1.99"     => "redhat-6.2",
       "conectiva-9"  => "debian-2.2",
       "conectiva-10" => "debian-2.2",
       "fedora-1"     => "redhat-6.2",
       "fedora-2"     => "redhat-6.2",
       "fedora-3"     => "redhat-6.2",
       "fedora-4"     => "redhat-6.2",
       "fedora-5"     => "redhat-6.2",
       "rpath"        => "redhat-6.2",
       "vine-3.0"     => "redhat-6.2",
       "vine-3.1"     => "redhat-6.2",
       "ark"          => "redhat-6.2",
       "slackware-9.1.0" => "redhat-6.2",
       "slackware-10.0.0" => "redhat-6.2",
       "slackware-10.1.0" => "redhat-6.2",
       "slackware-10.2.0" => "redhat-6.2",
       "gentoo"       => "debian-2.2",
       "vlos-1.2"     => "debian-2.2",
       "freebsd-5"    => "debian-2.2",
       "freebsd-6"    => "debian-2.2",
       );

  return $cmd_map{$dist_map{$gst_dist}};
}


# Run ping, taking what looks like the replying host addresses, return
# colon-sepparated (:) string. To be used with gst_process_fork.
sub gst_network_ping_broadcast_call
{
  my ($bcast) = @_;
  my ($cmd, %hosts, $fd);

  $cmd = &gst_network_get_broadcast_ping_cmd ($bcast);
  $fd = &gst_file_run_pipe_read ($cmd);
  return undef if $fd eq undef;
  while (<$fd>)
  {
    if (/([0-9.]+):?[ \t]+icmp_seq/)
    {
      # If it already exists, it is the second count, and we can return then.
      last if exists $hosts{$1};
      $hosts{$1} = 1 ;
    }
  }
  &gst_file_close ($fd);

  return join (':', keys (%hosts));
}


# Return an array of IPs of the hosts in all the adjacent local networks.
sub gst_network_find_hosts
{
  my ($iface, $ifaces, $dev, $hosts_str, @hosts, $proc, @procs);

  $ifaces = &gst_network_interfaces_get_info ();

  foreach $dev (keys %$ifaces)
  {
    $iface = $$ifaces{$dev};
    if ($$iface{"active"} && exists $$iface{"bcast"})
    {
      push @procs, &gst_process_fork (\&gst_network_ping_broadcast_call, $$iface{"bcast"});
    }
  }

  &gst_process_list_check_ready (3, \@procs);

  foreach $proc (@procs)
  {
    if ($$proc{"ready"})
    {
      sysread ($$proc{"fd"}, $hosts_str, 4096);
      push @hosts, split (':', $hosts_str);
    }
  }

  return @hosts;
}


# like, &gst_network_lookup_address_block ("132.248.10.2") returns 'dns2.unam.mx'
# but watch it: it will block until libc timeouts or a result is found. If
# nothing found, returns the address.
sub gst_network_lookup_address_block
{
  my ($addr) = @_;
  my $name;

  $name = (gethostbyaddr (&gst_network_ipv4_str2vec ($addr), AF_INET))[0];
  return $addr if length ($name) == 0;
  return $name;
}


# To be called from gst_process_fork by the child.
sub gst_network_address_lookup_call
{
  my ($addr) = @_;

  return &gst_network_lookup_address_block ($addr);
}


# Fork a lookup process for every passed address, return ref to
# array with info on every proc.
sub gst_network_lookup_address_start
{
  my (@addrs) = @_;
  my ($addr, $proc, @procs);

  # Fork a lookup for every address.
  foreach $addr (@addrs)
  {
    $proc = &gst_process_fork (\&gst_network_address_lookup_call, $addr);
    $$proc{"addr"} = $addr;
    push @procs, $proc;
  }

  return \@procs;
}

# Take a ref to a list of forked lookup processes,
# and collect the information from those ready.
sub gst_network_lookup_address_collect
{
  my ($procs) = @_;
  my ($name, @names, $proc);
  
  # For every process, if the "ready" flag is on, read info.
  # Else, populate with the given address. Then kill child and wait for it to die.
  foreach $proc (@$procs)
  {
    if ($$proc{"ready"})
    {
      sysread ($$proc{"fd"}, $name, 4096);
      push @names, $name;
    }
    else
    {
      push @names, $$proc{"addr"};
    }
    &gst_process_kill ($proc);
  }

  return @names;
}


# like, &gst_network_lookup_address (10, "132.248.10.2", "132.248.29.8") returns
# 'dns2.unam.mx', 'sphinx.nuclecu.unam.mx' after 10 seconds, in the worst case.
# If any of the given addresses is not resolved before timeout, the address will
# be returned as its name. Timeout of 0 or undef blocks.
# Timeout may be a decimal (0.5, half a second). You can split this process in
# two, so you can collect the information at the last moment, giving more time for
# the lookups to take place. Use the two functions above for this, as shown here.
sub gst_network_lookup_address
{
  my ($timeout, @addrs) = @_;
  my $procs;

  $procs = &gst_network_lookup_address_start (@addrs);
  &gst_process_list_check_ready ($timeout, $procs);
  return &gst_network_lookup_address_collect ($procs);
}


# This code is not under the portable table style because it is supposed to
# depend on wvdial.
sub gst_network_dialing_get
{
  my %vars =
      (
       "Inherits"        => "inherits",
       "Modem"           => "device",
       "Baud"            => "speed",
       "Init1"           => "init1",
       "Init2"           => "init2",
       "Phone"           => "phone",
       "Username"        => "login",
       "Password"        => "password",
       "Dial Command"    => "dialcmd",
       "SetVolume"       => "volume",
       "Stupid mode"     => "stupid",
       "Auto Reconnect"  => "persist",
       "Force Address"   => "address",
       "Auto DNS"        => "update_dns",
       "Check Def Route" => "set_default_gw",
       "GST Device"      => "gst_dev",
       "Dial Command"    => "dial_command"
       );
  my %title2type =
      (
       "Dialer (.*)" => "dialer",
       "Modem(.*)" => "modem"
       );
  
  my ($file) = @_;
  my (%ret, @sections);
  my ($i, $j, $name, $val);

  &gst_report_enter ();
  &gst_report ("network_dialing_get");
  
  @sections = &gst_parse_ini_sections ($file);
  foreach $i (@sections)
  {
    my %hash;
    
    $hash{"type"} = "unknown";
    foreach $j (keys %title2type)
    {
      if ($i =~ /$j/)
      {
        $hash{"name"} = $1;
        $hash{"type"} = $title2type{$j};
      }
    }
    
    $ret{$hash{"name"}} = \%hash;

    foreach $j (keys %vars)
    {
      $val = &gst_parse_ini ($file, $i, $j);
      $hash{$vars{$j}} = $val if ($val ne undef);
    }

    # we add the volume label, it's not a line in the wvdial config,
    # it's a hayes command in the init2 string, it can be ATM0 or ATL[1-3]
    if ($hash{"init2"} =~ /M0/)
    {
      $hash{"volume"} = 0;
    }
    elsif ($hash{"init2"} =~ /L[1-3]/)
    {
      $hash{"init2"} =~ /.*L([1-3]).*/;
      $volume = ($1);
      $hash{"volume"} = $volume;
    }
    else
    {
      $hash{"volume"} = 3;
    }

    # add the external line access number
    if ($hash{"phone"} =~ /(^.*)[wW]/)
    {
        $hash{"external_line"} = $1;
        $hash{"phone"} =~ s/^.*[wW]//;
    }
  }
  
  &gst_report_leave ();
  return \%ret;
}

# returns interface type depending on it's interface name
# types_cache is a global var for caching interface types
sub gst_network_get_interface_type
{
  my ($dev) = @_;
  my (@wireless_ifaces, $wi, $type);

  return $types_cache{$dev} if (exists $types_cache{$dev});

  #check whether interface is wireless
  $wireless_ifaces = &gst_network_get_wireless_ifaces ();
  foreach $wi (@$wireless_ifaces)
  {
    if ($dev eq $wi)
    {
      $types_cache{$dev} = "wireless";
      return $types_cache{$dev};
    }
  }

  if ($dev =~ /^(ppp|tun)/)
  {
    # check whether the proper plugin exists
    if (&gst_network_check_pppd_plugin ("capiplugin"))
    {
      $types_cache{$dev} = "isdn";
    }
    else
    {
      $types_cache{$dev} = "modem";
    }
  }
  elsif ($dev =~ /^(eth|dc|ed|bfe|em|fxp|bge|de|xl|ixgb|txp|vx|lge|nge|pcn|re|rl|sf|sis|sk|ste|ti|tl|tx|vge|vr|wb|cs|ex|ep|fe|ie|lnc|sn|xe|le|an|awi|wi|ndis|wlaue|axe|cue|kue|rue|fwe|nve)[0-9]/)
  {
    $types_cache{$dev} = "ethernet";
  }
  elsif ($dev =~ /^irlan[0-9]/)
  {
    $types_cache{$dev} = "irlan";
  }
  elsif ($dev =~ /^plip[0-9]/)
  {
    $types_cache{$dev} = "plip";
  }
  elsif ($dev =~ /^lo[0-9]?/)
  {
    $types_cache{$dev} = "loopback";
  }

  return $types_cache{$dev};
}

sub gst_network_freebsd_interfaces_get_info
{
  my ($dev, %ifaces, $fd);

  &gst_report_enter ();
  &gst_report ("network_iface_active_get");

  $fd = &gst_file_run_pipe_read ("ifconfig");
  return {} if $fd eq undef;
  
  while (<$fd>)
  {
    chomp;
    if (/^([^ \t:]+):.*(<.*>)/)
    {
      $dev = $1;
      $ifaces{$dev}{"dev"}    = $dev;
      $ifaces{$dev}{"enabled"} = 1 if ($2 =~ /[<,]UP[,>]/);
    }
    
    s/^[ \t]+//;
    if ($dev)
    {
      $ifaces{$dev}{"hwaddr"}  = $1 if /ether[ \t]+([^ \t]+)/i;
      $ifaces{$dev}{"addr"}    = $1 if /inet[ \t]+([^ \t]+)/i;
      $ifaces{$dev}{"mask"}    = $1 if /netmask[ \t]+([^ \t]+)/i;
      $ifaces{$dev}{"bcast"}   = $1 if /broadcast[ \t]+([^ \t]+)/i;
    }
  }
  
  &gst_file_close ($fd);
  &gst_report_leave ();
  return \%ifaces;
}

sub gst_network_linux_interfaces_get_info
{
  my ($dev, %ifaces, $fd);

  &gst_report_enter ();
  &gst_report ("network_iface_active_get");

  $fd = &gst_file_run_pipe_read ("ifconfig -a");
  return {} if $fd eq undef;
  
  while (<$fd>)
  {
    chomp;
    if (/^([^ \t:]+)/)
    {
      $dev = $1;
      $ifaces{$dev}{"enabled"} = 0;
      $ifaces{$dev}{"dev"}    = $dev;
    }
    
    s/^[ \t]+//;
    if ($dev)
    {
      $ifaces{$dev}{"hwaddr"}  = $1 if /HWaddr[ \t]+([^ \t]+)/i;
      $ifaces{$dev}{"addr"}    = $1 if /addr:([^ \t]+)/i;
      $ifaces{$dev}{"mask"}    = $1 if /mask:([^ \t]+)/i;
      $ifaces{$dev}{"bcast"}   = $1 if /bcast:([^ \t]+)/i;
      $ifaces{$dev}{"enabled"} = 1  if /^UP[ \t]/i;
    }
  }
  
  &gst_file_close ($fd);
  &gst_report_leave ();
  return \%ifaces;
}

sub gst_network_interfaces_get_info
{
  my (%ifaces);

  $ifaces = &gst_network_linux_interfaces_get_info   if ($$tool{"system"} eq "Linux");
  $ifaces = &gst_network_freebsd_interfaces_get_info if ($$tool{"system"} eq "FreeBSD");

  foreach $dev (keys %$ifaces)
  {
    #$$ifaces{$dev}{"enabled"} = 0 if $$ifaces{$dev}{"addr"} eq undef;
    $$ifaces{$dev}{"type"} = &gst_network_get_interface_type ($dev);

    #delete unknown ifaces
    if ($$ifaces{$dev}{"type"} eq undef)
    {
      delete $$ifaces{$dev};
    }
  }

  return $ifaces;
}

sub gst_network_active_interfaces_get
{
  my $fd;
  my @ret = ();

  &gst_report_enter ();
  &gst_report ("network_iface_active_get");

  $fd = &gst_file_run_pipe_read ("ifconfig");
  return undef if $fd eq undef;
  
  while (<$fd>)
  {
    chomp;
    s/:? .*//;
    next if /^$/;
    push @ret, $_;
  }

  &gst_file_close ($fd);

  &gst_report_leave ();
  return @ret;
}

sub gst_network_freebsd5_active_interfaces_get
{
  my ($fd, @ret);

  &gst_report_enter ();
  &gst_report ("network_iface_active_get");

  $fd = &gst_file_run_pipe_read ("ifconfig");
  return undef if ($fd eq undef);

  while (<$fd>)
  {
    chomp;
    if (/^([\w\d]*):.*[<,]UP[,>]/)
    {
      push @ret, $1;
    }
  }

  &gst_file_close ($fd);
  &gst_report_leave ();

  return @ret;
}

sub gst_network_suse70_active_interfaces_get
{
  my (@devs, $dev, @ret, $num);

  @devs = &gst_network_active_interfaces_get ();

  foreach $dev (@devs)
  {
    if ($dev ne "lo")
    {
      $num = &gst_network_suse70_parse_iface_num ("$gst_prefix/etc/rc.config", $dev);
      push @ret, $num if $num != -1;
    }
  }

  return @ret;
}
  
sub gst_network_interface_active
{
  my ($search_iface, $proc) = @_;
  my @ifaces;

  &gst_report_enter ();
  &gst_report ("network_iface_is_active", $search_iface);

  @ifaces = &$proc ();
  while ($ifaces[0] ne "")
  {
    return 1 if (shift (@ifaces) eq $search_iface);
  }

  &gst_report_leave ();
  return 0;
}

sub gst_network_interface_ensure_broadcast_and_network
{
  my ($iface) = @_;
    
  if (exists $$iface{"netmask"} &&
      exists $$iface{"address"})
  {
    if (! exists $$iface{"broadcast"})
    {
      $$iface{"broadcast"} = &gst_network_ipv4_calc_bcast ($$iface{"address"}, $$iface{"netmask"});
    }

    if (! exists $$iface{"network"})
    {
      $$iface{"network"} = &gst_network_ipv4_calc_subnet ($$iface{"address"}, $$iface{"netmask"});
    }
  }
}

sub gst_network_sysconfig_dir_ifaces_get_existing
{
  my ($dir) = @_;
  my (@ret, $i, $name);
  local *IFACE_DIR;
  
  if (opendir IFACE_DIR, "$gst_prefix/$dir")
  {
    foreach $i (readdir (IFACE_DIR))
    {
      push @ret, $1 if ($i =~ /^ifcfg-(.+)$/);
    }

    closedir (IFACE_DIR);
  }

  return \@ret;
}

sub gst_network_sysconfig_rh62_ifaces_get_existing
{
  return @{&gst_network_sysconfig_dir_ifaces_get_existing ("/etc/sysconfig/network-scripts")};
}

sub gst_network_sysconfig_rh72_ifaces_get_existing
{
  my ($ret);
  
  # This syncs /etc/sysconfig/network-scripts and /etc/sysconfig/networking
  &gst_file_run ("redhat-config-network-cmd");
  
  $ret = &gst_network_sysconfig_dir_ifaces_get_existing
      ("/etc/sysconfig/networking/profiles/default");
  
  &gst_arr_merge ($ret, 
                  &gst_network_sysconfig_dir_ifaces_get_existing
                  ("/etc/sysconfig/networking/devices"));
  
  return @$ret;
}

sub gst_network_debian_ifaces_get_existing
{
  my (@ret, @stanzas, $stanza);
  
  @stanzas = &gst_parse_interfaces_stanzas ("$gst_prefix/etc/network/interfaces", "iface");

  foreach $stanza (@stanzas)
  {
    if ($$stanza[1] eq "inet")
    {
      push @ret, $$stanza[0];
    }
  }

  return @ret;
}

sub gst_network_suse70_ifaces_get_existing
{
  my $file = "$gst_prefix/etc/rc.config";
  my ($i, $dev, @ret, $fd);

  $fd = &gst_file_open_read_from_names ($file);
  return @ret if !$fd;
  
  while ($i = <$fd>)
  {
    if ($i =~ /^[ \t]*NETDEV_([0-9]+)=/)
    {
      push @ret, $1 if &gst_parse_sh ($file, "NETDEV_$1") ne "";
    }
  }

  &gst_file_close ($fd);
  return @ret;
}

sub gst_network_suse90_ifaces_get_existing
{
  return @{&gst_network_sysconfig_dir_ifaces_get_existing ("/etc/sysconfig/network")};
}

sub gst_network_sysconfig_pld10_ifaces_get_existing
{
  return @{&gst_network_sysconfig_dir_ifaces_get_existing ("/etc/sysconfig/interfaces")};
}

sub gst_network_slackware91_ifaces_get_existing
{
  my $file = "/etc/rc.d/rc.inet1.conf";
  my $pppscript = "/etc/ppp/pppscript";
  my ($fd, $i, @ret);

  $fd = &gst_file_open_read_from_names ($file);
  return @ret if !$fd;

  while ($i = <$fd>)
  {
    if ($i =~ /^[ \t]*(IPADDR|USE_DHCP)\[([0-9]+)\][ \t]*=[ \t]*"(.*)"/)
    {
      push @ret, "eth$2" if ($3 ne "");
    }
  }

  &gst_file_close ($fd);

  if (&gst_file_exists ($pppscript))
  {
    push @ret, "ppp0";
  }

  return @ret;
}

sub gst_network_gentoo_ifaces_get_existing
{
  my $file = "/etc/conf.d/net";
  my ($fd, $i, @ret, $ppp);

  $fd = &gst_file_open_read_from_names ($file);
  return @ret if !$fd;

  while ($i = <$fd>)
  {
    if ($i =~ /^config_([a-zA-Z0-9]*)[ \t]*=/)
    {
      push @ret, $1;
    }
  }

  #now the PPP ifaces
  foreach $ppp (</etc/conf.d/net.*>)
  {
    $ppp =~ s#^/etc/conf.d/net\.##;
    push @ret, $ppp if ($ppp =~ /^ppp[0-9]$/);
  }

  return @ret;
}

sub gst_network_freebsd_ifaces_get_existing
{
  my $file = "/etc/rc.conf";
  my ($fd, $i, @ret);

  $fd = &gst_file_open_read_from_names ($file);
  return @ret if !$fd;

  while ($i = <$fd>)
  {
    if ($i =~ /^ifconfig_(.*)[ \t]*=/)
    {
      push @ret, $1;
    }
  }

  return @ret;
}

sub gst_network_suse70_parse_iface_num
{
  my ($file, $dev) = @_;
  my ($i, $ret);

  foreach $i (&gst_network_suse70_ifaces_get_existing ())
  {
    $ret = &gst_parse_sh ($file, "NETDEV_$i");
    return $i if $ret eq $dev;
  }

  &gst_debug_print_string ("\n\nWarning: gst_network_suse70_parse_iface_num: num for $dev not found.\n\n");
  return -1;
}

sub gst_network_suse70_parse_iface_auto
{
  my ($file, $iface, $key) = @_;
  my ($val, $i);
  
  $val = &gst_parse_sh_split ($file, $key, "[ \t]+");

  foreach $i (@$val)
  {
    return 1 if $i eq "_$iface";
  }

  return 0;
}

sub gst_network_suse70_replace_iface_auto
{
  my ($file, $iface, $key, $value) = @_;
  my ($val, $i, $found, @ret);
  
  $val = &gst_parse_sh_split ($file, $key, "[ \t]+");

  foreach $i (@$val)
  {
    $found = 1 if $i eq "_$iface";
    push @ret, $i unless ($i eq "_$iface" && !$value);
  }

  return 0 if ($found == $value);
  push @ret, "_$iface" if (!$found && $value);

  return &gst_replace_sh ($file, $key, join (" ", @ret));
}

sub gst_network_suse70_parse_iface_sh
{
  my ($file, $iface, $key) = @_;
  my ($val, $num);

  return &gst_parse_sh ($file, "${key}_$iface");
}

sub gst_network_suse70_replace_iface_sh
{
  my ($file, $iface, $key, $value) = @_;
  my ($val, $num);

  return &gst_replace_sh ($file, "${key}_$iface", $value);
}

sub gst_network_suse70_get_ifconfig_arg
{
  my ($file, $iface, $key) = @_;
  my ($val);

  $val = &gst_network_suse70_parse_iface_sh ($file, $iface, "IFCONFIG");
  $val =~ /[ \t]+$key[ \t]+([^ \t]+)/;
  return $1;
}

sub gst_network_suse70_set_ifconfig_arg
{
  my ($file, $iface, $key, $value) = @_;
  my ($val);

  $val = &gst_network_suse70_parse_iface_sh ($file, $iface, "IFCONFIG");
  
  return 0 if $val =~ /^dhcpclient$/ || $val =~ /^bootp$/;
  
  if (! ($val =~ s/([ \t]+$key[ \t]+)[^ \t]+/$1$value/))
  {
    $val =~ s/[ \t]+up$//;
    $val .= " " if $val =~ /[^ \t]$/;
    $val .= "$key $value up";
  }

  return &gst_network_suse70_replace_iface_sh ($file, $iface, "IFCONFIG", $val);
}

sub gst_network_suse70_set_ifconfig_ip
{
  my ($file, $iface, $value) = @_;

  $val = &gst_network_suse70_parse_iface_sh ($file, $iface, "IFCONFIG");
  
  return 0 if $val =~ /^dhcpclient$/ || $val =~ /^bootp$/;
  
  if (! ($val =~ s/^[0-9\.]+([ \t])/$value$1/))
  {
    $val = "$value $val";
    $val =~ s/[ \t]+/ /;
  }

  return &gst_network_suse70_replace_iface_sh ($file, $iface, "IFCONFIG", $val);
}

sub gst_network_get_new_dialing_dev
{
  my ($dial_hash, $type) = @_;
  my ($dial, $dev, $maxdev);

  $maxdev = -1;
  foreach $dial (keys %$dial_hash)
  {
    $dev = $ {$$dial_hash{$dial}}{"gst_dev"};
    if ($dev ne "")
    {
      if ($dev =~ s/^$type//)
      {
        $maxdev = $dev if $dev > $maxdev;
      }
    }
  }

  $maxdev ++;
  return "$type$maxdev";
}

sub gst_network_dial2iface
{
  my ($dial_hash, $name) = @_;
  my ($dial, %iface, $i);

  my %map = ("address"        => "address",
             "update_dns"     => "update_dns",
             "login"          => "login",
             "password"       => "password",
             "serial_port"    => "device",
             "serial_speed"   => "speed",
             "set_default_gw" => "set_default_gw",
             "persist"        => "persist",
             "phone_number"   => "phone",
             "dev"            => "gst_dev"
             );

  $dial = $$dial_hash{$name};
  if ($$dial{"gst_dev"} eq "")
  {
    $$dial{"gst_dev"} = "ppp0";
    # = &gst_network_get_new_dialing_dev ($dial_hash, "ppp");
  }
  else
  {
    $iface{"enabled"} = &gst_network_interface_active ($$dial{"gst_dev"},
                                                       \&gst_network_active_interfaces_get)? 1: 0;
  }

  foreach $i (keys %map)
  {
    $iface{$i} = $$dial{$map{$i}} if exists $$dial{$map{$i}};
  }

  $iface{"file"} = $iface{"name"} = $iface{"section"} = $name;
  $iface{"update_dns"} = 1 if ($iface{"update_dns"} eq "");
  $iface{"set_default_gw"} = 1 if ($iface{"set_default_gw"} eq "");
  
  return \%iface;
}

sub gst_network_iface2dial
{
  my ($dial_hash, $iface) = @_;
  my ($dial, $i);

  my %map = ("address"        => "address",
             "update_dns"     => "update_dns",
             "device"         => "serial_port",
             "speed"          => "serial_speed",
             "set_default_gw" => "set_default_gw",
             "persist"        => "persist",
             "gst_dev"        => "dev"
             );

  $dial = $$dial_hash{$$iface{"name"}};

  foreach $i (keys %map)
  {
    $$dial{$i} = $$iface{$map{$i}} if exists $$iface{$map{$i}};
  }

  $$dial{"type"} = "dialer";
}

sub gst_network_suse70_get_ppp
{
  my ($dial, $iface) = @_;
  my ($d);

  foreach $d (keys %$dial)
  {
    $$iface{$d} = &gst_network_dial2iface ($dial, $d);
  }

  return $iface;
}

sub gst_network_suse70_set_ppp
{
  my ($wvfile, $dial, $iface) = @_;
  my ($d, $i, @ppp_devs, $section);

  foreach $d (keys %$iface)
  {
    $i = $$iface{$d};
    if ($$i{"dev"} =~ /^ppp/)
    {
      my ($tmp, $wv, $name);
      
      push @ppp_devs, $i;
      
      # This takes care of connection name changes.
      $wv = $$i{"section"};
      $name = $$i{"name"};

      if ($wv ne $name)
      {
        $tmp = $$dial{$wv};
        delete $$dial{$wv};
        $$tmp{"name"} = $name;
        $$dial{$name} = $tmp;
      }
      
      # Then merges interface with dialing.
      &gst_network_iface2dial ($dial, $i);
    }
  }

  # Delete any wvdial sections that are no longer present as ifaces.
  OUTER: foreach $d (keys %$dial)
  {
    foreach $i (@ppp_devs)
    {
      next OUTER if $$i{"name"} eq $ {$$dial{$d}}{"name"};
    }

    # Don't delete the defaults: that's not very polite.
    delete $$dial{$d} unless $ {$$dial{$d}}{"name"} eq "Defaults";
  }

  return 0;
}

sub gst_network_suse70_ppp_iface_activate
{
  my ($name, $enabled);
  my ($wvdial_dod, $tool_ifconfig, @paths);

  @paths = &gst_service_sysv_get_paths ();
  $wvdial_dod = $paths[1] . "/wvdial.dod";
  $tool_ifconfig = &gst_file_locate_tool ("ifconfig");

  if ($enabled)
  {
    &gst_debug_print_string ("\n\nifup ppp iface $name\n\n");
    return -1 if &gst_file_run_bg ("$wvdial_dod start \"$name\"");
  }
  else
  {
    # Hmm... you'd  better not have more than one ppp connection active:
    # this is the only SuSE way of doing it.
    &gst_debug_print_string ("\n\nifdown ppp iface $name\n\n");
    return -1 if &gst_file_run ("$wvdial_dod stop");
  }

  return 0;
}

sub gst_network_suse70_activate_ppp
{
  my ($iface) = @_;
  my ($d, $i);

  foreach $d (keys %$iface)
  {
    $i = $$iface{$d};
    if ($$i{"dev"} =~ /^ppp/ && (! $$i{"enabled"}))
    {
      &gst_network_suse70_ppp_iface_activate ($$i{"name"}, 0);
      # deleted, so interfaces_set doesn't handle them.
      delete $$iface{$d};
    }
  }

  foreach $d (keys %$iface)
  {
    $i = $$iface{$d};
    if ($$i{"dev"} =~ /^ppp/ && $$i{"enabled"})
    {
      &gst_network_suse70_ppp_iface_activate ($$i{"name"}, 1);
      delete $$iface{$d};
    }
  }

  return 0;
}

sub gst_network_slackware91_create_pppgo
{
  my ($pppgo) = "/usr/sbin/ppp-go";
  my ($contents, $pppd, $chat);
  local *FILE;

  if (!&gst_file_exists ($pppgo))
  {
    $pppd = &gst_file_locate_tool ("pppd");
    $chat = &gst_file_locate_tool ("chat");
    
    # create a simple ppp-go from scratch
    # this script is based on the one that's created by pppsetup
    $contents  = "killall -INT pppd 2>/dev/null \n";
    $contents .= "rm -f /var/lock/LCK* /var/run/ppp*.pid \n";
    $contents .= "( $pppd connect \"$chat -v -f /etc/ppp/pppscript\") || exit 1 \n";
    $contents .= "exit 0 \n";

    &gst_file_buffer_save ($contents, $pppgo);
    chmod 0777, "$gst_prefix/$pppgo";
  }
}

sub gst_network_get_modem_volume
{
  my ($file) = @_;
  my ($volume);

  $volume = &gst_parse_chat ($file, "AT.*(M0|L[1-3])");

  return 3 if ($volume eq undef);

  $volume =~ s/^[ml]//i;
  return $volume;
}

sub gst_network_set_modem_volume_string
{
  my ($file, $key, $volume) = @_;
  my ($vol);

  if    ($volume == 0) { $vol = "ATM0" }
  elsif ($volume == 1) { $vol = "ATL1" }
  elsif ($volume == 2) { $vol = "ATL2" }
  else                 { $vol = "ATL3" }

  return &gst_replace_sh ($file, $key, $vol);
}

sub gst_network_set_modem_volume
{
  my ($file, $volume) = @_;
  my $line;

  $line = &gst_parse_chat ($file, "AT([^DZ][a-z0-9&]+)");
  $line =~ s/(M0|L[1-3])//g;

  if    ($volume == 0) { $line .= "M0"; }
  elsif ($volume == 1) { $line .= "L1"; }
  elsif ($volume == 2) { $line .= "L2"; }
  else                 { $line .= "L3"; }

  return &gst_replace_chat ($file, "AT([^DZ][a-z0-9&]+)", $line);
}

sub gst_network_create_pppscript
{
  my ($pppscript) = @_;
  my ($contents);

  if (!&gst_file_exists ($pppscript))
  {
    # create a template file from scratch
    $contents  = 'TIMEOUT 60' . "\n";
    $contents .= 'ABORT ERROR' . "\n";
    $contents .= 'ABORT BUSY' . "\n";
    $contents .= 'ABORT VOICE' . "\n";
    $contents .= 'ABORT "NO CARRIER"' . "\n";
    $contents .= 'ABORT "NO DIALTONE"' . "\n";
    $contents .= 'ABORT "NO DIAL TONE"' . "\n";
    $contents .= 'ABORT "NO ANSWER"' . "\n";
    $contents .= '"" "ATZ"' . "\n";
    $contents .= '"" "AT&FH0"' . "\n";
    $contents .= 'OK-AT-OK "ATDT000000000"' . "\n";
    $contents .= 'TIMEOUT 75' . "\n";
    $contents .= 'CONNECT' . "\n";

    &gst_file_buffer_save ($contents, $pppscript);
  }
}

sub gst_network_create_isdn_options
{
  my ($file) = @_;

  if (!&gst_file_exists ($file))
  {
    &gst_file_copy_from_stock ("general_isdn_ppp_options", $file);
  }
}

sub gst_network_check_pppd_plugin
{
  my ($plugin) = @_;
  my ($version, $output);

  $version = &gst_file_run_backtick ("pppd --version", 1);
  $version =~ s/.*version[ \t]+//;
  chomp $version;

  return 0 if !version;
  return &gst_file_exists ("/usr/lib/pppd/$version/$plugin.so");
}

sub gst_network_gentoo_parse_gateway
{
  my ($file, $iface) = @_;
  my ($str);

  $str = &gst_parse_confd_net ($file, "routes_$iface");

  if ($str =~ /default[ \t]+(via|gw)[ \t]+([0-9\.\:]*)/)
  {
    return $2;
  }
}

sub gst_network_interfaces_get
{
  my (%dist_attrib, %config_hash, %hash, %fn);
  my (@config_ifaces, $iface, $dev);
  my ($dist, $value, $file, $proc);
  my ($i, $j);
  my ($modem_settings);

  $hash = &gst_network_interfaces_get_info ();
  %dist_attrib = &gst_network_get_interface_parse_table ();
  %fn = %{$dist_attrib{"fn"}};
  $proc = $dist_attrib{"ifaces_get"};
  @ifaces = &$proc ();

  # clear unneeded hash elements
  foreach $i (keys %$hash)
  {
    delete $$hash{$i}{"addr"};
    delete $$hash{$i}{"bcast"};
    delete $$hash{$i}{"mask"};
  }

  foreach $i (@ifaces)
  {
    foreach $j (keys (%fn))
    {
      $ {$dist_attrib{"fn"}}{$j} = &gst_parse_expand ($fn{$j}, "iface", $i);
    }

    $iface = &gst_parse_from_table ($dist_attrib{"fn"},
                                    $dist_attrib{"table"});
    &gst_network_interface_ensure_broadcast_and_network ($iface);
    $$iface{"file"} = $i if ($$iface{"file"} eq undef);

    $dev = $$iface{"dev"};
    delete $$iface{"dev"};

    if (exists $$hash{$dev})
    {
      $$hash{$dev}{"configuration"} = $iface;
    }
    elsif (($dev eq "ppp0") || ($dev eq "tun0"))
    {
      $modem_settings = $iface;
    }
  }

  # only show PPP and ISDN devices if pppd exists
  # and they aren't configured yet
  $dev = "ppp0" if ($$tool{"system"} eq "Linux");
  $dev = "tun0" if ($$tool{"system"} eq "FreeBSD");

  if (!exists $$hash{$dev} && &gst_file_locate_tool ("pppd"))
  {
    $$hash{$dev}{"dev"} = $dev;
    $$hash{$dev}{"enabled"} = 0;
    $$hash{$dev}{"type"} = &gst_network_get_interface_type ($dev);
    $$hash{$dev}{"configuration"} = $modem_settings if ($modem_settings);
  }

  return \%$hash;
}

sub gst_network_conf_get
{
  my %dist_attrib;
  my $hash;

  %dist_attrib = &gst_network_get_parse_table ();

  $hash = &gst_parse_from_table ($dist_attrib{"fn"},
                                 $dist_attrib{"table"});
  return $hash;
}

sub gst_network_rh_get_smb_desc
{
  my ($file, $section, $var, $hostname) = @_;
  my $val;

  $val = &gst_parse_ini ($file, $section, $var);
  if (($val =~ /^Samba Server/) && ($hostname ne undef))
  {
    return $hostname;
  }

  return $val;
}

sub gst_network_rh62_get_file
{
  my ($iface) = @_;
  my ($pre, $i, $dev);

  $dev = $$iface{"dev"};
  $pre = "$gst_prefix/etc/sysconfig/network-scripts/ifcfg-";
  
  return $dev if ! -e "$pre$dev";
  
  $dev =~ s/[0-9]*$//;

  $i = 0;
  $i ++ while (-e "$pre$dev$i");
  
  return "$dev$i";
}

sub gst_network_rh72_get_file
{
  my ($iface) = @_;
  my ($pre, $i, $dev);

  $dev = $$iface{"dev"};  
  $pre = "$gst_prefix/etc/sysconfig/networking/devices/ifcfg-";
  
  return $dev if ! -e "$pre$dev";
  
  $i = 0;
  $i ++ while (-e "$pre$dev.$i");
  
  return "$dev.$i";
}

sub gst_network_deb22_get_file
{
  my ($iface) = @_;
  
  return $$iface{"dev"};
}

sub gst_network_suse70_get_file
{
  my ($i, $file);

  $file = "$gst_prefix/etc/rc.config";

  $i = 0;
  $i ++ while (&gst_parse_sh ($file, "NETDEV_$i") ne "");
  
  return $i;
}

sub gst_network_pld10_get_file
{
  my ($iface) = @_;
  my ($pre, $i, $dev);
                                                                                
  $dev = $$iface{"dev"};
  $pre = "$gst_prefix/etc/sysconfig/interfaces/ifcfg-";
                                                                                
  return $dev if ! -e "$pre$dev";
                                                                                
  $dev =~ s/[0-9]*$//;
                                                                                
  $i = 0;
  $i ++ while (-e "$pre$dev$i");
                                                                                
  return "$dev$i";
}

sub gst_network_get_file
{
  my ($iface) = @_;
  my $proc;

  my %dist_map =
	 (
          "redhat-6.0"   => \&gst_network_rh62_get_file,
          "redhat-6.1"   => \&gst_network_rh62_get_file,
          "redhat-6.2"   => \&gst_network_rh62_get_file,
          "redhat-7.0"   => \&gst_network_rh62_get_file,
          "redhat-7.1"   => \&gst_network_rh62_get_file,
          "redhat-7.2"   => \&gst_network_rh72_get_file,
          "redhat-8.0"   => \&gst_network_rh72_get_file,
          "redhat-9"     => \&gst_network_rh72_get_file,
          "openna-1.0"   => \&gst_network_rh62_get_file,
          "mandrake-7.2" => \&gst_network_rh62_get_file,
          "mandrake-7.1" => \&gst_network_rh62_get_file,
          "mandrake-9.0" => \&gst_network_rh62_get_file,
          "mandrake-9.1" => \&gst_network_rh62_get_file,
          "mandrake-9.2" => \&gst_network_rh62_get_file,
          "mandrake-10.0" => \&gst_network_rh62_get_file,
          "mandrake-10.1" => \&gst_network_rh62_get_file,
          "mandrake-10.2" => \&gst_network_rh62_get_file,
          "mandriva-2006.0" => \&gst_network_rh62_get_file,
          "mandriva-2006.1" => \&gst_network_rh62_get_file,
          "mandriva-2007.0" => \&gst_network_rh62_get_file,
	  "mandriva-2007.1" => \&gst_network_rh62_get_file,
          "yoper-2.2"       => \&gst_network_rh62_get_file,
          "blackpanther-4.0" => \&gst_network_rh62_get_file,
          "conectiva-9"  => \&gst_network_rh62_get_file,
          "conectiva-10" => \&gst_network_rh62_get_file,
          "debian-2.2"   => \&gst_network_deb22_get_file,
          "debian-3.0"   => \&gst_network_deb22_get_file,
          "debian-3.1"   => \&gst_network_deb22_get_file,
          "debian-4.0"   => \&gst_network_deb22_get_file,
          "debian-5.0"   => \&gst_network_deb22_get_file,
          "debian-testing" => \&gst_network_deb22_get_file,
          "ubuntu-5.04"  => \&gst_network_deb22_get_file,    
          "ubuntu-5.10"  => \&gst_network_deb22_get_file,    
          "ubuntu-6.06"  => \&gst_network_deb22_get_file,    
          "ubuntu-6.10"  => \&gst_network_deb22_get_file,    
          "ubuntu-7.04"  => \&gst_network_deb22_get_file,    
          "ubuntu-7.10"  => \&gst_network_deb22_get_file,    
          "ubuntu-8.04"  => \&gst_network_deb22_get_file,    
          "suse-7.0"     => \&gst_network_suse70_get_file,
          "suse-9.0"     => \&gst_network_deb22_get_file,
          "suse-9.1"     => \&gst_network_deb22_get_file,
          "turbolinux-7.0"   => \&gst_network_rh62_get_file,
          "pld-1.0"      => \&gst_network_pld10_get_file,
          "pld-1.1"      => \&gst_network_pld10_get_file,
          "pld-1.99"     => \&gst_network_pld10_get_file,
          "fedora-1"     => \&gst_network_rh72_get_file,
          "fedora-2"     => \&gst_network_rh72_get_file,
          "fedora-3"     => \&gst_network_rh72_get_file,
          "fedora-4"     => \&gst_network_rh72_get_file,
          "fedora-5"     => \&gst_network_rh72_get_file,
          "rpath"        => \&gst_network_rh72_get_file,
          "vine-3.0"     => \&gst_network_rh62_get_file,
          "vine-3.1"     => \&gst_network_rh62_get_file,
          "ark"          => \&gst_network_rh62_get_file,
          "slackware-9.1.0" => \&gst_network_deb22_get_file,
          "slackware-10.0.0" => \&gst_network_deb22_get_file,
          "slackware-10.1.0" => \&gst_network_deb22_get_file,
          "slackware-10.2.0" => \&gst_network_deb22_get_file,
          "gentoo"       => \&gst_network_deb22_get_file,
          "freebsd-5"    => \&gst_network_deb22_get_file,
          "freebsd-6"    => \&gst_network_deb22_get_file,
	 );

  $proc = $dist_map{$gst_dist};

  return &$proc ($iface) if $proc;

  # FIXME: should make us bail.
  &gst_debug_print_string ("\n\nWarning: No network_get_file proc for dist $gst_dist.\n\n");
  return undef;
}

sub gst_network_get_gateway_data
{
  my ($fd, $gateway, $dev);

  $fd = &gst_file_run_pipe_read ("route -n");
  while (<$fd>)
  {
    if (/^0\.0\.0\.0[\t ]+([^\t ]+).*[ \t]([a-zA-Z0-9]*)/)
    {
      $gateway = $1;
      $dev = $2;
      last;
    }
  }

  &gst_file_close ($fd);
  return ($gateway, $dev);
}

sub gst_network_get_default_gatewaydev
{
  my ($gateway, $dev) = &gst_network_get_gateway_data ();

  return $dev;
}

sub gst_network_get_default_gateway
{
  my ($gateway, $dev) = &gst_network_get_gateway_data ();

  return $gateway;
}

sub gst_network_route_set_default_gw
{
  my ($gatewaydev, $gateway) = @_;
  my ($tool_route, $curr_gateway, $curr_gatewaydev, $fd);

  # Just in case. This means that no static gateway is needed.
  return if $gateway eq "";

  $fd = &gst_file_run_pipe_read ("route -n");
  while (<$fd>)
  {
    if (/^0\.0\.0\.0[ \t]+([0-9.]+) /)
    {
      $curr_gateway = $1;
      if ($gatewaydev ne "")
      {
        /([a-z0-9]+)$/;
        $curr_gatewaydev = $1;
      }
      
      last;
    }
  }
  &gst_file_close ($fd);

  if (($curr_gateway    ne $gateway) ||
      ($curr_gatewaydev ne $gatewaydev))
  {
    &gst_file_run ("route del default gw $curr_gateway");
    &gst_file_run ("route add default gw $gateway $gatewaydev");
  }
}

sub gst_network_run_hostname
{
  my ($hostname) = @_;

  &gst_report_enter ();
  &gst_report ("network_hostname_set", $hostname);
  &gst_file_run ("hostname $hostname");
  &gst_report_leave ();
}

sub gst_network_dialing_get_section_name
{
  my ($dial) = @_;
  
  my %type2title =
	 (
	  "dialer" => "Dialer ",
	  "modem" => "Modem ",
	  "unknown" => ""
	  );

  return $type2title{$$dial{"type"}} . $$dial{"name"};
}

sub gst_network_dialing_set
{
  my ($file, $old_hash, $values_hash) = @_;
  my %vars =
	 (
	  "inherits"       => "Inherits",
	  "device"         => "Modem",
	  "speed"          => "Baud",
	  "init1"          => "Init1",
	  "init2"          => "Init2",
	  "phone"          => "Phone",
	  "login"          => "Username",
	  "password"       => "Password",
	  "dialcmd"        => "Dial Command",
	  "setvol"         => "SetVolume",
	  "stupid"         => "Stupid mode",
    "persist"        => "Auto Reconnect",
    "address"        => "Force Address",
    "update_dns"     => "Auto DNS",
    "set_default_gw" => "Check Def Route",
    "gst_dev"        => "GST Device",
    "dial_command"   => "Dial Command"
	  );

  my (%hash, $section);
  my $ret;
  my ($i, $j);

  &gst_report_enter ();
  &gst_report ("network_dialing_set");
  &gst_file_run ("wvdialconf $gst_prefix/$file") if (!-e "$gst_prefix/$file");

  # Remove those that are in old, but not in values.
  $old_hash = $$old_hash{"dialing"};
  OUTER: foreach $i (keys %$old_hash)
  {
    foreach $j (keys %$values_hash)
    {
      next OUTER if ($j eq $i) && ($ {$$values_hash{$j}}{"type"} eq $ {$$old_hash{$i}}{"type"});
    }

    $section = gst_network_dialing_get_section_name ($$old_hash{$i});
    $ret = -1 if &gst_replace_remove_ini_section ($file, $section);
  }

  # Now save the current stuff.
  foreach $i (keys %$values_hash)
  {
    %hash = %{$$values_hash{$i}};

    $section = gst_network_dialing_get_section_name (\%hash);
    delete $hash{"type"};
    delete $hash{"name"};

    # if init1 doesnt exist add one with ATZ
    if (!$hash{"init1"})
    {
      $hash{"init1"} = "ATZ";
    }

    # add the volume settings to init2
    if ($hash{"init2"} !~ /^(AT)/)
    {
      $hash{"init2"} = "AT";
    }
    $hash {"init2"} =~ s/L[1-3]//;
    $hash {"init2"} =~ s/M[0-2]//;
    if ($hash{"volume"} == 0)
    {
      $hash{"init2"} .= "M0";
    }
    else
    {
      $volume = $hash{"volume"};
      $hash {"init2"} .= "L$volume";
    }
    delete $hash {"volume"};

    # join again the "external_line" and "phone" tags
    if ($hash{"external_line"} ne undef)
    {
        $hash{"phone"} = $hash{"external_line"} . "W" . $hash{"phone"};
        delete $hash{"external_line"};
    }

    foreach $j (keys %hash)
    {
      $ret = -1 if &gst_replace_ini ($file, $section, $vars{$j}, $hash{$j});
    }
  }

  &gst_report_leave ();

  chmod (0600, $file) if !$ret;
  
  return $ret;
}

# This works for all systems that have ifup/ifdown scripts.
sub gst_network_rh62_interface_activate_by_dev
{
  my ($dev, $enabled) = @_;

  &gst_report_enter ();

  if ($enabled)
  {
    &gst_report ("network_iface_activate", $dev);
    return -1 if &gst_file_run ("ifup $dev");
  }
  else
  {
    &gst_report ("network_iface_deactivate", $dev);
    return -1 if &gst_file_run ("ifdown $dev");
  }
  
  &gst_report_leave ();

  return 0;
}

sub gst_network_rh62_interface_activate
{
  my ($hash, $old_hash, $enabled, $force) = @_;
  my $dev;

  if ($force || &gst_network_interface_changed ($hash, $old_hash))
  {
      if (exists $$hash{"configuration"}{"file"})
      {
          $dev = $$hash{"configuration"}{"file"};
      }
      else
      {
          $dev = $$hash{"dev"};
      }

      &gst_network_rh62_interface_activate_by_dev ($dev, $enabled);
  }
}

sub gst_network_suse9_interface_activate
{
  my ($hash, $old_hash, $enabled, $force) = @_;
  my ($iface, $dev);

  if ($force || &gst_network_interface_changed ($hash, $old_hash))
  {
    if (exists $$hash{"configuration"}{"file"})
    {
      $iface = $$hash{"configuration"}{"file"};
      $dev = &gst_network_suse9_get_dev_name ($iface);
    }
    else
    {
      $dev = $$hash{"dev"};
    }
      
    &gst_network_rh62_interface_activate_by_dev ($dev, $enabled);
  }
}

sub gst_network_suse70_interface_activate_by_dev
{
  my ($dev, $enabled) = @_;
  my ($network, @paths);

  @paths = &gst_service_sysv_get_paths ();
  $network = $paths[1] . "/network";

  &gst_report_enter ();
  
  if ($enabled)
  {
    &gst_report ("network_iface_deactivate", $hash{"dev"});
    return -1 if &gst_file_run_bg ("$network start $dev");
  }
  else
  {
    &gst_report ("network_iface_activate", $hash{"dev"});
    return -1 if &gst_file_run ("$network stop $dev");
  }
  
  &gst_report_leave ();

  return 0;
}

sub gst_network_suse70_interface_activate
{
  my ($hash, $old_hash, $enabled, $force) = @_;

  if ($force || &gst_network_interface_changed ($hash, $old_hash))
  {
    &gst_network_rh62_interface_activate_by_dev ($$hash{"dev"}, $enabled);
  }
}

sub gst_network_slackware91_interface_activate_by_dev
{
  my ($dev, $enabled) = @_;
  my ($address, $netmask, $gateway);
  my ($file) = "/etc/rc.d/rc.inet1.conf";
  my ($ret) = 0;

  &gst_report_enter ();

  if ($enabled)
  {
    &gst_report ("network_iface_activate", $dev);

    if ($dev =~ /^ppp/)
    {
      $ret = &gst_file_run ("ppp-go");
    }
    else
    {
      if (&gst_parse_rcinet1conf_bool ($file, $dev, USE_DHCP))
      {
        # Use DHCP
        $ret = &gst_file_run ("dhclient $dev");
      }
      else
      {
        $address = &gst_parse_rcinet1conf   ($file, $dev, "IPADDR");
        $netmask = &gst_parse_rcinet1conf   ($file, $dev, "NETMASK");
        $gateway = &gst_network_get_gateway ($file, "GATEWAY", $address, $netmask);

        $ret = &gst_file_run ("ifconfig $dev $address netmask $netmask up");

        # Add the gateway if necessary
        if ($gateway ne undef)
        {
          &gst_file_run ("route add default gw $gateway");
        }
      }
    }
  }
  else
  {
    &gst_report ("network_iface_deactivate", $dev);

    $ret = &gst_file_run ("ifconfig $dev down") if ($dev =~ /^eth/);
    $ret = &gst_file_run ("ppp-off") if ($dev =~ /^ppp/);
  }

  &gst_report_leave ();
  return -1 if ($ret != 0);
  return 0;
}

sub gst_network_slackware91_interface_activate
{
  my ($hash, $old_hash, $enabled, $force) = @_;
  my $dev = $$hash{"configuration"}{"file"};

  if ($force || &gst_network_interface_changed ($hash, $old_hash))
  {
    &gst_network_slackware91_interface_activate_by_dev ($dev, $enabled);
  }
}

sub gst_network_gentoo_interface_activate_by_dev
{
  my ($dev, $enabled) = @_;
  my $file = "/etc/init.d/net.$dev";
  my $action = ($enabled == 1)? "start" : "stop";

  return &gst_file_run ("$file $action");
}

sub gst_network_gentoo_interface_activate
{
  my ($hash, $old_hash, $enabled, $force) = @_;
  my $dev = $$hash{"configuration"}{"file"};

  if ($force || &gst_network_interface_changed ($hash, $old_hash))
  {
    &gst_network_gentoo_interface_activate_by_dev ($dev, $enabled);
  }
}

sub gst_network_freebsd_interface_activate_by_dev
{
  my ($hash, $enabled) = @_;
  my ($dev)     = $$hash{"configuration"}{"file"};
  my ($startif) = "/etc/start_if.$dev";
  my ($file)    = "/etc/rc.conf";
  my ($command, $dhcp_flags, $defaultroute, $fd);

  if ($enabled)
  {
    # Run the /etc/start_if.$dev commands
    $fd = &gst_file_open_read_from_names ($startif);

    while (<$fd>)
    {
      `$_`;
    }

    &gst_file_close ($fd);
    $command = &gst_parse_sh ($file, "ifconfig_$dev");

    # Bring up the interface
    if ($command =~ /DHCP/i)
    {
      $dhcp_flags = &gst_parse_sh ($file, "dhcp_flags");
      &gst_file_run ("dhclient $dhcp_flags $dev");
    }
    else
    {
      &gst_file_run ("ifconfig $dev $command");
    }

    # Add the default route
    $default_route = &gst_parse_sh ($file, "defaultrouter");
    &gst_file_run ("route add default $default_route") if ($default_route !~ /^no$/i);
  }
  else
  {
    &gst_file_run ("ifconfig $dev down");
  }
}

sub gst_network_freebsd_interface_activate
{
  my ($hash, $old_hash, $enabled, $force) =@_;

  if ($force || &gst_network_interface_changed ($hash, $old_hash))
  {
    &gst_network_freebsd_interface_activate_by_dev ($hash, $enabled);
  }
}

sub gst_network_remove_pap_entry
{
  my ($file, $login) = @_;
  my ($i, $buff);

  &gst_report_enter ();
  &gst_report ("network_remove_pap", $file, $login);
  
  $buff = &gst_file_buffer_load ($file);

  foreach $i (@$buff)
  {
    $i = "" if ($i =~ /^[ \t]*$login[ \t]/);
  }

  &gst_file_buffer_clean ($buff);
  &gst_report_leave ();
  return &gst_file_buffer_save ($buff, $file);
}

sub gst_network_rh62_interface_delete
{
  my ($old_hash) = @_;
  my $dev = $$old_hash{"configuration"}{"file"};
  my $file = "$gst_prefix/etc/sysconfig/network-scripts/ifcfg-$dev";
  my $login;

  &gst_network_rh62_interface_activate_by_dev ($dev, 0);

  $login = $old_hash{"configuration"}{"login"};
  if ($login ne "")
  {
    &gst_network_remove_pap_entry ("/etc/ppp/pap-secrets", $login);
    &gst_network_remove_pap_entry ("/etc/ppp/chap-secrets", $login);
  }

  &gst_file_remove ($file);
}

sub gst_network_rh72_interface_delete
{
  my ($old_hash) = @_;
  my ($dev, $login);

  $filedev = $$old_hash{"configuration"}{"file"};
  $dev     = $$old_hash{"dev"};
  $login   = $$old_hash{"configuration"}{"login"};
  
  &gst_network_rh62_interface_activate_by_dev ($filedev, 0);

  if ($login ne "")
  {
    &gst_network_remove_pap_entry ("/etc/ppp/pap-secrets", $login);
    &gst_network_remove_pap_entry ("/etc/ppp/chap-secrets", $login);
  }

  &gst_file_remove ("$gst_prefix/etc/sysconfig/networking/devices/ifcfg-$filedev");
  &gst_file_remove ("$gst_prefix/etc/sysconfig/networking/profiles/default/ifcfg-$filedev");
  &gst_file_remove ("$gst_prefix/etc/sysconfig/network-scripts/ifcfg-$dev");
 
  &gst_file_run ("redhat-config-network-cmd");
}

sub gst_network_deb22_interface_delete
{
  my ($old_hash) = @_;
  my $dev = $$old_hash{"dev"};

  &gst_network_rh62_interface_activate_by_dev ($dev, 0);
  &gst_replace_interfaces_iface_stanza_delete ("$gst_prefix/etc/network/interfaces", $dev);
  
  $login = $old_hash{"configuration"}{"login"};
  if ($login ne "")
  {
    &gst_network_remove_pap_entry ("/etc/ppp/pap-secrets", $login);
    &gst_network_remove_pap_entry ("/etc/ppp/chap-secrets", $login);
  }
}

sub gst_network_suse70_interface_delete
{
  my ($old_hash) = @_;
  my $dev = $$old_hash{"configuration"}{"file"};
  my $file = "$gst_prefix/etc/rc.config";
  my $login;

  &gst_network_suse70_interface_activate_by_dev ($$old_hash{"dev"}, 0);

# Commented until I know what to do with ppp connections.
#  $login = $old_hash{"login"};
#  if ($login ne "")
#  {
#    &gst_network_remove_pap_entry ("/etc/ppp/pap-secrets", $login);
#    &gst_network_remove_pap_entry ("/etc/ppp/chap-secrets", $login);
#  }

  &gst_replace_sh ($file, "IPADDR_$dev", "");
  &gst_replace_sh ($file, "NETDEV_$dev", "");
  &gst_replace_sh ($file, "IFCONFIG_$dev", "");
}

sub gst_network_suse90_interface_delete
{
  my ($old_hash) = @_;
  my $file = $$old_hash{"configuration"}{"file"};
  my $provider = &gst_parse_sh ("$gst_prefix/etc/sysconfig/network/ifcfg-$file", PROVIDER);
  my $dev = &gst_network_suse9_get_dev_name ($file);

  &gst_network_rh62_interface_activate_by_dev ($dev, 0);

  &gst_file_remove ("$gst_prefix/etc/sysconfig/network/ifroute-$file");
  &gst_file_remove ("$gst_prefix/etc/sysconfig/network/ifcfg-$file");
  &gst_file_remove ("$gst_prefix/etc/sysconfig/network/providers/$provider");
}

sub gst_network_pld10_interface_delete
{
  my ($old_hash) = @_;
  my $dev = $$old_hash{"configuration"}{"file"};
  my $file = "$gst_prefix/etc/sysconfig/interfaces/ifcfg-$dev";
  my $login;
                                                                                
  &gst_network_rh62_interface_activate_by_dev ($dev, 0);
                                                                                
  $login = $old_hash{"configuration"}{"login"};
  if ($login ne "")
  {
    &gst_network_remove_pap_entry ("/etc/ppp/pap-secrets", $login);
    &gst_network_remove_pap_entry ("/etc/ppp/chap-secrets", $login);
  }
                                                                                
  &gst_file_remove ($file);
}

sub gst_network_slackware91_interface_delete
{
  my ($old_hash) = @_;
  my $rcinetconf = "$gst_prefix/etc/rc.d/rc.inet1.conf";
  my $rcinet = "$gst_prefix/etc/rc.d/rc.inet1";
  my $pppscript = "$gst_prefix/etc/ppp/pppscript";
  my $dev = $$old_hash {"dev"};

  # remove ifup/ppp-go at startup if existing
  &gst_network_slackware91_set_auto ($rcinet, $dev);

  if ($dev =~ /^eth/)
  {
    # empty the values
    &gst_replace_rcinet1conf ($rcinetconf, $dev, "IPADDR", "");
    &gst_replace_rcinet1conf ($rcinetconf, $dev, "NETMASK", "");
    &gst_replace_rcinet1conf ($rcinetconf, $dev, "USE_DHCP", "");
    &gst_replace_rcinet1conf ($rcinetconf, $dev, "DHCP_HOSTNAME", "");
  }
  elsif ($dev =~ /^ppp/)
  {
    &gst_file_remove ($pppscript);
  }
}

sub gst_network_gentoo_interface_delete
{
  my ($old_hash) = @_;
  my ($dev) = $$old_hash {"dev"};
  my ($gateway) = $$old_hash {"configuration"}{"gateway"};
  my ($initfile) = "$gst_prefix/etc/init.d/net.$dev";
  my ($netconf);

  # bring down the interface and remove from init
  &gst_service_gentoo_set_status ($initfile, 1, 0);

  if ($dev =~ /^ppp/)
  {
    $netconf = "/etc/conf.d/net.$dev";
    gst_file_remove ($netconf);
  }
  else
  {
    $netconf = "/etc/conf.d/net";
    &gst_replace_sh ($netconf, "config_$dev", "");
  }
}

sub gst_network_freebsd_interface_delete
{
  my ($old_hash) = @_;
  my ($dev)      = $$old_hash{"dev"};
  my ($startif)  = "/etc/start_if.$dev";
  my ($file)     = "/etc/rc.conf";
  my ($pppconf)  = "/etc/ppp/ppp.conf";
  my ($buff, $line_no, $end_line_no, $i);

  &gst_file_run ("ifconfig $dev down");

  if ($dev =~ /^tun[0-9]+/)
  {
    # Delete the ppp.conf section
    $section = &gst_parse_startif ($startif, "ppp[ \t]+\-[^ \t]+[ \t]+([^ \t]+)");

    $buff = &gst_file_buffer_load ($pppconf);

    $line_no     = &gst_parse_pppconf_find_stanza      ($buff, $section);
    $end_line_no = &gst_parse_pppconf_find_next_stanza ($buff, $line_no + 1);
    $end_line_no = scalar @$buff + 1 if ($end_line_no == -1);
    $end_line_no--;

    for ($i = $line_no; $i <= $end_line_no; $i++)
    {
      delete $$buff[$i];
    }

    &gst_file_buffer_clean ($buff);
    &gst_file_buffer_save ($buff, $pppconf);
  }
  
  &gst_replace_sh  ($file, "ifconfig_$dev", "");
  &gst_file_remove ($startif);
}

sub gst_network_interface_changed
{
    my ($iface, $iface_old) = @_;

    delete $$iface{"type"};
    delete $$iface_old{"type"};
    return !&gst_util_struct_eq ($iface, $iface_old);
}

# silly function for leveling the configuration hash
# with the interface hash
sub level_interface_hash
{
  my ($hash) = @_;
  my (%config, $i);

  $config = $$hash{"configuration"};

  foreach $i (keys %$config)
  {
    $$hash{$i} = $$config{$i};
  }
}

sub gst_network_interface_set
{
  my ($dev, $values_hash, $old_hash) = @_;
  my (%dist_attrib, %fn);
  my ($proc, $i, $res);

  &gst_report_enter ();
  &gst_report ("network_iface_set", $dev);
  %dist_attrib = &gst_network_get_interface_replace_table ();
  $proc = $dist_attrib{"iface_set"};
  %fn = %{$dist_attrib{"fn"}};

  level_interface_hash (\%$values_hash);
  level_interface_hash (\%$old_hash);

  foreach $i (keys (%fn))
  {
    $ {$dist_attrib{"fn"}}{$i} = &gst_parse_expand ($fn{$i}, "iface", $dev);
  }

  $res = &gst_replace_from_table ($dist_attrib{"fn"}, $dist_attrib{"table"},
                                  $values_hash, $old_hash);

  # if success saving the settings for the interface, set up immediatly.
  &$proc ($values_hash, $old_hash, $$values_hash{"enabled"}, 0) if !$res;

  &gst_report_leave ();

  return $res;
}

sub gst_network_interfaces_set
{
  my ($old_hash, $values_hash) = @_; 
  my (%dist_attrib, @ifaces, %fn);
  my ($value, $proc);
  my ($i, $j);
  my ($tmp, $res);
  my ($delete_proc, $set_proc);
  my ($was_active);

  &gst_report_enter ();
  &gst_report ("network_ifaces_set");
  
  %dist_attrib = &gst_network_get_interface_replace_table ();
  $old_hash = $$old_hash{"interface"};

  $delete_proc = $dist_attrib{"iface_delete"};
  $set_proc    = $dist_attrib{"iface_set"};

  foreach $i (keys %$values_hash)
  {
    # delete it if it's no longer configured
    if (exists $$old_hash{$i}{"configuration"} &&
        not exists $$values_hash{$i}{"configuration"})
    {
      &$set_proc ($$values_hash{$i}, $$old_hash{$i}, 0, 1);
      &$delete_proc ($$old_hash{$i});
    }
    elsif (exists $$values_hash{$i}{"configuration"})
    {
      if (&gst_network_interface_changed ($$values_hash{$i}, $$old_hash{$i}))
      {
        $was_active = $$values_hash{$i}{"enabled"};

        &$set_proc ($$values_hash{$i}, $$old_hash{$i}, 0, 1);
        $tmp = &gst_network_interface_set ($i, $$values_hash{$i}, $$old_hash{$i});
        $res = $tmp if !$res;

        if ($was_active == 1)
        {
          &$set_proc ($$values_hash{$i}, $$old_hash{$i}, 1, 1);
        }
      }
    }
  }

  &gst_report_leave ();
  return $res;
}

sub gst_network_conf_set
{
  my $values_hash = $_[0];
  my $old_hash;
  my %dist_attrib;
  
  $old_hash = &gst_network_conf_get ();

  &gst_network_ensure_loopback ($values_hash, $old_hash);

  %dist_attrib = &gst_network_get_replace_table ();
  $res = &gst_replace_from_table ($dist_attrib{"fn"}, $dist_attrib{"table"},
                                  $values_hash, $old_hash);
  return $res;
}

sub gst_network_get_pap_passwd
{
  my ($file, $login) = @_;
  my (@arr, $passwd);

  $login = '"?' . $login . '"?';
  &gst_report_enter ();
  &gst_report ("network_get_pap_passwd", $login, $file);
  $arr = &gst_parse_split_first_array ($file, $login, "[ \t]+", "[ \t]+");

  $passwd = $$arr[1];
  &gst_report_leave ();

  $passwd =~ s/^\"([^\"]*)\"$/$1/;
  
  return $passwd;
}

sub gst_network_set_pap_passwd
{
  my ($file, $login, $passwd) = @_;
  my ($line);

  $login = '"' . $login . '"';
  $passwd = '"'. $passwd . '"';
  $line = "* $passwd";

  return &gst_replace_split ($file, $login, "[ \t]+", $line);
}

# These functions do not honour the file nor call directives.
sub gst_network_get_ppp_options_re
{
  my ($file, $re) = @_;
  my ($fd, @res);

  &gst_report_enter ();
  &gst_report ("network_get_ppp_option", &gst_replace_regexp_to_separator ($re), $file);
  $fd = &gst_file_open_read_from_names ("$file");
  &gst_report_leave ();
  return undef if !$fd;

  while (($_ = &gst_parse_chomp_line_hash_comment ($fd)) != -1)
  {
    $_ = $$_;

    if (/$re/)
    {
      return $1;
    }
  }

  return undef;
}

sub gst_network_set_ppp_options_re
{
  my ($file, $re, $value) = @_;
  my ($buff, $line, $replaced, $ret);
  my ($pre_space, $post_comment);

  &gst_report_enter ();
  &gst_report ("network_set_ppp_option", &gst_replace_regexp_to_separator ($re), $file);

  $buff = &gst_file_buffer_load ($file);

  foreach $line (@$buff)
  {
    $pre_space = $post_comment = "";
    chomp $line;
    $pre_space = $1 if $line =~ s/^([ \t]+)//;
    $post_comment = $1 if $line =~ s/([ \t]*\#.*)//;
    
    if ($line =~ /$re/)
    {
      $line = "$value\n";
      $replaced = 1;
      last;
    }

    $line = $pre_space . $line . $post_comment . "\n";
  }

  push @$buff, "$value\n" if !$replaced;
  
  &gst_file_buffer_clean ($buff);
  $ret = &gst_file_buffer_save ($buff, $file);
  &gst_report_leave ();
  return $ret;
}

sub gst_network_set_ppp_options_connect
{
  my ($file, $value) = @_;
  my $ret;

  &gst_report_enter ();
  &gst_report ("network_set_ppp_connect", $file);
  $ret = &gst_network_set_ppp_options_re ($file, "^connect", "connect \"/usr/sbin/chat -v -f $value\"");
  &gst_report_leave ();
  return $ret;
}

sub gst_network_get_ppp_options_unsup
{
  my ($file) = @_;
  my ($fd, $line, $res, $re);
  my @known_options = ("usepeerdns", "mtu", "mru", "user", "/dev/[^ \t]+", "[0-9]+",
                       "defaultroute", "debug", "persist", "escape", "crtscts", "connect",
                       "remotename", "hide-password", "noauth", "noipdefault", "ipparam", "name \".*\"");

  $res = "";
  &gst_report_enter ();
  &gst_report ("network_get_ppp_unsup", $file);
  $fd = &gst_file_open_read_from_names ("$file");
  &gst_report_leave ();
  return undef if !$fd;

  GET_LINE: while (($line = &gst_parse_chomp_line_hash_comment ($fd)) != -1)
  {
    $_ = $$line;
    next if /^[ \t]*$/;
      
    foreach $re (@known_options)
    {
      next GET_LINE if /^$re/;
    }
      
    $res .= "$_ ";
  }
  
  chop $res;
  
  return $res;
}

sub gst_network_set_ppp_options_unsup
{
  my ($file, $value) = @_;
  my ($buff, $line, $re, $ret);
  my ($pre_space, $post_comment);
  my @known_options = ("usepeerdns", "mtu", "mru", "user", "/dev/[^ \t]+", "[0-9]+",
                       "defaultroute", "debug", "persist", "escape", "crtscts", "connect",
                       "remotename", "hide-password", "noauth", "noipdefault", "ipparam", "name \".*\"");
  
  # The options in the last row are those that are on by default in pppd and we don't handle,
  # so we ignore them, as they are set implicitly if not specified.

  &gst_report_enter ();
  &gst_report ("network_set_ppp_unsup", $file);
  $buff = &gst_file_buffer_load ($file);

  GET_LINE: foreach $line (@$buff)
  {
    $pre_space = $post_comment = "";
    $pre_space = $1 if $line =~ s/^([ \t]+)//;
    $post_comment = $1 if $line =~ s/([ \t]*\#.*)//;

    foreach $re (@known_options)
    {
      next GET_LINE if $line =~ /^$re/;
    }
    $line = $pre_space . $post_comment . "\n";
    $line = "" if $line =~ /^[ \t]*$/;
  }
  
  $value =~ s/[ \t]+([^0-9])/\n$1/g;
  push @$buff, $value . "\n";
  
  &gst_file_buffer_clean ($buff);
  $ret = &gst_file_buffer_save ($buff, $file);
  &gst_report_leave ();
  return $ret;
}

sub gst_network_rh62_parse_bootproto
{
  my ($file, $key) = @_;
  my %rh62_to_proto_name =
	 (
	  "bootp" => "bootp",
	  "dhcp"  => "dhcp",
	  "none"  => "none"
	  );
  my $ret;

  $ret = &gst_parse_sh ($file, $key);
  
  if (!exists $rh62_to_proto_name{$ret})
  {
    &gst_report ("network_bootproto_unsup", $file, $ret);
    $ret = "none";
  }
  return $rh62_to_proto_name{$ret};
}

sub gst_network_rh62_replace_bootproto
{
  my ($file, $key, $value) = @_;
  my %proto_name_to_rh62 =
	 (
	  "bootp"    => "bootp",
	  "dhcp"     => "dhcp",
	  "none"     => "none"
	  );

  return &gst_replace_sh ($file, $key, $proto_name_to_rh62{$value});
}

sub gst_network_deb22_parse_bootproto
{
  my ($file, $iface) = @_;
  my (@stanzas, $stanza, $method, $bootproto);
  my %debian_to_proto_name =
      (
       "bootp"    => "bootp",
       "dhcp"     => "dhcp",
       "loopback" => "none",
       "ppp"      => "none",
       "static"   => "none"
       );

  &gst_report_enter ();
  @stanzas = &gst_parse_interfaces_stanzas ($file, "iface");

  foreach $stanza (@stanzas)
  {
    if (($$stanza[0] eq $iface) && ($$stanza[1] eq "inet"))
    {
      $method = $$stanza[2];
      last;
    }
  }

  if (exists $debian_to_proto_name {$method})
  {
    $bootproto = $debian_to_proto_name {$method};
  }
  else
  {
    $bootproto = "none";
    &gst_report ("network_bootproto_unsup", $method, $iface);
  }

  &gst_report_leave ();
  return $bootproto;
}

sub gst_network_deb22_replace_bootproto
{
  my ($file, $iface, $value) = @_;
  my (@stanzas, $stanza, $method, $bootproto);
  my %proto_name_to_debian =
      (
       "bootp"    => "bootp",
       "dhcp"     => "dhcp",
       "loopback" => "loopback",
       "ppp"      => "ppp",
       "none"     => "static"
       );

  my %dev_to_method = 
      (
       "lo" => "loopback",
       "ppp" => "ppp",
       "ippp" => "ppp"
       );

  foreach $i (keys %dev_to_method)
  {
    $value = $dev_to_method{$i} if $iface =~ /^$i/;
  }

  return &gst_replace_interfaces_stanza_value ($file, $iface, 2, $proto_name_to_debian{$value});
}

sub gst_network_slackware91_parse_bootproto
{
  my ($file, $iface) = @_;

  if (&gst_parse_rcinet1conf_bool ($file, $iface, USE_DHCP))
  {
    return "dhcp"
  }
  else
  {
    return "none";
  }
}

sub gst_network_slackware91_replace_bootproto
{
    my ($file, $iface, $value) = @_;

    if ($value eq "dhcp")
    {
      &gst_replace_rcinet1conf ($file, $iface, USE_DHCP, "yes");
    }
    else
    {
      &gst_replace_rcinet1conf ($file, $iface, USE_DHCP);
    }
}

sub gst_network_suse70_parse_bootproto
{
  my ($file, $iface) = @_;
  my ($ret);
  my %suse70_to_proto_name =
	 (
	  "bootp"      => "bootp",
	  "dhcpclient" => "dhcp"
	  );

  $ret = &gst_network_suse70_parse_iface_sh ($file, $iface, "IFCONFIG");
  if (!exists $suse70_to_proto_name {$ret})
  {
    &gst_report ("network_bootproto_unsup", $iface, $ret);
    return "none";
  }
  
  return $suse70_to_proto_name {$ret};
}

sub gst_network_suse70_replace_bootproto
{
  my ($file, $iface, $value) = @_;
  my ($ret, $val);
  my %proto_name_to_suse70 =
	 (
	  "bootp" => "bootp",
	  "dhcp"  => "dhcpclient",
    "none"  => ""
	  );

  $val = $proto_name_to_suse70{$value};
  return &gst_network_suse70_replace_iface_sh ($file, $iface, "IFCONFIG", $val);
}

sub gst_network_pld10_parse_bootproto
{
  my ($file, $key) = @_;
  my %pld10_to_proto_name =
    (
     "bootp" => "bootp",
     "dhcp"  => "dhcp",
     "pump"  => "pump",
     "none"  => "none"
    );
  my $ret;

  $ret = &gst_parse_sh ($file, $key);
  
  if (!exists $pld10_to_proto_name{$ret})
  {
    &gst_report ("network_bootproto_unsup", $file, $ret);
    $ret = "none";
  }
  return $pld10_to_proto_name{$ret};
}

sub gst_network_pld10_replace_bootproto
{
  my ($file, $key, $value) = @_;
  my %proto_name_to_pld10 =
    (
     "bootp"    => "bootp",
     "dhcp"     => "dhcp",
     "pump"     => "pump",
     "none"     => "none"
    );

  return &gst_replace_sh ($file, $key, $proto_name_to_pld10{$value});
}

sub gst_network_parse_bootproto
{
  my ($file, $key) = @_;

  return "dhcp"  if (&gst_parse_sh ($file, $key) =~ /dhcp/i);
  return "bootp" if (&gst_parse_sh ($file, $key) =~ /bootp/i);
  return "none";
}

sub gst_network_suse90_replace_bootproto
{
  my ($file, $key, $value) = @_;
  my %proto_name_to_suse90 =
     (
      "dhcp"     => "dhcp",
      "bootp"    => "bootp",
      "static"   => "none",
     );

  return &gst_replace_sh ($file, $key, $proto_name_to_suse90{$value});
}

sub gst_network_gentoo_parse_bootproto
{
  my ($file, $dev) = @_;

  return "dhcp" if (&gst_parse_confd_net ($file, "config_$dev") =~ /dhcp/i);
  return "none";
}

sub gst_network_gentoo_replace_bootproto
{
  my ($file, $dev, $value) = @_;

  return if ($dev =~ /^ppp/);

  return &gst_replace_confd_net ($file, "config_$dev", "dhcp") if ($value ne "none");

  # replace with a fake IP address, I know it's a hack
  return &gst_replace_confd_net ($file, "config_$dev", "0.0.0.0");
}

sub gst_network_freebsd5_replace_bootproto
{
  my ($file, $dev, $value) = @_;

  return &gst_replace_sh ($file, "ifconfig_$dev", "dhcp") if ($value ne "none");
  return &gst_replace_sh ($file, "ifconfig_$dev", "");
}

sub gst_network_pump_iface_supported
{
  my ($iface) = @_;
  my ($dev);
  my @devs = qw(eth wlan plip irlan);

  foreach $dev (@devs)
  {
    return 1 if $iface =~ /^$dev/;
  }

  return 0;
}

sub gst_network_parse_pppconf
{
  my ($pppconf, $startif, $iface, $key) = @_;
  my ($section);

  if ($iface =~ /^tun[0-9]+/)
  {
    $section = &gst_parse_startif ($startif, "ppp[ \t]+\-[^ \t]+[ \t]+([^ \t]+)");
    return undef if ($section eq undef);

    return &gst_parse_pppconf ($pppconf, $section, $key);
  }
}

sub gst_network_parse_pppconf_bool
{
  my ($pppconf, $startif, $iface, $key) = @_;
  my ($section);

  if ($iface =~ /^tun[0-9]+/)
  {
    $section = &gst_parse_startif ($startif, "ppp[ \t]+\-[^ \t]+[ \t]+([^ \t]+)");
    return undef if ($section eq undef);

    return &gst_parse_pppconf_bool ($pppconf, $section, $key);
  }
}

sub gst_network_parse_pppconf_re
{
  my ($pppconf, $startif, $iface, $key, $re) = @_;
  my ($section);

  if ($iface =~ /^tun[0-9]+/)
  {
    $section = &gst_parse_startif ($startif, "ppp[ \t]+\-[^ \t]+[ \t]+([^ \t]+)");
    return undef if ($section eq undef);

    return &gst_parse_pppconf_re ($pppconf, $section, $key, $re);
  }
}

sub gst_network_replace_pppconf
{
  my ($pppconf, $startif, $iface, $key, $val) = @_;
  my ($section);

  if ($iface =~ /^tun[0-9]+/)
  {
    $section = &gst_parse_startif ($startif, "ppp[ \t]+\-[^ \t]+[ \t]+([^ \t]+)");
    &gst_replace_pppconf ($pppconf, $section, $key, $val);
  }
}

sub gst_network_replace_pppconf_bool
{
  my ($pppconf, $startif, $iface, $key, $val) = @_;
  my ($section);

  if ($iface =~ /^tun[0-9]+/)
  {
    $section = &gst_parse_startif ($startif, "ppp[ \t]+\-[^ \t]+[ \t]+([^ \t]+)");
    &gst_replace_pppconf_bool ($pppconf, $section, $key, $val);
  }
}

sub gst_network_replace_pppconf_route
{
  my ($pppconf, $startif, $iface, $key, $val) = @_;
  my ($section);

  if ($iface =~ /^tun[0-9]+/)
  {
    $section = &gst_parse_startif ($startif, "ppp[ \t]+\-[^ \t]+[ \t]+([^ \t]+)");
    &gst_replace_pppconf_common ($pppconf, $section, $key,
                                 ($val == 1)? "add default HISADDR" : undef);
  }
}

sub gst_network_replace_pppconf_dial_command
{
  my ($pppconf, $startif, $iface, $val) = @_;
  my ($section, $dial);

  if ($iface =~ /^tun[0-9]+/)
  {
    $section = &gst_parse_startif ($startif, "ppp[ \t]+\-[^ \t]+[ \t]+([^ \t]+)");
    $dial = &gst_parse_pppconf ($pppconf, $section, "dial");
    $dial =~ s/ATD[TP]/$val/;

    &gst_replace_pppconf ($pppconf, $section, "dial", $dial);
  }
}

sub gst_network_replace_pppconf_volume
{
  my ($pppconf, $startif, $iface, $val) = @_;
  my ($section, $dial, $vol, $pre, $post);

  if ($iface =~ /^tun[0-9]+/)
  {
    $section = &gst_parse_startif ($startif, "ppp[ \t]+\-[^ \t]+[ \t]+([^ \t]+)");
    $dial = &gst_parse_pppconf ($pppconf, $section, "dial");

    if ($dial =~ /(.*AT[^ \t]*)([ML][0-3])(.* OK .*)/i)
    {
      $pre  = $1;
      $post = $3;
    }
    elsif ($dial =~ /(.*AT[^ \t]*)( OK .*)/i)
    {
      $pre  = $1;
      $post = $2;
    }

    if ($val == 0)
    {
      $vol = "M0";
    }
    else
    {
      $vol = "L$val";
    }

    $dial = $pre . $vol . $post;
    &gst_replace_pppconf ($pppconf, $section, "dial", $dial);
  }
}

sub gst_network_get_freebsd5_ppp_persist
{
  my ($startif, $iface) = @_;
  my ($val);

  if ($iface =~ /^tun[0-9]+/)
  {
    $val = &gst_parse_startif ($startif, "ppp[ \t]+\-(auto|ddial)[ \t]+");

    return 1 if ($val eq "ddial");
    return 0;
  }

  return undef;
}

sub gst_network_pump_get_nodns
{
  my ($file, $iface, $bootproto) = @_;
  
  return undef if (!&gst_network_pump_iface_supported ($iface));

  return &gst_parse_pump_get_iface_kw_not ($file, $iface, "nodns");
}

sub gst_network_pump_set_nodns
{
  my ($file, $iface, $bootproto, $value) = @_;

  return 0 if $bootproto ne "dhcp";
  return 0 if (!&gst_network_pump_iface_supported ($iface));

  return &gst_replace_pump_iface_kw_not ($file, $iface, "nodns", $value);
}

sub gst_network_debian_parse_remote_address
{
  my ($file, $iface) = @_;
  my ($str, @tuples, $tuple, @res);

  &gst_report_enter ();
  &gst_report ("network_get_remote", $iface);
  
  @tuples = &gst_parse_interfaces_option_tuple ($file, $iface, "up", 1);

  &gst_report_leave ();
  
  foreach $tuple (@tuples)
  {
    @res = $$tuple[1] =~ /[ \t]+pointopoint[ \t]+([^ \t]+)/;
    return $res[0] if $res[0];
  }

  return undef;
}

sub gst_network_debian_replace_remote_address
{
  my ($file, $iface, $value) = @_;
  my ($ifconfig, $ret);
  
  &gst_report_enter ();
  &gst_report ("network_set_remote", $iface);
  
  $ifconfig = &gst_file_locate_tool ("ifconfig");

  $ret = &gst_replace_interfaces_option_str ($file, $iface, "up", "$ifconfig $iface pointopoint $value");
  &gst_report_leave ();
  return $ret;
}

sub gst_network_debian_woody_get_auto_by_stanza
{
  my ($file, $iface) = @_;
  my (@stanzas, $stanza, $i);

  @stanzas = &gst_parse_interfaces_stanzas ($file, "auto");

  foreach $stanza (@stanzas)
  {
    foreach $i (@$stanza)
    {
      return $stanza if $i eq $iface;
    }
  }

  return undef;
}

sub gst_network_debian_woody_get_auto
{
  my ($file, $iface) = @_;

  return (&gst_network_debian_woody_get_auto_by_stanza ($file, $iface) ne undef)? 1 : 0;
}

sub gst_network_debian_woody_set_auto
{
  my ($file, $iface, $value) = @_;
  my ($buff, $line_no, $found);

  $buff = &gst_file_buffer_load ($file);
  &gst_file_buffer_join_lines ($buff);
  $line_no = 0;

  while (($found = &gst_replace_interfaces_get_next_stanza ($buff, \$line_no, "auto")) != -1)
  {
    if ($value)
    {
      if ($$buff[$line_no] =~ /[ \t]$iface([\# \t\n])/)
      {
        return &gst_file_buffer_save ($buff, $file);
      }
    }
    else
    {
      # I'm including the hash here, although the man page says it's not supported.
      last if $$buff[$line_no] =~ s/[ \t]$iface([\# \t\n])/$1/;
    }
		
		$line_no ++;
  }

  if ($found == -1)
  {
    if ($value)
    {
      &gst_replace_interfaces_auto_stanza_create ($buff, $iface);
    }
  }
  else
  {
    if ($value)
    {
      chomp $$buff[$line_no];
      $$buff[$line_no] .= " $iface\n";
    }
    $$buff[$line_no] =~ s/auto[ \t]*$//;
  }
  
  return &gst_file_buffer_save ($buff, $file);
}

# looks for eth_up $eth_iface_number
sub gst_network_slackware91_get_auto
{
  my ($file, $rclocal, $iface) = @_;
  my ($search) = 0;
  my ($buff);

  if ($iface =~ /^eth/)
  {
    $buff = &gst_file_buffer_load ($file);
    &gst_file_buffer_join_lines ($buff);

    $iface =~ s/eth//;

    foreach $i (@$buff)
    {
      if ($i =~ /^[ \t]*'start'\)/)
      {
        $search = 1;
      }
      elsif (($i =~ /^[ \t]*;;/) && ($search == 1))
      {
        return 0;
      }
      elsif (($i =~ /^[ \t]*eth_up (\S+)/) && ($search == 1))
      {
        return 1 if ($1 == $iface);
      }
    }

    return 0;
  }
  elsif ($iface =~ /^ppp/)
  {
    return &gst_parse_kw ($rclocal, "ppp-go");
  }
}

# adds or deletes eth_up $eth_iface_number
sub gst_network_slackware91_set_auto
{
  my ($file, $rclocal, $iface, $active) = @_;
  my ($search) = 0;
  my ($nline) = 0;
  my ($buff, $sline);

  if ($iface =~ /^eth/)
  {
    $buff = &gst_file_buffer_load ($file);
    &gst_file_buffer_join_lines ($buff);

    $iface =~ s/eth//;

    foreach $i (@$buff)
    {
      if ($i =~ /^[ \t]*('start'\)|\*\))/)
      {
        # if the line is 'start') or *), begin the search
        $search = 1;
      }
      elsif (($i =~ /^[ \t]*gateway_up/) && ($search == 1))
      {
        # save the line in which we're going to save the eth_up stuff
        $sline = $nline;
      }
      elsif (($i =~ /^[ \t]*(;;|esac)/) && ($search == 1))
      {
        # we've arrived to the end of the case, if we wanted to
        # add the iface, now it's the moment
        $$buff[$sline] = "\teth_up $iface\n" . $$buff[$sline] if ($active == 1);
        $search = 0;
      }
      elsif (($i =~ /^[ \t]*eth_up (\S+)/) && ($search == 1))
      {
        if ($1 == $iface)
        {
          delete $$buff[$nline] if ($active == 0);
          $search = 0;
        }
      }

      $nline++;
    }

    return &gst_file_buffer_save ($buff, $file);
  }
  elsif ($iface =~ /^ppp/)
  {
    return &gst_replace_kw ($rclocal, "ppp-go", $active);
  }
}

# finds out if a interface is active at boot time
sub gst_network_freebsd5_get_auto
{
  my ($file, $defaults_file, $iface) = @_;
  my ($val);

  $val = &gst_parse_sh ($file,          "network_interfaces");
  $val = &gst_parse_sh ($defaults_file, "network_interfaces") if ($val eq undef);

  return 1 if ($val eq "auto");
  return 1 if ($val =~ /$iface/);
  return 0;
}

sub gst_network_freebsd5_set_auto
{
  my ($file, $iface, $active) = @_;
  my ($val);

  $val  = &gst_parse_sh ($file, "network_interfaces");
  $val = &gst_file_run_backtick ("ifconfig -l") if ($val =~ /auto/);
  $val .= " ";

  if ($active && ($val !~ /$iface /))
  {
    $val .= $iface;
  }
  elsif (!$active && ($val =~ /$iface /))
  {
    $val =~ s/$iface //;
  }

  # Trim the string
  $val =~ s/^[ \t]*//;
  $val =~ s/[ \t]*$//;

  &gst_replace_sh ($file, "network_interfaces", $val);
}

sub gst_network_suse90_get_auto
{
  my ($file, $key) = @_;
  my ($ret);

  $ret = &gst_parse_sh ($file, $key);

  return 1 if ($ret =~ /^onboot$/i);
  return 0;
}

sub gst_network_suse90_set_auto
{
  my ($file, $key, $enabled) = @_;
  my ($ret);
  
  if($enabled)
  {
    return &gst_replace_sh($file, $key, "onboot");
  }
  else
  {
    return &gst_replace_sh($file, $key, "manual");
  }    
}

# Return IP address or netmask, depending on $what
sub gst_network_pld10_get_ipaddr
{
  my ($file, $key, $what) = @_;
  my ($ipaddr, $netmask, $ret, $i);
	my @netmask_prefixes = (0, 128, 192, 224, 240, 248, 252, 254, 255);
  
  $ipaddr = &gst_parse_sh($file, $key);
  return undef if $ipaddr eq "";
  
  if($ipaddr =~ /([^\/]*)\/([[:digit:]]*)/)
  {
    $netmask = $2;
    return undef if $netmask eq "";

    if($what eq "address")
    {
      return $1;
    }

    for($i = 0; $i < int($netmask/8); $i++)
    {
      $ret .= "255.";
    }

    $ret .= "$netmask_prefixes[$b%8]." if $netmask < 32;

    for($i = int($netmask/8) + 1; $i < 4; $i++)
    {
      $ret .= "0.";
    }

    chop($ret);
    return $ret;
  }
  return undef;
}

# Writes IP address or netmask, depending in $what, to $file
sub gst_network_pld10_set_ipaddr
{
  my ($file, $key, $what, $value) = @_;
  my %prefixes =
  (
    "0"   => 0,
    "128" => 1,
    "192" => 2,
    "224" => 3,
    "240" => 4,
    "248" => 5,
    "252" => 6,
    "254" => 7,
    "255" => 8
  );
  my ($ipaddr, $netmask);

  $ipaddr = &gst_parse_sh($file, $key);
  return undef if $ipaddr eq "";

  if($what eq "address")
  {
    $ipaddr =~ s/.*\//$value\//;
  }
	else
  {
    if($value =~ /([[:digit:]]*).([[:digit:]]*).([[:digit:]]*).([[:digit:]]*)/)
    {
      $netmask = $prefixes{$1} + $prefixes{$2} + $prefixes{$3} + $prefixes{$4};
      $ipaddr =~ s/\/[[:digit:]]*/\/$netmask/;
    }
  }

  return &gst_replace_sh($file, $key, $ipaddr);
}

# FIXME: this function isn't IPv6-aware
# it checks if a IP address is in the same network than another
sub gst_network_is_ip_in_same_network
{
  my ($address1, $address2, $netmask) = @_;
  my (@add1, @add2, @mask);
  my ($i);

  return 0 if (($address1 eq undef) ||
               ($address2 eq undef) ||
               ($netmask  eq undef));

  @add1 = split (/\./, $address1);
  @add2 = split (/\./, $address2);
  @mask = split (/\./, $netmask);

  for ($i = 0; $i < 4; $i++)
  {
    $add1[$i] += 0;
    $add2[$i] += 0;
    $mask[$i] += 0;

    return 0 if (($add1[$i] & $mask[$i]) != ($add2[$i] & $mask[$i]));
  }

  return 1;
}

# function that gets a gateway device from the gateway address
sub gst_network_get_gateway_dev_from_address
{
  my ($interface, $gateway) = @_;
  my ($address, $netmask, $key);

  foreach $key (keys %$interface)
  {
    $address = $$interface{$key}{"address"};
    $netmask = $$interface{$key}{"netmask"};

    return $$interface{$key}{"dev"} if (&gst_network_is_ip_in_same_network ($address, $gateway, $netmask));
  }

  return undef;
}

sub gst_network_get_plip_gateway
{
  my ($file, $key, $remote_address) = @_;
  my ($gateway);

  $gateway = &gst_parse_sh ($file, $key);

  return $gateway if ($gateway eq $remote_address);
}

sub gst_network_get_gateway
{
  my ($file, $key, $address, $netmask) = @_;
  my ($gateway);

  return undef if ($address eq undef);

  $gateway = &gst_parse_sh ($file, $key);

  return $gateway if &gst_network_is_ip_in_same_network ($address, $gateway, $netmask);
  return undef;
}

sub gst_network_suse90_get_gateway
{
  my ($file, $address, $netmask) = @_;
  my ($gateway) = &gst_parse_split_first_array_pos ($file, "default", 0, "[ \t]+", "[ \t]+");

  return $gateway if &gst_network_is_ip_in_same_network ($address, $gateway, $netmask);
  return undef;
}

sub gst_network_suse90_get_plip_gateway
{
  my ($file, $remote_address) = @_;
  my ($gateway) = &gst_parse_split_first_array_pos ($file, "default", 0, "[ \t]+", "[ \t]+");

  return $gateway if ($gateway eq $remote_address);
  return undef;
}

sub gst_network_suse90_replace_gateway
{
  my ($file, $dev, $address, $netmask, $value) = @_;

  
  return &gst_replace_split ($file, "default", "[ \t]+", "$value \- $dev") if &gst_network_is_ip_in_same_network ($address, $value, $netmask); 
#  return &gst_replace_split ($file, "default", "[ \t]+", "$value \- $dev") ; 
  return undef; 

}
    

# runs a function if the interface is of type $type
sub gst_network_check_type
{
  my ($iface) = shift @_;
  my ($type) =  shift @_;
  my ($func) =  shift @_;
  my ($t);

  $t = &gst_network_get_interface_type ($iface);

  if ($t =~ "^$type")
  {
    &$func (@_);
  }
}

# creates files neccesary for gentoo ifaces
sub gst_network_gentoo_create_files
{
  my ($dev) = @_;
  my ($init) = "/etc/init.d/net.$dev";
  my ($conf) = "/etc/conf.d/net.$dev";
  my ($backup) = "/etc/conf.d/net.ppp0.gstbackup";

  if ($dev =~ /ppp/)
  {
    &gst_file_copy ("/etc/init.d/net.ppp0", $init) if (!&gst_file_exists ($init));

    # backup the ppp config file
    &gst_file_copy ("/etc/conf.d/net.ppp0", $backup) if (!&gst_file_exists ($backup));
    &gst_file_copy ($backup, $conf) if (!&gst_file_exists ($conf));
  }
  else
  {
    &gst_file_copy ("/etc/init.d/net.eth0", $init) if (!&gst_file_exists ($init));
  }

  chmod 0755, "$gst_prefix/$init";
}

# we need this function because essid can be multiword, and thus it can't be in rc.conf
sub gst_network_freebsd5_replace_essid
{
  my ($file, $startif, $iface, $essid) = @_;

  if ($essid =~ /[ \t]/)
  {
    # It's multiword
    &gst_file_buffer_save ("ifconfig $iface ssid \"$essid\"", $startif);
    &gst_replace_sh_re ($file, "ifconfig_$iface", "ssid[ \t]+([^ \t]*)", "");
  }
  else
  {
    &gst_replace_sh_re ($file, "ifconfig_$iface", "ssid[ \t]+([^ \t]*)", " ssid $essid");
  }
}

sub gst_network_freebsd_create_ppp_startif
{
  my ($startif, $iface, $dev, $persist) = @_;
  my ($section);

  if ($dev =~ /^tun[0-9]+/)
  {
    $section = &gst_parse_startif ($startif, "ppp[ \t]+\-[^ \t]+[ \t]+([^ \t]+)");
    $section = $dev if ($section eq undef);

    return &gst_file_buffer_save ("ppp -ddial $section", $startif) if ($persist eq 1);
    return &gst_file_buffer_save ("ppp -auto  $section", $startif);
  }
}

# Functions for parsing provider file in suse 9.X
sub gst_network_suse90_parse_provider_file_func
{
  my ($provider, $key, $func) = @_;
  my ($path) = "/etc/sysconfig/network/providers/";

  return &$func ("$path/$provider", $key);
}

sub gst_network_suse90_parse_provider_file
{
  my ($provider, $key) = @_;
  return &gst_network_suse90_parse_provider_file_func ($provider, $key, \&gst_parse_sh);
}

sub gst_network_suse90_parse_provider_file_bool
{
  my ($provider, $key) = @_;
  return &gst_network_suse90_parse_provider_file_func ($provider, $key, \&gst_parse_sh_bool);
}

# Functions for replacing in provider file in SuSE 9.X
sub gst_network_suse90_replace_provider_file_func
{
  my ($provider, $key, $value, $func) = @_;
  my ($path) = "/etc/sysconfig/network/providers/";

  return &$func ("$path/$provider", $key, $value);
}

sub gst_network_suse90_replace_provider_file
{
  my ($provider, $key, $value) = @_;
  #make sure the function is called only by modem   
  if ($provider =~ /ppp/)
  {
    return &gst_network_suse90_replace_provider_file_func ($provider, $key, $value, \&gst_replace_sh);
  }
}

sub gst_network_suse90_replace_provider_file_bool
{
  my ($provider, $key, $value) = @_;
  #make sure the function is called only by modem   
  if ($provider =~ /ppp/)
  {
    return &gst_network_suse90_replace_provider_file_func ($provider, $key, $value, \&gst_replace_sh_bool);
  }
}

sub gst_network_suse9_get_dev_name
{
  my ($iface) = @_;
  my ($ifaces, $dev, $hwaddr, $d);
  my ($dev);

  $dev = &gst_parse_sh ("/var/run/sysconfig/if-$iface", "interface");

  if ($dev eq undef)
  {
    $fd = &gst_file_run_backtick ("getcfg-interface $iface");
  }

  if ($dev eq undef)
  {
    # Those are the last cases, we make rough guesses
    if ($iface =~ /-pcmcia-/)
    {
      # it's something like wlan-pcmcia-0
      $dev =~ s/-pcmcia-//;
    }
    elsif ($iface =~ /-id-([a-fA-F0-9\:]*)/)
    {
      # it's something like eth-id-xx:xx:xx:xx:xx:xx, which is the NIC MAC
      $hwaddr = $1;
      $ifaces = &gst_network_interfaces_get_info ();

      foreach $d (keys %$ifaces)
      {
        if ($hwaddr eq $$ifaces{$d}{"hwaddr"})
        {
          $dev = $d;
          last;
        }
      }
    }
  }

  if ($dev eq undef)
  {
    # We give up, take $iface as $dev
    $dev = $iface;
  }

  return $dev;
}

sub gst_network_detect_essids
{
  my ($iface) = @_;
  my ($fd, @arr, $encrypted);

  # some wireless cards need to be up before scanning
  &gst_file_run ("ifconfig $iface up");
  $fd = &gst_file_run_pipe_read ("iwlist $iface scanning");
  return undef if (!$fd);

  while (<$fd>)
  {
    if (/^[ \t]*Encryption key:([^ \t\n]+)/)
    {
      $encrypted = ($1 eq "off") ? 0 : 1;
    }
    elsif (/^[ \t]*ESSID\:"(.+)"/)
    {
      push @arr, {"essid" => $1,
                  "encrypted" => $encrypted };
    }
  }

  return \@arr;
}

sub gst_network_get_wep_key_type
{
  my ($func) = shift @_;
  my ($val);

  $val = &$func (@_);

  return undef if (!$val);
  return "ascii" if ($val =~ /^s\:/);
  return "hexadecimal";
}

sub gst_network_get_wep_key
{
  my ($func) = shift @_;
  my ($val);

  $val = &$func (@_);
  $val =~ s/^s\://;

  return $val;
}

sub gst_network_get_full_key
{
  my ($key, $key_type) = @_;

  if ($key_type eq "ascii")
  {
    $key = "s:" . $key;
  }

  return $key;
}

sub gst_network_set_wep_key_type
{
  my ($key, $key_type, $func);

  # seems kind of hackish, but we want to use distro
  # specific saving functions, so we need to leave
  # the args as variable as possible
  $func = shift @_;
  $key_type = pop @_;
  $key = pop @_;

  push @_, &gst_network_get_full_key ($key, $key_type);
  &$func (@_);
}

# Set dist_map for your distro to "" if you don't want
# loopback ensuring. See suse-7.0 entry for example.
sub gst_network_ensure_loopback_interface
{
  my ($interface) = @_;
  my $dev;
  my %dist_map =
         (
          "redhat-5.2"   => "lo",
          "redhat-6.0"   => "lo",
          "redhat-6.1"   => "lo",
          "redhat-6.2"   => "lo",
          "redhat-7.0"   => "lo",
          "redhat-7.1"   => "lo",
          "redhat-7.2"   => "lo",
          "redhat-8.0"   => "lo",
          "redhat-9"     => "",
          "openna-1.0"   => "lo",
          "mandrake-7.1" => "lo",
          "mandrake-7.2" => "lo",
          "mandrake-9.0" => "lo",
          "mandrake-9.1" => "lo",
          "mandrake-9.2" => "lo",
          "mandrake-10.0" => "lo",
          "mandrake-10.1" => "lo",
          "mandrake-10.2" => "lo",
          "mandriva-2006.0" => "lo",
          "mandriva-2006.1" => "lo",
          "mandriva-2007.0" => "lo",
	  "mandriva-2007.1" => "lo",
          "yoper-2.2"       => "lo",
          "blackpanther-4.0" => "lo",
          "conectiva-9"  => "lo",
          "conectiva-10" => "lo",
          "debian-2.2"   => "lo",
          "debian-3.0"   => "lo",
          "debian-3.1"   => "lo",
          "debian-4.0"   => "lo",
          "debian-5.0"   => "lo",
          "debian-testing" => "lo",
          "ubuntu-5.04"  => "lo",    
          "ubuntu-5.10"  => "lo",    
          "ubuntu-6.06"  => "lo",    
          "ubuntu-6.10"  => "lo",    
          "ubuntu-7.04"  => "lo",    
          "ubuntu-7.10"  => "lo",    
          "ubuntu-8.04"  => "lo",    
          "suse-7.0"     => "",
          "suse-9.0"     => "",
          "suse-9.1"     => "",
	  "turbolinux-7.0"   => "lo",
          "pld-1.0"      => "lo",
          "pld-1.1"      => "lo",
          "pld-1.99"     => "lo",
          "fedora-1"     => "",
          "fedora-2"     => "",
          "fedora-3"     => "",
          "fedora-4"     => "",
          "fedora-5"     => "",
          "rpath"        => "",
          "vine-3.0"     => "lo",
          "vine-3.1"     => "lo",
          "ark"          => "lo",
          "slackware-9.1.0" => "",
          "slackware-10.0.0" => "",
          "slackware-10.1.0" => "",
          "slackware-10.2.0" => "",
          "gentoo"       => "",
          "vlos-1.2"     => "",
          "freebsd-5"    => "",
          "freebsd-6"    => "",
         );

  $dev = $dist_map {$gst_dist};

  return if $dev eq "";

  if (!exists $$interface{$dev})
  {
    my %iface = (
                 "auto" => 1,
                 "user" => 0,
                 "dev" => "lo",
                 "address" => "127.0.0.1",
                 "netmask" => "255.0.0.0",
                 "broadcast" => "127.255.255.255",
                 "network" => "127.0.0.0",
                 "bootproto" => "none",
                 "enabled" => 1,
                 "update_dns" => 0
                 );

    $$interface{$dev} = \%iface;
    &gst_network_interface_set ($dev, \%iface);
  }
  elsif (! $ {$$interface{$dev}}{"enabled"})
  {
    $ {$$interface{$dev}}{"enabled"} = 1;
    &gst_network_interface_set ($dev, $$interface{$dev});
  }
}

sub gst_network_statichost_add_alias
{
  my ($localhost, $alias) = @_;
  my $i;

  foreach $i (@$localhost)
  {
    return if ($i eq $alias);
  }
  
  push @$localhost, $alias;
}

sub gst_network_statichost_remove_alias
{
  my ($localhost, $alias) = @_;
  my $i;

  for ($i = 0; $i < @$localhost; $i++) {
    if ($$localhost[$i] eq $alias)
    {
      delete $$localhost[$i];
      return;
    }
  }
}
  
sub gst_network_ensure_loopback_statichost
{
  my ($statichost, $hostname, $old_hostname, $lo_ip) = @_;
  my $i;

  if (exists $$statichost{$lo_ip})
  {
    my $localhost = $$statichost{$lo_ip};
    &gst_network_statichost_remove_alias ($localhost, $old_hostname) if ($old_hostname);
    &gst_network_statichost_add_alias ($localhost, $hostname);
  }
  else
  {
    $$statichost{$lo_ip} = [ ("localhost", "localhost.localdomain", $hostname) ];
  }
}

sub get_network_get_lo_ip
{
  my ($statichost) = @_;

  foreach $i (keys %$statichost)
  {
    return $i if ($i =~ /^127\./);
  }

  return "127.0.0.1";
}

sub gst_network_ensure_loopback
{
  my $values_hash = $_[0];
  my $old_values_hash = $_[1];
  my $interface = $$values_hash{"interface"};
  my $hostname = $$values_hash{"hostname"};
  my $statichost = $$values_hash{"statichost"};
  my $lo_ip = &get_network_get_lo_ip ($statichost);

  # needed for replacing hostname safely
  my $old_hostname = $$old_values_hash{"hostname"};

  &gst_report_enter ();
  &gst_report ("network_ensure_lo");

  &gst_network_ensure_loopback_statichost ($statichost, $hostname, $old_hostname, $lo_ip);
  &gst_network_ensure_loopback_interface ($interface, $lo_ip);

  &gst_report_leave ();
}

sub gst_network_get_parse_table
{
  my %dist_map =
         (
          "redhat-5.2"   => "redhat-6.2",
	  "redhat-6.0"   => "redhat-6.2",
	  "redhat-6.1"   => "redhat-6.2",
	  "redhat-6.2"   => "redhat-6.2",
	  "redhat-7.0"   => "redhat-7.0",
	  "redhat-7.1"   => "redhat-7.0",
	  "redhat-7.2"   => "redhat-7.2",
          "redhat-8.0"   => "redhat-7.2",
          "redhat-9"     => "redhat-7.2",
	  "openna-1.0"   => "redhat-6.2",
	  "mandrake-7.1" => "redhat-6.2",
	  "mandrake-7.2" => "redhat-6.2",
          "mandrake-9.0" => "redhat-7.0",
          "mandrake-9.1" => "redhat-7.0",
          "mandrake-9.2" => "redhat-7.0",
          "mandrake-10.0" => "redhat-7.0",
          "mandrake-10.1" => "redhat-7.0",
          "mandrake-10.2" => "redhat-7.0",
          "mandriva-2006.0" => "redhat-7.0",
          "mandriva-2006.1" => "redhat-7.0",
          "mandriva-2007.0" => "redhat-7.0",
	  "mandriva-2007.1" => "redhat-7.0",
          "yoper-2.2"       => "redhat-7.0",
          "blackpanther-4.0" => "redhat-7.0",
          "conectiva-9"  => "redhat-7.0", 
          "conectiva-10" => "redhat-7.0", 
          "debian-2.2"   => "debian-2.2",
          "debian-3.0"   => "debian-2.2",
          "debian-3.1"   => "debian-2.2",
          "debian-4.0"   => "debian-2.2",
          "debian-5.0"   => "debian-2.2",
          "debian-testing" => "debian-2.2",
          "ubuntu-5.04"  => "debian-2.2",
          "ubuntu-5.10"  => "debian-2.2",
          "ubuntu-6.06"  => "debian-2.2",
          "ubuntu-6.10"  => "debian-2.2",
          "ubuntu-7.04"  => "debian-2.2",
          "ubuntu-7.10"  => "debian-2.2",
          "ubuntu-8.04"  => "debian-2.2",
          "suse-7.0"     => "suse-7.0",
          "suse-9.0"     => "suse-9.0",
          "suse-9.1"     => "suse-9.0",
          "turbolinux-7.0"  => "redhat-7.0",
          "pld-1.0"      => "pld-1.0",
          "pld-1.1"      => "pld-1.0",
          "pld-1.99"     => "pld-1.0",
          "fedora-1"     => "redhat-7.2",
          "fedora-2"     => "redhat-7.2",
          "fedora-3"     => "redhat-7.2",
          "fedora-4"     => "redhat-7.2",
          "fedora-5"     => "redhat-7.2",
          "rpath"        => "redhat-7.2",
          "vine-3.0"     => "redhat-7.0",
          "vine-3.1"     => "redhat-7.0",
          "ark"          => "redhat-7.0",
          "slackware-9.1.0" => "slackware-9.1.0",
          "slackware-10.0.0" => "slackware-9.1.0",
          "slackware-10.1.0" => "slackware-9.1.0",
          "slackware-10.2.0" => "slackware-9.1.0",
          "gentoo"       => "gentoo",
          "vlos-1.2"     => "gentoo",
          "freebsd-5"    => "freebsd-5",
          "freebsd-6"    => "freebsd-5",
         );

  my %dist_tables =
	 (
	  "redhat-6.2" =>
	  {
	    fn =>
	    {
        SYSCONFIG_NW => "/etc/sysconfig/network",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        SMB_CONF     => "/etc/smb.conf",
        WVDIAL       => "/etc/wvdial.conf"
      },
	    table =>
		   [
		    [ "hostname",      \&gst_parse_sh,                SYSCONFIG_NW, HOSTNAME ],
        [ "gateway",       \&gst_parse_sh,                SYSCONFIG_NW, GATEWAY ],
        [ "gatewaydev",    \&gst_parse_sh,                SYSCONFIG_NW, GATEWAYDEV ],
        [ "userifacectl",  \&gst_parse_trivial,           1 ],
		    [ "nameserver",    \&gst_parse_split_all_unique_hash_comment, RESOLV_CONF,  "nameserver", "[ \t]+" ],
		    [ "searchdomain",  \&gst_parse_split_first_array_unique, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
		    [ "domain",        \&gst_parse_split_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
		    [ "order",         \&gst_parse_split_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
		    [ "hostmatch",     \&gst_parse_split_first_bool,  HOST_CONF,    "multi", "[ \t]+" ],
		    [ "statichost",    \&gst_parse_split_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
		    [ "workgroup",     \&gst_parse_ini,               SMB_CONF,     "global", "workgroup" ],     
		    [ "smbdesc",       \&gst_network_rh_get_smb_desc, SMB_CONF,   "global", "server string", "%hostname%" ],
		    [ "winsserver",    \&gst_parse_ini,               SMB_CONF,     "global", "wins server" ],
		    [ "winsuse",       \&gst_parse_ini_bool,          SMB_CONF,     "global", "wins support" ],
        [ "smbuse",        \&gst_service_sysv_get_status_any, "smbd", "nmbd" ],
        [ "smbinstalled",  \&gst_service_sysv_installed,  "smb" ],
        [ "smartdhcpcd",   \&gst_file_tool_installed,     "pump" ],
        [ "dialinstalled", \&gst_file_tool_installed,     "wvdial" ],
		    [ "interface",     \&gst_network_interfaces_get ]
		    ]
			 },

	  "redhat-7.0" =>
	  {
	    fn =>
	    {
        SYSCONFIG_NW => "/etc/sysconfig/network",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        SMB_CONF     => "/etc/samba/smb.conf",
        WVDIAL       => "/etc/wvdial.conf"
      },
	    table =>
		   [
		    [ "hostname",      \&gst_parse_sh,                SYSCONFIG_NW, HOSTNAME ],
        [ "gateway",       \&gst_parse_sh,                SYSCONFIG_NW, GATEWAY ],
        [ "gatewaydev",    \&gst_parse_sh,                SYSCONFIG_NW, GATEWAYDEV ],
        [ "userifacectl",  \&gst_parse_trivial,           1 ],
		    [ "nameserver",    \&gst_parse_split_all_unique_hash_comment, RESOLV_CONF,  "nameserver", "[ \t]+" ],
		    [ "searchdomain",  \&gst_parse_split_first_array_unique, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
		    [ "domain",        \&gst_parse_split_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
		    [ "order",         \&gst_parse_split_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
		    [ "hostmatch",     \&gst_parse_split_first_bool,  HOST_CONF,    "multi", "[ \t]+" ],
		    [ "statichost",    \&gst_parse_split_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
		    [ "workgroup",     \&gst_parse_ini,               SMB_CONF,     "global", "workgroup" ],     
		    [ "smbdesc",       \&gst_network_rh_get_smb_desc, SMB_CONF,   "global", "server string", "%hostname%" ],
		    [ "winsserver",    \&gst_parse_ini,               SMB_CONF,     "global", "wins server" ],
		    [ "winsuse",       \&gst_parse_ini_bool,          SMB_CONF,     "global", "wins support" ],
        [ "smbuse",        \&gst_service_sysv_get_status_any, "smbd", "nmbd" ],
        [ "smbinstalled",  \&gst_service_sysv_installed,  "smb" ],
        [ "smartdhcpcd",   \&gst_file_tool_installed,     "pump" ],
        [ "dialinstalled", \&gst_file_tool_installed,     "wvdial" ],
		    [ "interface",     \&gst_network_interfaces_get ]
		    ]
			 },

	  "redhat-7.2" =>
	  {
	    fn =>
	    {
        SYSCONFIG_NW => ["/etc/sysconfig/networking/profiles/default/network",
                         "/etc/sysconfig/networking/network",
                         "/etc/sysconfig/network"],
        RESOLV_CONF  => ["/etc/sysconfig/networking/profiles/default/resolv.conf",
                         "/etc/resolv.conf"],
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => ["/etc/sysconfig/networking/profiles/default/hosts",
                         "/etc/hosts"],
        SMB_CONF     => "/etc/samba/smb.conf",
        WVDIAL       => "/etc/wvdial.conf",
      },
	    table =>
		   [
		    [ "hostname",      \&gst_parse_sh,                SYSCONFIG_NW, HOSTNAME ],
        [ "gateway",       \&gst_parse_sh,                SYSCONFIG_NW, GATEWAY ],
        [ "gatewaydev",    \&gst_parse_sh,                SYSCONFIG_NW, GATEWAYDEV ],
        [ "userifacectl",  \&gst_parse_trivial,           1 ],
		    [ "nameserver",    \&gst_parse_split_all_unique_hash_comment, RESOLV_CONF,  "nameserver", "[ \t]+" ],
		    [ "searchdomain",  \&gst_parse_split_first_array_unique, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
		    [ "domain",        \&gst_parse_split_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
		    [ "order",         \&gst_parse_split_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
		    [ "hostmatch",     \&gst_parse_split_first_bool,  HOST_CONF,    "multi", "[ \t]+" ],
		    [ "statichost",    \&gst_parse_split_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
		    [ "workgroup",     \&gst_parse_ini,               SMB_CONF,     "global", "workgroup" ],     
		    [ "smbdesc",       \&gst_network_rh_get_smb_desc, SMB_CONF,   "global", "server string", "%hostname%" ],
		    [ "winsserver",    \&gst_parse_ini,               SMB_CONF,     "global", "wins server" ],
		    [ "winsuse",       \&gst_parse_ini_bool,          SMB_CONF,     "global", "wins support" ],
        [ "smbuse",        \&gst_service_sysv_get_status_any, "smbd", "nmbd" ],
        [ "smbinstalled",  \&gst_service_sysv_installed,  "smb" ],
        [ "smartdhcpcd",   \&gst_file_tool_installed,     "pump" ],
        [ "dialinstalled", \&gst_file_tool_installed,     "wvdial" ],
		    [ "interface",     \&gst_network_interfaces_get ]
		    ]
			 },

	  "debian-2.2" =>
	  {
	    fn =>
	    {
        OPTIONS     => "/etc/network/options",
        RESOLV_CONF => "/etc/resolv.conf",
        HOST_CONF   => "/etc/host.conf",
        HOSTS       => "/etc/hosts",
        HOSTNAME    => "/etc/hostname",
        SMB_CONF    => "/etc/samba/smb.conf",
        WVDIAL      => "/etc/wvdial.conf"
      },
	    table =>
          [
           [ "hostname",      \&gst_parse_line_first,        HOSTNAME ],
           [ "gateway",       \&gst_network_get_default_gateway ],
           [ "gatewaydev",    \&gst_network_get_default_gatewaydev ],
#           [ "gwdevunsup",    \&gst_parse_trivial,           1 ],
#           [ "userifacectl",  \&gst_parse_trivial,           0 ],
           [ "domain",				\&gst_parse_split_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
           [ "nameserver",    \&gst_parse_split_all_hash_comment, RESOLV_CONF,  "nameserver", "[ \t]+" ],
           [ "searchdomain",  \&gst_parse_split_first_array, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
           [ "order",         \&gst_parse_split_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
           [ "hostmatch",     \&gst_parse_split_first_bool,  HOST_CONF,    "multi", "[ \t]+" ],
           [ "statichost",    \&gst_parse_split_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
           [ "workgroup",     \&gst_parse_ini,               SMB_CONF,     "global", "workgroup" ],
           [ "smbdesc",       \&gst_parse_ini,               SMB_CONF,     "global", "server string" ],
           [ "winsserver",    \&gst_parse_ini,               SMB_CONF,     "global", "wins server" ],
           [ "winsuse",       \&gst_parse_ini_bool,          SMB_CONF,     "global", "wins support" ],
           [ "smbuse",        \&gst_service_sysv_get_status_any, "smbd", "nmbd" ],
           [ "smbinstalled",  \&gst_service_sysv_installed,  "samba" ],
           [ "smartdhcpcd",   \&gst_file_tool_installed,     "pump" ],
           [ "dialinstalled", \&gst_file_tool_installed,     "wvdial" ],
           [ "interface",     \&gst_network_interfaces_get ]
           ]
             },

	  "suse-7.0" =>
	  {
	    fn =>
	    {
        RC_CONFIG    => "/etc/rc.config",
        ROUTE_CONF   => "/etc/route.conf",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        SMB_CONF     => "/etc/smb.conf",
        WVDIAL       => "/etc/wvdial.conf"
      },
	    table =>
		   [
        [ "hostname",      \&gst_parse_sh_get_hostname,          RC_CONFIG, FQHOSTNAME ],
        [ "gateway",       \&gst_parse_split_first_str,          ROUTE_CONF, "default", "[ \t]+" ],
        [ "gateway",       \&gst_parse_split_first_str,          ROUTE_CONF, "0.0.0.0", "[ \t]+" ],
        [ "gwdevunsup",    \&gst_parse_trivial,                  1 ],
        [ "userifacectl",  \&gst_parse_trivial,                  0 ],
		    [ "domain",        \&gst_parse_sh_get_domain,            RC_CONFIG, FQHOSTNAME ],
		    [ "nameserver",    \&gst_parse_split_all_unique_hash_comment, RESOLV_CONF, "nameserver", "[ \t]+" ],
		    [ "searchdomain",  \&gst_parse_split_first_array_unique, RESOLV_CONF, "search", "[ \t]+", "[ \t]+" ],
# This was to take the values from SuSEConfig, but a better solution is to get
# the configuration from /etc/resolv.conf and then replace in rc.config, so those
# files stay in sync.        
#		    [ "nameserver",    \&gst_parse_sh_split,                 RC_CONFIG, NAMESERVER, "[ \t]+" ],
#		    [ "searchdomain",  \&gst_parse_sh_split,                 RC_CONFIG, SEARCHLIST, "[ \t]+" ],
		    [ "order",         \&gst_parse_split_first_array,        HOST_CONF, "order", "[ \t]+", ",[ \t]*" ],
		    [ "hostmatch",     \&gst_parse_split_first_bool,         HOST_CONF, "multi", "[ \t]+" ],
		    [ "statichost",    \&gst_parse_split_hash,               HOSTS,     "[ \t]+", "[ \t]+" ],
		    [ "workgroup",     \&gst_parse_ini,                      SMB_CONF,  "global", "workgroup" ],     
		    [ "smbdesc",       \&gst_parse_ini,                      SMB_CONF,  "global", "server string" ],    
		    [ "winsserver",    \&gst_parse_ini,                      SMB_CONF,  "global", "wins server" ],
		    [ "winsuse",       \&gst_parse_ini_bool,                 SMB_CONF,  "global", "wins support" ],
        [ "smbuse",        \&gst_service_sysv_get_status_any,    "smbd",    "nmbd" ],
        [ "smbinstalled",  \&gst_service_sysv_installed,         "smb" ],
        [ "dialinstalled", \&gst_file_tool_installed,            "wvdial" ],
		    [ "interface_tmp", \&gst_network_interfaces_get ],
        [ "interface",     \&gst_network_suse70_get_ppp,         "%dialing%", "%interface_tmp%" ],
		    ]
    },

    "suse-9.0" =>
    {
      fn =>
      {
        ROUTE_CONF   => "/etc/sysconfig/network/routes",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        HOSTNAME     => "/etc/HOSTNAME",
        SMB_CONF     => "/etc/samba/smb.conf",
      },
      table =>
      [
       [ "hostname",      \&gst_parse_fq_hostname, HOSTNAME ],
       [ "domain",        \&gst_parse_fq_domain,   HOSTNAME ],
       [ "domain",        \&gst_parse_split_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
       [ "nameserver",    \&gst_parse_split_all_unique_hash_comment, RESOLV_CONF,  "nameserver", "[ \t]+" ],
       [ "searchdomain",  \&gst_parse_split_first_array_unique, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
       [ "order",         \&gst_parse_split_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
       [ "hostmatch",     \&gst_parse_split_first_bool,  HOST_CONF,    "multi", "[ \t]+" ],
       [ "statichost",    \&gst_parse_split_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
       [ "workgroup",     \&gst_parse_ini,               SMB_CONF,     "global", "workgroup" ],
       [ "smbdesc",       \&gst_network_rh_get_smb_desc, SMB_CONF,   "global", "server string", "%hostname%" ],
       [ "winsserver",    \&gst_parse_ini,               SMB_CONF,     "global", "wins server" ],
       [ "winsuse",       \&gst_parse_ini_bool,          SMB_CONF,     "global", "wins support" ],
       [ "smbuse",        \&gst_service_get_status,      "smb" ],
       [ "smbinstalled",  \&gst_service_installed,       "smb" ],
       [ "dialinstalled", \&gst_parse_trivial, 1 ],
       [ "interface",     \&gst_network_interfaces_get ],
       [ "gateway",       \&gst_parse_split_first_array_pos, ROUTE_CONF, "default", 0, "[ \t]+", "[ \t]+" ],
       [ "gatewaydev",    \&gst_network_get_gateway_dev_from_address,  "%interface%", "%gateway%" ],
      ]
    },
    
	  "pld-1.0" =>
	  {
	    fn =>
	    {
        SYSCONFIG_NW => "/etc/sysconfig/network",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        SMB_CONF     => "/etc/smb/smb.conf",
        WVDIAL       => "/etc/wvdial.conf"
      },
	    table =>
		   [
		    [ "hostname",      \&gst_parse_sh,                SYSCONFIG_NW, HOSTNAME ],
        [ "gateway",       \&gst_parse_sh,                SYSCONFIG_NW, GATEWAY ],
        [ "gatewaydev",    \&gst_parse_sh,                SYSCONFIG_NW, GATEWAYDEV ],
        [ "userifacectl",  \&gst_parse_trivial,           1 ],
		    [ "nameserver",    \&gst_parse_split_all_unique_hash_comment, RESOLV_CONF,  "nameserver", "[ \t]+" ],
		    [ "searchdomain",  \&gst_parse_split_first_array_unique, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
		    [ "domain",        \&gst_parse_split_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
		    [ "order",         \&gst_parse_split_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
		    [ "hostmatch",     \&gst_parse_split_first_bool,  HOST_CONF,    "multi", "[ \t]+" ],
		    [ "statichost",    \&gst_parse_split_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
		    [ "workgroup",     \&gst_parse_ini,               SMB_CONF,     "global", "workgroup" ],     
		    [ "smbdesc",       \&gst_network_rh_get_smb_desc, SMB_CONF,   "global", "server string", "%hostname%" ],
		    [ "winsserver",    \&gst_parse_ini,               SMB_CONF,     "global", "wins server" ],
		    [ "winsuse",       \&gst_parse_ini_bool,          SMB_CONF,     "global", "wins support" ],
        [ "smbuse",        \&gst_service_sysv_get_status_any, "smbd", "nmbd" ],
        [ "smbinstalled",  \&gst_service_sysv_installed,  "smb" ],
        [ "smartdhcpcd",   \&gst_file_tool_installed,     "pump" ],
        [ "dialinstalled", \&gst_file_tool_installed,     "wvdial" ],
		    [ "interface",     \&gst_network_interfaces_get ]
		    ]
			 },

    "slackware-9.1.0" =>
    {
      fn =>
      {
        RC_INET_CONF => "/etc/rc.d/rc.inet1.conf",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        HOSTNAME     => "/etc/HOSTNAME",
        SMB_CONF     => "/etc/samba/smb.conf",
        WVDIAL       => "/etc/wvdial.conf"
      },
      table =>
      [
       [ "hostname",      \&gst_parse_fq_hostname, HOSTNAME ],
       [ "nameserver",    \&gst_parse_split_all_unique_hash_comment, RESOLV_CONF,  "nameserver", "[ \t]+" ],
       [ "searchdomain",  \&gst_parse_split_first_array_unique, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
       [ "domain",        \&gst_parse_split_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
       [ "order",         \&gst_parse_split_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
       [ "hostmatch",     \&gst_parse_split_first_bool,  HOST_CONF,    "multi", "[ \t]+" ],
       [ "statichost",    \&gst_parse_split_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
       [ "workgroup",     \&gst_parse_ini,               SMB_CONF,     "global", "workgroup" ],     
       [ "smbdesc",       \&gst_network_rh_get_smb_desc, SMB_CONF,   "global", "server string", "%hostname%" ],
       [ "winsserver",    \&gst_parse_ini,               SMB_CONF,     "global", "wins server" ],
       [ "winsuse",       \&gst_parse_ini_bool,          SMB_CONF,     "global", "wins support" ],
       [ "smbuse",        \&gst_service_sysv_get_status_any, "smbd", "nmbd" ],
       [ "smbinstalled",  \&gst_service_installed,  "/etc/rc.d/rc.samba" ],
       [ "dialinstalled", \&gst_parse_trivial, 1 ],
       [ "interface",     \&gst_network_interfaces_get ],
       [ "gateway",       \&gst_parse_sh,                RC_INET_CONF, GATEWAY ],
       [ "gatewaydev",    \&gst_network_get_gateway_dev_from_address,  "%interface%", "%gateway%" ],
      ]
    },

    "gentoo" =>
    {
      fn =>
      {
        HOSTNAME    => "/etc/conf.d/hostname",
        DOMAINNAME  => "/etc/conf.d/domainname",
        NET         => "/etc/conf.d/net",
        RESOLV_CONF => "/etc/resolv.conf",
        HOSTS       => "/etc/hosts",
        SMB_CONF    => "/etc/samba/smb.conf"
      },
      table =>
      [
       [ "hostname",      \&gst_parse_sh,                            HOSTNAME,     "HOSTNAME" ],
       [ "domain",        \&gst_parse_sh,                            DOMAINNAME,   "DNSDOMAIN" ],
       [ "domain",        \&gst_parse_split_first_str,               RESOLV_CONF,  "domain", "[ \t]+" ],
       [ "nameserver",    \&gst_parse_split_all_unique_hash_comment, RESOLV_CONF,  "nameserver", "[ \t]+" ],
       [ "searchdomain",  \&gst_parse_split_first_array_unique,      RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
       [ "statichost",    \&gst_parse_split_hash,                    HOSTS,        "[ \t]+", "[ \t]+" ],
       [ "workgroup",     \&gst_parse_ini,                           SMB_CONF,     "global", "workgroup" ],
       [ "smbdesc",       \&gst_network_rh_get_smb_desc,             SMB_CONF,     "global", "server string", "%hostname%" ],
       [ "winsserver",    \&gst_parse_ini,                           SMB_CONF,     "global", "wins server" ],
       [ "winsuse",       \&gst_parse_ini_bool,                      SMB_CONF,     "global", "wins support" ],
       [ "smbuse",        \&gst_service_gentoo_get_status,           "samba" ],
       [ "smbinstalled",  \&gst_service_installed,                   "samba" ],
       [ "dialinstalled", \&gst_parse_trivial, 1 ],
       [ "gateway",       \&gst_network_get_default_gateway ],
       [ "gatewaydev",    \&gst_network_get_default_gatewaydev ],
       [ "interface",     \&gst_network_interfaces_get ],
       ]
     },

    "freebsd-5" =>
    {
      fn =>
      {
        RC_CONF     => "/etc/rc.conf",
        RESOLV_CONF => "/etc/resolv.conf",
        HOSTS       => "/etc/hosts",
        SMB_CONF    => "/usr/local/etc/smb.conf"
      },
      table =>
      [
       [ "hostname",      \&gst_parse_sh_re,                          RC_CONF, hostname, "^([^\.]*)\." ],
       [ "domain",        \&gst_parse_split_first_str,                RESOLV_CONF, "domain", "[ \t]+" ],
       [ "nameserver",    \&gst_parse_split_all_unique_hash_comment,  RESOLV_CONF,  "nameserver", "[ \t]+" ],
       [ "searchdomain",  \&gst_parse_split_first_array_unique,       RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
       [ "statichost",    \&gst_parse_split_hash,                     HOSTS,        "[ \t]+", "[ \t]+" ],
       [ "workgroup",     \&gst_parse_ini,                            SMB_CONF,     "global", "workgroup" ],
       [ "smbdesc",       \&gst_network_rh_get_smb_desc,              SMB_CONF,     "global", "server string", "%hostname%" ],
       [ "winsserver",    \&gst_parse_ini,                            SMB_CONF,     "global", "wins server" ],
       [ "winsuse",       \&gst_parse_ini_bool,                       SMB_CONF,     "global", "wins support" ],
       [ "dialinstalled", \&gst_parse_trivial, 1 ],
       [ "smbinstalled",  \&gst_service_installed,                    "samba" ],
       [ "smbuse",        \&gst_service_rcng_get_status,              "smbd" ],
       [ "interface",     \&gst_network_interfaces_get ],
       [ "gateway",       \&gst_parse_sh,                             RC_CONF, defaultrouter ],
       [ "gatewaydev",    \&gst_network_get_gateway_dev_from_address, "%interface%", "%gateway%" ],
       ]
     },
   );
  
  my $dist = $dist_map{$gst_dist};
  return %{$dist_tables{$dist}} if $dist;

  &gst_report ("platform_no_table", $gst_dist);
  return undef;
}

sub gst_network_get_interface_parse_table
{
  my %dist_map =
         (
          "redhat-5.2"   => "redhat-6.2",
	  "redhat-6.0"   => "redhat-6.2",
	  "redhat-6.1"   => "redhat-6.2",
	  "redhat-6.2"   => "redhat-6.2",
	  "redhat-7.0"   => "redhat-6.2",
	  "redhat-7.1"   => "redhat-6.2",
	  "redhat-7.2"   => "redhat-7.2",
          "redhat-8.0"   => "redhat-8.0",
          "redhat-9"     => "redhat-8.0",
	  "openna-1.0"   => "redhat-6.2",
	  "mandrake-7.1" => "redhat-6.2",
          "mandrake-7.2" => "redhat-6.2",
          "mandrake-9.0" => "mandrake-9.0",
          "mandrake-9.1" => "mandrake-9.0",
          "mandrake-9.2" => "mandrake-9.0",
          "mandrake-10.0" => "mandrake-9.0",
          "mandrake-10.1" => "mandrake-9.0",
          "mandrake-10.2" => "mandrake-9.0",
          "mandriva-2006.0" => "mandrake-9.0",
          "mandriva-2006.1" => "mandrake-9.0",
          "mandriva-2007.0" => "mandrake-9.0",
	  "mandriva-2007.1" => "mandrake-9.0",
          "yoper-2.2"       => "redhat-6.2",
          "blackpanther-4.0" => "mandrake-9.0",
          "conectiva-9"  => "conectiva-9",
          "conectiva-10" => "conectiva-9",
          "debian-2.2"   => "debian-2.2",
          "debian-3.0"   => "debian-3.0",
          "debian-3.1"   => "debian-3.0",
          "debian-4.0"   => "debian-3.0",
          "debian-5.0"   => "debian-3.0",
          "debian-testing" => "debian-3.0",
          "ubuntu-5.04"  => "debian-3.0",
          "ubuntu-5.10"  => "debian-3.0",
          "ubuntu-6.06"  => "debian-3.0",
          "ubuntu-6.10"  => "debian-3.0",
          "ubuntu-7.04"  => "debian-3.0",
          "ubuntu-7.10"  => "debian-3.0",
          "ubuntu-8.04"  => "debian-3.0",
          "suse-7.0"     => "suse-7.0",
          "suse-9.0"     => "suse-9.0",
          "suse-9.1"     => "suse-9.0",
          "turbolinux-7.0"   => "redhat-6.2",
          "pld-1.0"      => "pld-1.0",
          "pld-1.1"      => "pld-1.0",
          "pld-1.99"     => "pld-1.0",
          "fedora-1"     => "redhat-7.2",
          "fedora-2"     => "redhat-7.2",
          "fedora-3"     => "redhat-7.2",
          "fedora-4"     => "redhat-7.2",
          "fedora-5"     => "redhat-7.2",
          "rpath"        => "redhat-7.2",
          "vine-3.0"     => "vine-3.0",
          "vine-3.1"     => "vine-3.0",
          "ark"          => "vine-3.0",
          "slackware-9.1.0" => "slackware-9.1.0",
          "slackware-10.0.0" => "slackware-9.1.0",
          "slackware-10.1.0" => "slackware-9.1.0",
          "slackware-10.2.0" => "slackware-9.1.0",
          "gentoo"       => "gentoo",
          "vlos-1.2"     => "gentoo",
          "freebsd-5"    => "freebsd-5",
          "freebsd-6"    => "freebsd-5",
         );
  
  my %dist_tables =
	 (
	  "redhat-6.2" =>
	  {
	    ifaces_get => \&gst_network_sysconfig_rh62_ifaces_get_existing,
	    fn =>
	    {
        IFCFG => "/etc/sysconfig/network-scripts/ifcfg-#iface#",
        CHAT  => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE => "#iface#",
        PAP   => "/etc/ppp/pap-secrets",
        CHAP  => "/etc/ppp/chap-secrets",
        PUMP  => "/etc/pump.conf",
        WVDIAL => "/etc/wvdial.conf"
      },
	    table =>
          [
           [ "bootproto",          \&gst_network_rh62_parse_bootproto, IFCFG, BOOTPROTO ],
           [ "auto",               \&gst_parse_sh_bool, IFCFG, ONBOOT ],
#           [ "user",               \&gst_parse_sh_bool, IFCFG, USERCTL ],
           [ "dev",                \&gst_parse_sh,      IFCFG, DEVICE ],
           [ "address",            \&gst_parse_sh,      IFCFG, IPADDR ],
           [ "netmask",            \&gst_parse_sh,      IFCFG, NETMASK ],
           [ "broadcast",          \&gst_parse_sh,      IFCFG, BROADCAST ],
           [ "network",            \&gst_parse_sh,      IFCFG, NETWORK ],
           [ "gateway",            \&gst_parse_sh,      IFCFG, GATEWAY ],
           [ "remote_address",     \&gst_parse_sh,      IFCFG, REMIP ],
#           [ "update_dns",         \&gst_network_pump_get_nodns, PUMP, "%dev%", "%bootproto%" ],
#           [ "dns1",               \&gst_parse_sh,      IFCFG, DNS1 ],
#           [ "dns2",               \&gst_parse_sh,      IFCFG, DNS2 ],
#           [ "ppp_options",        \&gst_parse_sh,      IFCFG, PPPOPTIONS ],
           [ "section",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, WVDIALSECT ]],
           [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PEERDNS ]],
           [ "mtu",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MTU ]],
           [ "mru",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MRU ]],
           [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, PAPNAME ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, PAP,  "%login%" ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, CHAP, "%login%" ]],
           [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MODEMPORT ]],
           [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, LINESPEED ]],
           [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, DEFROUTE ]],
           [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PERSIST ]],
           [ "serial_escapechars", \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, ESCAPECHARS ]],
           [ "serial_hwctl",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, HARDFLOWCTL ]],
           [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_chat,    CHAT, "^atd[^0-9]*([0-9, -]+)" ]],
           [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Phone" ]],
           [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto DNS" ]],
           [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Username" ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Password" ]],
           [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Modem" ]],
           [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Baud" ]],
           [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Check Def Route" ]],
           [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
           [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Command" ]],
           [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Prefix" ]],
#           [ "enabled",            \&gst_network_interface_active, IFACE,
#                                                                   \&gst_network_active_interfaces_get ],
#           [ "enabled",            \&gst_parse_trivial, 0 ]
           ]
    },

	  "redhat-7.2" =>
	  {
	    ifaces_get => \&gst_network_sysconfig_rh72_ifaces_get_existing,
	    fn =>
	    {
        IFCFG => ["/etc/sysconfig/networking/profiles/default/ifcfg-#iface#",
                  "/etc/sysconfig/networking/devices/ifcfg-#iface#",
                  "/etc/sysconfig/network-scripts/ifcfg-#iface#"],
        CHAT  => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE => "#iface#",
        PAP   => "/etc/ppp/pap-secrets",
        CHAP  => "/etc/ppp/chap-secrets",
        PUMP  => "/etc/pump.conf",
        WVDIAL => "/etc/wvdial.conf"
      },
	    table =>
          [
           [ "bootproto",          \&gst_network_rh62_parse_bootproto, IFCFG, BOOTPROTO ],
           [ "auto",               \&gst_parse_sh_bool, IFCFG, ONBOOT ],
#           [ "user",               \&gst_parse_sh_bool, IFCFG, USERCTL ],
#           [ "name",               \&gst_parse_sh,      IFCFG, NAME ],
#           [ "name",               \&gst_parse_trivial, IFACE ],
           [ "dev",                \&gst_parse_sh,      IFCFG, DEVICE ],
           [ "address",            \&gst_parse_sh,      IFCFG, IPADDR ],
           [ "netmask",            \&gst_parse_sh,      IFCFG, NETMASK ],
           [ "broadcast",          \&gst_parse_sh,      IFCFG, BROADCAST ],
           [ "network",            \&gst_parse_sh,      IFCFG, NETWORK ],
           [ "gateway",            \&gst_parse_sh,      IFCFG, GATEWAY ],
           [ "essid",              \&gst_parse_sh,      IFCFG, ESSID ],
           [ "key_type",           \&gst_network_get_wep_key_type, [ \&gst_parse_sh, IFCFG, KEY ]],
           [ "key",                \&gst_network_get_wep_key,      [ \&gst_parse_sh, IFCFG, KEY ]],
           [ "remote_address",     \&gst_parse_sh,      IFCFG, REMIP ],
#           [ "update_dns",         \&gst_network_pump_get_nodns, PUMP, "%dev%", "%bootproto%" ],
#           [ "dns1",               \&gst_parse_sh,      IFCFG, DNS1 ],
#           [ "dns2",               \&gst_parse_sh,      IFCFG, DNS2 ],
           [ "section",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, WVDIALSECT ]],
           [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PEERDNS ]],
           [ "mtu",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MTU ]],
           [ "mru",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MRU ]],
           [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, PAPNAME ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, PAP,  "%login%" ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, CHAP, "%login%" ]],
           [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MODEMPORT ]],
           [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, LINESPEED ]],
#           [ "ppp_options",        \&gst_parse_sh,      IFCFG, PPPOPTIONS ],
           [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, DEFROUTE ]],
#           [ "debug",              \&gst_parse_sh_bool, IFCFG, DEBUG ],
           [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PERSIST ]],
           [ "serial_escapechars", \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, ESCAPECHARS ]],
           [ "serial_hwctl",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, HARDFLOWCTL ]],
           [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_chat,    CHAT, "^atd[^0-9]*([0-9, -]+)" ]],
#           [ "enabled",            \&gst_network_interface_active, "%dev%",
#                                                                   \&gst_network_active_interfaces_get ],
#           [ "enabled",            \&gst_network_interface_active, IFACE,
#                                                                   \&gst_network_active_interfaces_get ],
#           [ "enabled",            \&gst_parse_trivial, 0 ]
           # wvdial settings
           [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Phone" ]],
           [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto DNS" ]],
           [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Username" ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Password" ]],
           [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Modem" ]],
           [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Baud" ]],
           [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Check Def Route" ]],
           [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
           [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Command" ]],
           [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Prefix" ]],
          ]
    },

	  "redhat-8.0" =>
	  {
	    ifaces_get => \&gst_network_sysconfig_rh72_ifaces_get_existing,
	    fn =>
	    {
        IFCFG => ["/etc/sysconfig/networking/profiles/default/ifcfg-#iface#",
                  "/etc/sysconfig/networking/devices/ifcfg-#iface#",
                  "/etc/sysconfig/network-scripts/ifcfg-#iface#"],
        CHAT  => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE => "#iface#",
        PAP   => "/etc/ppp/pap-secrets",
        CHAP  => "/etc/ppp/chap-secrets",
        PUMP  => "/etc/pump.conf",
        WVDIAL => "/etc/wvdial.conf"
      },
	    table =>
          [
           [ "bootproto",          \&gst_network_rh62_parse_bootproto, IFCFG, BOOTPROTO ],
           [ "auto",               \&gst_parse_sh_bool, IFCFG, ONBOOT ],
#           [ "user",               \&gst_parse_sh_bool, IFCFG, USERCTL ],
#           [ "name",               \&gst_parse_sh,      IFCFG, NAME ],
#           [ "name",               \&gst_parse_trivial, IFACE ],
           [ "dev",                \&gst_parse_sh,      IFCFG, DEVICE ],
           [ "address",            \&gst_parse_sh,      IFCFG, IPADDR ],
           [ "netmask",            \&gst_parse_sh,      IFCFG, NETMASK ],
           [ "broadcast",          \&gst_parse_sh,      IFCFG, BROADCAST ],
           [ "network",            \&gst_parse_sh,      IFCFG, NETWORK ],
           [ "gateway",            \&gst_parse_sh,      IFCFG, GATEWAY ],
           [ "essid",              \&gst_parse_sh,      IFCFG, WIRELESS_ESSID ],
           [ "key_type",           \&gst_network_get_wep_key_type, [ \&gst_parse_sh, IFCFG, WIRELESS_KEY ]],
           [ "key",                \&gst_network_get_wep_key,      [ \&gst_parse_sh, IFCFG, WIRELESS_KEY ]],
           [ "remote_address",     \&gst_parse_sh,      IFCFG, REMIP ],
#           [ "update_dns",         \&gst_network_pump_get_nodns, PUMP, "%dev%", "%bootproto%" ],
#           [ "dns1",               \&gst_parse_sh,      IFCFG, DNS1 ],
#           [ "dns2",               \&gst_parse_sh,      IFCFG, DNS2 ],
           [ "section",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, WVDIALSECT ]],
           [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PEERDNS ]],
           [ "mtu",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MTU ]],
           [ "mru",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MRU ]],
           [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, PAPNAME ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, PAP,  "%login%" ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, CHAP, "%login%" ]],
           [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MODEMPORT ]],
           [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, LINESPEED ]],
#           [ "ppp_options",        \&gst_parse_sh,      IFCFG, PPPOPTIONS ],
           [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, DEFROUTE ]],
#           [ "debug",              \&gst_parse_sh_bool, IFCFG, DEBUG ],
           [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PERSIST ]],
           [ "serial_escapechars", \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, ESCAPECHARS ]],
           [ "serial_hwctl",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, HARDFLOWCTL ]],
           [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_chat,    CHAT, "^atd[^0-9]*([0-9, -]+)" ]],
#           [ "enabled",            \&gst_network_interface_active, "%dev%",
#                                                                   \&gst_network_active_interfaces_get ],
#           [ "enabled",            \&gst_network_interface_active, IFACE,
#                                                                   \&gst_network_active_interfaces_get ],
#           [ "enabled",            \&gst_parse_trivial, 0 ]
           # wvdial settings
           [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Phone" ]],
           [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto DNS" ]],
           [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Username" ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Password" ]],
           [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Modem" ]],
           [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Baud" ]],
           [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Check Def Route" ]],
           [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
           [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Command" ]],
           [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Prefix" ]],
          ]
    },

    "vine-3.0" =>
	  {
	    ifaces_get => \&gst_network_sysconfig_rh62_ifaces_get_existing,
	    fn =>
	    {
        IFCFG => "/etc/sysconfig/network-scripts/ifcfg-#iface#",
        CHAT  => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE => "#iface#",
        PAP   => "/etc/ppp/pap-secrets",
        CHAP  => "/etc/ppp/chap-secrets",
        PUMP  => "/etc/pump.conf",
        WVDIAL => "/etc/wvdial.conf"
      },
	    table =>
      [
       [ "bootproto",          \&gst_network_rh62_parse_bootproto, IFCFG, BOOTPROTO ],
       [ "auto",               \&gst_parse_sh_bool, IFCFG, ONBOOT ],
#       [ "user",               \&gst_parse_sh_bool, IFCFG, USERCTL ],
#       [ "name",               \&gst_parse_sh,      IFCFG, NAME ],
       [ "dev",                \&gst_parse_sh,      IFCFG, DEVICE ],
       [ "address",            \&gst_parse_sh,      IFCFG, IPADDR ],
       [ "netmask",            \&gst_parse_sh,      IFCFG, NETMASK ],
       [ "broadcast",          \&gst_parse_sh,      IFCFG, BROADCAST ],
       [ "network",            \&gst_parse_sh,      IFCFG, NETWORK ],
       [ "gateway",            \&gst_parse_sh,      IFCFG, GATEWAY ],
       [ "essid",              \&gst_parse_sh,      IFCFG, ESSID ],
       [ "key_type",           \&gst_network_get_wep_key_type, [ \&gst_parse_sh, IFCFG, KEY ]],
       [ "key",                \&gst_network_get_wep_key,      [ \&gst_parse_sh, IFCFG, KEY ]],
       [ "remote_address",     \&gst_parse_sh,      IFCFG, REMIP ],
#       [ "update_dns",         \&gst_network_pump_get_nodns, PUMP, "%dev%", "%bootproto%" ],
#       [ "dns1",               \&gst_parse_sh,      IFCFG, DNS1 ],
#       [ "dns2",               \&gst_parse_sh,      IFCFG, DNS2 ],
       [ "section",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, WVDIALSECT ]],
       [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PEERDNS ]],
       [ "mtu",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MTU ]],
       [ "mru",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MRU ]],
       [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, PAPNAME ]],
       [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, PAP,  "%login%" ]],
       [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, CHAP, "%login%" ]],
       [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MODEMPORT ]],
       [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, LINESPEED ]],
#       [ "ppp_options",        \&gst_parse_sh,      IFCFG, PPPOPTIONS ],
       [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, DEFROUTE ]],
#       [ "debug",              \&gst_parse_sh_bool, IFCFG, DEBUG ],
       [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PERSIST ]],
       [ "serial_escapechars", \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, ESCAPECHARS ]],
       [ "serial_hwctl",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, HARDFLOWCTL ]],
       [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_chat,    CHAT, "^atd[^0-9]*([0-9, -]+)" ]],
#       [ "enabled",            \&gst_network_interface_active, IFACE, \&gst_network_active_interfaces_get ],
#       [ "enabled",            \&gst_parse_trivial, 0 ]
       # wvdial settings
       [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Phone" ]],
       [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto DNS" ]],
       [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Username" ]],
       [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Password" ]],
       [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Modem" ]],
       [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Baud" ]],
       [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Check Def Route" ]],
       [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
       [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Command" ]],
       [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Prefix" ]],
      ]
    },

    "mandrake-9.0" =>
	  {
	    ifaces_get => \&gst_network_sysconfig_rh62_ifaces_get_existing,
	    fn =>
	    {
        IFCFG => "/etc/sysconfig/network-scripts/ifcfg-#iface#",
        CHAT  => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE => "#iface#",
        PAP   => "/etc/ppp/pap-secrets",
        CHAP  => "/etc/ppp/chap-secrets",
        PUMP  => "/etc/pump.conf",
        WVDIAL => "/etc/wvdial.conf"
      },
	    table =>
          [
           [ "bootproto",          \&gst_network_rh62_parse_bootproto, IFCFG, BOOTPROTO ],
           [ "auto",               \&gst_parse_sh_bool, IFCFG, ONBOOT ],
#           [ "user",               \&gst_parse_sh_bool, IFCFG, USERCTL ],
#           [ "name",               \&gst_parse_sh,      IFCFG, NAME ],
           [ "dev",                \&gst_parse_sh,      IFCFG, DEVICE ],
           [ "address",            \&gst_parse_sh,      IFCFG, IPADDR ],
           [ "netmask",            \&gst_parse_sh,      IFCFG, NETMASK ],
           [ "broadcast",          \&gst_parse_sh,      IFCFG, BROADCAST ],
           [ "network",            \&gst_parse_sh,      IFCFG, NETWORK ],
           [ "gateway",            \&gst_parse_sh,      IFCFG, GATEWAY ],
           [ "essid",              \&gst_parse_sh,      IFCFG, WIRELESS_ESSID ],
           [ "key_type",           \&gst_network_get_wep_key_type, [ \&gst_parse_sh, IFCFG, WIRELESS_KEY ]],
           [ "key",                \&gst_network_get_wep_key,      [ \&gst_parse_sh, IFCFG, WIRELESS_KEY ]],
           [ "remote_address",     \&gst_parse_sh,      IFCFG, REMIP ],
#           [ "update_dns",         \&gst_network_pump_get_nodns, PUMP, "%dev%", "%bootproto%" ],
#           [ "dns1",               \&gst_parse_sh,      IFCFG, DNS1 ],
#           [ "dns2",               \&gst_parse_sh,      IFCFG, DNS2 ],
           [ "section",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, WVDIALSECT ]],
           [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PEERDNS ]],
           [ "mtu",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MTU ]],
           [ "mru",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MRU ]],
           [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, PAPNAME ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, PAP,  "%login%" ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, CHAP, "%login%" ]],
           [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MODEMPORT ]],
           [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, LINESPEED ]],
#           [ "ppp_options",        \&gst_parse_sh,      IFCFG, PPPOPTIONS ],
           [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, DEFROUTE ]],
#           [ "debug",              \&gst_parse_sh_bool, IFCFG, DEBUG ],
           [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PERSIST ]],
           [ "serial_escapechars", \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, ESCAPECHARS ]],
           [ "serial_hwctl",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, HARDFLOWCTL ]],
           [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_chat,    CHAT, "^atd[^0-9]*([0-9, -]+)" ]],
#           [ "enabled",            \&gst_network_interface_active, IFACE,
#                                                                   \&gst_network_active_interfaces_get ],
#           [ "enabled",            \&gst_parse_trivial, 0 ]
           # wvdial settings
           [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Phone" ]],
           [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto DNS" ]],
           [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Username" ]],
           [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Password" ]],
           [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Modem" ]],
           [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Baud" ]],
           [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Check Def Route" ]],
           [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
           [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Command" ]],
           [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Prefix" ]],
          ]
    },

    "conectiva-9" =>
    {
      ifaces_get => \&gst_network_sysconfig_rh62_ifaces_get_existing,
      fn =>
 	    {
        IFCFG => "/etc/sysconfig/network-scripts/ifcfg-#iface#",
        CHAT  => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE => "#iface#",
        PAP   => "/etc/ppp/pap-secrets",
        CHAP  => "/etc/ppp/chap-secrets",
        PUMP  => "/etc/pump.conf",
        WVDIAL => "/etc/wvdial.conf"
      },
      table =>
      [
       [ "bootproto",          \&gst_network_rh62_parse_bootproto, IFCFG, BOOTPROTO ],
       [ "auto",               \&gst_parse_sh_bool, IFCFG, ONBOOT ],
#       [ "user",               \&gst_parse_sh_bool, IFCFG, USERCTL ],
#       [ "name",               \&gst_parse_sh,      IFCFG, NAME ],
       [ "dev",                \&gst_parse_sh,      IFCFG, DEVICE ],
       [ "address",            \&gst_parse_sh,      IFCFG, IPADDR ],
       [ "netmask",            \&gst_parse_sh,      IFCFG, NETMASK ],
       [ "broadcast",          \&gst_parse_sh,      IFCFG, BROADCAST ],
       [ "network",            \&gst_parse_sh,      IFCFG, NETWORK ],
       [ "gateway",            \&gst_parse_sh,      IFCFG, GATEWAY ],
       [ "essid",              \&gst_parse_sh,      IFCFG, WIRELESS_ESSID ],
       [ "key_type",           \&gst_network_get_wep_key_type, [ \&gst_parse_sh, IFCFG, WIRELESS_KEY ]],
       [ "key",                \&gst_network_get_wep_key,      [ \&gst_parse_sh, IFCFG, WIRELESS_KEY ]],
       [ "remote_address",     \&gst_parse_sh,      IFCFG, REMIP ],
#       [ "update_dns",         \&gst_network_pump_get_nodns, PUMP, "%dev%", "%bootproto%" ],
#       [ "dns1",               \&gst_parse_sh,      IFCFG, DNS1 ],
#       [ "dns2",               \&gst_parse_sh,      IFCFG, DNS2 ],
       [ "section",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, WVDIALSECT ]],
       [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PEERDNS ]],
       [ "mtu",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MTU ]],
       [ "mru",                \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MRU ]],
       [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, PAPNAME ]],
       [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, PAP,  "%login%" ]],
       [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_network_get_pap_passwd, CHAP, "%login%" ]],
       [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, MODEMPORT ]],
       [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, LINESPEED ]],
       [ "ppp_options",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh,      IFCFG, PPPOPTIONS ]],
       [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, DEFROUTE ]],
#       [ "debug",              \&gst_parse_sh_bool, IFCFG, DEBUG ],
       [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, PERSIST ]],
       [ "serial_escapechars", \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, ESCAPECHARS ]],
       [ "serial_hwctl",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_sh_bool, IFCFG, HARDFLOWCTL ]],
       [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_chat,    CHAT, "^atd[^0-9]*([0-9, -]+)" ]],
#       [ "enabled",            \&gst_network_interface_active, IFACE,
#                                                               \&gst_network_active_interfaces_get ],
#       [ "enabled",            \&gst_parse_trivial, 0 ]
       # wvdial settings
       [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Phone" ]],
       [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto DNS" ]],
       [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Username" ]],
       [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Password" ]],
       [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Modem" ]],
       [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Baud" ]],
       [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Check Def Route" ]],
       [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
       [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Command" ]],
       [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_parse_ini,     WVDIAL, "Dialer %section%", "Dial Prefix" ]],
      ]
    },

	  "debian-2.2" =>
	  {
	    ifaces_get => \&gst_network_debian_ifaces_get_existing,
	    fn =>
	    {
        INTERFACES  => "/etc/network/interfaces",
        IFACE       => "#iface#",
        CHAT        => "/etc/chatscripts/%section%",
        PPP_OPTIONS => "/etc/ppp/peers/%section%",
        PAP         => "/etc/ppp/pap-secrets",
        CHAP        => "/etc/ppp/chap-secrets",
        PUMP        => "/etc/pump.conf",
        WVDIAL      => "/etc/wvdial.conf"
      },
	    table =>
          [
#           [ "user",               \&gst_parse_trivial,                  0 ], # not supported.
           [ "dev",                \&gst_parse_trivial,                  IFACE ],
           [ "bootproto",          \&gst_network_deb22_parse_bootproto,  [INTERFACES, IFACE]],
           [ "auto",               \&gst_parse_interfaces_option_kw_not, [INTERFACES, IFACE], "noauto" ],
#           [ "name",               \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "name" ],
           [ "address",            \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "address" ],
           [ "netmask",            \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "netmask" ],
           [ "broadcast",          \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "broadcast" ],
           [ "network",            \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "network" ],
           [ "gateway",            \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "gateway" ],
           [ "essid",              \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "wireless[_-]essid" ],
           [ "key_type",           \&gst_network_get_wep_key_type,       [ \&gst_parse_interfaces_option_str, INTERFACES, IFACE, "wireless[_-]key1?" ]],
           [ "key",                \&gst_network_get_wep_key,            [ \&gst_parse_interfaces_option_str, INTERFACES, IFACE, "wireless[_-]key1?" ]],
           [ "remote_address",     \&gst_network_debian_parse_remote_address, [INTERFACES, IFACE]],
           [ "section",            \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "provider" ],
           [ "update_dns",         \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "usepeerdns" ]],
           [ "noauth",             \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "noauth" ]],
           [ "mtu",                \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_split_first_str, PPP_OPTIONS, "mtu", "[ \t]+" ]],
           [ "mru",                \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_split_first_str, PPP_OPTIONS, "mru", "[ \t]+" ]],
           [ "serial_port",        \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^(/dev/[^ \t]+)" ]],
           [ "serial_speed",       \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^([0-9]+)" ]],
           [ "login",              \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^login \"?([^\"]*)\"?" ]],
           [ "password",           \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_pap_passwd, PAP, "%login%" ]],
           [ "password",           \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_pap_passwd, CHAP, "%login%" ]],
           [ "ppp_options",        \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_unsup, PPP_OPTIONS ]],
           [ "set_default_gw",     \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "defaultroute" ]],
           [ "debug",              \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "debug" ]],
           [ "persist",            \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "persist" ]],
           [ "serial_escapechars", \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_split_first_str, PPP_OPTIONS, "escape", "[ \t]+" ]],
           [ "serial_hwctl",       \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "crtscts" ]],
           [ "external_line",      \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_chat, CHAT, "atd[^0-9]([0-9*#]*)[wW]" ]],
           [ "phone_number",       \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_chat, CHAT, "atd.*[ptwW]([0-9, -]+)" ]],
           [ "dial_command",       \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_chat, CHAT, "(atd[tp])[0-9, -w]+" ]],
           [ "volume",             \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_modem_volume, CHAT ]],
#           [ "enabled",            \&gst_network_interface_active,       IFACE,
#                                                                       \&gst_network_active_interfaces_get ],
#           [ "enabled",            \&gst_parse_trivial,                  0 ]
           ]
    },

    # Basicly the same as debian-2.2, but the "auto" option changes.
	  "debian-3.0" =>
	  {
	    ifaces_get => \&gst_network_debian_ifaces_get_existing,
	    fn =>
	    {
        INTERFACES  => "/etc/network/interfaces",
        IFACE       => "#iface#",
        CHAT        => "/etc/chatscripts/%section%",
        PPP_OPTIONS => "/etc/ppp/peers/%section%",
        PAP         => "/etc/ppp/pap-secrets",
        CHAP        => "/etc/ppp/chap-secrets",
      },
	    table =>
          [
#           [ "user",               \&gst_parse_trivial,                  0 ], # not supported.
           [ "dev",                \&gst_parse_trivial,                  IFACE ],
           [ "bootproto",          \&gst_network_deb22_parse_bootproto,  [INTERFACES, IFACE]],
           [ "auto",               \&gst_network_debian_woody_get_auto,  [INTERFACES, IFACE]],
#           [ "name",               \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "name" ],
           [ "address",            \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "address" ],
           [ "netmask",            \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "netmask" ],
           [ "broadcast",          \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "broadcast" ],
           [ "network",            \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "network" ],
           [ "gateway",            \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "gateway" ],
           [ "essid",              \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "wireless[_-]essid" ],
           [ "key_type",           \&gst_network_get_wep_key_type,       [ \&gst_parse_interfaces_option_str, INTERFACES, IFACE, "wireless[_-]key1?" ]],
           [ "key",                \&gst_network_get_wep_key,            [ \&gst_parse_interfaces_option_str, INTERFACES, IFACE, "wireless[_-]key1?" ]],
           [ "remote_address",     \&gst_network_debian_parse_remote_address, [INTERFACES, IFACE]],
           [ "section",            \&gst_parse_interfaces_option_str,    [INTERFACES, IFACE], "provider" ],
           [ "update_dns",         \&gst_network_check_type,            [IFACE, "(modem|isdn)", \&gst_parse_kw, PPP_OPTIONS, "usepeerdns" ]],
           [ "noauth",             \&gst_network_check_type,            [IFACE, "(modem|isdn)", \&gst_parse_kw, PPP_OPTIONS, "noauth" ]],
           [ "mtu",                \&gst_network_check_type,            [IFACE, "(modem|isdn)", \&gst_parse_split_first_str, PPP_OPTIONS, "mtu", "[ \t]+" ]],
           [ "mru",                \&gst_network_check_type,            [IFACE, "(modem|isdn)", \&gst_parse_split_first_str, PPP_OPTIONS, "mru", "[ \t]+" ]],
           [ "serial_port",        \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^(/dev/[^ \t]+)" ]],
           [ "serial_speed",       \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^([0-9]+)" ]],
           [ "login",              \&gst_network_check_type,            [IFACE, "(modem|isdn)", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^user \"?([^\"]*)\"?" ]],
           [ "password",           \&gst_network_check_type,            [IFACE, "(modem|isdn)", \&gst_network_get_pap_passwd, PAP, "%login%" ]],
           [ "password",           \&gst_network_check_type,            [IFACE, "(modem|isdn)", \&gst_network_get_pap_passwd, CHAP, "%login%" ]],
           [ "ppp_options",        \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_unsup, PPP_OPTIONS ]],
           [ "set_default_gw",     \&gst_network_check_type,            [IFACE, "(modem|isdn)", \&gst_parse_kw, PPP_OPTIONS, "defaultroute" ]],
           [ "debug",              \&gst_network_check_type,            [IFACE, "(modem|isdn)", \&gst_parse_kw, PPP_OPTIONS, "debug" ]],
           [ "persist",            \&gst_network_check_type,            [IFACE, "(modem|isdn)", \&gst_parse_kw, PPP_OPTIONS, "persist" ]],
           [ "serial_escapechars", \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_split_first_str, PPP_OPTIONS, "escape", "[ \t]+" ]],
           [ "serial_hwctl",       \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "crtscts" ]],
           [ "external_line",      \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_chat, CHAT, "atd[^0-9]([0-9*#]*)[wW]" ]],
           [ "external_line",      \&gst_network_check_type,            [IFACE, "isdn", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^number[ \t]+(.+)[wW]" ]],
           [ "phone_number",       \&gst_network_check_type,            [IFACE, "isdn", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^number.*[wW \t](.*)" ]],
           [ "phone_number",       \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_chat, CHAT, "atd.*[ptwW]([0-9, -]+)" ]],
           [ "dial_command",       \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_chat, CHAT, "(atd[tp])[0-9, -w]+" ]],
           [ "volume",             \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_modem_volume, CHAT ]],
#           [ "enabled",            \&gst_network_interface_active,       IFACE,
#                                                                       \&gst_network_active_interfaces_get ],
#           [ "enabled",            \&gst_parse_trivial,                  0 ]
         ]
       },

	  "suse-7.0" =>
	  {
	    ifaces_get => \&gst_network_suse70_ifaces_get_existing,
	    fn =>
	    {
        IFCFG => "/etc/rc.config",
        IFACE => "#iface#"
      },
	    table =>
          [
           [ "bootproto",          \&gst_network_suse70_parse_bootproto,  [IFCFG, IFACE] ],
           [ "auto",               \&gst_network_suse70_parse_iface_auto, [IFCFG, IFACE], NETCONFIG ],
           [ "user",               \&gst_parse_trivial,                   0 ], # not supported.
           [ "name",               \&gst_network_suse70_parse_iface_sh,   [IFCFG, IFACE], GST_IFACE_NAME ],
           [ "dev",                \&gst_network_suse70_parse_iface_sh,   [IFCFG, IFACE], NETDEV ],
           [ "address",            \&gst_network_suse70_parse_iface_sh,   [IFCFG, IFACE], IPADDR ],
           [ "netmask",            \&gst_network_suse70_get_ifconfig_arg, [IFCFG, IFACE], netmask ],
           [ "broadcast",          \&gst_network_suse70_get_ifconfig_arg, [IFCFG, IFACE], broadcast ],
#          [ "network",            \&gst_parse_trivial,                   0 ], # not supported.
#          [ "gateway",            \&gst_parse_sh,      IFCFG, GATEWAY ], # not supported
           [ "remote_address",     \&gst_network_suse70_get_ifconfig_arg, [IFCFG, IFACE], pointopoint ],
           [ "enabled",            \&gst_network_interface_active,        IFACE,
                                                                \&gst_network_suse70_active_interfaces_get ],
           [ "enabled",            \&gst_parse_trivial,                   0 ]
           ]
       },

    "suse-9.0" =>
	  {
	    ifaces_get => \&gst_network_suse90_ifaces_get_existing,
	    fn =>
	    {
        IFCFG      => "/etc/sysconfig/network/ifcfg-#iface#",
        ROUTE_CONF => "/etc/sysconfig/network/routes",
        IFACE      => "#iface#"
      },
      table =>
      [
       [ "dev",                \&gst_network_suse9_get_dev_name,      IFACE ],
#       [ "enabled",            \&gst_network_interface_active,        "%dev%", \&gst_network_active_interfaces_get ],
       [ "auto",               \&gst_network_suse90_get_auto,         IFCFG,   STARTMODE      ],
       [ "bootproto",          \&gst_network_parse_bootproto,         IFCFG,   BOOTPROTO      ],
       [ "address",            \&gst_parse_sh,                        IFCFG,   IPADDR         ],
       [ "netmask",            \&gst_parse_sh,                        IFCFG,   NETMASK        ],
       [ "remote_address",     \&gst_parse_sh,                        IFCFG,   REMOTE_IPADDR  ],
       [ "essid",              \&gst_parse_sh,                        IFCFG,   WIRELESS_ESSID ],
       [ "key_type",           \&gst_network_get_wep_key_type,        [ \&gst_parse_sh, IFCFG, WIRELESS_KEY ]],
       [ "key",                \&gst_network_get_wep_key,             [ \&gst_parse_sh, IFCFG, WIRELESS_KEY ]],
       [ "gateway",            \&gst_network_suse90_get_gateway,      ROUTE_CONF, "%address%", "%netmask%" ],
       [ "gateway",            \&gst_network_suse90_get_plip_gateway, ROUTE_CONF, "%remote_address%"       ],
       # Modem stuff goes here
       [ "serial_port",        \&gst_parse_sh,                        IFCFG,   MODEM_DEVICE   ],
       [ "serial_speed",       \&gst_parse_sh,                        IFCFG,   SPEED          ],
       [ "mtu",                \&gst_parse_sh,                        IFCFG,   MTU            ],
       [ "mru",                \&gst_parse_sh,                        IFCFG,   MRU            ],
#       [ "ppp_options",        \&gst_parse_sh,                        IFCFG,   PPPD_OPTIONS   ],
       [ "dial_command",       \&gst_parse_sh,                        IFCFG,   DIALCOMMAND    ],
       [ "external_line",      \&gst_parse_sh,                        IFCFG,   DIALPREFIX     ],
       [ "section",            \&gst_parse_sh,                        IFCFG,   PROVIDER       ],
       [ "volume",             \&gst_parse_sh_re,                     IFCFG,   INIT8, "AT.*[ml]([0-3])" ],
       [ "login",              \&gst_network_suse90_parse_provider_file,       "%section%", USERNAME     ],
       [ "password",           \&gst_network_suse90_parse_provider_file,       "%section%", PASSWORD     ],
       [ "phone_number",       \&gst_network_suse90_parse_provider_file,       "%section%", PHONE        ],
       [ "dns1",               \&gst_network_suse90_parse_provider_file,       "%section%", DNS1         ],
       [ "dns2",               \&gst_network_suse90_parse_provider_file,       "%section%", DNS2         ],
       [ "update_dns",         \&gst_network_suse90_parse_provider_file_bool,  "%section%", MODIFYDNS    ],
       [ "persist",            \&gst_network_suse90_parse_provider_file_bool,  "%section%", PERSIST      ],
       [ "stupid",             \&gst_network_suse90_parse_provider_file_bool,  "%section%", STUPIDMODE   ],
       [ "set_default_gw",     \&gst_network_suse90_parse_provider_file_bool,  "%section%", DEFAULTROUTE ],
      ]
    },

	  "pld-1.0" =>
	  {
	    ifaces_get => \&gst_network_sysconfig_pld10_ifaces_get_existing,
	    fn =>
	    {
        IFCFG => "/etc/sysconfig/interfaces/ifcfg-#iface#",
        CHAT  => "/etc/sysconfig/interfaces/data/chat-#iface#",
        IFACE => "#iface#",
        PAP   => "/etc/ppp/pap-secrets",
        CHAP  => "/etc/ppp/chap-secrets",
        PUMP  => "/etc/pump.conf"
      },
	    table =>
          [
           [ "bootproto",          \&gst_network_pld10_parse_bootproto, IFCFG, BOOTPROTO ],
           [ "auto",               \&gst_parse_sh_bool, IFCFG, ONBOOT ],
#           [ "user",               \&gst_parse_sh_bool, IFCFG, USERCTL ],
#           [ "name",               \&gst_parse_sh,      IFCFG, DEVICE ],
           [ "dev",                \&gst_parse_sh,      IFCFG, DEVICE ],
           [ "address",            \&gst_network_pld10_get_ipaddr, IFCFG, IPADDR, "address" ],
           [ "netmask",            \&gst_network_pld10_get_ipaddr, IFCFG, IPADDR, "netmask" ],
#           [ "broadcast",          \&gst_parse_sh,      IFCFG, BROADCAST ],
#           [ "network",            \&gst_parse_sh,      IFCFG, NETWORK ],
           [ "gateway",            \&gst_parse_sh,      IFCFG, GATEWAY ],
           [ "remote_address",     \&gst_parse_sh,      IFCFG, REMIP ],
#           [ "update_dns",         \&gst_network_pump_get_nodns, PUMP, "%dev%", "%bootproto%" ],
#           [ "dns1",               \&gst_parse_sh,      IFCFG, DNS1 ],
#           [ "dns2",               \&gst_parse_sh,      IFCFG, DNS2 ],
           [ "update_dns",         \&gst_parse_sh_bool, IFCFG, PEERDNS ],
           [ "mtu",                \&gst_parse_sh,      IFCFG, MTU ],
           [ "mru",                \&gst_parse_sh,      IFCFG, MRU ],
           [ "login",              \&gst_parse_sh,      IFCFG, PAPNAME ],
           [ "password",           \&gst_network_get_pap_passwd, PAP,  "%login%" ],
           [ "password",           \&gst_network_get_pap_passwd, CHAP, "%login%" ],
           [ "serial_port",        \&gst_parse_sh,      IFCFG, MODEMPORT ],
           [ "serial_speed",       \&gst_parse_sh,      IFCFG, LINESPEED ],
#           [ "ppp_options",        \&gst_parse_sh,      IFCFG, PPPOPTIONS ],
#           [ "section",            \&gst_parse_sh,      IFCFG, WVDIALSECT ],
           [ "set_default_gw",     \&gst_parse_sh_bool, IFCFG, DEFROUTE ],
#           [ "debug",              \&gst_parse_sh_bool, IFCFG, DEBUG ],
           [ "persist",            \&gst_parse_sh_bool, IFCFG, PERSIST ],
           [ "serial_escapechars", \&gst_parse_sh_bool, IFCFG, ESCAPECHARS ],
           [ "serial_hwctl",       \&gst_parse_sh_bool, IFCFG, HARDFLOWCTL ],
           [ "phone_number",       \&gst_parse_chat,    CHAT, "^atd[^0-9]*([0-9, -]+)" ],
#           [ "enabled",            \&gst_network_interface_active, IFACE, \&gst_network_active_interfaces_get ],
#           [ "enabled",            \&gst_parse_trivial, 0 ]
          ]
    },
    "slackware-9.1.0" =>
	  {
      ifaces_get => \&gst_network_slackware91_ifaces_get_existing,
      fn =>
      {
        RC_INET_CONF => "/etc/rc.d/rc.inet1.conf",
        RC_INET      => "/etc/rc.d/rc.inet1",
        RC_LOCAL     => "/etc/rc.d/rc.local",
        IFACE        => "#iface#",
        WIRELESS     => "/etc/pcmcia/wireless.opts",
        PPP_OPTIONS  => "/etc/ppp/options",
        PAP          => "/etc/ppp/pap-secrets",
        CHAP         => "/etc/ppp/chap-secrets",
        CHAT         => "/etc/ppp/pppscript",
      },
      table =>
      [
       [ "user",               \&gst_parse_trivial,                  0 ], # not supported.
       [ "dev",                \&gst_parse_trivial,                  IFACE ],
       [ "address",            \&gst_parse_rcinet1conf,              [RC_INET_CONF, IFACE], IPADDR ],
       [ "netmask",            \&gst_parse_rcinet1conf,              [RC_INET_CONF, IFACE], NETMASK ],
       [ "gateway",            \&gst_network_get_gateway,            RC_INET_CONF, GATEWAY, "%address%", "%netmask%" ],
       [ "auto",               \&gst_network_slackware91_get_auto,   [RC_INET, RC_LOCAL, IFACE]],
       [ "bootproto",          \&gst_network_slackware91_parse_bootproto, [RC_INET_CONF, IFACE]],
       [ "essid",              \&gst_parse_wireless_opts,            [ WIRELESS, IFACE ], \&gst_network_get_wireless_ifaces, ESSID ],
       [ "key_type",           \&gst_network_get_wep_key_type,      [ \&gst_parse_wireless_opts, WIRELESS, IFACE, \&gst_network_get_wireless_ifaces, KEY ]],
       [ "key",                \&gst_network_get_wep_key,           [ \&gst_parse_wireless_opts, WIRELESS, IFACE, \&gst_network_get_wireless_ifaces, KEY ]],
       [ "enabled",            \&gst_network_interface_active,       IFACE, \&gst_network_active_interfaces_get ],
       # Modem stuff
       [ "update_dns",         \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "usepeerdns" ]],
       [ "noauth",             \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "noauth" ]],
       [ "mtu",                \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_split_first_str, PPP_OPTIONS, "mtu", "[ \t]+" ]],
       [ "mru",                \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_split_first_str, PPP_OPTIONS, "mru", "[ \t]+" ]],
       [ "serial_port",        \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^(/dev/[^ \t]+)" ]],
       [ "serial_speed",       \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^([0-9]+)" ]],
       [ "login",              \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_re, PPP_OPTIONS, "^name \"?([^\"]*)\"?" ]],
       [ "password",           \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_pap_passwd, PAP, "%login%" ]],
       [ "password",           \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_pap_passwd, CHAP, "%login%" ]],
       [ "ppp_options",        \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_ppp_options_unsup, PPP_OPTIONS ]],
       [ "set_default_gw",     \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "defaultroute" ]],
       [ "debug",              \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "debug" ]],
       [ "persist",            \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "persist" ]],
       [ "serial_escapechars", \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_split_first_str, PPP_OPTIONS, "escape", "[ \t]+" ]],
       [ "serial_hwctl",       \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_kw, PPP_OPTIONS, "crtscts" ]],
       [ "external_line",      \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_chat, CHAT, "atd[^0-9]*([0-9*#]*)[wW]" ]],
       [ "phone_number",       \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_chat, CHAT, "atd.*[ptw]([0-9, -]+)" ]],
       [ "dial_command",       \&gst_network_check_type,            [IFACE, "modem", \&gst_parse_chat, CHAT, "(atd[tp])[0-9, -w]+" ]],
       [ "volume",             \&gst_network_check_type,            [IFACE, "modem", \&gst_network_get_modem_volume, CHAT ]],
       ]
      },

    "gentoo" =>
    {
      ifaces_get => \&gst_network_gentoo_ifaces_get_existing,
      fn =>
      {
        NET          => "/etc/conf.d/net",
        PPPNET       => "/etc/conf.d/net.#iface#",
        INIT         => "net.#iface#",
        IFACE        => "#iface#",
        WIRELESS     => "/etc/conf.d/wireless",
      },
      table =>
      [
       [ "auto",               \&gst_service_gentoo_get_service_status, INIT, "default" ],
       [ "user",               \&gst_parse_trivial,                  0 ], # not supported.
       [ "dev",                \&gst_parse_trivial,                  IFACE ],
       [ "address",            \&gst_parse_confd_net_re,             NET, "config_%dev%", "^[ \t]*([0-9\.]+)" ],
       [ "netmask",            \&gst_parse_confd_net_re,             NET, "config_%dev%", "netmask[ \t]+([0-9\.]*)" ],
       [ "remote_address",     \&gst_parse_confd_net_re,             NET, "config_%dev%", "dest_address[ \t]+([0-9\.]*)" ],
#       [ "gateway",            \&gst_parse_sh_re,                    NET, "gateway", "%dev%/([0-9\.\:]*)" ],
       [ "gateway",            \&gst_network_gentoo_parse_gateway,   [ NET, IFACE ]],
       [ "enabled",            \&gst_network_interface_active,       IFACE, \&gst_network_active_interfaces_get ],
       [ "bootproto",          \&gst_network_gentoo_parse_bootproto, [ NET, IFACE ]],
       [ "essid",              \&gst_parse_sh,                       WIRELESS, "essid_%dev%" ],
       [ "key_type",           \&gst_network_get_wep_key_type,       [ \&gst_parse_sh, WIRELESS, "key_%essid%" ]],
       [ "key",                \&gst_network_get_wep_key,            [ \&gst_parse_sh, WIRELESS, "key_%essid%" ]],
       # modem stuff
       [ "update_dns",         \&gst_parse_sh_bool,                  PPPNET, PEERDNS ],
       [ "mtu",                \&gst_parse_sh,                       PPPNET, MTU ],
       [ "mru",                \&gst_parse_sh,                       PPPNET, MRU ],
       [ "serial_port",        \&gst_parse_sh,                       PPPNET, MODEMPORT ],
       [ "serial_speed",       \&gst_parse_sh,                       PPPNET, LINESPEED ],
       [ "login",              \&gst_parse_sh,                       PPPNET, USERNAME ],
       [ "password",           \&gst_parse_sh,                       PPPNET, PASSWORD ],
       [ "ppp_options",        \&gst_parse_sh,                       PPPNET, PPPOPTIONS ],
       [ "set_default_gw",     \&gst_parse_sh_bool,                  PPPNET, DEFROUTE ],
       [ "debug",              \&gst_parse_sh_bool,                  PPPNET, DEBUG ],
       [ "persist",            \&gst_parse_sh_bool,                  PPPNET, PERSIST ],
       [ "serial_escapechars", \&gst_parse_sh_bool,                  PPPNET, ESCAPECHARS ],
       [ "serial_hwctl",       \&gst_parse_sh_bool,                  PPPNET, HARDFLOWCTL ],
       [ "external_line",      \&gst_parse_sh_re,                    PPPNET, NUMBER, "^([0-9*#]*)wW" ],
       [ "phone_number",       \&gst_parse_sh_re,                    PPPNET, NUMBER, "w?([0-9]*)\$" ],
       [ "volume",             \&gst_parse_sh_re,                    PPPNET, INITSTRING, "^at.*[ml]([0-3])" ],
      ]
    },

    "freebsd-5" =>
    {
      ifaces_get => \&gst_network_freebsd_ifaces_get_existing,
      fn =>
      {
        RC_CONF         => "/etc/rc.conf",
        RC_CONF_DEFAULT => "/etc/defaults/rc.conf",
        STARTIF         => "/etc/start_if.#iface#",
        PPPCONF         => "/etc/ppp/ppp.conf",
        IFACE           => "#iface#",
      },
      table =>
      [
       [ "auto",               \&gst_network_freebsd5_get_auto,      [RC_CONF, RC_CONF_DEFAULT, IFACE ]],
       [ "user",               \&gst_parse_trivial,                  0 ], # not supported.
       [ "dev",                \&gst_parse_trivial,                  IFACE ],
       # we need to double check these values both in the start_if and in the rc.conf files, in this order
       [ "address",            \&gst_parse_startif,                  STARTIF, "inet[ \t]+([0-9\.]+)" ],
       [ "address",            \&gst_parse_sh_re,                    RC_CONF, "ifconfig_%dev%", "inet[ \t]+([0-9\.]+)" ],
       [ "netmask",            \&gst_parse_startif,                  STARTIF, "netmask[ \t]+([0-9\.]+)" ],
       [ "netmask",            \&gst_parse_sh_re,                    RC_CONF, "ifconfig_%dev%", "netmask[ \t]+([0-9\.]+)" ],
       [ "remote_address",     \&gst_parse_startif,                  STARTIF, "dest_address[ \t]+([0-9\.]+)" ],
       [ "remote_address",     \&gst_parse_sh_re,                    RC_CONF, "ifconfig_%dev%", "dest_address[ \t]+([0-9\.]+)" ],
       [ "essid",              \&gst_parse_startif,                  STARTIF, "ssid[ \t]+(\".*\"|[^\"][^ ]+)" ],
       [ "essid",              \&gst_parse_sh_re,                    RC_CONF, "ifconfig_%dev%", "ssid[ \t]+([^ ]*)" ],
       # this is for plip interfaces
       [ "gateway",            \&gst_network_get_plip_gateway,       RC_CONF, "defaultrouter", "%remote_address%" ],
       [ "gateway",            \&gst_network_get_gateway,            RC_CONF, "defaultrouter", "%address%", "%netmask%" ],
       [ "enabled",            \&gst_network_interface_active,       IFACE, \&gst_network_freebsd5_active_interfaces_get ],
       [ "bootproto",          \&gst_network_parse_bootproto,        RC_CONF, "ifconfig_%dev%" ],
       # Modem stuff
       [ "serial_port",        \&gst_network_parse_pppconf,          [ PPPCONF, STARTIF, IFACE ], "device"   ],
       [ "serial_speed",       \&gst_network_parse_pppconf,          [ PPPCONF, STARTIF, IFACE ], "speed"    ],
       [ "mtu",                \&gst_network_parse_pppconf,          [ PPPCONF, STARTIF, IFACE ], "mtu"      ],
       [ "mru",                \&gst_network_parse_pppconf,          [ PPPCONF, STARTIF, IFACE ], "mru"      ],
       [ "login",              \&gst_network_parse_pppconf,          [ PPPCONF, STARTIF, IFACE ], "authname" ],
       [ "password",           \&gst_network_parse_pppconf,          [ PPPCONF, STARTIF, IFACE ], "authkey"  ],
       [ "update_dns",         \&gst_network_parse_pppconf_bool,     [ PPPCONF, STARTIF, IFACE ], "dns"      ],
       [ "set_default_gw",     \&gst_network_parse_pppconf_bool,     [ PPPCONF, STARTIF, IFACE ], "default HISADDR" ],
       [ "external_line",      \&gst_network_parse_pppconf_re,       [ PPPCONF, STARTIF, IFACE ], "phone", "[ \t]+([0-9]+)[wW]" ],
       [ "phone_number",       \&gst_network_parse_pppconf_re,       [ PPPCONF, STARTIF, IFACE ], "phone", "[wW]?([0-9]+)[ \t]*\$" ],
       [ "dial_command",       \&gst_network_parse_pppconf_re,       [ PPPCONF, STARTIF, IFACE ], "dial",  "(ATD[TP])" ],
       [ "volume",             \&gst_network_parse_pppconf_re,       [ PPPCONF, STARTIF, IFACE ], "dial",  "AT.*[ml]([0-3]) OK " ],
       [ "persist",            \&gst_network_get_freebsd5_ppp_persist, [ STARTIF, IFACE ]],
      ]
    },
	  );
  
  my $dist = $dist_map{$gst_dist};
  return %{$dist_tables{$dist}} if $dist;

  &gst_report ("platform_no_table", $gst_dist);
  return undef;
}

sub gst_network_get_replace_table
{
  my %dist_map =
	 (
    "redhat-5.2"   => "redhat-5.2",
	  "redhat-6.0"   => "redhat-6.2",
	  "redhat-6.1"   => "redhat-6.2",
	  "redhat-6.2"   => "redhat-6.2",
	  "redhat-7.0"   => "redhat-7.0",
	  "redhat-7.1"   => "redhat-7.0",
	  "redhat-7.2"   => "redhat-7.2",
          "redhat-8.0"   => "redhat-7.2",
          "redhat-9"     => "redhat-7.2",
	  "mandrake-7.1" => "redhat-6.2",
	  "mandrake-7.2" => "redhat-6.2",
	  "mandrake-9.0" => "redhat-7.0",
	  "mandrake-9.1" => "redhat-7.0",
	  "mandrake-9.2" => "redhat-7.0",
          "mandrake-10.0" => "redhat-7.0",
          "mandrake-10.1" => "redhat-7.0",
          "mandrake-10.2" => "redhat-7.0",
          "mandriva-2006.0" => "redhat-7.0",
          "mandriva-2006.1" => "redhat-7.0",
          "mandriva-2007.0" => "redhat-7.0",
	  "mandriva-2007.1" => "redhat-7.0",
          "yoper-2.2"       => "redhat-7.0",
          "blackpanther-4.0" => "redhat-7.0",
 	  "conectiva-9"  => "redhat-7.0",
 	  "conectiva-10" => "redhat-7.0",
          "debian-2.2"   => "debian-2.2",
          "debian-3.0"   => "debian-2.2",
          "debian-3.1"   => "debian-2.2",
          "debian-4.0"   => "debian-2.2",
          "debian-5.0"   => "debian-2.2",
          "debian-testing" => "debian-2.2",
          "ubuntu-5.04"  => "debian-2.2",
          "ubuntu-5.10"  => "debian-2.2",
          "ubuntu-6.06"  => "debian-2.2",
          "ubuntu-6.10"  => "debian-2.2",
          "ubuntu-7.04"  => "debian-2.2",
          "ubuntu-7.10"  => "debian-2.2",
          "ubuntu-8.04"  => "debian-2.2",
          "suse-7.0"     => "suse-7.0",
          "suse-9.0"     => "suse-9.0",
          "suse-9.1"     => "suse-9.0",
	  "turbolinux-7.0"   => "redhat-7.0",
          "pld-1.0"      => "pld-1.0",
          "pld-1.1"      => "pld-1.0",
          "pld-1.99"     => "pld-1.0",
          "fedora-1"     => "redhat-7.2",
          "fedora-2"     => "redhat-7.2",
          "fedora-3"     => "redhat-7.2",
          "fedora-4"     => "redhat-7.2",
          "fedora-5"     => "redhat-7.2",
          "rpath"        => "redhat-7.2",
          "vine-3.0"     => "redhat-7.0",
          "vine-3.1"     => "redhat-7.0",
          "ark"          => "redhat-7.0",
          "slackware-9.1.0" => "slackware-9.1.0",
          "slackware-10.0.0" => "slackware-9.1.0",
          "slackware-10.1.0" => "slackware-9.1.0",
          "slackware-10.2.0" => "slackware-9.1.0",
          "gentoo"       => "gentoo",
          "vlos-1.2"     => "gentoo",
          "freebsd-5"    => "freebsd-5",
          "freebsd-6"    => "freebsd-5",
         );

  my %dist_tables =
	 (
	  "redhat-6.2" =>
	  {
	    fn =>
	    {
        SYSCONFIG_NW => "/etc/sysconfig/network",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        SMB_CONF     => "/etc/smb.conf",
        WVDIAL       => "/etc/wvdial.conf"
      },
	    table =>
          [
           [ "auto",          \&gst_replace_sh_bool,          SYSCONFIG_NW, NETWORKING ],
           [ "hostname",      \&gst_replace_sh,               SYSCONFIG_NW, HOSTNAME ],
           [ "hostname",      \&gst_network_run_hostname ],
           [ "gateway",       \&gst_replace_sh,               SYSCONFIG_NW, GATEWAY],
           [ "gatewaydev",    \&gst_replace_sh,               SYSCONFIG_NW, GATEWAYDEV],
           [ "domain",        \&gst_replace_sh,               SYSCONFIG_NW, DOMAIN],
           [ "domain",        \&gst_replace_join_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
           [ "nameserver",    \&gst_replace_join_all,         RESOLV_CONF,  "nameserver", "[ \t]+" ],
           [ "searchdomain",  \&gst_replace_join_first_array, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
           [ "order",         \&gst_replace_join_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
           [ "hostmatch",     \&gst_replace_join_first_bool,  HOST_CONF,    "multi", "[ \t]+", "on", "off" ],
           [ "statichost",    \&gst_replace_join_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
           [ "workgroup",     \&gst_replace_ini,              SMB_CONF,     "global", "workgroup" ],     
           [ "smbdesc",       \&gst_replace_ini,              SMB_CONF,     "global", "server string" ],
           [ "winsserver",    \&gst_replace_ini,              SMB_CONF,     "global", "wins server" ],
           [ "winsuse",       \&gst_replace_ini_bool,         SMB_CONF,     "global", "wins support" ],
           [ "smbuse",        \&gst_service_sysv_set_status,  91, "smb", "%smbuse%" ],
           [ "interface",     \&gst_network_interfaces_set,		OLD_HASH ],
           [ "gateway",       \&gst_network_route_set_default_gw, "%gatewaydev%" ]
           ]
             },
    
	  "redhat-7.0" =>
	  {
	    fn =>
	    {
        SYSCONFIG_NW => "/etc/sysconfig/network",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        SMB_CONF     => "/etc/samba/smb.conf",
        WVDIAL       => "/etc/wvdial.conf"
      },
	    table =>
          [
           [ "auto",          \&gst_replace_sh_bool,          SYSCONFIG_NW, NETWORKING ],
           [ "hostname",      \&gst_replace_sh,               SYSCONFIG_NW, HOSTNAME ],
           [ "hostname",      \&gst_network_run_hostname ],
           [ "gateway",       \&gst_replace_sh,               SYSCONFIG_NW, GATEWAY],
           [ "gatewaydev",    \&gst_replace_sh,               SYSCONFIG_NW, GATEWAYDEV],
           [ "domain",        \&gst_replace_sh,               SYSCONFIG_NW, DOMAIN],
           [ "domain",        \&gst_replace_join_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
           [ "nameserver",    \&gst_replace_join_all,         RESOLV_CONF,  "nameserver", "[ \t]+" ],
           [ "searchdomain",  \&gst_replace_join_first_array, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
           [ "order",         \&gst_replace_join_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
           [ "hostmatch",     \&gst_replace_join_first_bool,  HOST_CONF,    "multi", "[ \t]+", "on", "off" ],
           [ "statichost",    \&gst_replace_join_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
           [ "workgroup",     \&gst_replace_ini,              SMB_CONF,     "global", "workgroup" ],     
           [ "smbdesc",       \&gst_replace_ini,              SMB_CONF,     "global", "server string" ],
           [ "winsserver",    \&gst_replace_ini,              SMB_CONF,     "global", "wins server" ],
           [ "winsuse",       \&gst_replace_ini_bool,         SMB_CONF,     "global", "wins support" ],
           [ "smbuse",        \&gst_service_sysv_set_status,  91, "smb", "%smbuse%" ],
           [ "interface",     \&gst_network_interfaces_set,		OLD_HASH ],
           [ "gateway",       \&gst_network_route_set_default_gw, "%gatewaydev%" ]
           ]
             },

	  "redhat-7.2" =>
	  {
	    fn =>
	    {
        SYSCONFIG_NW => ["/etc/sysconfig/networking/profiles/default/network",
                         "/etc/sysconfig/networking/network",
                         "/etc/sysconfig/network"],
        RESOLV_CONF  => ["/etc/sysconfig/networking/profiles/default/resolv.conf",
                         "/etc/resolv.conf"],
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => ["/etc/sysconfig/networking/profiles/default/hosts",
                         "/etc/hosts"],
        SMB_CONF     => "/etc/samba/smb.conf",
        WVDIAL       => "/etc/wvdial.conf"
      },
	    table =>
          [
           [ "auto",          \&gst_replace_sh_bool,          SYSCONFIG_NW, NETWORKING ],
           [ "hostname",      \&gst_replace_sh,               SYSCONFIG_NW, HOSTNAME ],
           [ "hostname",      \&gst_network_run_hostname ],
           [ "gateway",       \&gst_replace_sh,               SYSCONFIG_NW, GATEWAY],
           [ "gatewaydev",    \&gst_replace_sh,               SYSCONFIG_NW, GATEWAYDEV],
           [ "domain",        \&gst_replace_sh,               SYSCONFIG_NW, DOMAIN],
           [ "domain",        \&gst_replace_join_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
           [ "nameserver",    \&gst_replace_join_all,         RESOLV_CONF,  "nameserver", "[ \t]+" ],
           [ "searchdomain",  \&gst_replace_join_first_array, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
           [ "order",         \&gst_replace_join_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
           [ "hostmatch",     \&gst_replace_join_first_bool,  HOST_CONF,    "multi", "[ \t]+", "on", "off" ],
           [ "statichost",    \&gst_replace_join_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
           [ "workgroup",     \&gst_replace_ini,              SMB_CONF,     "global", "workgroup" ],     
           [ "smbdesc",       \&gst_replace_ini,              SMB_CONF,     "global", "server string" ],
           [ "winsserver",    \&gst_replace_ini,              SMB_CONF,     "global", "wins server" ],
           [ "winsuse",       \&gst_replace_ini_bool,         SMB_CONF,     "global", "wins support" ],
           [ "smbuse",        \&gst_service_sysv_set_status,  91, "smb", "%smbuse%" ],
           [ "interface",     \&gst_network_interfaces_set,		OLD_HASH ],
           [ "gateway",       \&gst_network_route_set_default_gw, "%gatewaydev%" ]
           ]
             },

	  "debian-2.2" =>
	  {
	    fn =>
	    {
        OPTIONS     => "/etc/network/options",
        RESOLV_CONF => "/etc/resolv.conf",
        HOST_CONF   => "/etc/host.conf",
        HOSTS       => "/etc/hosts",
        HOSTNAME    => "/etc/hostname",
        SMB_CONF    => "/etc/samba/smb.conf",
        WVDIAL      => "/etc/wvdial.conf"
      },
	    table =>
          [
           [ "hostname",      \&gst_replace_line_first,       HOSTNAME ],
           [ "hostname",      \&gst_network_run_hostname ],
           [ "domain",        \&gst_replace_join_first_str,   RESOLV_CONF, "domain", "[ \t]+" ],
           [ "nameserver",    \&gst_replace_join_all,         RESOLV_CONF, "nameserver", "[ \t]+" ],
           [ "searchdomain",  \&gst_replace_join_first_array, RESOLV_CONF, "search", "[ \t]+", "[ \t]+" ],
           [ "order",         \&gst_replace_join_first_array, HOST_CONF,   "order", "[ \t]+", ",[ \t]*" ],
           [ "hostmatch",     \&gst_replace_join_first_bool,  HOST_CONF,   "multi", "[ \t]+", "on", "off" ],
           [ "statichost",    \&gst_replace_join_hash,        HOSTS,       "[ \t]+", "[ \t]+" ],
           [ "workgroup",     \&gst_replace_ini,              SMB_CONF,    "global", "workgroup" ],
           [ "smbdesc",       \&gst_replace_ini,              SMB_CONF,    "global", "server string" ],
           [ "winsserver",    \&gst_replace_ini,              SMB_CONF,    "global", "wins server" ],
           [ "winsuse",       \&gst_replace_ini_bool,         SMB_CONF,    "global", "wins support" ],
           [ "smbuse",        \&gst_service_sysv_set_status,  91, "samba" ],
           [ "interface",     \&gst_network_interfaces_set,		OLD_HASH ]
           ]
             },

	  "suse-7.0" =>
	  {
	    fn =>
	    {
        RC_CONFIG    => "/etc/rc.config",
        ROUTE_CONF   => "/etc/route.conf",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        SMB_CONF     => "/etc/smb.conf",
        WVDIAL       => "/etc/wvdial.conf",
        SUSECONFIG   => "SuSEconfig"
      },
	    table =>
		   [
        [ "hostname",      \&gst_replace_sh_set_hostname,  RC_CONFIG, FQHOSTNAME ],
        [ "hostname",      \&gst_network_run_hostname ],
        [ "gateway",       \&gst_replace_join_first_str,   ROUTE_CONF, "default", "[ \t]+" ],
        [ "gateway",       \&gst_replace_join_first_str,   ROUTE_CONF, "0.0.0.0", "[ \t]+" ],
		    [ "domain",        \&gst_replace_sh_set_domain,    RC_CONFIG, FQHOSTNAME ],
		    [ "nameserver",    \&gst_replace_sh_join,          RC_CONFIG, NAMESERVER, "[ \t]+" ],
		    [ "searchdomain",  \&gst_replace_sh_join,          RC_CONFIG, SEARCHLIST, "[ \t]+" ],
# Remove /etc/resolv.conf so SuSEconfig sets the new values.
        [ "searchdomain",  \&gst_file_remove,              RESOLV_CONF ],
        [ "order",         \&gst_replace_join_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
        [ "hostmatch",     \&gst_replace_join_first_bool,  HOST_CONF,    "multi", "[ \t]+", "on", "off" ],
        [ "statichost",    \&gst_replace_join_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
        [ "workgroup",     \&gst_replace_ini,              SMB_CONF,     "global", "workgroup" ],     
        [ "smbdesc",       \&gst_replace_ini,              SMB_CONF,     "global", "server string" ],
        [ "winsserver",    \&gst_replace_ini,              SMB_CONF,     "global", "wins server" ],
        [ "winsuse",       \&gst_replace_ini_bool,         SMB_CONF,     "global", "wins support" ],
        [ "smbuse",        \&gst_service_sysv_set_status,  20, "smb", "%smbuse%" ],
        [ "interface",     \&gst_network_suse70_set_ppp,   [WVDIAL, "%dialing%"] ],
# We're calling this sepparated cuz we are going to call wvdial to activate.           
        [ "interface",     \&gst_network_suse70_activate_ppp ],
        [ "interface",     \&gst_network_interfaces_set,	 OLD_HASH ],
        [ "_always_",      \&gst_file_run,                 SUSECONFIG ],
		    ]
    },

    "suse-9.0" =>
    {
      fn =>
      {
        ROUTE_CONF   => "/etc/sysconfig/network/routes",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        HOSTNAME     => "/etc/HOSTNAME",
        SMB_CONF     => "/etc/samba/smb.conf",
      },
      table =>
      [
       [ "hostname",      \&gst_replace_fq_hostname,      HOSTNAME,    "%hostname%", "%domain%" ],
       [ "hostname",      \&gst_network_run_hostname ],
       [ "domain",        \&gst_replace_join_first_str,   RESOLV_CONF, "domain", "[ \t]+" ],
       [ "nameserver",    \&gst_replace_join_all,         RESOLV_CONF, "nameserver", "[ \t]+" ],
       [ "searchdomain",  \&gst_replace_join_first_array, RESOLV_CONF, "search", "[ \t]+", "[ \t]+" ],
       [ "order",         \&gst_replace_join_first_array, HOST_CONF,   "order", "[ \t]+", ",[ \t]*" ],
       [ "hostmatch",     \&gst_replace_join_first_bool,  HOST_CONF,   "multi", "[ \t]+", "on", "off" ],
       [ "statichost",    \&gst_replace_join_hash,        HOSTS,       "[ \t]+", "[ \t]+" ],
       [ "workgroup",     \&gst_replace_ini,              SMB_CONF,    "global", "workgroup" ],
       [ "smbdesc",       \&gst_replace_ini,              SMB_CONF,    "global", "server string" ],
       [ "winsserver",    \&gst_replace_ini,              SMB_CONF,    "global", "wins server" ],
       [ "winsuse",       \&gst_replace_ini_bool,         SMB_CONF,    "global", "wins support" ],
       [ "smbuse",        \&gst_service_suse_set_status,  "smb" ],
       [ "gateway",       \&gst_replace_join_first_str,   ROUTE_CONF,  "default", "[ \t]+", "%gateway% - -" ],
       [ "interface",     \&gst_network_interfaces_set,		OLD_HASH ],
      ]
    },
    
	  "pld-1.0" =>
	  {
	    fn =>
	    {
        SYSCONFIG_NW => "/etc/sysconfig/network",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        SMB_CONF     => "/etc/smb/smb.conf",
        WVDIAL       => "/etc/wvdial.conf"
      },
	    table =>
          [
           [ "auto",          \&gst_replace_sh_bool,          SYSCONFIG_NW, NETWORKING ],
           [ "hostname",      \&gst_replace_sh,               SYSCONFIG_NW, HOSTNAME ],
           [ "hostname",      \&gst_network_run_hostname ],
           [ "gateway",       \&gst_replace_sh,               SYSCONFIG_NW, GATEWAY],
           [ "gatewaydev",    \&gst_replace_sh,               SYSCONFIG_NW, GATEWAYDEV],
#           [ "domain",        \&gst_replace_sh,               SYSCONFIG_NW, DOMAIN],
           [ "domain",        \&gst_replace_join_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
           [ "nameserver",    \&gst_replace_join_all,         RESOLV_CONF,  "nameserver", "[ \t]+" ],
           [ "searchdomain",  \&gst_replace_join_first_array, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
           [ "order",         \&gst_replace_join_first_array, HOST_CONF,    "order", "[ \t]+", ",[ \t]*" ],
           [ "hostmatch",     \&gst_replace_join_first_bool,  HOST_CONF,    "multi", "[ \t]+", "on", "off" ],
           [ "statichost",    \&gst_replace_join_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
           [ "workgroup",     \&gst_replace_ini,              SMB_CONF,     "global", "workgroup" ],     
           [ "smbdesc",       \&gst_replace_ini,              SMB_CONF,     "global", "server string" ],
           [ "winsserver",    \&gst_replace_ini,              SMB_CONF,     "global", "wins server" ],
           [ "winsuse",       \&gst_replace_ini_bool,         SMB_CONF,     "global", "wins support" ],
           [ "smbuse",        \&gst_service_sysv_set_status,  91, "smb", "%smbuse%" ],
           [ "interface",     \&gst_network_interfaces_set,		OLD_HASH ],
           [ "gateway",       \&gst_network_route_set_default_gw, "%gatewaydev%" ]
           ]
       },
    "slackware-9.1.0" =>
    {
      fn =>
      {
        RC_INET_CONF => "/etc/rc.d/rc.inet1.conf",
        RESOLV_CONF  => "/etc/resolv.conf",
        HOST_CONF    => "/etc/host.conf",
        HOSTS        => "/etc/hosts",
        HOSTNAME     => "/etc/HOSTNAME",
        SMB_CONF     => "/etc/samba/smb.conf",
        WVDIAL       => "/etc/wvdial.conf"
      },
      table =>
      [
       [ "hostname",      \&gst_replace_fq_hostname, HOSTNAME, "%hostname%", "%domain%" ],
       [ "hostname",      \&gst_network_run_hostname ],
       [ "gateway",       \&gst_replace_rcinet1conf_global, RC_INET_CONF, GATEWAY ],
       [ "domain",        \&gst_replace_join_first_str,   RESOLV_CONF, "domain", "[ \t]+" ],
       [ "nameserver",    \&gst_replace_join_all,         RESOLV_CONF, "nameserver", "[ \t]+" ],
       [ "searchdomain",  \&gst_replace_join_first_array, RESOLV_CONF, "search", "[ \t]+", "[ \t]+" ],
       [ "order",         \&gst_replace_join_first_array, HOST_CONF,   "order", "[ \t]+", ",[ \t]*" ],
       [ "hostmatch",     \&gst_replace_join_first_bool,  HOST_CONF,   "multi", "[ \t]+", "on", "off" ],
       [ "statichost",    \&gst_replace_join_hash,        HOSTS,       "[ \t]+", "[ \t]+" ],
       [ "workgroup",     \&gst_replace_ini,              SMB_CONF,    "global", "workgroup" ],     
       [ "smbdesc",       \&gst_replace_ini,              SMB_CONF,    "global", "server string" ],
       [ "winsserver",    \&gst_replace_ini,              SMB_CONF,    "global", "wins server" ],
       [ "winsuse",       \&gst_replace_ini_bool,         SMB_CONF,    "global", "wins support" ],
       [ "smbuse",        \&gst_service_bsd_set_status,   "/etc/rc.d/rc.samba" ],
       [ "interface",     \&gst_network_interfaces_set,   OLD_HASH ],
       [ "gateway",       \&gst_network_route_set_default_gw, "%gatewaydev%" ]
      ]
    },

    "gentoo" =>
    {
      fn =>
      {
        HOSTNAME    => "/etc/conf.d/hostname",
        DOMAINNAME  => "/etc/conf.d/domainname",
        NET         => "/etc/conf.d/net",
        RESOLV_CONF => "/etc/resolv.conf",
        HOSTS       => "/etc/hosts",
        SMB_CONF    => "/etc/samba/smb.conf"
      },
      table =>
      [
       [ "hostname",      \&gst_replace_sh,               HOSTNAME,     "HOSTNAME" ],
       [ "hostname",      \&gst_network_run_hostname ],
       [ "gateway",       \&gst_network_route_set_default_gw, "%gatewaydev%" ],
       [ "domain",        \&gst_replace_sh,               DOMAINNAME,   "DNSDOMAIN" ],
       [ "domain",        \&gst_replace_join_first_str,   RESOLV_CONF,  "domain", "[ \t]+" ],
       [ "nameserver",    \&gst_replace_join_all,         RESOLV_CONF,  "nameserver", "[ \t]+" ],
       [ "searchdomain",  \&gst_replace_join_first_array, RESOLV_CONF,  "search", "[ \t]+", "[ \t]+" ],
       [ "statichost",    \&gst_replace_join_hash,        HOSTS,        "[ \t]+", "[ \t]+" ],
       [ "workgroup",     \&gst_replace_ini,              SMB_CONF,     "global", "workgroup" ],
       [ "smbdesc",       \&gst_replace_ini,              SMB_CONF,     "global", "server string", "%hostname%" ],
       [ "winsserver",    \&gst_replace_ini,              SMB_CONF,     "global", "wins server" ],
       [ "winsuse",       \&gst_replace_ini_bool,         SMB_CONF,     "global", "wins support" ],
       [ "smbuse",        \&gst_service_gentoo_set_status, "samba", 1 ],
       # first set the gateway to something like "eth0", then add the IP address like "eth0/1.1.1.1"
#       [ "gatewaydev",    \&gst_replace_sh,               NET, gateway ],
#       [ "gateway",       \&gst_replace_sh_re,            NET, gateway, "\/[0-9\.]*\$", "/%gateway%" ],
       [ "interface",     \&gst_network_interfaces_set,   OLD_HASH ]
      ]
    },

    "freebsd-5" =>
    {
      fn =>
      {
        RC_CONF     => "/etc/rc.conf",
        RESOLV_CONF => "/etc/resolv.conf",
        HOSTS       => "/etc/hosts",
        SMB_CONF    => "/usr/local/etc/smb.conf"
      },
      table =>
      [
       [ "hostname",      \&gst_replace_sh,               RC_CONF, "hostname", "%hostname%.%domain%" ],
       [ "hostname",      \&gst_network_run_hostname,     "%hostname%.%domain%" ],
       [ "domain",        \&gst_replace_join_first_str,   RESOLV_CONF, "domain",     "[ \t]+" ],
       [ "nameserver",    \&gst_replace_join_all,         RESOLV_CONF, "nameserver", "[ \t]+" ],
       [ "searchdomain",  \&gst_replace_join_first_array, RESOLV_CONF, "search",     "[ \t]+", "[ \t]+" ],
       [ "statichost",    \&gst_replace_join_hash,        HOSTS, "[ \t]+", "[ \t]+" ],
       [ "workgroup",     \&gst_replace_ini,              SMB_CONF,     "global", "workgroup" ],
       [ "smbdesc",       \&gst_replace_ini,              SMB_CONF,     "global", "server string", "%hostname%" ],
       [ "winsserver",    \&gst_replace_ini,              SMB_CONF,     "global", "wins server" ],
       [ "winsuse",       \&gst_replace_ini_bool,         SMB_CONF,     "global", "wins support" ],
       [ "smbuse",        \&gst_service_rcng_set_status,  "samba" ],
       [ "gateway",       \&gst_replace_sh,               RC_CONF, "defaultrouter" ],
       [ "interface",     \&gst_network_interfaces_set,   OLD_HASH ]
       ]
    }
	  );
  
  my $dist = $dist_map{$gst_dist};
  return %{$dist_tables{$dist}} if $dist;

  &gst_report ("platform_no_table", $gst_dist);
  return undef;
}

sub gst_network_get_interface_replace_table
{
  my %dist_map =
         (
          "redhat-5.2"   => "redhat-5.2",
	  "redhat-6.0"   => "redhat-6.2",
	  "redhat-6.1"   => "redhat-6.2",
	  "redhat-6.2"   => "redhat-6.2",
	  "redhat-7.0"   => "redhat-6.2",
	  "redhat-7.1"   => "redhat-6.2",
	  "redhat-7.2"   => "redhat-7.2",
          "redhat-8.0"   => "redhat-8.0",
          "redhat-9"     => "redhat-8.0",
	  "mandrake-7.1" => "redhat-6.2",
          "mandrake-7.2" => "redhat-6.2",
          "mandrake-9.0" => "mandrake-9.0",
          "mandrake-9.1" => "mandrake-9.0",
          "mandrake-9.2" => "mandrake-9.0",
          "mandrake-10.0" => "mandrake-9.0",
          "mandrake-10.1" => "mandrake-9.0",
          "mandrake-10.2" => "mandrake-9.0",
          "mandriva-2006.0" => "mandrake-9.0",
          "mandriva-2006.1" => "mandrake-9.0",
          "mandriva-2007.0" => "mandrake-9.0",
	  "mandriva-2007.1" => "mandrake-9.0",
          "yoper-2.2" => "redhat-6.2",
          "blackpanther-4.0" => "mandrake-9.0",
          "conectiva-9"  => "conectiva-9",
          "conectiva-10" => "conectiva-9",
          "debian-2.2"   => "debian-2.2",
          "debian-3.0"   => "debian-3.0",
          "debian-3.1"   => "debian-3.0",
          "debian-4.0"   => "debian-3.0",
          "debian-5.0"   => "debian-3.0",
          "debian-testing" => "debian-3.0",
          "ubuntu-5.04"  => "debian-3.0",
          "ubuntu-5.10"  => "debian-3.0",
          "ubuntu-6.06"  => "debian-3.0",
          "ubuntu-6.10"  => "debian-3.0",
          "ubuntu-7.04"  => "debian-3.0",
          "ubuntu-7.10"  => "debian-3.0",
          "ubuntu-8.04"  => "debian-3.0",
          "suse-7.0"     => "suse-7.0",
          "suse-9.0"     => "suse-9.0",
          "suse-9.1"     => "suse-9.0",
          "turbolinux-7.0"   => "redhat-6.2",
          "pld-1.0"      => "pld-1.0",
          "pld-1.1"      => "pld-1.0",
          "pld-1.99"     => "pld-1.0",
          "fedora-1"     => "redhat-7.2",
          "fedora-2"     => "redhat-7.2",
          "fedora-3"     => "redhat-7.2",
          "fedora-4"     => "redhat-7.2",
          "fedora-5"     => "redhat-7.2",
          "rpath"        => "redhat-7.2",
          "vine-3.0"     => "vine-3.0",
          "vine-3.1"     => "vine-3.0",
          "ark"          => "vine-3.0",
          "slackware-9.1.0" => "slackware-9.1.0",
          "slackware-10.0.0" => "slackware-9.1.0",
          "slackware-10.1.0" => "slackware-9.1.0",
          "slackware-10.2.0" => "slackware-9.1.0",
          "gentoo"       => "gentoo",
          "vlos-1.2"     => "gentoo",
          "freebsd-5"    => "freebsd-5",
          "freebsd-6"    => "freebsd-5",
         );

  my %dist_tables =
	(
    "redhat-6.2" =>
	  {
      iface_set    => \&gst_network_rh62_interface_activate,
      iface_delete => \&gst_network_rh62_interface_delete,
      ifaces_get   => \&gst_network_sysconfig_rh62_ifaces_get_existing,
      fn =>
	    {
        IFCFG  => "/etc/sysconfig/network-scripts/ifcfg-#iface#",
        CHAT   => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE  => "#iface#",
        WVDIAL => "/etc/wvdial.conf",
        PUMP   => "/etc/pump.conf"
      },
      table =>
       [
        [ "bootproto",          \&gst_network_rh62_replace_bootproto, IFCFG, BOOTPROTO ],
        [ "auto",               \&gst_replace_sh_bool, IFCFG, ONBOOT ],
        [ "user",               \&gst_replace_sh_bool, IFCFG, USERCTL ],
        [ "dev",                \&gst_replace_sh,      IFCFG, NAME ],
        [ "dev",                \&gst_replace_sh,      IFCFG, DEVICE ],
        [ "address",            \&gst_replace_sh,      IFCFG, IPADDR ],
        [ "netmask",            \&gst_replace_sh,      IFCFG, NETMASK ],
        [ "broadcast",          \&gst_replace_sh,      IFCFG, BROADCAST ],
        [ "network",            \&gst_replace_sh,      IFCFG, NETWORK ],
        [ "gateway",            \&gst_replace_sh,      IFCFG, GATEWAY ],
        [ "update_dns",         \&gst_network_pump_set_nodns, PUMP, "%dev%", "%bootproto%" ],
        [ "update_dns",         \&gst_replace_sh_bool, IFCFG, PEERDNS ],
        [ "dns1",               \&gst_replace_sh,      IFCFG, DNS1 ],
        [ "dns2",               \&gst_replace_sh,      IFCFG, DNS2 ],
        [ "mtu",                \&gst_replace_sh,      IFCFG, MTU ],
        [ "mru",                \&gst_replace_sh,      IFCFG, MRU ],
        [ "remote_address",     \&gst_replace_sh,      IFCFG, REMIP ],
        [ "login",              \&gst_replace_sh,      IFCFG, PAPNAME ],
        [ "serial_port",        \&gst_replace_sh,      IFCFG, MODEMPORT ],
        [ "serial_speed",       \&gst_replace_sh,      IFCFG, LINESPEED ],
        [ "ppp_options",        \&gst_replace_sh,      IFCFG, PPPOPTIONS ],
        [ "section",            \&gst_replace_sh,      IFCFG, WVDIALSECT ],
        [ "set_default_gw",     \&gst_replace_sh_bool, IFCFG, DEFROUTE ],
        [ "debug",              \&gst_replace_sh_bool, IFCFG, DEBUG ],
        [ "persist",            \&gst_replace_sh_bool, IFCFG, PERSIST ],
        [ "serial_escapechars", \&gst_replace_sh_bool, IFCFG, ESCAPECHARS ],
        [ "serial_hwctl",       \&gst_replace_sh_bool, IFCFG, HARDFLOWCTL ],
        [ "phone_number",       \&gst_replace_chat,    CHAT,  "^atd[^0-9]*([0-9, -]+)" ],
        # wvdial settings
        [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Phone" ]],
        [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto DNS" ]],
        [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Username" ]],
        [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Password" ]],
        [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Modem" ]],
        [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Baud" ]],
        [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Check Def Route" ]],
        [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
        [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Command" ]],
        [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Prefix" ]],
      ]
    },

    "redhat-7.2" =>
	  {
      iface_set    => \&gst_network_rh62_interface_activate,
      iface_delete => \&gst_network_rh72_interface_delete,
      ifaces_get   => \&gst_network_sysconfig_rh72_ifaces_get_existing,
      fn =>
	    {
        IFCFG => ["/etc/sysconfig/network-scripts/ifcfg-#iface#",
                  "/etc/sysconfig/networking/profiles/default/ifcfg-#iface#",
                  "/etc/sysconfig/networking/devices/ifcfg-#iface#"],
        CHAT   => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE  => "#iface#",
        WVDIAL => "/etc/wvdial.conf",
        PUMP   => "/etc/pump.conf"
      },
      table =>
       [
        [ "bootproto",          \&gst_network_rh62_replace_bootproto, IFCFG, BOOTPROTO ],
        [ "auto",               \&gst_replace_sh_bool, IFCFG, ONBOOT ],
        [ "user",               \&gst_replace_sh_bool, IFCFG, USERCTL ],
        [ "name",               \&gst_replace_sh,      IFCFG, NAME ],
        [ "dev",                \&gst_replace_sh,      IFCFG, DEVICE ],
        [ "address",            \&gst_replace_sh,      IFCFG, IPADDR ],
        [ "netmask",            \&gst_replace_sh,      IFCFG, NETMASK ],
        [ "broadcast",          \&gst_replace_sh,      IFCFG, BROADCAST ],
        [ "network",            \&gst_replace_sh,      IFCFG, NETWORK ],
        [ "gateway",            \&gst_replace_sh,      IFCFG, GATEWAY ],
        [ "essid",              \&gst_replace_sh,      IFCFG, ESSID ],
        [ "key",                \&gst_replace_sh,      IFCFG, KEY ],
        [ "key_type",           \&gst_network_set_wep_key_type, [ \&gst_replace_sh, IFCFG, KEY, "%key%" ]],
        [ "update_dns",         \&gst_network_pump_set_nodns, PUMP, "%dev%", "%bootproto%" ],
        [ "update_dns",         \&gst_replace_sh_bool, IFCFG, PEERDNS ],
        [ "dns1",               \&gst_replace_sh,      IFCFG, DNS1 ],
        [ "dns2",               \&gst_replace_sh,      IFCFG, DNS2 ],
        [ "mtu",                \&gst_replace_sh,      IFCFG, MTU ],
        [ "mru",                \&gst_replace_sh,      IFCFG, MRU ],
        [ "remote_address",     \&gst_replace_sh,      IFCFG, REMIP ],
        [ "login",              \&gst_replace_sh,      IFCFG, PAPNAME ],
        [ "serial_port",        \&gst_replace_sh,      IFCFG, MODEMPORT ],
        [ "serial_speed",       \&gst_replace_sh,      IFCFG, LINESPEED ],
        [ "ppp_options",        \&gst_replace_sh,      IFCFG, PPPOPTIONS ],
        [ "section",            \&gst_replace_sh,      IFCFG, WVDIALSECT ],
        [ "set_default_gw",     \&gst_replace_sh_bool, IFCFG, DEFROUTE ],
        [ "debug",              \&gst_replace_sh_bool, IFCFG, DEBUG ],
        [ "persist",            \&gst_replace_sh_bool, IFCFG, PERSIST ],
        [ "serial_escapechars", \&gst_replace_sh_bool, IFCFG, ESCAPECHARS ],
        [ "serial_hwctl",       \&gst_replace_sh_bool, IFCFG, HARDFLOWCTL ],
        [ "phone_number",       \&gst_replace_chat,    CHAT,  "^atd[^0-9]*([0-9, -]+)" ],
        # wvdial settings
        [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Phone" ]],
        [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto DNS" ]],
        [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Username" ]],
        [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Password" ]],
        [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Modem" ]],
        [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Baud" ]],
        [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Check Def Route" ]],
        [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
        [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Command" ]],
        [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Prefix" ]],
      ]
    },
   
    "redhat-8.0" =>
	  {
      iface_set    => \&gst_network_rh62_interface_activate,
      iface_delete => \&gst_network_rh72_interface_delete,
      ifaces_get   => \&gst_network_sysconfig_rh72_ifaces_get_existing,
      fn =>
	    {
        IFCFG => ["/etc/sysconfig/network-scripts/ifcfg-#iface#",
                  "/etc/sysconfig/networking/profiles/default/ifcfg-#iface#",
                  "/etc/sysconfig/networking/devices/ifcfg-#iface#"],
        CHAT   => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE  => "#iface#",
        WVDIAL => "/etc/wvdial.conf",
        PUMP   => "/etc/pump.conf"
      },
      table =>
       [
        [ "bootproto",          \&gst_network_rh62_replace_bootproto, IFCFG, BOOTPROTO ],
        [ "auto",               \&gst_replace_sh_bool, IFCFG, ONBOOT ],
        [ "user",               \&gst_replace_sh_bool, IFCFG, USERCTL ],
        [ "name",               \&gst_replace_sh,      IFCFG, NAME ],
        [ "dev",                \&gst_replace_sh,      IFCFG, DEVICE ],
        [ "address",            \&gst_replace_sh,      IFCFG, IPADDR ],
        [ "netmask",            \&gst_replace_sh,      IFCFG, NETMASK ],
        [ "broadcast",          \&gst_replace_sh,      IFCFG, BROADCAST ],
        [ "network",            \&gst_replace_sh,      IFCFG, NETWORK ],
        [ "gateway",            \&gst_replace_sh,      IFCFG, GATEWAY ],
        [ "essid",              \&gst_replace_sh,      IFCFG, WIRELESS_ESSID ],
        [ "key",                \&gst_replace_sh,      IFCFG, WIRELESS_KEY   ],
        [ "key_type",           \&gst_network_set_wep_key_type, [ \&gst_replace_sh, IFCFG, WIRELESS_KEY, "%key%" ]],
        [ "update_dns",         \&gst_network_pump_set_nodns, PUMP, "%dev%", "%bootproto%" ],
        [ "update_dns",         \&gst_replace_sh_bool, IFCFG, PEERDNS ],
        [ "dns1",               \&gst_replace_sh,      IFCFG, DNS1 ],
        [ "dns2",               \&gst_replace_sh,      IFCFG, DNS2 ],
        [ "mtu",                \&gst_replace_sh,      IFCFG, MTU ],
        [ "mru",                \&gst_replace_sh,      IFCFG, MRU ],
        [ "remote_address",     \&gst_replace_sh,      IFCFG, REMIP ],
        [ "login",              \&gst_replace_sh,      IFCFG, PAPNAME ],
        [ "serial_port",        \&gst_replace_sh,      IFCFG, MODEMPORT ],
        [ "serial_speed",       \&gst_replace_sh,      IFCFG, LINESPEED ],
        [ "ppp_options",        \&gst_replace_sh,      IFCFG, PPPOPTIONS ],
        [ "section",            \&gst_replace_sh,      IFCFG, WVDIALSECT ],
        [ "set_default_gw",     \&gst_replace_sh_bool, IFCFG, DEFROUTE ],
        [ "debug",              \&gst_replace_sh_bool, IFCFG, DEBUG ],
        [ "persist",            \&gst_replace_sh_bool, IFCFG, PERSIST ],
        [ "serial_escapechars", \&gst_replace_sh_bool, IFCFG, ESCAPECHARS ],
        [ "serial_hwctl",       \&gst_replace_sh_bool, IFCFG, HARDFLOWCTL ],
        [ "phone_number",       \&gst_replace_chat,    CHAT,  "^atd[^0-9]*([0-9, -]+)" ],
        # wvdial settings
        [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Phone" ]],
        [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto DNS" ]],
        [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Username" ]],
        [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Password" ]],
        [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Modem" ]],
        [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Baud" ]],
        [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Check Def Route" ]],
        [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
        [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Command" ]],
        [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Prefix" ]],
      ]
    },
   
    "vine-3.0" =>
	  {
      iface_set    => \&gst_network_rh62_interface_activate,
      iface_delete => \&gst_network_rh62_interface_delete,
      ifaces_get   => \&gst_network_sysconfig_rh62_ifaces_get_existing,
      fn =>
	    {
        IFCFG  => "/etc/sysconfig/network-scripts/ifcfg-#iface#",
        CHAT   => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE  => "#iface#",
        WVDIAL => "/etc/wvdial.conf",
        PUMP   => "/etc/pump.conf"
      },
      table =>
       [
        [ "bootproto",          \&gst_network_rh62_replace_bootproto, IFCFG, BOOTPROTO ],
        [ "auto",               \&gst_replace_sh_bool, IFCFG, ONBOOT ],
        [ "user",               \&gst_replace_sh_bool, IFCFG, USERCTL ],
        [ "name",               \&gst_replace_sh,      IFCFG, NAME ],
        [ "dev",                \&gst_replace_sh,      IFCFG, DEVICE ],
        [ "address",            \&gst_replace_sh,      IFCFG, IPADDR ],
        [ "netmask",            \&gst_replace_sh,      IFCFG, NETMASK ],
        [ "broadcast",          \&gst_replace_sh,      IFCFG, BROADCAST ],
        [ "network",            \&gst_replace_sh,      IFCFG, NETWORK ],
        [ "gateway",            \&gst_replace_sh,      IFCFG, GATEWAY ],
        [ "essid",              \&gst_replace_sh,      IFCFG, ESSID ],
        [ "key",                \&gst_replace_sh,      IFCFG, KEY   ],
        [ "key_type",           \&gst_network_set_wep_key_type, [ \&gst_replace_sh, IFCFG, KEY, "%key%" ]],
        [ "update_dns",         \&gst_network_pump_set_nodns, PUMP, "%dev%", "%bootproto%" ],
        [ "update_dns",         \&gst_replace_sh_bool, IFCFG, PEERDNS ],
        [ "dns1",               \&gst_replace_sh,      IFCFG, DNS1 ],
        [ "dns2",               \&gst_replace_sh,      IFCFG, DNS2 ],
        [ "mtu",                \&gst_replace_sh,      IFCFG, MTU ],
        [ "mru",                \&gst_replace_sh,      IFCFG, MRU ],
        [ "remote_address",     \&gst_replace_sh,      IFCFG, REMIP ],
        [ "login",              \&gst_replace_sh,      IFCFG, PAPNAME ],
        [ "serial_port",        \&gst_replace_sh,      IFCFG, MODEMPORT ],
        [ "serial_speed",       \&gst_replace_sh,      IFCFG, LINESPEED ],
        [ "ppp_options",        \&gst_replace_sh,      IFCFG, PPPOPTIONS ],
        [ "section",            \&gst_replace_sh,      IFCFG, WVDIALSECT ],
        [ "set_default_gw",     \&gst_replace_sh_bool, IFCFG, DEFROUTE ],
        [ "debug",              \&gst_replace_sh_bool, IFCFG, DEBUG ],
        [ "persist",            \&gst_replace_sh_bool, IFCFG, PERSIST ],
        [ "serial_escapechars", \&gst_replace_sh_bool, IFCFG, ESCAPECHARS ],
        [ "serial_hwctl",       \&gst_replace_sh_bool, IFCFG, HARDFLOWCTL ],
        [ "phone_number",       \&gst_replace_chat,    CHAT,  "^atd[^0-9]*([0-9, -]+)" ],
        # wvdial settings
        [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Phone" ]],
        [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto DNS" ]],
        [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Username" ]],
        [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Password" ]],
        [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Modem" ]],
        [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Baud" ]],
        [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Check Def Route" ]],
        [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
        [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Command" ]],
        [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Prefix" ]],
      ]
    },

    "mandrake-9.0" =>
	  {
      iface_set    => \&gst_network_rh62_interface_activate,
      iface_delete => \&gst_network_rh62_interface_delete,
      ifaces_get   => \&gst_network_sysconfig_rh62_ifaces_get_existing,
      fn =>
	    {
        IFCFG  => "/etc/sysconfig/network-scripts/ifcfg-#iface#",
        CHAT   => "/etc/sysconfig/network-scripts/chat-#iface#",
        IFACE  => "#iface#",
        WVDIAL => "/etc/wvdial.conf",
        PUMP   => "/etc/pump.conf"
      },
      table =>
       [
        [ "bootproto",          \&gst_network_rh62_replace_bootproto, IFCFG, BOOTPROTO ],
        [ "auto",               \&gst_replace_sh_bool, IFCFG, ONBOOT ],
        [ "user",               \&gst_replace_sh_bool, IFCFG, USERCTL ],
        [ "name",               \&gst_replace_sh,      IFCFG, NAME ],
        [ "dev",                \&gst_replace_sh,      IFCFG, DEVICE ],
        [ "address",            \&gst_replace_sh,      IFCFG, IPADDR ],
        [ "netmask",            \&gst_replace_sh,      IFCFG, NETMASK ],
        [ "broadcast",          \&gst_replace_sh,      IFCFG, BROADCAST ],
        [ "network",            \&gst_replace_sh,      IFCFG, NETWORK ],
        [ "gateway",            \&gst_replace_sh,      IFCFG, GATEWAY ],
        [ "essid",              \&gst_replace_sh,      IFCFG, WIRELESS_ESSID ],
        [ "key",                \&gst_replace_sh,      IFCFG, WIRELESS_KEY   ],
        [ "key_type",           \&gst_network_set_wep_key_type, [ \&gst_replace_sh, IFCFG, WIRELESS_KEY, "%key%" ]],
        [ "update_dns",         \&gst_network_pump_set_nodns, PUMP, "%dev%", "%bootproto%" ],
        [ "update_dns",         \&gst_replace_sh_bool, IFCFG, PEERDNS ],
        [ "dns1",               \&gst_replace_sh,      IFCFG, DNS1 ],
        [ "dns2",               \&gst_replace_sh,      IFCFG, DNS2 ],
        [ "mtu",                \&gst_replace_sh,      IFCFG, MTU ],
        [ "mru",                \&gst_replace_sh,      IFCFG, MRU ],
        [ "remote_address",     \&gst_replace_sh,      IFCFG, REMIP ],
        [ "login",              \&gst_replace_sh,      IFCFG, PAPNAME ],
        [ "serial_port",        \&gst_replace_sh,      IFCFG, MODEMPORT ],
        [ "serial_speed",       \&gst_replace_sh,      IFCFG, LINESPEED ],
        [ "ppp_options",        \&gst_replace_sh,      IFCFG, PPPOPTIONS ],
        [ "section",            \&gst_replace_sh,      IFCFG, WVDIALSECT ],
        [ "set_default_gw",     \&gst_replace_sh_bool, IFCFG, DEFROUTE ],
        [ "debug",              \&gst_replace_sh_bool, IFCFG, DEBUG ],
        [ "persist",            \&gst_replace_sh_bool, IFCFG, PERSIST ],
        [ "serial_escapechars", \&gst_replace_sh_bool, IFCFG, ESCAPECHARS ],
        [ "serial_hwctl",       \&gst_replace_sh_bool, IFCFG, HARDFLOWCTL ],
        [ "phone_number",       \&gst_replace_chat,    CHAT,  "^atd[^0-9]*([0-9, -]+)" ],
        # wvdial settings
        [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Phone" ]],
        [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto DNS" ]],
        [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Username" ]],
        [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Password" ]],
        [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Modem" ]],
        [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Baud" ]],
        [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Check Def Route" ]],
        [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
        [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Command" ]],
        [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Prefix" ]],
      ]
    },

   "conectiva-9" =>
   {
     iface_set    => \&gst_network_rh62_interface_activate,
     iface_delete => \&gst_network_rh62_interface_delete,
     ifaces_get   => \&gst_network_sysconfig_rh62_ifaces_get_existing,
     fn =>
     {
       IFCFG  => "/etc/sysconfig/network-scripts/ifcfg-#iface#",
       CHAT   => "/etc/sysconfig/network-scripts/chat-#iface#",
       IFACE  => "#iface#",
       WVDIAL => "/etc/wvdial.conf",
       PUMP   => "/etc/pump.conf"
     },
     table =>
     [
      [ "bootproto",          \&gst_network_rh62_replace_bootproto, IFCFG, BOOTPROTO ],
      [ "auto",               \&gst_replace_sh_bool, IFCFG, ONBOOT ],
      [ "user",               \&gst_replace_sh_bool, IFCFG, USERCTL ],
      [ "name",               \&gst_replace_sh,      IFCFG, NAME ],
      [ "dev",                \&gst_replace_sh,      IFCFG, DEVICE ],
      [ "address",            \&gst_replace_sh,      IFCFG, IPADDR ],
      [ "netmask",            \&gst_replace_sh,      IFCFG, NETMASK ],
      [ "broadcast",          \&gst_replace_sh,      IFCFG, BROADCAST ],
      [ "network",            \&gst_replace_sh,      IFCFG, NETWORK ],
      [ "gateway",            \&gst_replace_sh,      IFCFG, GATEWAY ],
      [ "essid",              \&gst_replace_sh,      IFCFG, WIRELESS_ESSID ],
      [ "key",                \&gst_replace_sh,      IFCFG, WIRELESS_KEY ],
      [ "key_type",           \&gst_network_set_wep_key_type, [ \&gst_replace_sh, IFCFG, WIRELESS_KEY, "%key%" ]],
      [ "update_dns",         \&gst_network_pump_set_nodns, PUMP, "%dev%", "%bootproto%" ],
      [ "update_dns",         \&gst_replace_sh_bool, IFCFG, PEERDNS ],
      [ "dns1",               \&gst_replace_sh,      IFCFG, DNS1 ],
      [ "dns2",               \&gst_replace_sh,      IFCFG, DNS2 ],
      [ "mtu",                \&gst_replace_sh,      IFCFG, MTU ],
      [ "mru",                \&gst_replace_sh,      IFCFG, MRU ],
      [ "remote_address",     \&gst_replace_sh,      IFCFG, REMIP ],
      [ "login",              \&gst_replace_sh,      IFCFG, PAPNAME ],
      [ "serial_port",        \&gst_replace_sh,      IFCFG, MODEMPORT ],
      [ "serial_speed",       \&gst_replace_sh,      IFCFG, LINESPEED ],
      [ "ppp_options",        \&gst_replace_sh,      IFCFG, PPPOPTIONS ],
      [ "section",            \&gst_replace_sh,      IFCFG, WVDIALSECT ],
      [ "set_default_gw",     \&gst_replace_sh_bool, IFCFG, DEFROUTE ],
      [ "debug",              \&gst_replace_sh_bool, IFCFG, DEBUG ],
      [ "persist",            \&gst_replace_sh_bool, IFCFG, PERSIST ],
      [ "serial_escapechars", \&gst_replace_sh_bool, IFCFG, ESCAPECHARS ],
      [ "serial_hwctl",       \&gst_replace_sh_bool, IFCFG, HARDFLOWCTL ],
      [ "phone_number",       \&gst_replace_chat,    CHAT,  "^atd[^0-9]*([0-9, -]+)" ],
      # wvdial settings
      [ "phone_number",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Phone" ]],
      [ "update_dns",         \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto DNS" ]],
      [ "login",              \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Username" ]],
      [ "password",           \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Password" ]],
      [ "serial_port",        \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Modem" ]],
      [ "serial_speed",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Baud" ]],
      [ "set_default_gw",     \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Check Def Route" ]],
      [ "persist",            \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Auto Reconnect" ]],
      [ "dial_command",       \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Command" ]],
      [ "external_line",      \&gst_network_check_type,  ["%dev%", "modem", \&gst_replace_ini, WVDIAL, "Dialer %section%", "Dial Prefix" ]],
     ]
   },

   "debian-2.2" =>
	  {
      iface_set    => \&gst_network_rh62_interface_activate, # They use the same ifup/ifdown cmds.
      iface_delete => \&gst_network_deb22_interface_delete,
      ifaces_get   => \&gst_network_debian_ifaces_get_existing,
	    fn =>
	    {
        INTERFACES  => "/etc/network/interfaces",
        IFACE       => "#iface#",
        CHAT        => "/etc/chatscripts/%section%",
        PPP_OPTIONS => "/etc/ppp/peers/%section%",
        PAP         => "/etc/ppp/pap-secrets",
        CHAP        => "/etc/ppp/chap-secrets",
        PUMP        => "/etc/pump.conf"
      },
	    table =>
          [
# not sup  [ "user",               \&gst_parse_sh_bool, IFCFG, USERCTL ],
           [ "_always_",           \&gst_network_deb22_replace_bootproto,  [INTERFACES, IFACE]],
           [ "bootproto",          \&gst_network_deb22_replace_bootproto,  [INTERFACES, IFACE]],
           [ "auto",               \&gst_replace_interfaces_option_kw_not, [INTERFACES, IFACE], "noauto" ],
           [ "name",               \&gst_replace_interfaces_option_str,    [INTERFACES, IFACE], "name" ],
           [ "address",            \&gst_replace_interfaces_option_str,    [INTERFACES, IFACE], "address" ],
           [ "netmask",            \&gst_replace_interfaces_option_str,    [INTERFACES, IFACE], "netmask" ],
           [ "broadcast",          \&gst_replace_interfaces_option_str,    [INTERFACES, IFACE], "broadcast" ],
           [ "network",            \&gst_replace_interfaces_option_str,    [INTERFACES, IFACE], "network" ],
           [ "gateway",            \&gst_replace_interfaces_option_str,    [INTERFACES, IFACE], "gateway" ],
           [ "section",            \&gst_replace_interfaces_option_str,    [INTERFACES, IFACE], "provider" ],
           [ "update_dns",         \&gst_network_pump_set_nodns,           PUMP, "%dev%", "%bootproto%" ],
           [ "update_dns",         \&gst_replace_kw,                       PPP_OPTIONS, "usepeerdns" ],
# not sup  [ "dns1",               \&gst_replace_sh,                       IFCFG,       DNS1 ],
# not sup  [ "dns2",               \&gst_replace_sh,                       IFCFG, DNS2 ],
           [ "noauth",             \&gst_replace_kw,                       PPP_OPTIONS, "noauth" ],
           [ "mtu",                \&gst_replace_join_first_str,           PPP_OPTIONS, "mtu", "[ \t]+" ],
           [ "mru",                \&gst_replace_join_first_str,           PPP_OPTIONS, "mru", "[ \t]+" ],
           [ "remote_address",     \&gst_network_debian_replace_remote_address, [INTERFACES, IFACE]],
           [ "login",              \&gst_replace_join_first_str,           PPP_OPTIONS, "user", "[ \t]+" ],
           [ "serial_port",        \&gst_network_set_ppp_options_re,       PPP_OPTIONS, "^(/dev/[^ \t]+)" ],
           [ "serial_speed",       \&gst_network_set_ppp_options_re,       PPP_OPTIONS, "^([0-9]+)" ],
           [ "section",            \&gst_network_set_ppp_options_connect,  PPP_OPTIONS ],
           [ "ppp_options",        \&gst_network_set_ppp_options_unsup,    PPP_OPTIONS ],
           [ "set_default_gw",     \&gst_replace_kw,                       PPP_OPTIONS, "defaultroute" ],
           [ "debug",              \&gst_replace_kw,                       PPP_OPTIONS, "debug" ],
           [ "persist",            \&gst_replace_kw,                       PPP_OPTIONS, "persist" ],
           [ "serial_escapechars", \&gst_replace_join_first_str,           PPP_OPTIONS, "escape", "[ \t]+" ],
           [ "serial_hwctl",       \&gst_replace_kw,                       PPP_OPTIONS, "crtscts" ],
           [ "phone_number",       \&gst_replace_chat,                     CHAT, "^atd[^0-9]*([0-9, -]+)" ],
           ]
             },

    # Basicly the same as debian-2.2, but the "auto" option changes.
    "debian-3.0" =>
	  {
      iface_set    => \&gst_network_rh62_interface_activate, # They use the same ifup/ifdown cmds.
      iface_delete => \&gst_network_deb22_interface_delete,
      ifaces_get   => \&gst_network_debian_ifaces_get_existing,
	    fn =>
	    {
        INTERFACES  => "/etc/network/interfaces",
        IFACE       => "#iface#",
        CHAT        => "/etc/chatscripts/%section%",
        PPP_OPTIONS => "/etc/ppp/peers/%section%",
        PAP         => "/etc/ppp/pap-secrets",
        CHAP        => "/etc/ppp/chap-secrets",
      },
	    table =>
          [
           [ "_always_",           \&gst_network_deb22_replace_bootproto, [INTERFACES, IFACE]],
           [ "bootproto",          \&gst_network_deb22_replace_bootproto, [INTERFACES, IFACE]],
           [ "auto",               \&gst_network_debian_woody_set_auto,   [INTERFACES, IFACE]],
           [ "address",            \&gst_replace_interfaces_option_str,   [INTERFACES, IFACE], "address" ],
           [ "netmask",            \&gst_replace_interfaces_option_str,   [INTERFACES, IFACE], "netmask" ],
           [ "gateway",            \&gst_replace_interfaces_option_str,   [INTERFACES, IFACE], "gateway" ],
           [ "essid",              \&gst_replace_interfaces_option_str,   [INTERFACES, IFACE], "wireless-essid" ],
           [ "key",                \&gst_replace_interfaces_option_str,   [INTERFACES, IFACE], "wireless-key"   ],
           [ "key_type",           \&gst_network_set_wep_key_type,        [ \&gst_replace_interfaces_option_str, INTERFACES, IFACE, "wireless-key", "%key%" ]],
           # ugly hack for deleting undesired options (due to syntax duality)
           [ "essid",              \&gst_replace_interfaces_option_str,   [INTERFACES, IFACE], "wireless_essid", "" ],
           [ "key",                \&gst_replace_interfaces_option_str,   [INTERFACES, IFACE], "wireless_key", ""   ],
           [ "key",                \&gst_replace_interfaces_option_str,   [INTERFACES, IFACE], "wireless_key1", ""  ],
           # End of hack
           [ "section",            \&gst_replace_interfaces_option_str,   [INTERFACES, IFACE], "provider" ],
           [ "remote_address",     \&gst_network_debian_replace_remote_address, [INTERFACES, IFACE]],
           # Modem stuff
           [ "section",            \&gst_network_check_type,             [IFACE, "modem", \&gst_network_set_ppp_options_connect,  PPP_OPTIONS ]],
           [ "phone_number",       \&gst_network_check_type,             [IFACE, "modem", \&gst_network_create_pppscript, CHAT ]],
           [ "phone_number",       \&gst_network_check_type,             [IFACE, "isdn", \&gst_network_create_isdn_options, PPP_OPTIONS ]],
           [ "update_dns",         \&gst_network_check_type,             [IFACE, "(modem|isdn)", \&gst_replace_kw, PPP_OPTIONS, "usepeerdns" ]],
           [ "noauth",             \&gst_network_check_type,             [IFACE, "(modem|isdn)", \&gst_replace_kw, PPP_OPTIONS, "noauth" ]],
           [ "set_default_gw",     \&gst_network_check_type,             [IFACE, "(modem|isdn)", \&gst_replace_kw, PPP_OPTIONS, "defaultroute" ]],
           [ "debug",              \&gst_network_check_type,             [IFACE, "(modem|isdn)", \&gst_replace_kw, PPP_OPTIONS, "debug" ]],
           [ "persist",            \&gst_network_check_type,             [IFACE, "(modem|isdn)", \&gst_replace_kw, PPP_OPTIONS, "persist" ]],
           [ "serial_hwctl",       \&gst_network_check_type,             [IFACE, "modem", \&gst_replace_kw, PPP_OPTIONS, "crtscts" ]],
           [ "mtu",                \&gst_network_check_type,             [IFACE, "(modem|isdn)", \&gst_replace_join_first_str, PPP_OPTIONS, "mtu", "[ \t]+" ]],
           [ "mru",                \&gst_network_check_type,             [IFACE, "(modem|isdn)", \&gst_replace_join_first_str, PPP_OPTIONS, "mru", "[ \t]+" ]],
           [ "serial_port",        \&gst_network_check_type,             [IFACE, "modem", \&gst_network_set_ppp_options_re, PPP_OPTIONS, "^(/dev/[^ \t]+)" ]],
           [ "serial_speed",       \&gst_network_check_type,             [IFACE, "modem", \&gst_network_set_ppp_options_re, PPP_OPTIONS, "^([0-9]+)" ]],
           [ "login",              \&gst_network_check_type,             [IFACE, "(modem|isdn)", \&gst_network_set_ppp_options_re, PPP_OPTIONS, "^user (.*)", "user \"%login%\"" ]],
           [ "password",           \&gst_network_check_type,             [IFACE, "(modem|isdn)", \&gst_network_set_pap_passwd, PAP, "%login%" ]],
           [ "password",           \&gst_network_check_type,             [IFACE, "(modem|isdn)", \&gst_network_set_pap_passwd, CHAP, "%login%" ]],
           [ "serial_escapechars", \&gst_network_check_type,             [IFACE, "modem", \&gst_replace_join_first_str, PPP_OPTIONS, "escape", "[ \t]+" ]],
           [ "dial_command",       \&gst_network_check_type,             [IFACE, "modem", \&gst_replace_chat, CHAT, "(atd[tp])[0-9w, -]+" ]],
           [ "phone_number",       \&gst_network_check_type,             [IFACE, "modem", \&gst_replace_chat, CHAT, "atd[tp]([0-9w]+)" ]],
           [ "external_line",      \&gst_network_check_type,             [IFACE, "modem", \&gst_replace_chat, CHAT, "atd[tp]([0-9w, -]+)", "%external_line%W%phone_number%" ]],
           [ "phone_number",       \&gst_network_check_type,             [IFACE, "isdn", \&gst_network_set_ppp_options_re, PPP_OPTIONS, "^number (.*)", "number %phone_number%" ]],
           [ "external_line",      \&gst_network_check_type,             [IFACE, "isdn", \&gst_network_set_ppp_options_re, PPP_OPTIONS, "^number (.*)", "number %external_line%W%phone_number%" ]],
           [ "volume",             \&gst_network_check_type,             [IFACE, "modem", \&gst_network_set_modem_volume, CHAT ]],
           ]
       },

    "suse-7.0" =>
	  {
      iface_set    => \&gst_network_suse70_interface_activate,
      iface_delete => \&gst_network_suse70_interface_delete,
      ifaces_get   => \&gst_network_suse70_ifaces_get_existing,
      fn =>
	    {
        IFCFG       => "/etc/rc.config",
        IFACE       => "#iface#",
        PPP_OPTIONS => "/etc/ppp/options"
      },
	    table =>
          [
# bootproto has to go second for suse, as it uses the same value for bootproto and params.           
           [ "bootproto",          \&gst_network_suse70_replace_bootproto,  [IFCFG, IFACE] ],
           [ "auto",               \&gst_network_suse70_replace_iface_auto, [IFCFG, IFACE], NETCONFIG ],
           [ "dev",                \&gst_network_suse70_replace_iface_sh,   [IFCFG, IFACE], NETDEV ],
# not sup  [ "user",               \&gst_parse_trivial,                   0 ], # not supported.
           [ "name",               \&gst_network_suse70_replace_iface_sh,   [IFCFG, IFACE], GST_IFACE_NAME ],
           [ "address",            \&gst_network_suse70_replace_iface_sh,   [IFCFG, IFACE], IPADDR ],
           [ "address",            \&gst_network_suse70_set_ifconfig_ip,    [IFCFG, IFACE] ],
           [ "netmask",            \&gst_network_suse70_set_ifconfig_arg,   [IFCFG, IFACE], netmask ],
           [ "broadcast",          \&gst_network_suse70_set_ifconfig_arg,   [IFCFG, IFACE], broadcast ],
#          [ "network",            \&gst_parse_trivial,                   0 ], # not supported.
#          [ "gateway",            \&gst_parse_sh,      IFCFG, GATEWAY ], # not supported
           [ "remote_address",     \&gst_network_suse70_set_ifconfig_arg,   [IFCFG, IFACE], pointopoint ],
           [ "update_dns",         \&gst_replace_kw,                        PPP_OPTIONS, "usepeerdns" ]
      ]
   },
    
   "suse-9.0" =>
   {
     iface_set    => \&gst_network_suse9_interface_activate, 
     iface_delete => \&gst_network_suse90_interface_delete,  
     ifaces_get   => \&gst_network_suse90_ifaces_get_existing, 
     fn =>
     {
       IFCFG       => "/etc/sysconfig/network/ifcfg-#iface#",
       ROUTE_CONF  => "/etc/sysconfig/network/routes",
       IFACE       => "#iface#",
       PPP_OPTIONS => "/etc/ppp/options"
     },
     table =>
     [
      [ "auto",               \&gst_network_suse90_set_auto,           IFCFG,   STARTMODE      ],
      [ "bootproto",          \&gst_network_suse90_replace_bootproto,  IFCFG,   BOOTPROTO      ],
      [ "address",            \&gst_replace_sh,                        IFCFG,   IPADDR         ], 
      [ "netmask",            \&gst_replace_sh,                        IFCFG,   NETMASK        ],
      [ "remote_address",     \&gst_replace_sh,                        IFCFG,   REMOTE_IPADDR  ],
      [ "essid",              \&gst_replace_sh,                        IFCFG,   WIRELESS_ESSID ],
      [ "key",                \&gst_replace_sh,                        IFCFG,   WIRELESS_KEY ],
      [ "key_type",           \&gst_network_set_wep_key_type,          [ \&gst_replace_sh, IFCFG, WIRELESS_KEY, "%key%" ]],
      # Modem stuff goes here
      [ "serial_port",        \&gst_replace_sh,                        IFCFG,   MODEM_DEVICE   ],
      [ "serial_speed",       \&gst_replace_sh,                        IFCFG,   SPEED          ],
      [ "mtu",                \&gst_replace_sh,                        IFCFG,   MTU            ],
      [ "mru",                \&gst_replace_sh,                        IFCFG,   MRU            ],
      [ "ppp_options",        \&gst_replace_sh,                        IFCFG,   PPPD_OPTIONS   ],
      [ "dial_command",       \&gst_replace_sh,                        IFCFG,   DIALCOMMAND    ],
      [ "external_line",      \&gst_replace_sh,                        IFCFG,   DIALPREFIX     ],
      [ "provider",           \&gst_replace_sh,                        IFCFG,   PROVIDER       ],
      [ "volume",             \&gst_network_check_type,               [ IFACE, "modem", \&gst_network_set_modem_volume_string, IFCFG, INIT8 ]],
      [ "login",              \&gst_network_suse90_replace_provider_file,       "%provider%", USERNAME     ],
      [ "password",           \&gst_network_suse90_replace_provider_file,       "%provider%", PASSWORD     ],
      [ "phone_number",       \&gst_network_suse90_replace_provider_file,       "%provider%", PHONE        ],
      [ "dns1",               \&gst_network_suse90_replace_provider_file,       "%provider%", DNS1         ],
      [ "dns2",               \&gst_network_suse90_replace_provider_file,       "%provider%", DNS2         ],
      [ "update_dns",         \&gst_network_suse90_replace_provider_file_bool,  "%provider%", MODIFYDNS    ],
      [ "stupid",             \&gst_network_suse90_replace_provider_file_bool,  "%provider%", STUPIDMODE   ],
      [ "persist",            \&gst_network_suse90_replace_provider_file_bool,  "%provider%", PERSIST      ],
      [ "set_default_gw",     \&gst_network_suse90_replace_provider_file_bool,  "%provider%", DEFAULTROUTE ],
      ]
   },

    "pld-1.0" =>
	  {
      iface_set    => \&gst_network_rh62_interface_activate,
      iface_delete => \&gst_network_pld10_interface_delete,
      ifaces_get   => \&gst_network_sysconfig_pld10_ifaces_get_existing,
      fn =>
	    {
        IFCFG  => "/etc/sysconfig/interfaces/ifcfg-#iface#",
        CHAT   => "/etc/sysconfig/interfaces/data/chat-#iface#",
        IFACE  => "#iface#",
        WVDIAL => "/etc/wvdial.conf",
        PUMP   => "/etc/pump.conf"
      },
      table =>
       [
        [ "bootproto",          \&gst_network_pld10_replace_bootproto, IFCFG, BOOTPROTO ],
        [ "auto",               \&gst_replace_sh_bool, IFCFG, ONBOOT ],
        [ "user",               \&gst_replace_sh_bool, IFCFG, USERCTL ],
#        [ "name",               \&gst_replace_sh,      IFCFG, NAME ],
        [ "dev",                \&gst_replace_sh,      IFCFG, DEVICE ],
        [ "address",            \&gst_network_pld10_set_ipaddr, IFCFG, IPADDR, "address" ],
        [ "netmask",            \&gst_network_pld10_set_ipaddr, IFCFG, IPADDR, "netmask" ],
#        [ "broadcast",          \&gst_replace_sh,      IFCFG, BROADCAST ],
#        [ "network",            \&gst_replace_sh,      IFCFG, NETWORK ],
        [ "gateway",            \&gst_replace_sh,      IFCFG, GATEWAY ],
#        [ "update_dns",         \&gst_network_pump_set_nodns, PUMP, "%dev%", "%bootproto%" ],
        [ "update_dns",         \&gst_replace_sh_bool, IFCFG, PEERDNS ],
#        [ "dns1",               \&gst_replace_sh,      IFCFG, DNS1 ],
#        [ "dns2",               \&gst_replace_sh,      IFCFG, DNS2 ],
        [ "mtu",                \&gst_replace_sh,      IFCFG, MTU ],
        [ "mru",                \&gst_replace_sh,      IFCFG, MRU ],
        [ "remote_address",     \&gst_replace_sh,      IFCFG, REMIP ],
        [ "login",              \&gst_replace_sh,      IFCFG, PAPNAME ],
        [ "serial_port",        \&gst_replace_sh,      IFCFG, MODEMPORT ],
        [ "serial_speed",       \&gst_replace_sh,      IFCFG, LINESPEED ],
        [ "ppp_options",        \&gst_replace_sh,      IFCFG, PPPOPTIONS ],
#        [ "section",            \&gst_replace_sh,      IFCFG, WVDIALSECT ],
        [ "set_default_gw",     \&gst_replace_sh_bool, IFCFG, DEFROUTE ],
        [ "debug",              \&gst_replace_sh_bool, IFCFG, DEBUG ],
        [ "persist",            \&gst_replace_sh_bool, IFCFG, PERSIST ],
        [ "serial_escapechars", \&gst_replace_sh_bool, IFCFG, ESCAPECHARS ],
        [ "serial_hwctl",       \&gst_replace_sh_bool, IFCFG, HARDFLOWCTL ],
        [ "phone_number",       \&gst_replace_chat,    CHAT,  "^atd[^0-9]*([0-9, -]+)" ]
      ]
    },

   "slackware-9.1.0" =>
   {
     iface_set    => \&gst_network_slackware91_interface_activate,
     iface_delete => \&gst_network_slackware91_interface_delete,
     ifaces_get => \&gst_network_slackware91_ifaces_get_existing,
     fn =>
     {
       RC_INET_CONF => "/etc/rc.d/rc.inet1.conf",
       RC_INET      => "/etc/rc.d/rc.inet1",
       RC_LOCAL     => "/etc/rc.d/rc.local",
       IFACE        => "#iface#",
       WIRELESS     => "/etc/pcmcia/wireless.opts",
       PPP_OPTIONS  => "/etc/ppp/options",
       PAP          => "/etc/ppp/pap-secrets",
       CHAP         => "/etc/ppp/chap-secrets",
       CHAT         => "/etc/ppp/pppscript",
     },
     table =>
     [
      [ "address",            \&gst_replace_rcinet1conf,                   [ RC_INET_CONF, IFACE ], IPADDR ],
      [ "netmask",            \&gst_replace_rcinet1conf,                   [ RC_INET_CONF, IFACE ], NETMASK ],
      [ "gateway",            \&gst_replace_rcinet1conf_global,            RC_INET_CONF, GATEWAY ],
      [ "bootproto",          \&gst_network_slackware91_replace_bootproto, [ RC_INET_CONF, IFACE ] ],
      [ "auto",               \&gst_network_slackware91_set_auto,          [ RC_INET, RC_LOCAL, IFACE ] ],
      [ "essid",              \&gst_replace_wireless_opts,                 [ WIRELESS, IFACE ], \&gst_network_get_wireless_ifaces, ESSID ],
      [ "key",                \&gst_replace_wireless_opts,                 [ WIRELESS, IFACE ], \&gst_network_get_wireless_ifaces, KEY   ],
      [ "key_type",           \&gst_network_set_wep_key_type, [ \&gst_replace_wireless_opts, [ WIRELESS, IFACE ], \&gst_network_get_wireless_ifaces, KEY, "%key%" ]],
      # Modem stuff
      [ "phone_number",       \&gst_network_check_type,                   [IFACE, "modem", \&gst_network_create_pppscript, CHAT ]],
      [ "phone_number",       \&gst_network_check_type,                   [IFACE, "modem", \&gst_network_slackware91_create_pppgo ]],
      [ "update_dns",         \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_kw, PPP_OPTIONS, "usepeerdns" ]],
      [ "noauth",             \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_kw, PPP_OPTIONS, "noauth" ]],
      [ "set_default_gw",     \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_kw, PPP_OPTIONS, "defaultroute" ]],
      [ "debug",              \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_kw, PPP_OPTIONS, "debug" ]],
      [ "persist",            \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_kw, PPP_OPTIONS, "persist" ]],
      [ "serial_hwctl",       \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_kw, PPP_OPTIONS, "crtscts" ]],
      [ "mtu",                \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_join_first_str, PPP_OPTIONS, "mtu", "[ \t]+" ]],
      [ "mru",                \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_join_first_str, PPP_OPTIONS, "mru", "[ \t]+" ]],
      [ "serial_port",        \&gst_network_check_type,                   [IFACE, "modem", \&gst_network_set_ppp_options_re, PPP_OPTIONS, "^(/dev/[^ \t]+)" ]],
      [ "serial_speed",       \&gst_network_check_type,                   [IFACE, "modem", \&gst_network_set_ppp_options_re, PPP_OPTIONS, "^([0-9]+)" ]],
      [ "login",              \&gst_network_check_type,                   [IFACE, "modem", \&gst_network_set_ppp_options_re, PPP_OPTIONS, "^name \"(.*)\"", "name \"%login%\"" ]],
      [ "ppp_options",        \&gst_network_check_type,                   [IFACE, "modem", \&gst_network_set_ppp_options_unsup, PPP_OPTIONS ]],
      [ "serial_escapechars", \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_join_first_str, PPP_OPTIONS, "escape", "[ \t]+" ]],
      [ "password",           \&gst_network_check_type,                   [IFACE, "modem", \&gst_network_set_pap_passwd, PAP, "%login%" ]],
      [ "password",           \&gst_network_check_type,                   [IFACE, "modem", \&gst_network_set_pap_passwd, CHAP, "%login%" ]],
      [ "dial_command",       \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_chat, CHAT, "(atd[tp])[0-9w, -]+" ]],
      [ "phone_number",       \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_chat, CHAT, "atd[tp]([0-9w]+)" ]],
      [ "external_line",      \&gst_network_check_type,                   [IFACE, "modem", \&gst_replace_chat, CHAT, "atd[tp]([0-9w, -]+)", "%external_line%W%phone_number%" ]],
      [ "volume",             \&gst_network_check_type,                   [IFACE, "modem", \&gst_network_set_modem_volume, CHAT ]],
     ]
   },

   "gentoo" =>
   {
     iface_set    => \&gst_network_gentoo_interface_activate,
     iface_delete => \&gst_network_gentoo_interface_delete,
     ifaces_get   => \&gst_network_gentoo_ifaces_get_existing,
     fn =>
     {
       NET          => "/etc/conf.d/net",
       PPPNET       => "/etc/conf.d/net.#iface#",
       INIT         => "net.#iface#",
       IFACE        => "#iface#",
       WIRELESS     => "/etc/conf.d/wireless",
     },
     table =>
     [
      [ "dev",                \&gst_network_gentoo_create_files ],
      [ "auto",               \&gst_service_gentoo_set_status,        INIT , 0 ],
      [ "bootproto",          \&gst_network_gentoo_replace_bootproto, [ NET, IFACE ]],
      [ "address",            \&gst_replace_confd_net_re,             NET, "config_%dev%", "^[ \t]*([0-9\.]+)" ],
      [ "address",            \&gst_replace_confd_net_re,             NET, "config_%dev%", "^[ \t]*([0-9\.]+)" ],
      [ "netmask",            \&gst_replace_confd_net_re,             NET, "config_%dev%", "[ \t]+netmask[ \t]+[0-9\.]*", " netmask %netmask%"],
      [ "broadcast",          \&gst_replace_confd_net_re,             NET, "config_%dev%", "[ \t]+broadcast[ \t]+[0-9\.]*", " broadcast %broadcast%" ],
      [ "remote_address",     \&gst_replace_confd_net_re,             NET, "config_%dev%", "[ \t]+dest_address[ \t]+[0-9\.]*", " dest_address %remote_address%" ],
      [ "gateway",            \&gst_replace_confd_net_re,             NET, "routes_%dev%", "[ \t]*default[ \t]+(via|gw)[ \t]+[0-9\.\:]*", "default via %gateway%" ],
      [ "essid",              \&gst_replace_sh,                       WIRELESS, "essid_%dev%" ],
      [ "key",                \&gst_replace_sh,                       WIRELESS, "key_%essid%" ],
      [ "key_type",           \&gst_network_set_wep_key_type,         [ \&gst_replace_sh, WIRELESS, "key_%essid%", "%key%" ]],
      # modem stuff
      [ "dev",                \&gst_network_check_type,              [ IFACE, "modem", \&gst_replace_sh, PPPNET, PEER ]],
      [ "update_dns",         \&gst_network_check_type,              [ IFACE, "modem", \&gst_replace_sh_bool, PPPNET, PEERDNS ]],
      [ "mtu",                \&gst_replace_sh,                       PPPNET, MTU ],
      [ "mru",                \&gst_replace_sh,                       PPPNET, MRU ],
      [ "serial_port",        \&gst_replace_sh,                       PPPNET, MODEMPORT ],
      [ "serial_speed",       \&gst_replace_sh,                       PPPNET, LINESPEED ],
      [ "login",              \&gst_replace_sh,                       PPPNET, USERNAME ],
      [ "password",           \&gst_replace_sh,                       PPPNET, PASSWORD ],
      [ "ppp_options",        \&gst_replace_sh,                       PPPNET, PPPOPTIONS ],
      [ "set_default_gw",     \&gst_replace_sh_bool,                  PPPNET, DEFROUTE ],
      [ "debug",              \&gst_replace_sh_bool,                  PPPNET, DEBUG ],
      [ "persist",            \&gst_replace_sh_bool,                  PPPNET, PERSIST ],
      [ "serial_escapechars", \&gst_replace_sh_bool,                  PPPNET, ESCAPECHARS ],
      [ "serial_hwctl",       \&gst_replace_sh_bool,                  PPPNET, HARDFLOWCTL ],
      [ "phone_number",       \&gst_replace_sh,                       PPPNET, NUMBER ],
      [ "external_line",      \&gst_replace_sh,                       PPPNET, NUMBER, "%external_line%W%phone_number%" ],
      [ "volume",             \&gst_network_set_modem_volume_string,  PPPNET, INITSTRING ],
     ]
    },

    "freebsd-5" =>
    {
      iface_set    => \&gst_network_freebsd_interface_activate,
      iface_delete => \&gst_network_freebsd_interface_delete,
      ifaces_get   => \&gst_network_freebsd_ifaces_get_existing,
      fn =>
      {
        RC_CONF         => "/etc/rc.conf",
        STARTIF         => "/etc/start_if.#iface#",
        PPPCONF         => "/etc/ppp/ppp.conf",
        IFACE           => "#iface#",
      },
      table =>
      [
       [ "auto",           \&gst_network_freebsd5_set_auto,          [ RC_CONF, IFACE ]],
       [ "bootproto",      \&gst_network_freebsd5_replace_bootproto, [ RC_CONF, IFACE ]],
       [ "address",        \&gst_replace_sh_re,    RC_CONF, "ifconfig_%dev%", "inet[ \t]+([0-9\.]+)", "inet %address%" ],
       [ "netmask",        \&gst_replace_sh_re,    RC_CONF, "ifconfig_%dev%", "netmask[ \t]+([0-9\.]+)", " netmask %netmask%" ],
       [ "remote_address", \&gst_replace_sh_re,    RC_CONF, "ifconfig_%dev%", "dest_address[ \t]+([0-9\.]+)", " dest_address %remote_address%" ],
       [ "essid",          \&gst_network_freebsd5_replace_essid,     [ RC_CONF, STARTIF, IFACE ]],
       # Modem stuff
       # we need this for putting an empty ifconfig_tunX command in rc.conf
       [ "phone_number",   \&gst_replace_sh,                         RC_CONF, "ifconfig_%dev%", " " ],
       [ "file",           \&gst_network_freebsd_create_ppp_startif, [ STARTIF, IFACE ]],
       [ "persist",        \&gst_network_freebsd_create_ppp_startif, [ STARTIF, IFACE ], "%file%" ],
       [ "serial_port",    \&gst_network_replace_pppconf,            [ PPPCONF, STARTIF, IFACE ], "device" ],
       [ "serial_speed",   \&gst_network_replace_pppconf,            [ PPPCONF, STARTIF, IFACE ], "speed"    ],
       [ "mtu",            \&gst_network_replace_pppconf,            [ PPPCONF, STARTIF, IFACE ], "mtu"      ],
       [ "mru",            \&gst_network_replace_pppconf,            [ PPPCONF, STARTIF, IFACE ], "mru"      ],
       [ "login",          \&gst_network_replace_pppconf,            [ PPPCONF, STARTIF, IFACE ], "authname" ],
       [ "password",       \&gst_network_replace_pppconf,            [ PPPCONF, STARTIF, IFACE ], "authkey"  ],
       [ "update_dns",     \&gst_network_replace_pppconf_bool,       [ PPPCONF, STARTIF, IFACE ], "dns"      ],
       [ "set_default_gw", \&gst_network_replace_pppconf_route,      [ PPPCONF, STARTIF, IFACE ], "default HISADDR" ],
       [ "phone_number",   \&gst_network_replace_pppconf,            [ PPPCONF, STARTIF, IFACE ], "phone"    ],
       [ "external_line",  \&gst_network_replace_pppconf,            [ PPPCONF, STARTIF, IFACE ], "phone", "%external_line%W%phone_number%" ],
       [ "dial_command",   \&gst_network_replace_pppconf_dial_command, [ PPPCONF, STARTIF, IFACE ]],
       [ "volume",         \&gst_network_replace_pppconf_volume,       [ PPPCONF, STARTIF, IFACE ]],
      ]
    }
  );
  
  my $dist = $dist_map{$gst_dist};
  return %{$dist_tables{$dist}} if $dist;

  &gst_report ("platform_no_table", $gst_dist);
  return undef;
}

1;
