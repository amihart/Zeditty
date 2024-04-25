#Compile our code
sdcc -mz80 --no-std-crt0 $1.c --reserve-regs-iy --data-loc 0xA000 --code-loc 0x0000 -o ~tmp.hex
if [ "$?" != "0" ]
then
	exit 1
fi

#Deformat our hex file into a raw hex string
echo $(cut -c 10- < ~tmp.hex | sed 's/..$//' | tr -d '\n') > ~tmp.hxs
if [ "$?" != "0" ]
then
	exit 1
fi

#Convert our hex string into a binary file
xxd -r -p < ~tmp.hxs > ~tmp.z80
if [ "$?" != "0" ]
then
	exit 1
fi

#Fix the program's name so it doesn't get deleted
mv ~tmp.z80 $1.z80
if [ "$?" != "0" ]
then
	exit 1
fi

#Clean up
rm -f ~tmp*
if [ "$?" != "0" ]
then
	exit 1
fi

