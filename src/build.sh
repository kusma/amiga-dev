#!/bin/sh

cd $(dirname $0)

export DESTDIR=../..
export TARGET=m68k
export CPU=m68k
export SYNTAX=mot

make -C vbcc install &&
make -C vasm install &&
make -C vlink install || {
	echo "Build failed" >&2
	exit 1
}

