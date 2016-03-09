OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")

ENTRY(_start)

SECTIONS
{
  . = 0x1FFF0000;
  .ro : {
    start.o (.text)
    *(.text)
    *(.rodata)
  }
  .rw : {
    *(.data)
    *(.bss)
    *(COMMON)
  }
  . = ALIGN(8);
  . = . + 0x1000; /* 4kB of stack memory */
  stack_top = .;
}
