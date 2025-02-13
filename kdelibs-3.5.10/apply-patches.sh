zcat $CWD/kdelibs-utempter.diff.gz | patch -p1 --verbose --backup --suffix=.orig || exit 1
cat $CWD/ftp-gcc-4.4-fix.diff | patch -p1 --verbose || exit 1
cat $CWD/kdelibs.inotify.diff | patch -p1 --verbose || exit 1
cat $CWD/kdelibs.no.inotify.externs.diff | patch -p1 --verbose || exit 1
cat $CWD/kdelibs_ark_xz_lmza_support.diff | patch -p1 --verbose || exit 1
cat $CWD/kdelibs-3.5.10-qcolor_gcc_ftbfs.patch | patch -p1 --verbose || exit 1
cat $CWD/python_and_automake.patch | patch -p0 --verbose || exit 1
cat $CWD/kcontrol_crash_patch.diff | patch -p0 --verbose || exit 1

# only enable if you have openssl >= 1.0.0!
cat $CWD/kdelibs-3.5-openssl-1.0.0.patch | patch -p1 --verbose || exit
cat $CWD/sni_ssl.patch | patch -p1 --verbose || exit

cat $CWD/kdelibs-3.5.10-62_fix_googlemaps_backport.diff | patch -p1 --verbose || exit
cat $CWD/kdelibs-3.5.10-63_fixed-layout-table.diff | patch -p1 --verbose || exit
zcat $CWD/kdelibs-3.5.10-CVE-2009-2702.patch.gz | patch -p1 --verbose || exit
cat $CWD/kdelibs-3.5.10-CVE-2013-2074.patch | patch -p1 --verbose || exit
zcat $CWD/kdelibs-3.5.10-assert.patch.gz | patch -p1 --verbose || exit
cat $CWD/kdelibs-3.5.10-charset-1.patch | patch -p1 --verbose || exit
cat $CWD/kdelibs-3.5.10-cups.patch | patch -p0 --verbose || exit
zcat $CWD/kdelibs-3.5.10-cve-2009-1698.patch.gz | patch -p1 --verbose || exit
zcat $CWD/kdelibs-3.5.10-cve-2009-1725.patch.gz | patch -p0 --verbose || exit
zcat $CWD/kdelibs-3.5.10-cve-2009-2537-select-length.patch.gz | patch -p1 --verbose || exit
cat $CWD/kdelibs-3.5.10-khtml.patch | patch -p0 --verbose || exit
cat $CWD/kdelibs-3.5.10-kjs-gcc44.patch | patch -p0 --verbose || exit
zcat $CWD/kdelibs-3.5.10-oCERT-2009-015-xmlhttprequest.patch.gz | patch -p0 --verbose || exit
cat $CWD/kdelibs-gcc44.patch | patch -p1 --verbose || exit
cat $CWD/kdelibs_gcc45.patch | patch -p1 --verbose || exit

cat $CWD/kdelibs-3.5.10-render_text.patch | patch -p0 --verbose || exit 
cat $CWD/kdelibs-3.5.10-fix_crash_on_big_tables.patch | patch -p0 --verbose || exit
#cat $CWD/kdelibs-3.5.10-fix_kio_http_401.patch | patch -p0 --verbose || exit
#cat $CWD/kdelibs-3.5.10-Class_node_list.patch | patch -p0 --verbose || exit
cat $CWD/kdelibs-3.5.10-bidi.patch | patch -p0 --verbose|| exit

cat $CWD/kdelibs-3.5.10-fix_abs_posit_static_elem.patch | patch -p0 --verbose || exit
# cat $CWD/kdelibs-3.5.10-fix_bug_156947.patch | patch -p0 --verbose || exit
# cat $CWD/kdelibs-3.5.10-fix_charset_in_css.patch | patch -p0 --verbose || exit
# cat $CWD/kdelibs-3.5.10-linebreak.patch | patch -p0 --verbose || exit
#cat $CWD/kdelibs-3.5.10-new_jscompat_mode.patch | patch -p0 --verbose || exit
cat $CWD/kdelibs-3.5.10_fix_http_resp_parsing.patch | patch -p0 --verbose || exit
