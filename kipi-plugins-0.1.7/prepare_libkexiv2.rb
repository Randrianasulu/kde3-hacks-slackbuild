#!/usr/bin/env ruby
#
# Ruby script for preparing libkexiv2 releases from KDE SVN
#
# (c) 2006 Angelo Naselli <anaselli at linux dot it>
# 
# License: GNU General Public License V2

name       = "libkexiv2"
egmodule   = "libs"
# tarball version
version    = "0.1.7"
# KEXIV2_VERSION into version.h (has to be equal to version)
version_n  = "0x000107"
# Makefile.am libtool version (current:revision:age)
version_info = "4:0:1"

usesv2cl   = "yes"
# svn2cl depends on how you installed it
svn2cl     = "svn2cl"
#svnbase    = "svn+ssh://anaselli@svn.kde.org/home/kde"
svnbase    = "svn+ssh://vfuoglio@svn.kde.org/home/kde"
svnroot    = "branches/extragear/kde3"
# last release revision tag + 1 (look at released Changelog) 
chlog_rev  = "707243"

t          = Time.now
date       = t.strftime("%Y-%m-%d")

# modify libkexiv2/libkexiv2.lsm
old_file   = name + "/" + name + ".lsm"
new_file   = name + "/" + name + ".lsm.new"

old = File.open(old_file)
new = File.open(new_file, "w")
while old.gets do
    fileLine = $_
    if (fileLine.index("Version") )
      new.print  "Version: " + version + "\n"
    elsif (fileLine.index("Entered-date") )
      new.print  "Entered-date: " + date + "\n"
    else
      new.print  fileLine
    end
end
old.close
new.close
File.rename(old_file, "old.orig")
File.rename(new_file, old_file)
puts( "done #{old_file}\n" )

# modify libkexiv2/libexiv2/version.h
old_file   = name + "/" + name + "/version.h"
new_file   = name + "/" + name + "/version.h.new"

old = File.open(old_file)
new = File.open(new_file, "w")
while old.gets do
    fileLine = $_
    if (fileLine.index("kexiv2_version") )
      new.print  "static const char kexiv2_version[] = \"" + version + "\";\n"
    elsif (fileLine.index("KEXIV2_VERSION") )
       if (fileLine.index("0x"))
         new.print  "#define KEXIV2_VERSION " + version_n +"\n"
       else
         new.print  fileLine
       end
    else
      new.print  fileLine
    end
end
old.close
new.close
File.rename(old_file, "old.orig")
File.rename(new_file, old_file)
puts( "done #{old_file}\n" )

# modify libkexiv2/libkexiv2.pc.in
old_file   = name + "/" + name + ".pc.in"
new_file   = name + "/" + name + ".pc.in.new"

old = File.open(old_file)
new = File.open(new_file, "w")
while old.gets do
    fileLine = $_
    if (fileLine.index("Version") )
      new.print  "Version: " + version + "\n"
    else
      new.print  fileLine
    end
end
old.close
new.close
File.rename(old_file, "old.orig")
File.rename(new_file, old_file)
puts( "done #{old_file}\n" )

# modify libkexiv2/libkexiv2/Makefile.am
old_file   = name + "/" + name + "/" + "Makefile.am"
new_file   = name + "/" + name + "/" + "Makefile.am.new"

old = File.open(old_file)
new = File.open(new_file, "w")
while old.gets do
    fileLine = $_
    if (fileLine.index("libkexiv2_version_info =") )
      new.print  "libkexiv2_version_info = " + version_info + "\n"
    else
      new.print  fileLine
    end
end
old.close
new.close
File.rename(old_file, "old.orig")
File.rename(new_file, old_file)
puts( "done #{old_file}\n" )

# modify libkexiv2/Changelog
if (usesv2cl  == "yes")
	chfile = name + "/ChangeLog"
	`rm -f #{chfile} 2> /dev/null`
	chlog = File.open(chfile, "w")
	chlog.print "V " + version + " - " + date + "\n"
	chlog.print "----------------------------------------------------------------------------"
	chlog.print "\n"
	chlog.close
	
`#{svn2cl} --break-before-msg \
    --strip-prefix=#{svnroot}/#{egmodule} --stdout \
    -i #{svnbase}/#{svnroot}/#{egmodule}/#{name} -r head:#{chlog_rev} >> #{chfile}`

	`echo \"----------------------------------------------------------------------------\" >> #{chfile}`
	`svn cat #{svnbase}/#{svnroot}/#{egmodule}/#{name}/ChangeLog >> #{chfile}`
	puts( "done #{chfile}\n" )
	puts( "\n" )
end

