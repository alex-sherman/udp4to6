# Copyright (c) 2005-2007 Arada Syatems, Inc. All rights reserved.

# Proprietary and Confidential Material.

#TOOLCHAIN=/opt/build/toolchain/PB44/buildroot/build_mips/staging_dir/usr/bin
#CC=mips-linux-uclibc-gcc
include ./makefile_tlcn.inc

export PATH:=$(TOOLCHAIN):/${PATH}
OBJDIR= ..
INCS+= -I ${OBJDIR}/incs
#INCS+= -I ${OBJDIR}/socket-CAN/include

INCS+= -I ${OBJDIR}/src

BINDIR= ${OBJDIR}/bin

LIBDIR= ${OBJDIR}/lib

all: udp4to6
	mkdir -p ${BINDIR}
	mv -f udp4to6 ${BINDIR}/
	sshpass -p "password" scp ${BINDIR}/udp4to6 root@192.168.0.41:/var/

%.o: %.c
	$(CC) $(INCS) -c -DLOCOMATE -DSDK_NEW -o $@ $<

LDFLAGS= -lm -lpthread -lwave -lbluetooth -L $(LIBDIR) -Xlinker -rpath -Xlinker $(LIBDIR) -lwave-encdec -Lasn1c/


udp4to6: udp4to6.o 
	${CC} -g $(LDFLAGS) -o udp4to6  udp4to6.o

clean:
	rm -f *.o
	cd ${BINDIR}; rm -f udp4to6; cd -
