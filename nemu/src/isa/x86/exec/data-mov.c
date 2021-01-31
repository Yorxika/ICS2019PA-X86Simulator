#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_dest->val);
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO: IF OperandSize = 16 (* PUSHA instruction *)
  s0 = reg_l(R_ESP);
  rtl_push(&reg_l(R_EAX));
  rtl_push(&reg_l(R_ECX));
  rtl_push(&reg_l(R_EDX));	
  rtl_push(&reg_l(R_EBX));
  rtl_push(&s0);
  rtl_push(&reg_l(R_EBP));
  rtl_push(&reg_l(R_ESI));
  rtl_push(&reg_l(R_EDI));
  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&reg_l(R_EDI));
  rtl_pop(&reg_l(R_ESI));
  rtl_pop(&reg_l(R_EBP));
  rtl_pop(&s0);  //(* Skip ESP *)
  rtl_pop(&reg_l(R_EBX));
  rtl_pop(&reg_l(R_EDX));
  rtl_pop(&reg_l(R_ECX));
  rtl_pop(&reg_l(R_EAX));

  print_asm("popa");
}

make_EHelper(leave) {
  reg_l(R_ESP) = reg_l(R_EBP);
  rtl_pop(&reg_l(R_EBP));
  print_asm("leave");
}

make_EHelper(cltd) {
//see https://www.fermimn.edu.it/linux/quarta/x86/cwd.htm
  if (decinfo.isa.is_operand_size_16) {
    rtl_lr(&s0, R_AX, 2);
    s1 = s0 < 0 ? 0xFFFF : 0;
    rtl_sr(R_DX, &s1, 2);
  }
  else {
    rtl_lr(&s0, R_EAX, 4);
    s1 = s0 < 0 ? 0xFFFFFFFF : 0;
    rtl_sr(R_EDX, &s1, 4);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
//see https://www.fermimn.edu.it/linux/quarta/x86/cbw.htm
  if (decinfo.isa.is_operand_size_16) {
    rtl_lr(&s0, R_AL, 1);
    rtl_sext(&s0, &s0, 1);
    rtl_sr(R_AX, &s0, 1);
  }
  else {
    rtl_lr(&s0, R_AX, 2);
    rtl_sext(&s0, &s0, 2);
    rtl_sr(R_EAX, &s0, 4);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  //see https://www.fermimn.edu.it/linux/quarta/x86/movzx.htm
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  id_src -> width = (decinfo.opcode & 0xff) == 0xb6 ? 1 : 2;
  switch (id_src -> width) {
    case 1:
      s0 = id_src -> val & 0xff;
      break;
    case 2:
      s0 = id_src -> val & 0xffff;
      break;
    default:
      s0 = id_src -> val;
  }
  operand_write(id_dest, &s0);
  //operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}

make_EHelper(movsb) {
  //refer https://www.fermimn.edu.it/linux/quarta/x86/movs.htm

  int incdec = cpu.eflags.DF ? -1 : 1; 
  s0 = reg_l(R_ESI);
  rtl_lm(&s1, &s0, 1);
  s0 += incdec;
  reg_l(R_ESI) = s0;
  s0 = reg_l(R_EDI);
  rtl_sm(&s0, &s1, 1);
  s0 += incdec;
  reg_l(R_EDI) = s0;
  
  print_asm("movsb");
}

make_EHelper(movsl) {
  int incdec = cpu.eflags.DF ? -4 : 4;
  s0 = reg_l(R_ESI);
  rtl_lm(&s1, &s0, 4);
  s0 += incdec;
  reg_l(R_ESI) = s0;
  s0 = reg_l(R_EDI);
  rtl_sm(&s0, &s1, 4);
  s0 += incdec;
  reg_l(R_EDI) = s0;
  
  print_asm("movsl");
}
