cat $CWD/kpdf.diff | patch -p4 --verbose || exit 1
cat $CWD/kpdf-rotate-option.patch | patch -p1 --verbose || exit 1 
cat $CWD/potential-resize-bug_and-xpdf-fixes.patch | patch -p1 --verbose || exit 1
cat $CWD/python_and_automake.patch | patch -p0 --verbose || exit 1
cat $CWD/libdjvu.patch | patch -p1 --verbose || exit 1

# apply those only if you have new fribidi - still not work on sw 14.1

cat $CWD/fribidi-pkg-fix.patch | patch -p1 --verbose || exit 1
cat $CWD/fribidi-headers-detect.patch | patch -p1 --verbose || continue
cat $CWD/fribidi-fall-out-fix.patch | patch -p1 --verbose || exit 1

#cat $CWD/support-for-gphoto25.patch | patch -p1 --verbose || exit 1

# only apply those two if you have gphoto 2.5.x!
#cat $CWD/gphoto25-1.patch | patch -p1 --verbose || exit 1
#cat $CWD/gphoto25-2.patch | patch -p1 --verbose || exit 1