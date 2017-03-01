Current build consists of:
  - vbcc 0.9e
	- vlink 0.15d
	- vasm 1.7h

All was done following excellent guide by Blitter Studio:
http://blitterstudio.com/setting-up-an-amiga-cross-compiler-windows/
http://blitterstudio.com/setting-up-an-amiga-cross-compiler-windows-part-2/
EXCEPT:
  1) When compiling vbcc, in makefile remove params from CC.
  2) When running 'make TARGET=m68k bin/vbccm68k', be sure to answer that
     floats/doubles are LITTLE ENDIAN, OTHERWISE THEY WILL BE UTTERLY MESSED UP.

Enjoy!
- KaiN