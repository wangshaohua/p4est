#! /bin/bash

# This program comes with ABSOLUTELY NO WARRANTY.

# unpack under current directory
UNPACK=`pwd`
# choose names for fast and debug compilation directories
BUILD_FAST="$UNPACK/p4est-build/FAST"
BUILD_DEBUG="$UNPACK/p4est-build/DEBUG"
# choose names for fast and debug installation directories
INSTALL_FAST="$UNPACK/p4est-install/FAST"
INSTALL_DEBUG="$UNPACK/p4est-install/DEBUG"

function busage() {
        echo "Usage: `basename $0` <p4est_tar.gz_file>"
}
function bdie () {
        echo "Error: $@"
        exit 1
}

echo
echo "This script tries to unpack, configure and build the p4est library."
echo "Build FAST: $BUILD_FAST"
echo "Build DEBUG: $BUILD_DEBUG"
echo "Install FAST: $INSTALL_FAST"
echo "Install DEBUG: $INSTALL_DEBUG"
echo "Checking environment: CFLAGS P4EST_CFLAGS_FAST P4EST_CFLAGS_DEBUG"
if test -z "$CFLAGS" -a -z "$P4EST_CFLAGS_FAST" ; then
        export CFLAGS_FAST="-O2"
else
        export CFLAGS_FAST="$CFLAGS $P4EST_CFLAGS_FAST"
fi
echo "CFLAGS_FAST: $CFLAGS_FAST"
if test -z "$CFLAGS" -a -z "$P4EST_CFLAGS_DEBUG" ; then
        export CFLAGS_DEBUG="-O0 -g"
else
        export CFLAGS_DEBUG="$CFLAGS $P4EST_CFLAGS_DEBUG"
fi
echo "CFLAGS_DEBUG: $CFLAGS_DEBUG"

TGZ="$1"; shift
if test ! -f "$TGZ" ; then
        busage
        bdie "File not found"
fi
if ! (echo "$TGZ" | grep -q 'p4est.*.tar.gz') ; then
        busage
        bdie "File name mismatch"
fi
DIR=`echo "$TGZ" | sed 's/\(p4est.*\).tar.gz/\1/'`
DIR=`basename $DIR`
echo "Unpack directory: $UNPACK/$DIR"
if test -d "$UNPACK/$DIR" ; then
        echo \
            "Directory found (remove it and also the build directories" \
            "to start over)"
else
        echo -n "Unpacking... "
        tar -xvz -f "$TGZ" -C "$UNPACK" >/dev/null
        echo "done"
fi
test -f "$UNPACK/$DIR/src/p4est.h" || bdie "Main header file missing"
test -f "$UNPACK/$DIR/configure" || bdie "Configure script missing"

echo
echo "See output in files .../config.output and .../make.output"
echo "Build FAST in $BUILD_FAST"
mkdir -p "$BUILD_FAST"
cd "$BUILD_FAST"
"$UNPACK/$DIR/configure" --enable-mpi --without-blas \
        --prefix="$INSTALL_FAST" CFLAGS="$CFLAGS_FAST" \
        CPPFLAGS="-DSC_LOG_PRIORITY=SC_LP_ESSENTIAL" \
        "$@" > config.output || bdie "Error in configure"
make -C sc -j 8 > make.output || bdie "Error in make sc"
make -j 8 >> make.output || bdie "Error in make p4est"
make install >> make.output || bdie "Error in make install"
echo "FAST installed in $INSTALL_FAST"

echo
echo "Build DEBUG in $BUILD_DEBUG"
mkdir -p "$BUILD_DEBUG"
cd "$BUILD_DEBUG"
"$UNPACK/$DIR/configure" --enable-mpi --without-blas \
        --enable-debug --disable-vtk-binary \
        --prefix="$INSTALL_DEBUG" CFLAGS="$CFLAGS_DEBUG" \
        "$@" > config.output || bdie "Error in configure"
make -C sc -j 8 > make.output || bdie "Error in make sc"
make -j 8 >> make.output || bdie "Error in make p4est"
make install >> make.output || bdie "Error in make install"
echo "DEBUG installed in $INSTALL_DEBUG"
echo
