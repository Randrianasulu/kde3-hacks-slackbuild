cat $CWD/kdewebdev_svn.diff | patch -p4 --verbose || exit 1
cat $CWD/python_and_automake.patch | patch -p0 --verbose || exit 1
