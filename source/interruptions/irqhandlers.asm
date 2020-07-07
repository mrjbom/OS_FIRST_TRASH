global page_fault
global general_protection_fault
global invalid_opcode
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15
global load_idt

global invalid_opcode_handler
global page_fault_handler
global general_protection_fault_handler
global irq0_handler
global irq1_handler
global irq2_handler
global irq3_handler
global irq4_handler
global irq5_handler
global irq6_handler
global irq7_handler
global irq8_handler
global irq9_handler
global irq10_handler
global irq11_handler
global irq12_handler
global irq13_handler
global irq14_handler
global irq15_handler

extern invalid_opcode_handler
extern page_fault_handler
extern general_protection_fault_handler
extern irq0_handler
extern irq1_handler
extern irq2_handler
extern irq3_handler
extern irq4_handler
extern irq5_handler
extern irq6_handler
extern irq7_handler
extern irq8_handler
extern irq9_handler
extern irq10_handler
extern irq11_handler
extern irq12_handler
extern irq13_handler
extern irq14_handler
extern irq15_handler

invalid_opcode:
  cli
  ;save all 32bit registers
  pushad
  push dword [esp + 32] ;push eip
  push word [esp + 48] ;push cs
  call invalid_opcode_handler
  pop dword [esp + 48] ;pop cs
  pop word [esp + 32] ;pop eip
  ;return all 32bit registers
  popad
  sti
  iretd

general_protection_fault:
  cli
  ;save all 32bit registers
  pushad
  ;pass error code in function argument
  push dword [esp + 32]
  call general_protection_fault_handler
  pop dword [esp + 32]
  ;return all 32bit registers
  popad
  ;delete error code from stack
  add esp, 4
  sti
  iretd

page_fault:
  cli
  ;save all 32bit registers
  pushad
  ;pass error code in function argument
  push dword [esp + 32]
  call page_fault_handler
  pop dword [esp + 32]
  ;return all 32bit registers
  popad
  ;delete error code from stack
  add esp, 4
  sti
  iretd

irq0:
  cli
  pushad
  call irq0_handler
  popad
  sti
  iretd
 
irq1:
  cli
  pushad
  call irq1_handler
  popad
  sti
  iretd
 
irq2:
  cli
  pushad
  call irq2_handler
  popad
  sti
  iretd
 
irq3:
  cli
  pushad
  call irq3_handler
  popad
  sti
  iretd
 
irq4:
  cli
  pushad
  call irq4_handler
  popad
  sti
  iretd
 
irq5:
  cli
  pushad
  call irq5_handler
  popad
  sti
  iretd
 
irq6:
  cli
  pushad
  call irq6_handler
  popad
  sti
  iretd
 
irq7:
  cli
  pushad
  call irq7_handler
  popad
  sti
  iretd
 
irq8:
  cli
  pushad
  call irq8_handler
  popad
  sti
  iretd
 
irq9:
  cli
  pushad
  call irq9_handler
  popad
  sti
  iretd
 
irq10:
  cli
  pushad
  call irq10_handler
  popad
  sti
  iretd
 
irq11:
  cli
  pushad
  call irq11_handler
  popad
  sti
  iretd
 
irq12:
  cli
  pushad
  call irq12_handler
  popad
  sti
  iretd
 
irq13:
  cli
  pushad
  call irq13_handler
  popad
  sti
  iretd
 
irq14:
  cli
  pushad
  call irq14_handler
  popad
  sti
  iretd
 
irq15:
  cli
  pushad
  call irq15_handler
  popad
  sti
  iretd
 
load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti
	ret