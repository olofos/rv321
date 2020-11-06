OUTPUT_ARCH( "riscv" )
ENTRY(_start)

MEMORY
{
RAM (xrw)      : ORIGIN = 0x00000000, LENGTH = 64K
RAM2 (xrw)      : ORIGIN = 0x80000000, LENGTH = 64K
}


SECTIONS
{
  . = 0x00000000;
  .text.init : { *(.text.init) }
  . = ALIGN(0x1000);
  .text : { *(.text) *(.text.*) }
  . = ALIGN(0x1000);
  .data : { *(.data) }
  .bss : { *(.bss) }
  . = ALIGN(0x1000);
  .rodata : { *(.rodata) }
  _end = .;
}

