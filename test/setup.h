//Remove this if you're using an
//older version of SDCC
#define NEW_CALLING_CONVENTION
void main();
void __init() __naked
{
	main();
	__asm
		ld a, #255
		out (0), a
	__endasm;
}

int putchar(int c)
{
#ifdef NEW_CALLING_CONVENTION
	__asm
		ld a, e
		out (0), a
	__endasm;
#else
	__asm
		ld iy, #2
		add iy, sp
		ld a, (iy)
		out (0), a
	__endasm;
#endif
	return c;
}
