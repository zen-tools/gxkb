#!/usr/bin/env bash
set -e

test -n "$(which automake)" || {
    echo "[ERROR] automake is missing on your system" 1>&2;
    exit 1;
}

test -n "$(which autoconf)" || {
    echo "[ERROR] autoconf is missing on your system" 1>&2;
    exit 2;
}

echo "If you get errors, you may need newer versions of automake and autoconf." 2>&1;
echo "You'll need at least automake 1.5 and autoconf 2.50." 2>&1;

aclocal $ACLOCAL_FLAGS \
&& autoheader \
&& automake --add-missing --copy \
&& autoconf \
&& echo "Now you are ready to run ./configure";

exit 0;

