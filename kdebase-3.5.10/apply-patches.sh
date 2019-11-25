cat $CWD/ksmserver-3.5.9-ksmserver_suspend.diff | patch -p1 --verbose || exit 1
cat $CWD/gcc-4.4-patch.diff | patch -p3 --verbose || exit 1
cat $CWD/suspend_arts.diff | patch -p3 --verbose || exit 1

# possibly only for ssl 1.0.0?
#cat $CWD/crypto_patch.diff | patch -p4 --verbose || exit 1
#cat $CWD/crypto2.patch.diff | patch -p3 --verbose || exit 1

cat $CWD/kicker.diff | patch -p4 --verbose || exit 1
 cat $CWD/fish.diff | patch -p4 --verbose || exit 1
cat $CWD/libkonq.diff | patch -p4 --verbose || exit 1
cat $CWD/nsplugins.diff | patch -p4 --verbose || exit 1
#cat $CWD/40_use_hal_mountoptions.diff | patch -p1 --verbose || exit 1
#cat $CWD/kdebase-3.5.10-alt-nonhal_backend_options_fix.patch | patch -p1 --verbose || exit 1
cat $CWD/kdebase-3.5.10-alt-usb_close.patch |patch -p1 --verbose || exit 1
cat $CWD/kdebase-3.5.10-alt-ntfs3g-locale.patch | patch -p1 --verbose || exit 1
#cat $CWD/kdebase-3.5.6-kioslave_media_dbus.patch | patch -p1 --verbose || exit 1
#cat $CWD/kdebase-3.5.7-alt-media-baseurl-encoding.patch | patch -p1 --verbose || exit 1
#cat $CWD/kdebase_kioslave-r911492.patch | patch -p1 --verbose || exit 1

#cat $CWD/python_and_automake.patch | patch -p0 --verbose || exit 1
