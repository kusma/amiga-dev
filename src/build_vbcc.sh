#!/bin/sh
export TARGET=m68k

cd vbcc &&
mkdir -p bin &&
prefix=../.. make install
