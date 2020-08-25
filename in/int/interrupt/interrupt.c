#include"interrupt.h" 
#include<int/int.h>
#include<mm/pro/process.h>
//#include<dev/char/tty.h>

void _divide_error();
void _debug();
void _nmi();
void _break_point();
void _overflow();
void _bondre();
void _invalid_opcode();
void _device_not_avaliable();
void _double_fault();
void _cso();
void _invalid_tss();
void _segment_not_present();
void _ss_fault();
void _general_protection();
void _page_fault();
void _reserved();
void _floating_point_error();
void _alignment_check();
void _machine_check();
void _sse();
void sys_call();

/*声明系统调用模块*/
void sys_fork();
void s_str_pro_2();
void s_str_pro_3();
void print_pcb_info();

unsigned int sys_call_table[]={(unsigned int)(&sys_fork),(unsigned int)(&s_str_pro_2),(unsigned int)(&s_str_pro_3),(unsigned int)(&print_pcb_info),(unsigned int)(&test_snack)};

void init_int()
{
  int i;
  set_trap_gate(0,_divide_error);
  set_trap_gate(1,&_debug);
  set_trap_gate(2,&_nmi);
  set_system_gate(3,&_break_point);
  set_system_gate(4,&_overflow);
  set_system_gate(5,&_bondre);
  set_trap_gate(6,&_invalid_opcode);
  set_trap_gate(7,&_device_not_avaliable);
  set_trap_gate(8,&_double_fault);
  set_trap_gate(9,&_cso);
  set_trap_gate(10,&_invalid_tss);
  set_trap_gate(11,&_segment_not_present);
  set_trap_gate(12,&_ss_fault);
  set_trap_gate(13,&_general_protection);
  set_trap_gate(14,&_page_fault);
  set_trap_gate(15,&_reserved);
  set_trap_gate(16,&_floating_point_error);
  set_trap_gate(17,&_alignment_check);
  set_trap_gate(18,&_machine_check);
  set_trap_gate(19,&_sse);
  for(i=20;i<32;i++){
    set_trap_gate(i,&_reserved);
  }
  /*设置系统调用*/
  set_system_gate(0x80,&sys_call);
}
