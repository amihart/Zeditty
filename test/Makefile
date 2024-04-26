all:
	gcc interpret.c -o interpret -I../include -L../lib -lzeditty
	sdasz80 -o crt0.rel crt0.s
	sdcc crt0.rel -mz80 --no-std-crt0 hello.c -o ~tmp.hex
	srec_cat ~tmp.hex -Intel -Output -Binary > hello.z80
	cp ~tmp.hex hex
	rm -f ~tmp*

clean:
	rm -f interpret crt0.rel hello.z80
