#include <stdio.h>
#include <zeditty.h>

void ports_out(z_Machine *mm, unsigned char port, unsigned char value)
{
	switch (port)
	{
		case 0x00: putchar(value); break;
		case 0xFF: z_Stop(mm); break;
		default:
			fprintf(stderr, "Invalid port write (%i;%i).\n", port, value);
	}
}

unsigned char ports_in(z_Machine *mm, unsigned char port)
{
	switch (port)
	{
		case 0x00: return getchar();
		default:
			fprintf(stderr, "Invalid port read (%i).\n", port);
	}
}

int main()
{
	//Setup our virtual machine
	z_Machine mm;
	z_InitMachine(&mm);
	mm.PortOutCallback = ports_out;
	mm.PortInCallback = ports_in;
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
