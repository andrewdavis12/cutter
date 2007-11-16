#!/bin/sh

run()
{
    $@
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

run libtoolize --copy --force
run glib-gettextize --force --copy
run gtkdocize --copy
run aclocal
run autoheader
run automake --add-missing --foreign --copy
run autoconf
