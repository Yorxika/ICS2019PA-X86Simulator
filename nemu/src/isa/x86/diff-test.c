#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {

  Assert(ref_r -> eax == reg_l(R_EAX), "eax should be 0x%x but 0x%x at pc = 0x%x\n", ref_r -> eax, reg_l(R_EAX), cpu.pc);
  Assert(ref_r -> ecx == reg_l(R_ECX), "ecx should be 0x%x but 0x%x at pc = 0x%x\n", ref_r -> ecx, reg_l(R_ECX), cpu.pc);
  Assert(ref_r -> edx == reg_l(R_EDX), "edx should be 0x%x but 0x%x at pc = 0x%x\n", ref_r -> edx, reg_l(R_EDX), cpu.pc);
  Assert(ref_r -> ebx == reg_l(R_EBX), "ebx should be 0x%x but 0x%x at pc = 0x%x\n", ref_r -> ebx, reg_l(R_EBX), cpu.pc);
  Assert(ref_r -> esp == reg_l(R_ESP), "esp should be 0x%x but 0x%x at pc = 0x%x\n", ref_r -> esp, reg_l(R_ESP), cpu.pc);
  Assert(ref_r -> ebp == reg_l(R_EBP), "ebp should be 0x%x but 0x%x at pc = 0x%x\n", ref_r -> ebp, reg_l(R_EBP), cpu.pc);
  Assert(ref_r -> esi == reg_l(R_ESI), "esi should be 0x%x but 0x%x at pc = 0x%x\n", ref_r -> esi, reg_l(R_ESI), cpu.pc);
  Assert(ref_r -> edi == reg_l(R_EDI), "edi should be 0x%x but 0x%x at pc = 0x%x\n", ref_r -> edi, reg_l(R_EDI), cpu.pc);
  Assert(ref_r -> pc == cpu.pc, "eip should be 0x%x but 0x%x \n", ref_r -> pc, cpu.pc);

	// if (ref_r -> eax != reg_l(R_EAX) || 
  // ref_r -> ecx != reg_l(R_ECX) || 
  // ref_r -> edx != reg_l(R_EDX) || 
	// ref_r -> ebx != reg_l(R_EBX) || 
  // ref_r -> esp != reg_l(R_ESP) || 
  // ref_r -> ebp != reg_l(R_EBP) ||
	// ref_r -> esi != reg_l(R_ESI) ||
  // ref_r -> edi != reg_l(R_EDI) || 
  // ref_r -> pc != cpu.pc ) {
	// 	return false;
	// }

	return true;
}

void isa_difftest_attach(void) {
}
