cat $CWD/kdeaddons_ark_xz_lmza_support.diff | patch -p4 --verbose || exit 1
cat $CWD/kdeaddons-3.5.10-gcc45.diff | patch -p0 --verbose || exit 1
cat $CWD/python_and_automake.patch | patch -p0 --verbose || exit 1
