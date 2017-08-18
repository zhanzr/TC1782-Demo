/*
 * TriBoard-TC1782 memory description
 *   (using only internal RAM for code and data)
 *
 * Copyright (C) 1982-2014 HighTec EDV-Systeme GmbH.
 */

OUTPUT_FORMAT("elf32-tricore")
OUTPUT_ARCH(tricore)
ENTRY(_start)

/* __TC1782__ with Core TC1.3.1 */
__TRICORE_DERIVATE_MEMORY_MAP__ = 0x1782;
/* the internal FLASH description */
__EXT_CODE_RAM_BEGIN = 0x80000000;
__EXT_CODE_RAM_SIZE = 2560K;
/* the internal ram description */
__INT_CODE_RAM_BEGIN = 0xd4000000;
__INT_CODE_RAM_SIZE = 24K;
__INT_DATA_RAM_BEGIN = 0xd0000000;
__INT_DATA_RAM_SIZE = 124K;
/* the pcp memory description */
__PCP_CODE_RAM_BEGIN = 0xf0060000;
__PCP_CODE_RAM_SIZE = 32K;
__PCP_DATA_RAM_BEGIN = 0xf0050000;
__PCP_DATA_RAM_SIZE = 16K;

/*
 * used to check for HEAP_SIZE
 */
__RAM_END = __INT_DATA_RAM_BEGIN + __INT_DATA_RAM_SIZE;

MEMORY
{
  int_cram (rx!p): org = 0xd4000000, len = 24K
  int_dram (w!xp): org = 0xd0000000, len = 124K
  pcp_data (wp!x): org = 0xf0050000, len = 16K
  pcp_text (rxp):  org = 0xf0060000, len = 32K
}

/* Use alias for memory regions */
REGION_ALIAS("DATA_MEM", int_dram)
REGION_ALIAS("CODE_MEM", int_cram)
REGION_ALIAS("ICODE_MEM", int_cram)
REGION_ALIAS("SDATA_MEM", int_dram)
REGION_ALIAS("BSS_MEM", int_dram)
REGION_ALIAS("ZDATA_MEM", int_dram)
REGION_ALIAS("CSA_MEM", int_dram)
REGION_ALIAS("PCP_CODE", pcp_text)
REGION_ALIAS("PCP_DATA", pcp_data)


/*
 * Define the sizes of the user and system stacks.
 */
__ISTACK_SIZE = DEFINED (__ISTACK_SIZE) ? __ISTACK_SIZE : 1K;
__USTACK_SIZE = DEFINED (__USTACK_SIZE) ? __USTACK_SIZE : 1K;

/*
 * The heap is the memory between the top of the user stack and
 * __RAM_END (as defined above); programs can dynamically allocate
 * space in this area using malloc() and various other functions.
 * Below you can define the minimum amount of memory that the heap
 * should provide.
 */
__HEAP_MIN = DEFINED (__HEAP_MIN) ? __HEAP_MIN : 4K;

/*
 * Define the size of the context save area.
 */
__CSA_SIZE = DEFINED (__CSA_SIZE) ? __CSA_SIZE : 8K;

/* Include section description */
INCLUDE targetSections.lsf
