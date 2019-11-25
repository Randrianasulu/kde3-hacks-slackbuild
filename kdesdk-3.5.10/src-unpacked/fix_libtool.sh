#! /bin/sh


for f in `find $PWD -type f -name Makefile.am`
do
    grep -q LDFLAGS $f || continue
        RPATH_LINK_OPTS+=" -Wl,-rpath-link,`dirname $f`/.libs"
done