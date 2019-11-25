cat $CWD/kdebindings.diff | patch -p1 || exit 1
cat $CWD/Qt_cpp.diff |patch -p3 || exit 1
cat $CWD/Qt_cpp2.diff |patch -p3 || exit 1
cat $CWD/Qt_cpp3.diff |patch -p3 || exit 1
cat $CWD/handlers_cpp.diff |patch -p3 || exit 1
cat $CWD/qtrubyinit_cpp.diff |patch -p3 || exit 1
cat $CWD/krubyinit_cpp.diff |patch -p3 || exit 1
cat $CWD/Korundum_cpp.diff |patch -p3 || exit 1
cat $CWD/kdehandlers_cpp.diff |patch -p3 || exit 1
cat $CWD/python_and_automake.patch |patch -p0 || exit 1
cat $CWD/fpermissive.patch | patch -p1 || exit 1
cat $CWD/fpermissive-4.patch | patch -p1 || exit 1
#cat $CWD/kdebindings-3.5.10-alt-qtruby-const-char.patch | patch -p1 --verbose || exit 1
cat $CWD/kdebindings-3.5.10-alt-ruby-getopts.patch | patch -p1 --verbose || exit 1
cat $CWD/kdebindings-3.5.10-alt-ruby-paths.patch | patch -p1 --verbose || exit 1
#cat $CWD/kdebindings-3.5.10-alt-ruby-compile.patch | patch -p1 --verbose || exit 1
cat $CWD/qtruby-1.9.patch | patch -p1 --verbose || exit 1
cat $CWD/no_ruby.patch | patch -p1 --verbose || exit 1

