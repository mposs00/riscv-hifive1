OUTPUT_ARCH("riscv")
 
ENTRY(_start)
 
MEMORY
{
  flash (rxai!w) : ORIGIN = 0x20400000, LENGTH = 512M
  ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 16K
}
 
PHDRS
{
  flash PT_LOAD;
  ram_init PT_LOAD;
  ram PT_NULL;
}
 
SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 2K;
  PROVIDE(__stack_top = ORIGIN(ram) + LENGTH(ram));
 
  .init           :
  {
    KEEP (*(SORT_NONE(.init)))
  } >flash AT>flash :flash
 
  .text           :
  {
    *(.text .text.*)
  } >flash AT>flash :flash
 
  .fini           :
  {
    KEEP (*(SORT_NONE(.fini)))
  } >flash AT>flash :flash
 
  .rodata         :
  {
    *(.rdata)
    *(.rodata .rodata.*)
  } >flash AT>flash :flash
 
  . = ALIGN(4);
 
  .data          :
  {
    *(.data .data.*)
    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
  } >ram AT>flash :ram_init
 
  . = ALIGN(4);
  PROVIDE( _edata = . );
  PROVIDE( edata = . );
 
  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );
  .bss            :
  {
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(4);
  } >ram AT>ram :ram
 
  . = ALIGN(8);
  PROVIDE( _end = . );
  PROVIDE( end = . );
 
  .stack ORIGIN(ram) + LENGTH(ram) - __stack_size :
  {
    PROVIDE( _heap_end = . );
    . = __stack_size;
    PROVIDE( _sp = . );
  } >ram AT>ram :ram
}