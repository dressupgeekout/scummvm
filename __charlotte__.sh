#!/bin/sh

set -ex

#--enable-engine-dynamic=tetraedge,wintermute \

configure() {
  ./configure \
    --disable-all-engines \
    --enable-engine-static=rockett \
    --with-vorbis-prefix=/usr/pkg \
    --with-ogg-prefix=/usr/pkg \
    --with-theoradec-prefix=/usr/pkg \
    --with-jpeg-prefix=/usr/pkg 
}

run() {
  ./scummvm \
    --game=rockett_tricky \
    --engine=rockett \
    --debuglevel=11
}

if [ "$1" = "configure" ]; then
  configure
else
  run
fi
