#include <stdio.h>
#include <zeditty.h>

void z_putchar(z_Machine *mm, unsigned char port, unsigned char value)
{
	if (port == 0x00 && value == 0xFF)
	{
		z_Stop(mm);
	}
	else if (port == 0x00)
	{
		putchar(value);
	}
	else
	{
		fprintf(stderr, "Invalid port write of value 0x%02X to port #%i.\n", port, value);
	}
}

int main()
{
	//Setup our virtual machine
	z_Machine mm;
	z_InitMachine(&mm);
	mm.PortOutCallback = z_putchar;
	//Load our program
	FILE *f = fopen("hello.z80", "r");
	for (int c, i = 0; (c = fgetc(f)) != EOF; i++)
	{
		mm.MEM[i] = (unsigned char)c;
	}
	fclose(f);
	//Run the program
	z_Run(&mm, 0x0000);
	return 0;
}
