zcat $CWD/amarok-libmtp8-buildfix.diff.gz | patch -p0 || exit 1
cat $CWD/amarok-wikipedia.diff | patch -p1 || exit 1
cat $CWD/amarok_hide_menubar.diff | patch -p4 || exit 1
cat $CWD/amarok_gcc-44.diff | patch -p0 || exit 1
cat $CWD/mongrel_gcc44.diff | patch -p1 || exit 1
cat $CWD/mongrel_gcc44_2.diff | patch -p1 || exit 1
cat $CWD/gcc-45.diff | patch -p0 || exit 1
cat $CWD/fix-for-ruby-1.9.patch | patch -p1 || exit
cat $CWD/ruby-1.9.patch | patch -p1 || exit
cat $CWD/amarok-1.4.10-fix-autoconf-2.64.patch | patch -p1 || exit
cat $CWD/amarok-gcc-4.6.patch | patch -p1 || exit
cat $CWD/gcc-4.7.patch | patch -p1 || exit
cat $CWD/python_and_automake.patch | patch -p0 || exit