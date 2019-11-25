cat $CWD/callgraphview.diff | patch -p3 --verbose || exit 1
cat $CWD/CharScanner.diff | patch -p3 --verbose || exit 1
cat $CWD/pofiles.diff | patch -p2 --verbose || exit 1
