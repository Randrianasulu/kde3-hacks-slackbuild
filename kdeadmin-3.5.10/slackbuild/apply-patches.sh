cat $CWD/kdeadmin.slackware.diff | patch -p1 --verbose || exit 1
cat $CWD/kdeadmin_svn.diff | patch -p4 --verbose || exit 1
cat $CWD/packageDisplay_cpp.diff | patch -p1 --verbose || exit 1
cat $CWD/python_and_automake.patch | patch -p0 --verbose || exit 1
