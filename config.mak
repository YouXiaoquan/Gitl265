SRCPATH=.
ARCH=X86
SYS=LINUX
CC=gcc
CFLAGS= -Wshadow -DHIGH_BIT_DEPTH=0 -O3 -ffloat-store -Wuninitialized -Wall -I. -I$(SRCPATH) -march=i686 -mfpmath=sse -msse -std=gnu99 -fno-tree-vectorize
DEPMM=-MM
DEPMT=-MT
LD=gcc -o 
LDFLAGS = -m32 -lm -lpthread
LIBX265=libx265.a
AR=ar rc 
RANLIB=ranlib
STRIP=strip
AS=yasm
ASFLAGS= DWARF2 -f elf -DHIGH_BIT_DEPTH=0 -DBIT_DEPTH=8
RC=
RCFLAGS=
EXE=


default: cli
install: install-cli
CLI_LIBX265 = $(LIBX265)