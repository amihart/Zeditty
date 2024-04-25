#ifndef __z_H__
#define __z_H__
#include <stdlib.h>
typedef struct z_Machine z_Machine;
struct z_Machine { //!show struct
	int T0, T1, ST0, ST1, INT_ENABLED, INT_MODE, TABLE, STATUS;
	int REGS[14];
	////!show struct The program memory is located here.
	unsigned char MEM[65536]; //!show struct
	////!show struct This callback is executed whenever
	////!show struct the interpreter encounters an OUT
	////!show struct instruction.
	void (*PortOutCallback)(z_Machine*, unsigned char, unsigned char); //!show struct
	////!show struct This callback is executed whenever
	////!show struct the interpreter encounters an IN
	////!show struct instruction.
	unsigned char (*PortInCallback)(z_Machine*, unsigned char); //!show struct
	////!show struct This callback is executed
	////!show struct repeatedly when interrupts are
	////!show struct enabled. The lower byte is 1 or 0
	////!show struct indicating whether or not the
	////!show struct interrupt should be fired. The
	////!show struct upper byte is used for Mode 2
	////!show struct interrupts as the value combined
	////!show struct combined with the I register.
	unsigned short (*InterruptCallback)(z_Machine*, unsigned char); //!show struct
}; //!show struct
//Instruction info
typedef struct {
	unsigned char OpCode, Table;
	unsigned char SimplifiedLength;
	unsigned char Simplified[256];
} z_InstructionInfo;
//Defines
#define z_IDLE 0
#define z_RUNNING 1
#define z_REG_AF 0
#define z_REG_BC 1
#define z_REG_DE 2
#define z_REG_HL 3
#define z_REG_IX 4
#define z_REG_IY 5
#define z_REG_PC 6
#define z_REG_SP 7
#define z_REG_AFS 8
#define z_REG_BCS 9
#define z_REG_DES 10
#define z_REG_HLS 11
#define z_REG_I 12
#define z_REG_R 13
#define z_REG_A 100
#define z_REG_F 200
#define z_REG_B 101
#define z_REG_C 201
#define z_REG_D 102
#define z_REG_E 202
#define z_REG_H 103
#define z_REG_L 203
#define z_REG_AS 108
#define z_REG_FS 208
#define z_REG_BS 109
#define z_REG_CS 209
#define z_REG_DS 110
#define z_REG_ES 210
#define z_REG_HS 111
#define z_REG_LS 211
#define z_REG_IXH 105
#define z_REG_IXL 205
#define z_REG_IYH 106
#define z_REG_IYL 206
static void z_DefaultPortOutCallback(z_Machine* mm, unsigned char port, unsigned char value);
static unsigned char z_DefaultPortInCallback(z_Machine* mm, unsigned char port);
static unsigned short z_DefaultInterruptCallback(z_Machine* mm, unsigned char mode);
static int z_MemEqu(char* a, char* b, int lena, int lenb);
static int z_SignExt16(int val);
static z_InstructionInfo z_InstructionLookup(unsigned char opcode, unsigned char table);
//Creates a new z_Machine with the
//default values set.
extern void z_InitMachine(z_Machine* mm);
//Resets a z_Machine to its default values.
extern void z_Reset(z_Machine* mm);
//If the status of the machine is z_RUNNING and
//not z_IDLE, then this will execute a single
//instruction at the current memory address. If it
//encounters the HALT instruction, then the status
//will be changed to z_IDLE.
extern void z_Step(z_Machine* mm);
//Sets the status of the machine to z_RUNNING.
extern void z_Cont(z_Machine* mm);
//Sets the status of the machine to z_IDLE.
extern void z_Stop(z_Machine* mm);
//Jumps to a memory address (sets PC).
extern void z_Jump(z_Machine* mm, unsigned short addr);
//Begins executing instructions at the current
//program counter until the HALT instruction is
//reached. This is a blocking operation.
extern void z_Trace(z_Machine* mm);
//Jumps to a memory address and begins executing
//instructions until the HALT instruction is reached.
//This is a blocking operation.
extern void z_Run(z_Machine* mm, unsigned short addr);
//Set the value of a register.
//Use this rather than trying to access registers directly.
extern void z_SetReg(z_Machine* mm, unsigned char reg, unsigned short val);
//Gets the value of a register.
extern unsigned short z_GetReg(z_Machine* mm, unsigned char reg);
//If compiling Z80 code with the Small Device C compiler
//	and a port read/write callback is triggered
//	at the top of that function, then this can be
//	be used to get the parameters passed into that
//	function, assuming the parameters are all
//	2 bytes long (pointers/shorts/ints).
extern unsigned short z_GetParameter(z_Machine* mm, unsigned short num);
//If compiling Z80 code with the Small Device C compiler
//	and a port read/write callback is triggered
//	at the top of that function, then this can be
//	be used to set the return value of that
//	function, assuming the function returns a data
//	type that is 2 bytes long (pointers/shorts/ints).
extern void z_Return(z_Machine* mm, unsigned short num);
//opcode information
extern const unsigned char z_OPINFO[45785];
#endif
