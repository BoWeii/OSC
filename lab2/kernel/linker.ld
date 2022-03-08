SECTIONS
{
  . = 0x80000;
  .text.boot : { *(.text.boot) }
  .text :  { *(.text) }
  . = ALIGN(0x1000);
  .rodata : { *(.rodata) }
  .data : { *(.data) }
  . = ALIGN(0x1000);
  bss_begin = .;
  .bss : { *(.bss*) } 
  bss_end = .;  
  . = ALIGN(0x1000);
}
