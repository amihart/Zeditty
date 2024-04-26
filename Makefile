ifndef CC
	CC=gcc
endif
ifndef AR
	AR=ar
endif

all:
	mkdir -p lib/
	$(CC) -c src/zeditty_ops.c -o src/zeditty_ops.o
	$(CC) -c src/zeditty.c -o src/zeditty.o -Iinclude
	$(AR) rcs lib/libzeditty.a src/zeditty.o src/zeditty_ops.o
	make -C test/
	cd test/ && ./interpret

clean:
	rm -rf src/*.o lib/
	make clean -C test/
