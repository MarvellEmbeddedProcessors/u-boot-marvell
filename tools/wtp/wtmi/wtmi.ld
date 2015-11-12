SECTIONS
{
  . = 0x200040F0;
  .ro : {
    wtmi.o (.text)
    *(.text)
    *(.rodata)
  }
  .rw : {
    *(.data)
    *(.bss)
    *(COMMON)
  }
 . = ALIGN(8);
}
