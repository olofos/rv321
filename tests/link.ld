OUTPUT_ARCH( "riscv" )
ENTRY(_start)

MEMORY
{
ROM (xrw)      : ORIGIN = 0x00000000, LENGTH = 64K
RAM (xrw)      : ORIGIN = 0x80000000, LENGTH = 64K
}


SECTIONS
{
  . = 0x00000000;
  .text.init : { *(.text.init) } > ROM
  . = ALIGN(16);
  .text : { *(.text) *(.text.*) } > ROM
  . = ALIGN(16);
  .data : { *(.data) } > ROM
  _edata = .;
  . = ALIGN(16);
  .rodata : { *(.rodata) } > ROM
  _end = .;
  /* . = 0x80000000; */
  .bss : { *(.bss) } > RAM
}
