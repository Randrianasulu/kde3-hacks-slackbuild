#!/usr/bin/env ruby
#
# Ruby script for generating libkexiv2 tarball releases from KDE SVN
#
# (c) 2005 Mark Kretschmann <markey@web.de>
# Some parts of this code taken from cvs2dist
# License: GNU General Public License V2


name       = "libkexiv2"
egmodule   = "libs"
version    = "0.1.8"
docs       = "no"
usesvnver  = "no"

#svnbase    = "svn+ssh://gkulzer@svn.kde.org/home/kde"
svnbase    = "https://cgilles@svn.kde.org/home/kde"
#svnbase    = "svn+ssh://anaselli@svn.kde.org/home/kde"
#svnbase    = "svn://anonsvn.kde.org/home/kde"
svnroot    = "#{svnbase}/trunk"
extragear  = "#{svnbase}/branches/extragear/kde3"
adminroot  = "#{svnbase}/branches/KDE/3.5"

addDocs    = []
addPo      = []

#----------------------------------------------------------------

if (usesvnver  != "yes")
    folder     = name + "-" + version
else
    t = Time.now
    folder     = name + "-" + version + "." + "0." + t.strftime("%Y%m%d")
end

addPo      = [name] + addPo
addDocs    = [name] + addDocs

# Prevent using unsermake
oldmake = ENV["UNSERMAKE"]
ENV["UNSERMAKE"] = "no"

puts "Fetching #{egmodule}/#{name}..."
# Remove old folder, if exists
`rm -rf #{folder} 2> /dev/null`
`rm -rf folder.tar.bz2 2> /dev/null`

# Dir.mkdir( folder )
Dir.mkdir( "#{folder}" )
Dir.chdir( folder )

`svn co -N #{extragear}/#{egmodule}`
Dir.chdir( egmodule )
`svn up #{name}`

if ( docs != "no")
`svn up -N doc`
    for dg in addDocs
        dg.chomp!
        `svn up doc/#{dg}`
    end
end

`svn co #{adminroot}/kde-common/admin`
puts "done\n"

puts "\n"
puts "Fetching l10n docs for #{egmodule}/#{name}...\n"
puts "\n"

i18nlangs = `svn cat #{svnroot}/l10n-kde3/subdirs`
i18nlangsCleaned = []
for lang in i18nlangs
  l = lang.chomp
  if (l != "xx")
    i18nlangsCleaned += [l];
  end
end
i18nlangs = i18nlangsCleaned

Dir.mkdir( "l10n" )
Dir.chdir( "l10n" )

# docs
for lang in i18nlangs
  lang.chomp!

  for dg in addDocs
    dg.chomp!
    `rm -rf #{dg}`
    docdirname = "l10n-kde3/#{lang}/docs/extragear-#{egmodule}/#{dg}"
    if ( docs != "no")
        `svn co -q #{svnroot}/#{docdirname} > /dev/null 2>&1`
    end
    next unless FileTest.exists?( dg )
    print "Copying #{lang}'s #{dg} documentation over...  "
    `cp -R #{dg}/ ../doc/#{lang}_#{dg}`

    # we don't want KDE_DOCS = AUTO, cause that makes the
    # build system assume that the name of the app is the
    # same as the name of the dir the Makefile.am is in.
    # Instead, we explicitly pass the name..
    makefile = File.new( "../doc/#{lang}_#{dg}/Makefile.am", File::CREAT | File::RDWR | File::TRUNC )
    makefile << "KDE_LANG = #{lang}\n"
    makefile << "KDE_DOCS = #{dg}\n"
    makefile.close()

    puts( "done.\n" )
  end
end

puts "\n"
puts "Fetching l10n po for #{egmodule}/#{name}...\n"
puts "\n"

Dir.chdir( ".." ) # in egmodule now

$subdirs = false
Dir.mkdir( "po" )

for lang in i18nlangs
  lang.chomp!
  dest = "po/#{lang}"

  for dg in addPo
    dg.chomp!
    pofilename = "l10n-kde3/#{lang}/messages/extragear-#{egmodule}/#{dg}.po"
    `svn cat #{svnroot}/#{pofilename} 2> /dev/null | tee l10n/#{dg}.po`
    next if FileTest.size( "l10n/#{dg}.po" ) == 0

    if !FileTest.exist?( dest )
      Dir.mkdir( dest )
    end

    print "Copying #{lang}'s #{dg}.po over ..  "
    `mv l10n/#{dg}.po #{dest}`
    puts( "done.\n" )

    makefile = File.new( "#{dest}/Makefile.am", File::CREAT | File::RDWR | File::TRUNC )
    makefile << "KDE_LANG = #{lang}\n"
    makefile << "SUBDIRS  = $(AUTODIRS)\n"
    makefile << "POFILES  = AUTO\n"
    makefile.close()

    $subdirs = true
  end
end

if $subdirs
  makefile = File.new( "po/Makefile.am", File::CREAT | File::RDWR | File::TRUNC )
  makefile << "SUBDIRS = $(AUTODIRS)\n"
  makefile.close()
else
  `rm -Rf po`
end

`rm -rf l10n`
puts "\n"

#add a release marker
`export LANGUAGE=en_US.UTF-8; export LC_ALL=en_US.UTF-8; svn info #{name} | egrep '^Last Changed (Rev|Date):' > RELEASE.rev`

# Remove SVN data folder
`find -name ".svn" | xargs rm -rf`

# Remove release_XXX.rb files 
`/bin/rm -rf release_*.rb`  
# Remove prepare_XXX.rb files 
`/bin/rm -rf prepare_*.rb`  
# Remove release_kipi-plugins_cl.sh
`/bin/rm -rf release_*.sh`

`/bin/mv * ..`
Dir.chdir( ".." ) # name-version
`rmdir #{egmodule}`

# Move some important files to the root folder
Dir.chdir( "#{name}" )
`/bin/mv -f #{name}.lsm ..`
`/bin/mv -f AUTHORS ..`
`/bin/mv -f ChangeLog ..`
`/bin/mv -f COPYING ..`
`/bin/mv -f INSTALL ..`
`/bin/mv -f NEWS ..`
`/bin/mv -f README ..`
`/bin/mv -f TODO ..`
Dir.chdir( ".." )


# Generate makefiles
`find | xargs touch`

puts "\n"
puts "Generating Makefiles..  "
`make -f Makefile.cvs`
puts "done.\n"

`rm -rf autom4te.cache`
`rm stamp-h.in`

puts "\n"
puts "Compressing..  "
Dir.chdir( ".." ) # root folder
`tar -jcf #{folder}.tar.bz2 #{folder}`
`rm -rf #{folder}`
puts "done.\n"


ENV["UNSERMAKE"] = oldmake
