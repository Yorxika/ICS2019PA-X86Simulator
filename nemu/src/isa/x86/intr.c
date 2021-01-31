#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  rtl_push(&cpu.eflags.val);
  cpu.eflags.IF = 0; // close intr
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  
  uint32_t low = vaddr_read(cpu.IDTR.base + NO * 8, 4) & 0x0000ffff;
  uint32_t high = vaddr_read(cpu.IDTR.base + NO * 8 + 4, 4) & 0xffff0000;
  rtl_j(high | low);
}

bool isa_query_intr(void) {
  if (cpu.INTR && cpu.eflags.IF) {
    cpu.INTR = false;
    
  }
  return false;
}
