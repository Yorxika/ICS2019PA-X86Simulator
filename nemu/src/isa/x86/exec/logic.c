#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_and(&s0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&s0, id_dest->width);
  s1 = 0;
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&s0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&s0, id_dest -> width);
  s1 = 0;
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);
  
  operand_write(id_dest, &s0);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&s0, &id_dest -> val, &id_src -> val);
  rtl_update_ZFSF(&s0, id_dest -> width);
  s1 = 0;
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);

  operand_write(id_dest, &s0);

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&s0, &id_dest -> val, &id_src -> val);
  rtl_update_ZFSF(&s0, id_dest -> width);
  s1 = 0;
  rtl_set_CF(&s1);
  rtl_set_OF(&s1);

  operand_write(id_dest, &s0);

  print_asm_template2(or);
}

make_EHelper(sar) {
  // unnecessary to update CF and OF in NEMU
  //rtl_sar(&s1, &id_dest -> val, &id_src -> val);
  uint8_t width = decinfo.opcode && 0xff;
  if (width == 0xd0 || width == 0xd2 || width == 0xc0) {
    s0 = 1;
  } else {
    s0 = decinfo.isa.is_operand_size_16 ? 2 : 4;
  }

  rtl_my_sar(&s1, &id_dest -> val, id_src -> val, s0);
  //printf("sar src: %x, times: %x, ans: %x\n", id_dest -> val, id_src -> val, s1);
  rtl_update_ZFSF(&s1, id_dest -> width);

  operand_write(id_dest, &s1);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&s0, &id_dest -> val, &id_src -> val);
  rtl_update_ZFSF(&s0, id_dest -> width);  

  operand_write(id_dest, &s0);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&s0, &id_dest -> val, &id_src -> val);
  rtl_update_ZFSF(&s0, id_dest -> width);  

  operand_write(id_dest, &s0);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_not(&s0, &id_dest -> val);
  operand_write(id_dest, &s0);

  print_asm_template1(not);
}

make_EHelper(rol) {
  //see https://www.fermimn.edu.it/linux/quarta/x86/rcl.htm
  uint8_t width = decinfo.opcode && 0xff;
  if (width == 0xd0 || width == 0xd2 || width == 0xc0) {
    s0 = 1;
  } else {
    s0 = decinfo.isa.is_operand_size_16 ? 2 : 4;
  }

  rtl_rol(&s1, &id_dest -> val, id_src -> val, s0);

  if (id_src -> val == 1) {
    t0 = (cpu.eflags.CF != (s1 >> (s0 * 8 - 1)));
    rtl_set_OF(&t0);
  }

  operand_write(id_dest, &s1);
  print_asm_template1(not);
}

make_EHelper(rcl) {
  TODO();

  print_asm_template1(not);
}

make_EHelper(ror) {
  TODO();

  print_asm_template1(not);
}

make_EHelper(rcr) {
  TODO();

  print_asm_template1(not);
}

