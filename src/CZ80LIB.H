#ifndef __CZ80LIB_H__
#define __CZ80LIB_H__
typedef struct { //!show struct
	int T0, T1, ST0, ST1, INT_ENABLED, INT_MODE, TABLE, STATUS;
	int REGS[14];
	////!show struct The program memory is located here.
	unsigned char MEM[65536];
	////!show struct This callback is executed whenever
	////!show struct the interpreter encounters an OUT
	////!show struct instruction.
	void (*PortOutCallback)(unsigned char, unsigned char); //!show struct
	////!show struct This callback is executed whenever
	////!show struct the interpreter encounters an IN
	////!show struct instruction.
	unsigned char (*PortInCallback)(unsigned char); //!show struct
	////!show struct This callback is executed
	////!show struct repeatedly when interrupts are
	////!show struct enabled. The lower byte is 1 or 0
	////!show struct indicating whether or not the
	////!show struct interrupt should be fired. The
	////!show struct upper byte is used for Mode 2
	////!show struct interrupts as the value combined
	////!show struct combined with the I register.
	unsigned short (*InterruptCallback)(unsigned char); //!show struct
} CZ80LIB_Machine; //!show struct
//Instruction info
typedef struct {
	unsigned char OpCode, Table;
	unsigned char SimplifiedLength;
	unsigned char Simplified[256];
} CZ80LIB_InstructionInfo;
//Defines
#define CZ80LIB_IDLE 0
#define CZ80LIB_RUNNING 1
#define CZ80LIB_REG_AF 0
#define CZ80LIB_REG_BC 1
#define CZ80LIB_REG_DE 2
#define CZ80LIB_REG_HL 3
#define CZ80LIB_REG_IX 4
#define CZ80LIB_REG_IY 5
#define CZ80LIB_REG_PC 6
#define CZ80LIB_REG_SP 7
#define CZ80LIB_REG_AFS 8
#define CZ80LIB_REG_BCS 9
#define CZ80LIB_REG_DES 10
#define CZ80LIB_REG_HLS 11
#define CZ80LIB_REG_I 12
#define CZ80LIB_REG_R 13
#define CZ80LIB_REG_A 100
#define CZ80LIB_REG_F 200
#define CZ80LIB_REG_B 101
#define CZ80LIB_REG_C 201
#define CZ80LIB_REG_D 102
#define CZ80LIB_REG_E 202
#define CZ80LIB_REG_H 103
#define CZ80LIB_REG_L 203
#define CZ80LIB_REG_AS 108
#define CZ80LIB_REG_FS 208
#define CZ80LIB_REG_BS 109
#define CZ80LIB_REG_CS 209
#define CZ80LIB_REG_DS 110
#define CZ80LIB_REG_ES 210
#define CZ80LIB_REG_HS 111
#define CZ80LIB_REG_LS 211
#define CZ80LIB_REG_IXH 105
#define CZ80LIB_REG_IXL 205
#define CZ80LIB_REG_IYH 106
#define CZ80LIB_REG_IYL 206
static void CZ80LIB_DefaultPortOutCallback(unsigned char port, unsigned char value);
static unsigned char CZ80LIB_DefaultPortInCallback(unsigned char port);
static unsigned short CZ80LIB_DefaultInterruptCallback(unsigned char mode);
static int CZ80LIB_MemEqu(char* a, char* b, int lena, int lenb);
static int CZ80LIB_SignExt16(int val);
static CZ80LIB_InstructionInfo CZ80LIB_InstructionLookup(unsigned char opcode, unsigned char table);
//Creates a new CZ80LIB_Machine with the
//default values set.
extern CZ80LIB_Machine CZ80LIB_NewMachine();
//Resets a CZ80LIB_Machine to its default values.
extern void CZ80LIB_Reset(CZ80LIB_Machine* mm);
//If the status of the machine is CZ80LIB_RUNNING and
//not CZ80LIB_IDLE, then this will execute a single
//instruction at the current memory address. If it
//encounters the HALT instruction, then the status
//will be changed to CZ80LIB_IDLE.
extern void CZ80LIB_Step(CZ80LIB_Machine* mm);
//Sets the status of the machine to CZ80LIB_RUNNING.
extern void CZ80LIB_Cont(CZ80LIB_Machine* mm);
//Sets the status of the machine to CZ80LIB_IDLE.
extern void CZ80LIB_Stop(CZ80LIB_Machine* mm);
//Jumps to a memory address (sets PC).
extern void CZ80LIB_Jump(CZ80LIB_Machine* mm, unsigned short addr);
//Begins executing instructions at the current
//program counter until the HALT instruction is
//reached. This is a blocking operation.
extern void CZ80LIB_Trace(CZ80LIB_Machine* mm);
//Jumps to a memory address and begins executing
//instructions until the HALT instruction is reached.
//This is a blocking operation.
extern void CZ80LIB_Run(CZ80LIB_Machine* mm, unsigned short addr);
//Set the value of a register.
//Use this rather than trying to access registers directly.
extern void CZ80LIB_SetReg(CZ80LIB_Machine* mm, unsigned char reg, unsigned short val);
//Gets the value of a register.
extern unsigned short CZ80LIB_GetReg(CZ80LIB_Machine* mm, unsigned char reg);
//If compiling Z80 code with the Small Device C compiler
//	and a port read/write callback is triggered
//	at the top of that function, then this can be
//	be used to get the parameters passed into that
//	function, assuming the parameters are all
//	2 bytes long (pointers/shorts/ints).
extern unsigned short CZ80LIB_GetParameter(CZ80LIB_Machine* mm, unsigned short num);
//If compiling Z80 code with the Small Device C compiler
//	and a port read/write callback is triggered
//	at the top of that function, then this can be
//	be used to set the return value of that
//	function, assuming the function returns a data
//	type that is 2 bytes long (pointers/shorts/ints).
extern void CZ80LIB_Return(CZ80LIB_Machine* mm, unsigned short num);
//Includes
#include "CZ80LIB_OPCODES.C"
#include "CZ80LIB.C"
#endif
