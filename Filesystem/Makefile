BASE_SRC := base/system.h base/kernel.h base/libc.cc
BASE_CC  := g++ -g -I. --std=c++11 -Wall
CC       := g++ -g -I. --std=c++11 -Wall filesystem.o base/libc.cc base/disk.cc

alltests: emptydump open write read delete mkdir cd pwd relative move


filesystem.o: filesystem.h filesystem.cc base/disk.h base/disk.cc
	${BASE_CC} filesystem.cc -g -c -I. --std=c++11 -Wall

emptydump: tests/emptydump.cc filesystem.o
	${CC} tests/emptydump.cc -oemptydump

open: open1 open2
open1: tests/open1.cc filesystem.o ${BASE_SRC}
	${CC} tests/open1.cc -oopen1

open2: tests/open2.cc filesystem.o ${BASE_SRC}
	${CC} tests/open2.cc -oopen2

write: write1 write2 write3
write1: tests/write1.cc filesystem.o ${BASE_SRC}
	${CC} tests/write1.cc -owrite1

write2: tests/write2.cc filesystem.o ${BASE_SRC}
	${CC} tests/write2.cc -owrite2

write3: tests/write3.cc filesystem.o ${BASE_SRC}
	${CC} tests/write3.cc -owrite3

read: read1 read2
read1: tests/read1.cc filesystem.o ${BASE_SRC}
	${CC} tests/read1.cc -oread1

read2: tests/read2.cc filesystem.o ${BASE_SRC}
	${CC} tests/read2.cc -oread2

delete: delete1 delete2 delete3
delete1: tests/delete1.cc filesystem.o ${BASE_SRC}
	${CC} tests/delete1.cc -odelete1
delete2: tests/delete2.cc filesystem.o ${BASE_SRC}
	${CC} tests/delete2.cc -odelete2
delete3: tests/delete3.cc filesystem.o ${BASE_SRC}
	${CC} tests/delete3.cc -odelete3

mkdir: mkdir1 mkdir2
mkdir1: tests/mkdir1.cc filesystem.o ${BASE_SRC}
	${CC} tests/mkdir1.cc -omkdir1
mkdir2: tests/mkdir2.cc filesystem.o ${BASE_SRC}
	${CC} tests/mkdir2.cc -omkdir2

cd: cd1 cd2
cd1: tests/cd1.cc filesystem.o ${BASE_SRC}
	${CC} tests/cd1.cc -ocd1
cd2: tests/cd2.cc filesystem.o ${BASE_SRC}
	${CC} tests/cd2.cc -ocd2

pwd: pwd1 pwd2
pwd1: tests/pwd1.cc filesystem.o ${BASE_SRC}
	${CC} tests/pwd1.cc -opwd1
pwd2: tests/pwd2.cc filesystem.o ${BASE_SRC}
	${CC} tests/pwd2.cc -opwd2

relative: relative1 relative2
relative1: tests/relative1.cc filesystem.o ${BASE_SRC}
	${CC} tests/relative1.cc -orelative1
relative2: tests/relative2.cc filesystem.o ${BASE_SRC}
	${CC} tests/relative2.cc -orelative2

move: move1 move2 move3
move1: tests/move1.cc filesystem.o ${BASE_SRC}
	${CC} tests/move1.cc -omove1
move2: tests/move2.cc filesystem.o ${BASE_SRC}
	${CC} tests/move2.cc -omove2
move3: tests/move3.cc filesystem.o ${BASE_SRC}
	${CC} tests/move3.cc -omove3

# have to type make disk.iso before we can use the qemu...
disk.iso:
	truncate -s 16M disk.iso

randomdisk:
	dd if=/dev/urandom of=disk.iso bs=1024 count=16K

clean:
	rm -f emptydump open1 open2 write1 write2 write3 read1 read2 delete1 delete2 delete3 mkdir1 mkdir2 cd1 cd2 pwd1 pwd2 relative1 relative2 move1 move2 move3 disk.iso filesystem.o

runempty: emptydump randomdisk
	./emptydump

runopen: open disk.iso
	./open1
	./open2

runwrite: write disk.iso
	./write1
	./write2
	./write3

runread: read disk.iso
	./read1
	./read2

rundelete: delete disk.iso
	./delete1
	./delete2
	./delete3

runmkdir: mkdir disk.iso
	./mkdir1
	./mkdir2

runcd: cd disk.iso
	./cd1
	./cd2

runpwd: pwd disk.iso
	./pwd1
	./pwd2

runrelative: relative disk.iso
	./relative1
	./relative2

runmove: move disk.iso
	./move1
	./move2
	./move3
