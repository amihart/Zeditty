static void CZ80LIB_DefaultPortOutCallback(CZ80LIB_Machine* mm, unsigned char port, unsigned char value) {
#ifdef CZ80LIB_VERBOSE
	fprintf(stderr, "WARNING: An unhandled write of value %i to port %i occurred.\n", value, port);
#else
	(void)port;
	(void)value;
#endif
}
static unsigned char CZ80LIB_DefaultPortInCallback(CZ80LIB_Machine* mm, unsigned char port) {
#ifdef CZ80LIB_VERBOSE
	fprintf(stderr, "WARNING: An unhandled read from port %i occurred.\n", port);
#else
	(void)port;
#endif
	return 0;
}
static unsigned short CZ80LIB_DefaultInterruptCallback(CZ80LIB_Machine* mm, unsigned char mode) {
#ifdef CZ80LIB_VERBOSE
	fprintf(stderr, "WARNING: Interrupts are enabled but unhandled.\n");
#else
	(void)mode;
#endif
	return 0;
}
static int CZ80LIB_SignExt16(int val) {
	return (val >> 15) & 1 ? val | 0xFFFF0000 : val;
}
static int CZ80LIB_MemEqu(char *a, char *b, int lena, int lenb) {
	if (lena != lenb) {
		return 0;
	}
	int i;
	for (i = 0; i < lena; i++) {
		if (a[i] != b[i]) return 0;
	}
	return 1;
}
static CZ80LIB_InstructionInfo CZ80LIB_InstructionLookup(unsigned char opcode, unsigned char table) {
	unsigned int loc = 3 * (opcode + ((unsigned int)table) * 256);
	CZ80LIB_InstructionInfo ii;
	ii.OpCode = opcode;
	ii.Table = table;
	ii.SimplifiedLength = CZ80LIB_OPINFO[loc + 2];
	loc = (CZ80LIB_OPINFO[loc] << 8) | CZ80LIB_OPINFO[loc + 1];
	int i;
	for (i = 0; i < ii.SimplifiedLength; i++) {
		ii.Simplified[i] = CZ80LIB_OPINFO[loc + i];
	}
	return ii;
}
extern void CZ80LIB_InitMachine(CZ80LIB_Machine* mm) {
	int i;
	for (i = 0; i < 14; i++) {
		mm->REGS[i] = 0;
	}
	for (i = 0; i < 65536; i++) {
		mm->MEM[i] = 0;
	}
	mm->T0 = 0;
	mm->T1 = 0;
	mm->TABLE = 0;
	mm->INT_ENABLED = 0;
	mm->INT_MODE = 0;
	mm->PortOutCallback = CZ80LIB_DefaultPortOutCallback;
	mm->PortInCallback = CZ80LIB_DefaultPortInCallback;
	mm->InterruptCallback = CZ80LIB_DefaultInterruptCallback;
}
extern void CZ80LIB_Reset(CZ80LIB_Machine* mm) {
	int i;
	for (i = 0; i < 14; i++) {
		mm->REGS[i] = 0;
	}
	for (i = 0; i < 65536; i++) {
		mm->MEM[i] = 0;
	}
	mm->T0 = 0;
	mm->T1 = 0;
	mm->TABLE = 0;
	mm->INT_ENABLED = 0;
	mm->INT_MODE = 0;
	mm->PortOutCallback = CZ80LIB_DefaultPortOutCallback;
	mm->PortInCallback = CZ80LIB_DefaultPortInCallback;
	mm->InterruptCallback = CZ80LIB_DefaultInterruptCallback;
}
extern void CZ80LIB_Step(CZ80LIB_Machine *mm) {
	if (mm->STATUS == CZ80LIB_IDLE) return;
	const int REG_AF = 0;
	const int REG_PC = 6;
	const int REG_SP = 7;
	const int REG_I = 12;
	const int REG_R = 13;
	unsigned char instrop = mm->MEM[mm->REGS[REG_PC]];
	unsigned char instrtbl = mm->TABLE;
	CZ80LIB_InstructionInfo ii = CZ80LIB_InstructionLookup(instrop, instrtbl);
	int arg8 = mm->MEM[(mm->REGS[REG_PC] + 1) & 0xFFFF];
	int arg16 = mm->MEM[(mm->REGS[REG_PC] + 1) & 0xFFFF] | (mm->MEM[(mm->REGS[REG_PC] + 2) & 0xFFFF] << 8);
	int arg8i = mm->MEM[(mm->REGS[REG_PC] - 1) & 0xFFFF];
	int i;
	mm->TABLE = 0;

	for (i = 0; i < ii.SimplifiedLength; i++) {
		int op0 = ii.Simplified[i] >> 4;
		int op1 = ii.Simplified[i] & 0xF;
		int tmp;
		switch (op0) {
			case 0:
				mm->T0 = op1;
			break;
			case 1:
				if (op1 < 8) mm->T0 = mm->REGS[op1];
				else mm->REGS[op1 - 8] = mm->T0 & 0xFFFF;
			break;
			case 2:
				if (op1 < 8) mm->T0 = mm->REGS[op1 + 8];
				else if (op1 == 12) mm->REGS[op1 + 8 - 8] = mm->T0 & 0xFF;
				else if (op1 == 13) mm->REGS[op1 + 8 - 8] = mm->T0 & 0x7F;
				else mm->REGS[op1 + 8 - 8] = mm->T0 & 0xFFFF;
			break;
			case 3:
				if (op1 == 0) {
					tmp = mm->T0;
					mm->T0 = mm->T1;
					mm->T1 = tmp;
				} else if (op1 == 1) {
					mm->ST0 = mm->T0;
					mm->ST1 = mm->T1;
				} else if (op1 == 2) {
					mm->T0 = mm->ST0;
					mm->T1 = mm->ST1;
				} else if (op1 == 3) {
					mm->T0 += mm->T1;
				} else if (op1 == 4) {
					mm->T0 -= mm->T1;
				} else if (op1 == 5) {
					mm->T0 &= mm->T1;
				} else if (op1 == 6) {
					mm->T0 |= mm->T1;
				} else if (op1 == 7) {
					mm->T0 ^= mm->T1;
				} else if (op1 == 8) {
					mm->T0 = ~(mm->T0);
				} else if (op1 == 9) {
					mm->T0 = (mm->T0 >> 7) & 1 ? mm->T0 | 0xFFFFFF00 : mm->T0;
				} else if (op1 == 10) {
					tmp = 1;
					tmp ^= (mm->T0 >> 7) & 1;
					tmp ^= (mm->T0 >> 6) & 1;
					tmp ^= (mm->T0 >> 5) & 1;
					tmp ^= (mm->T0 >> 4) & 1;
					tmp ^= (mm->T0 >> 3) & 1;
					tmp ^= (mm->T0 >> 2) & 1;
					tmp ^= (mm->T0 >> 1) & 1;
					tmp ^= (mm->T0) & 1;
					mm->T0 = tmp;
				} else if (op1 == 11) {
					mm->T0 &= 1;
				} else if (op1 == 12) {
					mm->T0 &= 3;
				} else if (op1 == 13) {
					mm->T0 &= 0xF;
				} else if (op1 == 14) {
					mm->T0 &= 0xFF;
				} else if (op1 == 15) {
					mm->T0 &= 0xFFFF;
				}
			break;
			case 4:
				if (op1 == 0) {
					mm->T0 = mm->MEM[mm->T1 & 0xFFFF];
				} else if (op1 == 1) {
					mm->T0 = mm->MEM[mm->T1 & 0xFFFF] | (mm->MEM[(mm->T1 + 1) & 0xFFFF] << 8);
				} else if (op1 == 2) {
					mm->MEM[(mm->T1) & 0xFFFF] = mm->T0 & 0xFF;
				} else if (op1 == 3) {
					mm->MEM[(mm->T1) & 0xFFFF] = mm->T0 & 0xFF;
					mm->MEM[(mm->T1 + 1) & 0xFFFF] = (mm->T0 & 0xFF00) >> 8;
				}
			break;
			case 5:
				if (op1 == 0) {
					mm->T0 = mm->T0 == mm->T1 ? 1 : 0;
				} else if (op1 == 1) {
					mm->T0 = mm->T0 != mm->T1 ? 1 : 0;
				} else if (op1 == 2) {
					mm->T0 = mm->T0 > mm->T1 ? 1 : 0;
				} else if (op1 == 3) {
					mm->T0 = mm->T0 < mm->T1 ? 1 : 0;
				}
			break;
			case 6:
				i += mm->T0 != 0 ? op1 : 0;
			break;
			case 7:
				i += mm->T0 != 0 ? op1 + 16 : 0;
			break;
			case 8:
				mm->T0 <<= op1;
			break;
			case 9:
				mm->T0 >>= op1;
			break;
			case 10:
				mm->T0 += op1;
			break;
			case 11:
				if (op1 == 0) {
					mm->T0 = arg8;
				} else if (op1 == 1) {
					mm->T0 = arg16;
				} else if (op1 == 2) {
					mm->T0 = arg8i;
				} else if (op1 == 3) {
					mm->PortOutCallback(mm, mm->T1, mm->T0);
				} else if (op1 == 4) {
					mm->T0 = mm->PortInCallback(mm, mm->T1);
				}
			break;
			case 12:
				mm->TABLE = op1;
			break;
			case 13:
				if (op1 == 0) {
					mm->T0 = 0;
				} else if (op1 == 1) {
					mm->T0 &= 0x00FF;
				} else if (op1 == 2) {
					mm->T0 &= 0xFF00;
				} else if (op1 == 3) {
					#ifdef CZ80LIB_VERBOSE
					fprintf(stderr, "An unimplemented instruction was used (OP: %X; TBL: %i).\n", instrop, instrtbl);
					#endif
				} else if (op1 == 4) {
					#ifdef CZ80LIB_VERBOSE
					fprintf(stderr, "An invalid instruction was used (OP: %X, TBL: %i).\n", instrop, instrtbl);
					#endif
				} else if (op1 == 5) {
					mm->STATUS = 0;
				}
			break;
			case 14:
				if (op1 < 3) {
					mm->INT_MODE = op1;
				} else if (op1 == 3) {
					mm->INT_ENABLED = 0;
				} else if (op1 == 4) {
					mm->INT_ENABLED = 1;
				}
			break;
			case 15:
				tmp = (~(1 << op1)) & 0xFFFF;
				mm->REGS[REG_AF] &= tmp;
				tmp = (mm->T0 & 1) << op1;
				mm->REGS[REG_AF] |= tmp;
			break;
		}
	}
	if (mm->INT_ENABLED && mm->INT_MODE > 0) {
		unsigned char fire, ival;
		unsigned short iret = mm->InterruptCallback(mm, mm->INT_MODE);
		fire = iret & 0xFF;
		ival = iret >> 8;
		if (fire) {
			mm->REGS[REG_SP] -= 2;
			mm->MEM[mm->REGS[REG_SP]] = mm->REGS[REG_PC] & 0xFF;
			mm->MEM[mm->REGS[REG_SP] + 1] = mm->REGS[REG_PC] >> 8;
			if (mm->INT_MODE == 1) {
				mm->REGS[REG_PC] = 0x0038;
			} else {
				unsigned short iaddr = (mm->REGS[REG_I] << 8) | ival;
				mm->REGS[REG_PC] = mm->MEM[iaddr] | (mm->MEM[iaddr + 1] << 8);
			}
		}
	}
}
extern void CZ80LIB_Cont(CZ80LIB_Machine *mm) {
	mm->STATUS = CZ80LIB_RUNNING;
}
extern void CZ80LIB_Stop(CZ80LIB_Machine *mm) {
	mm->STATUS = CZ80LIB_IDLE;
}
extern void CZ80LIB_Jump(CZ80LIB_Machine *mm, unsigned short addr) {
	mm->REGS[CZ80LIB_REG_PC] = addr;
}
extern void CZ80LIB_Trace(CZ80LIB_Machine *mm) {
	CZ80LIB_Cont(mm);
	while (mm->STATUS == CZ80LIB_RUNNING) {
		CZ80LIB_Step(mm);
	}
}
extern void CZ80LIB_Run(CZ80LIB_Machine *mm, unsigned short addr) {
	CZ80LIB_Jump(mm, addr);
	CZ80LIB_Trace(mm);
}
extern void CZ80LIB_SetReg(CZ80LIB_Machine* mm, unsigned char reg, unsigned short val) {
	switch (reg) {
		case CZ80LIB_REG_AF:
		case CZ80LIB_REG_BC:
		case CZ80LIB_REG_DE:
		case CZ80LIB_REG_HL:
		case CZ80LIB_REG_IX:
		case CZ80LIB_REG_IY:
		case CZ80LIB_REG_PC:
		case CZ80LIB_REG_SP:
		case CZ80LIB_REG_AFS:
		case CZ80LIB_REG_BCS:
		case CZ80LIB_REG_DES:
		case CZ80LIB_REG_HLS:
			mm->REGS[reg] = val;
		break;
		case CZ80LIB_REG_A:
		case CZ80LIB_REG_B:
		case CZ80LIB_REG_D:
		case CZ80LIB_REG_H:
		case CZ80LIB_REG_IXH:
		case CZ80LIB_REG_IYH:
		case CZ80LIB_REG_AS:
		case CZ80LIB_REG_BS:
		case CZ80LIB_REG_DS:
		case CZ80LIB_REG_HS:
			mm->REGS[reg - 100] = (mm->REGS[reg - 100] & 0xFF) | ((val & 0xFF) << 8);
		break;
		case CZ80LIB_REG_F:
		case CZ80LIB_REG_C:
		case CZ80LIB_REG_E:
		case CZ80LIB_REG_L:
		case CZ80LIB_REG_IXL:
		case CZ80LIB_REG_IYL:
		case CZ80LIB_REG_FS:
		case CZ80LIB_REG_CS:
		case CZ80LIB_REG_ES:
		case CZ80LIB_REG_LS:
			mm->REGS[reg - 200] = (mm->REGS[reg - 200] & 0xFF00) | (val & 0xFF);
		break;
		case CZ80LIB_REG_I:
			mm->REGS[reg] = val & 0xFF;
		break;
		case CZ80LIB_REG_R:
			mm->REGS[reg] = val & 0x7F;
		break;
	}
}
extern unsigned short CZ80LIB_GetReg(CZ80LIB_Machine* mm, unsigned char reg) {
	switch (reg) {
		case CZ80LIB_REG_AF:
		case CZ80LIB_REG_BC:
		case CZ80LIB_REG_DE:
		case CZ80LIB_REG_HL:
		case CZ80LIB_REG_IX:
		case CZ80LIB_REG_IY:
		case CZ80LIB_REG_PC:
		case CZ80LIB_REG_SP:
		case CZ80LIB_REG_AFS:
		case CZ80LIB_REG_BCS:
		case CZ80LIB_REG_DES:
		case CZ80LIB_REG_HLS:
		case CZ80LIB_REG_I:
		case CZ80LIB_REG_R:
			return mm->REGS[reg];
		break;
		case CZ80LIB_REG_A:
		case CZ80LIB_REG_B:
		case CZ80LIB_REG_D:
		case CZ80LIB_REG_H:
		case CZ80LIB_REG_IXH:
		case CZ80LIB_REG_IYH:
		case CZ80LIB_REG_AS:
		case CZ80LIB_REG_BS:
		case CZ80LIB_REG_DS:
		case CZ80LIB_REG_HS:
			return mm->REGS[reg - 100] >> 8;
		break;
		case CZ80LIB_REG_F:
		case CZ80LIB_REG_C:
		case CZ80LIB_REG_E:
		case CZ80LIB_REG_L:
		case CZ80LIB_REG_IXL:
		case CZ80LIB_REG_IYL:
		case CZ80LIB_REG_FS:
		case CZ80LIB_REG_CS:
		case CZ80LIB_REG_ES:
		case CZ80LIB_REG_LS:
			return mm->REGS[reg - 200] & 0xFF;
		break;
	}
	return 0;
}
extern unsigned short CZ80LIB_GetParameter(CZ80LIB_Machine* mm, unsigned short num) {
	unsigned short addr = (mm->REGS[CZ80LIB_REG_SP]) + (num * 2) + 2;
	unsigned short ret = mm->MEM[addr++];
	ret |= mm->MEM[addr] << 8;
	return ret;
}
extern void CZ80LIB_Return(CZ80LIB_Machine* mm, unsigned short num) {
	mm->REGS[CZ80LIB_REG_HL] = num;
}
