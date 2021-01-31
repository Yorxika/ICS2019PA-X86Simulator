#include "cpu/exec.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr); 

make_EHelper(lidt) {
  //If a 32-bit operand is used, a 16-bit limit and a 32-bit base is loaded; 
  //the high-order eight bits of the six-byte operand are used as high-order base address bits.
  cpu.IDTR.limit = vaddr_read(id_dest -> addr, 2);
  cpu.IDTR.base = vaddr_read(id_dest -> addr + 2, 4);

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  raise_intr(id_dest -> val, decinfo.seq_pc);

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  rtl_pop(&decinfo.jmp_pc);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags.val);
  
  rtl_j(decinfo.jmp_pc);

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
//see https://www.fermimn.edu.it/linux/quarta/x86/in.htm

  //printf("in src: %x dest: %x\n",id_src -> val, id_dest -> val);

  switch (id_dest -> width) {
    case 1:
      s0 = pio_read_b(id_src -> val);
      break;
    case 2:
      s0 = pio_read_w(id_src -> val);
      break;
    case 4:
      s0 = pio_read_l(id_src -> val);
      break;
    default:
      assert(0);
  }

  operand_write(id_dest, &s0);

  print_asm_template2(in);
}

make_EHelper(out) {
//see https://www.fermimn.edu.it/linux/quarta/x86/out.htm

  //printf("out width : %d ,src: %x dest: %x\n",id_src -> width, id_src -> val, id_dest -> val);

  switch (id_src -> width) {
    case 1:
      pio_write_b(id_dest -> val, id_src -> val);
      break;
    case 2:
      pio_write_w(id_dest -> val, id_src -> val);
      break;
    case 4:
       pio_write_l(id_dest -> val, id_src -> val);
      break;
    default:
      assert(0);
  }

  print_asm_template2(out);
}
