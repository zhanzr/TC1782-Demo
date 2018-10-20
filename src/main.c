#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "led.h"
#include "cpufreq.h"

/* TriCore specific includes. */
#include <tc1782.h>
#include <machine/intrinsics.h>
#include <machine/cint.h>
#include <machine/wdtcon.h>

#include "dhry.h"

#define RUN_NUMBER	1000000

extern void __CSA_BEGIN(void);
extern void __CSA(void);
extern void __CSA_SIZE(void);
extern void __CSA_END(void);

#define	CSA_ITEM_SIZE	64

extern void __PMI_SPRAM_BEGIN(void);
extern void __PMI_SPRAM_SIZE(void);
extern void __DMI_LDRAM_BEGIN(void);
extern void __DMI_LDRAM_SIZE(void);

#define	direct_read(a)	(*(uint32_t*)a)

void enable_performance_cnt(void)
{
	unlock_wdtcon();
	{
		//Call Depth Counting
		PSW_t tmpPSW;
		tmpPSW.reg = _mfcr(PSW_ADDR);
		tmpPSW.bits.CDE = 1;
		tmpPSW.bits.CDC = 0x00;
		_dsync();
		_mtcr(PSW_ADDR, tmpPSW.reg);
		_isync();
	}
	lock_wdtcon();

	unlock_wdtcon();
	{
		CCTRL_t tmpCCTRL;
		tmpCCTRL.reg = _mfcr(CCTRL_ADDR);

		//Instruction Cache Hit Count
		//		tmpCCTRL.bits.M1 = 2;
		//Data Cache Hit Count.
		tmpCCTRL.bits.M1 = 3;
		//Instruction Cache Miss Count

		//		tmpCCTRL.bits.M2 = 1;
		//Data Cache Clean Miss Count
		tmpCCTRL.bits.M2 = 3;

		//Data Cache Dirty Miss Count
		tmpCCTRL.bits.M3 = 3;

		//Normal Mode
		tmpCCTRL.bits.CM = 0;
		//Task Mode
		//		tmpCCTRL.bits.CM = 1;

		tmpCCTRL.bits.CE = 1;

		_dsync();
		_mtcr(CCTRL_ADDR, tmpCCTRL.reg);
		_isync();
	}
	lock_wdtcon();
}

void print_csa_reg(void)
{
	volatile uint32_t tmp_u32;
	//	tmp_u32 = _mfcr(CPU_ID_ADDR);
	//	printf("CPUID\t%08X\t:%08X\n\n", &CPU_ID, tmp_u32);

	tmp_u32 = _mfcr(PSW_ADDR);
	printf("PSW_ADDR\t%08X\t:%08X\n", PSW_ADDR, tmp_u32);
	tmp_u32 = _mfcr(SYSCON_ADDR);
	printf("SYSCON\t%08X\t:%08X\n", SYSCON_ADDR, tmp_u32);
	tmp_u32 = _mfcr(COMPAT_ADDR);
	printf("COMPAT\t%08X\t:%08X\n", COMPAT_ADDR, tmp_u32);

	//	tmp_u32 = _mfcr(FCX_ADDR);
	//	printf("FCX\t%08X\t:%08X %08X\n", FCX_ADDR, tmp_u32, portCSA_TO_ADDRESS(tmp_u32));
	//	tmp_u32 = _mfcr(LCX_ADDR);
	//	printf("LCX\t%08X\t:%08X %08X\n", LCX_ADDR, tmp_u32, portCSA_TO_ADDRESS(tmp_u32));
	//	tmp_u32 = _mfcr(PCXI_ADDR);
	//	printf("PCXI\t%08X\t:%08X %08X\n", PCXI_ADDR, tmp_u32, portCSA_TO_ADDRESS(tmp_u32));

	tmp_u32 = (uint32_t)__CSA_SIZE >> 6;
	printf("Total CSA Number:%u\n", tmp_u32);

	//	printf("First CSA\n");
	//	for(uint32_t t_begin = (uint32_t)__CSA_BEGIN;
	//			t_begin<(uint32_t)__CSA_BEGIN+CSA_ITEM_SIZE;
	//			t_begin=t_begin+4)
	//	{
	//		printf("%08X ", *(uint32_t*)(t_begin+4));
	//	}
	//	printf("\n");
	//	printf("Second CSA\n");
	//	for(uint32_t t_begin = (uint32_t)__CSA_BEGIN+CSA_ITEM_SIZE;
	//			t_begin<(uint32_t)__CSA_BEGIN+2*CSA_ITEM_SIZE;
	//			t_begin=t_begin+4)
	//	{
	//		printf("%08X ", *(uint32_t*)(t_begin+4));
	//	}
	//	printf("\n");

	_isync();
	_dsync();
}

const uint32_t GetFreeRTOSRunTimeTicks(void);

inline void HAL_Delay(uint32_t m)
{
	uint32_t c_tick = m + GetFreeRTOSRunTimeTicks();
	while(c_tick > GetFreeRTOSRunTimeTicks())
	{
		;
	}
}

inline void no_rtos_loop(void) __attribute__((always_inline));

inline void no_rtos_loop(void)
{
	while(1)
	{
		//		printf("No RTOS Loop. %u, CPU:%u Hz Tricore %04X, NewLibVer:%s\n",
		//				GetFreeRTOSRunTimeTicks(),
		//				get_cpu_frequency(),
		//				__TRICORE_NAME__,
		//				_NEWLIB_VERSION
		//		);

		//		test_func_spram_1();

		//		LEDTOGGLE(0);
		//		LEDTOGGLE(1);
		//		LEDTOGGLE(2);
		//		LEDTOGGLE(3);
		LEDTOGGLE(4);
		//		LEDTOGGLE(5);
		//		LEDTOGGLE(6);
		//		LEDTOGGLE(7);

		HAL_Delay(1000);
	}
}

static void prvSetupHardware( void )
{
	extern void set_cpu_frequency(void);

	/* Set-up the PLL. */
	set_cpu_frequency();

//	vParTestInitialise();

	ConfigureTimeForRunTimeStats();

	_init_uart(BAUDRATE);

	/* Initialise LED outputs. */
	InitLED();
}
/*-----------------------------------------------------------*/

/* Global Variables: */

Rec_Pointer     Ptr_Glob,
Next_Ptr_Glob;
int             Int_Glob;
Boolean         Bool_Glob;
char            Ch_1_Glob,
Ch_2_Glob;
int             Arr_1_Glob [50];
int             Arr_2_Glob [50] [50];

#define REG	register

#ifndef REG
Boolean Reg = false;
#define REG
/* REG becomes defined as empty */
/* i.e. no register variables   */
#else
Boolean Reg = true;
#endif

/* variables for time measurement: */
#ifdef TIMES
struct tms      time_info;
extern  int     times (void);
/* see library function "times" */
#define Too_Small_Time (2*HZ)
/* Measurements should last at least about 2 seconds */
#endif
#ifdef TIME
extern long     time(long *);
/* see library function "time"  */
#define Too_Small_Time 2
/* Measurements should last at least 2 seconds */
#endif
#ifdef MSC_CLOCK
//extern clock_t clock(void);
#define Too_Small_Time (2*HZ)
#endif

long            Begin_Time,
End_Time,
User_Time;
float           Microseconds,
Dhrystones_Per_Second;
/* end of variables for time measurement */

//#pragma section ".text" ax
#pragma section ".internalcode" ax

void Proc_1 (Rec_Pointer Ptr_Val_Par)
/******************/
/* executed once */
{
	REG Rec_Pointer Next_Record = Ptr_Val_Par->Ptr_Comp;
	/* == Ptr_Glob_Next */
	/* Local variable, initialized with Ptr_Val_Par->Ptr_Comp,    */
	/* corresponds to "rename" in Ada, "with" in Pascal           */

	structassign (*Ptr_Val_Par->Ptr_Comp, *Ptr_Glob);
	Ptr_Val_Par->variant.var_1.Int_Comp = 5;
	Next_Record->variant.var_1.Int_Comp = Ptr_Val_Par->variant.var_1.Int_Comp;
	Next_Record->Ptr_Comp = Ptr_Val_Par->Ptr_Comp;
	Proc_3 (&Next_Record->Ptr_Comp);
	/* Ptr_Val_Par->Ptr_Comp->Ptr_Comp == Ptr_Glob->Ptr_Comp */
	if (Next_Record->Discr == Ident_1)
		/* then, executed */
	{
		Next_Record->variant.var_1.Int_Comp = 6;
		Proc_6 (Ptr_Val_Par->variant.var_1.Enum_Comp,
				&Next_Record->variant.var_1.Enum_Comp);
		Next_Record->Ptr_Comp = Ptr_Glob->Ptr_Comp;
		Proc_7 (Next_Record->variant.var_1.Int_Comp, 10,
				&Next_Record->variant.var_1.Int_Comp);
	}
	else /* not executed */
		structassign (*Ptr_Val_Par, *Ptr_Val_Par->Ptr_Comp);
} /* Proc_1 */


void Proc_2 (One_Fifty *Int_Par_Ref)
/******************/
/* executed once */
/* *Int_Par_Ref == 1, becomes 4 */
{
	One_Fifty  Int_Loc;
	Enumeration   Enum_Loc;

	Int_Loc = *Int_Par_Ref + 10;
	do /* executed once */
		if (Ch_1_Glob == 'A')
			/* then, executed */
		{
			Int_Loc -= 1;
			*Int_Par_Ref = Int_Loc - Int_Glob;
			Enum_Loc = Ident_1;
		} /* if */
	while (Enum_Loc != Ident_1); /* true */
} /* Proc_2 */


void Proc_3 (Rec_Pointer *Ptr_Ref_Par)
/******************/
/* executed once */
/* Ptr_Ref_Par becomes Ptr_Glob */
{
	if (Ptr_Glob != Null)
		/* then, executed */
		*Ptr_Ref_Par = Ptr_Glob->Ptr_Comp;
	Proc_7 (10, Int_Glob, &Ptr_Glob->variant.var_1.Int_Comp);
} /* Proc_3 */


void Proc_4 (void) /* without parameters */
/*******/
/* executed once */
{
	Boolean Bool_Loc;

	Bool_Loc = Ch_1_Glob == 'A';
	Bool_Glob = Bool_Loc | Bool_Glob;
	Ch_2_Glob = 'B';
} /* Proc_4 */


void Proc_5 (void) /* without parameters */
/*******/
/* executed once */
{
	Ch_1_Glob = 'A';
	Bool_Glob = false;
} /* Proc_5 */


void Proc_6 (Enumeration Enum_Val_Par, Enumeration *Enum_Ref_Par)
/*********************************/
    /* executed once */
    /* Enum_Val_Par == Ident_3, Enum_Ref_Par becomes Ident_2 */
{
  *Enum_Ref_Par = Enum_Val_Par;
  if (! Func_3 (Enum_Val_Par))
    /* then, not executed */
    *Enum_Ref_Par = Ident_4;
  switch (Enum_Val_Par)
  {
    case Ident_1:
      *Enum_Ref_Par = Ident_1;
      break;
    case Ident_2:
      if (Int_Glob > 100)
        *Enum_Ref_Par = Ident_1;
      else
        *Enum_Ref_Par = Ident_4;
      break;
    case Ident_3: /* executed */
      *Enum_Ref_Par = Ident_2;
       break;
    case Ident_4:
       break;
    case Ident_5:
       *Enum_Ref_Par = Ident_3;
       break;
    } /* switch */
} /* Proc_6 */


void Proc_7 (One_Fifty Int_1_Par_Val,
             One_Fifty Int_2_Par_Val,
             One_Fifty *Int_Par_Ref )
/**********************************************/
    /* executed three times                                      */
    /* first call:      Int_1_Par_Val == 2, Int_2_Par_Val == 3,  */
    /*                  Int_Par_Ref becomes 7                    */
    /* second call:     Int_1_Par_Val == 10, Int_2_Par_Val == 5, */
    /*                  Int_Par_Ref becomes 17                   */
    /* third call:      Int_1_Par_Val == 6, Int_2_Par_Val == 10, */
    /*                  Int_Par_Ref becomes 18                   */
{
  One_Fifty Int_Loc;

  Int_Loc = Int_1_Par_Val + 2;
  *Int_Par_Ref = Int_2_Par_Val + Int_Loc;
} /* Proc_7 */


void Proc_8 (Arr_1_Dim Arr_1_Par_Ref,
             Arr_2_Dim Arr_2_Par_Ref,
             int Int_1_Par_Val,
             int Int_2_Par_Val)
/*********************************************************************/
    /* executed once      */
    /* Int_Par_Val_1 == 3 */
    /* Int_Par_Val_2 == 7 */
{
  REG One_Fifty Int_Index;
  REG One_Fifty Int_Loc;

  Int_Loc = Int_1_Par_Val + 5;
  Arr_1_Par_Ref [Int_Loc] = Int_2_Par_Val;
  Arr_1_Par_Ref [Int_Loc+1] = Arr_1_Par_Ref [Int_Loc];
  Arr_1_Par_Ref [Int_Loc+30] = Int_Loc;
  for (Int_Index = Int_Loc; Int_Index <= Int_Loc+1; ++Int_Index)
    Arr_2_Par_Ref [Int_Loc] [Int_Index] = Int_Loc;
  Arr_2_Par_Ref [Int_Loc] [Int_Loc-1] += 1;
  Arr_2_Par_Ref [Int_Loc+20] [Int_Loc] = Arr_1_Par_Ref [Int_Loc];
  Int_Glob = 5;
} /* Proc_8 */


Enumeration Func_1 (Capital_Letter Ch_1_Par_Val, Capital_Letter Ch_2_Par_Val)
/*************************************************/
    /* executed three times                                         */
    /* first call:      Ch_1_Par_Val == 'H', Ch_2_Par_Val == 'R'    */
    /* second call:     Ch_1_Par_Val == 'A', Ch_2_Par_Val == 'C'    */
    /* third call:      Ch_1_Par_Val == 'B', Ch_2_Par_Val == 'C'    */
{
  Capital_Letter        Ch_1_Loc;
  Capital_Letter        Ch_2_Loc;

  Ch_1_Loc = Ch_1_Par_Val;
  Ch_2_Loc = Ch_1_Loc;
  if (Ch_2_Loc != Ch_2_Par_Val)
    /* then, executed */
    return (Ident_1);
  else  /* not executed */
  {
    Ch_1_Glob = Ch_1_Loc;
    return (Ident_2);
  }
} /* Func_1 */


Boolean Func_2 (Str_30  Str_1_Par_Ref, Str_30  Str_2_Par_Ref)
/*************************************************/
    /* executed once */
    /* Str_1_Par_Ref == "DHRYSTONE PROGRAM, 1'ST STRING" */
    /* Str_2_Par_Ref == "DHRYSTONE PROGRAM, 2'ND STRING" */
{
  REG One_Thirty      Int_Loc;
  Capital_Letter      Ch_Loc;

  Int_Loc = 2;
  while (Int_Loc <= 2) /* loop body executed once */
    if (Func_1 (Str_1_Par_Ref[Int_Loc],
                Str_2_Par_Ref[Int_Loc+1]) == Ident_1)
    /* then, executed */
    {
      Ch_Loc = 'A';
      Int_Loc += 1;
    } /* if, while */

  if (Ch_Loc >= 'W' && Ch_Loc < 'Z')
    /* then, not executed */
    Int_Loc = 7;
  if (Ch_Loc == 'R')
    /* then, not executed */
    return (true);
  else /* executed */
  {
    if (strcmp (Str_1_Par_Ref, Str_2_Par_Ref) > 0)
      /* then, not executed */
    {
      Int_Loc += 7;
      Int_Glob = Int_Loc;
      return (true);
    }
    else /* executed */
      return (false);
  } /* if Ch_Loc */
} /* Func_2 */


Boolean Func_3 (Enumeration Enum_Par_Val)
/***************************/
    /* executed once        */
    /* Enum_Par_Val == Ident_3 */
{
  Enumeration Enum_Loc;

  Enum_Loc = Enum_Par_Val;
  if (Enum_Loc == Ident_3)
    /* then, executed */
    return (true);
  else /* not executed */
    return (false);
} /* Func_3 */

void dhry_benchmark(void)
{
	One_Fifty       Int_1_Loc;
	REG One_Fifty   Int_2_Loc;
	One_Fifty       Int_3_Loc;
	REG char        Ch_Index;
	Enumeration     Enum_Loc;
	Str_30          Str_1_Loc;
	Str_30          Str_2_Loc;
	REG int         Run_Index;
	REG int         Number_Of_Runs;

	/* Initializations */
	Next_Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));
	Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));

	Ptr_Glob->Ptr_Comp                    = Next_Ptr_Glob;
	Ptr_Glob->Discr                       = Ident_1;
	Ptr_Glob->variant.var_1.Enum_Comp     = Ident_3;
	Ptr_Glob->variant.var_1.Int_Comp      = 40;
	strcpy (Ptr_Glob->variant.var_1.Str_Comp,
			"DHRYSTONE PROGRAM, SOME STRING");
	strcpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");

	Arr_2_Glob [8][7] = 10;
	/* Was missing in published program. Without this statement,    */
	/* Arr_2_Glob [8][7] would have an undefined value.             */
	/* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
	/* overflow may occur for this array element.                   */

	printf ("\n");
	printf ("Dhrystone Benchmark, Version 2.1 (Language: C)\n");
	printf ("\n");
	if (Reg)
	{
		printf ("Program compiled with 'register' attribute\n");
		printf ("\n");
	}
	else
	{
		printf ("Program compiled without 'register' attribute\n");
		printf ("\n");
	}
	printf ("Please give the number of runs through the benchmark: ");
	{
		//    int n = 100000;
		//    scanf ("%d", &n);
		Number_Of_Runs = RUN_NUMBER;
	}
	printf ("\n");

	printf("Execution starts, %d runs through Dhrystone\n", Number_Of_Runs);
	/***************/
	/* Start timer */
	/***************/

#ifdef TIMES
	times (&time_info);
	Begin_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
	Begin_Time = time ( (long *) 0);
#endif
#ifdef MSC_CLOCK
	Begin_Time = GetFreeRTOSRunTimeTicks();
#endif

	for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
	{

		Proc_5();
		Proc_4();
		/* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
		Int_1_Loc = 2;
		Int_2_Loc = 3;
		strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
		Enum_Loc = Ident_2;
		Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
		/* Bool_Glob == 1 */
		while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
		{
			Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
			/* Int_3_Loc == 7 */
			Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
			/* Int_3_Loc == 7 */
			Int_1_Loc += 1;
		} /* while */
		/* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
		Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
		/* Int_Glob == 5 */
		Proc_1 (Ptr_Glob);
		for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
			/* loop body executed twice */
		{
			if (Enum_Loc == Func_1 (Ch_Index, 'C'))
				/* then, not executed */
			{
				Proc_6 (Ident_1, &Enum_Loc);
				strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING");
				Int_2_Loc = Run_Index;
				Int_Glob = Run_Index;
			}
		}
		/* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
		Int_2_Loc = Int_2_Loc * Int_1_Loc;
		Int_1_Loc = Int_2_Loc / Int_3_Loc;
		Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
		/* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
		Proc_2 (&Int_1_Loc);
		/* Int_1_Loc == 5 */

	} /* loop "for Run_Index" */

	/**************/
	/* Stop timer */
	/**************/

#ifdef TIMES
	times (&time_info);
	End_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
	End_Time = time ( (long *) 0);
#endif
#ifdef MSC_CLOCK
	End_Time = GetFreeRTOSRunTimeTicks();
#endif

	printf ("Execution ends\n");
	printf ("\n");
	printf ("Final values of the variables used in the benchmark:\n");
	printf ("\n");
	printf( "Int_Glob:            %d\n", Int_Glob);
	printf("        should be:   %d\n", 5);
	printf( "Bool_Glob:           %d\n", Bool_Glob);
	printf( "        should be:   %d\n", 1);
	printf("Ch_1_Glob:           %c\n", Ch_1_Glob);
	printf("        should be:   %c\n", 'A');
	printf("Ch_2_Glob:           %c\n", Ch_2_Glob);
	printf("        should be:   %c\n", 'B');
	printf("Arr_1_Glob[8]:       %d\n", Arr_1_Glob[8]);
	printf("        should be:   %d\n", 7);
	printf("Arr_2_Glob[8][7]:    %d\n", Arr_2_Glob[8][7]);
	printf ("        should be:   Number_Of_Runs + 10\n");
	printf ("Ptr_Glob->\n");
	printf("  Ptr_Comp:          %d\n", (int) Ptr_Glob->Ptr_Comp);
	printf ("        should be:   (implementation-dependent)\n");
	printf("  Discr:             %d\n", Ptr_Glob->Discr);
	printf("        should be:   %d\n", 0);
	printf("  Enum_Comp:         %d\n", Ptr_Glob->variant.var_1.Enum_Comp);
	printf("        should be:   %d\n", 2);
	printf("  Int_Comp:          %d\n", Ptr_Glob->variant.var_1.Int_Comp);
	printf("        should be:   %d\n", 17);
	printf("  Str_Comp:          %s\n", Ptr_Glob->variant.var_1.Str_Comp);
	printf ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
	printf ("Next_Ptr_Glob->\n");
	printf("  Ptr_Comp:          %d\n", (int) Next_Ptr_Glob->Ptr_Comp);
	printf ("        should be:   (implementation-dependent), same as above\n");
	printf("  Discr:             %d\n", Next_Ptr_Glob->Discr);
	printf("        should be:   %d\n", 0);
	printf("  Enum_Comp:         %d\n", Next_Ptr_Glob->variant.var_1.Enum_Comp);
	printf("        should be:   %d\n", 1);
	printf("  Int_Comp:          %d\n", Next_Ptr_Glob->variant.var_1.Int_Comp);
	printf("        should be:   %d\n", 18);
	printf("  Str_Comp:          %s\n",
			Next_Ptr_Glob->variant.var_1.Str_Comp);
	printf ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
	printf("Int_1_Loc:           %d\n", Int_1_Loc);
	printf("        should be:   %d\n", 5);
	printf("Int_2_Loc:           %d\n", Int_2_Loc);
	printf("        should be:   %d\n", 13);
	printf("Int_3_Loc:           %d\n", Int_3_Loc);
	printf("        should be:   %d\n", 7);
	printf("Enum_Loc:            %d\n", Enum_Loc);
	printf("        should be:   %d\n", 1);
	printf("Str_1_Loc:           %s\n", Str_1_Loc);
	printf ("        should be:   DHRYSTONE PROGRAM, 1'ST STRING\n");
	printf("Str_2_Loc:           %s\n", Str_2_Loc);
	printf ("        should be:   DHRYSTONE PROGRAM, 2'ND STRING\n");
	printf ("\n");

	User_Time = End_Time - Begin_Time;

	if (User_Time < Too_Small_Time)
	{
		printf( "Measured time too small to obtain meaningful results %d-%d\n", Begin_Time, End_Time);
		printf ("Please increase number of runs\n");
	}
	else
	{
#ifdef TIME
		Microseconds = (float) User_Time * Mic_secs_Per_Second
				/ (float) Number_Of_Runs;
		Dhrystones_Per_Second = (float) Number_Of_Runs / (float) User_Time;
#else
		Microseconds = (float) User_Time * (float)Mic_secs_Per_Second
				/ ((float) HZ * ((float) Number_Of_Runs));
		Dhrystones_Per_Second = ((float) HZ * (float) Number_Of_Runs)
	                        				/ (float) User_Time;
#endif
		printf("Microseconds for one run through Dhrystone[%d-%d]:  ", Begin_Time, End_Time);

		printf("%6.1f \n", Microseconds);

		printf ("Dhrystones per Second:                      ");
		printf("%6.1f \n", Dhrystones_Per_Second);

	}
}

#pragma section

int main(void)
{
	prvSetupHardware();
	//	enable_performance_cnt();

	/* enable global interrupts */
	_enable();

	printf("Tricore %04X @CPU:%u MHz Core:%04X\n",
			__TRICORE_NAME__,
			get_cpu_frequency()/1000000,
			__TRICORE_CORE__
	);

	//	extern void vTrapInstallHandlers( void );
	//	/* Install the Trap Handlers. */
	//	vTrapInstallHandlers();

	// CSA Experimentation
	//	printf("errno %d @ %08X %p\n",
	//			errno,
	//			(uint32_t)&errno,
	//			__errno );
	//
	//	printf("__CSA_BEGIN\t:%08X\n", (uint32_t)__CSA_BEGIN);
	//	printf("__CSA\t:%08X\n", (uint32_t)__CSA);
	//	printf("__CSA_SIZE\t:%08X\n", (uint32_t)__CSA_SIZE);
	//	printf("__CSA_END\t:%08X\n", (uint32_t)__CSA_END);
	//
	//	printf("__PMI_SPRAM_BEGIN\t:%08X\n", (uint32_t)__PMI_SPRAM_BEGIN);
	//	printf("__PMI_SPRAM_SIZE\t:%08X\n", (uint32_t)__PMI_SPRAM_SIZE);
	//	printf("__DMI_LDRAM_BEGIN\t:%08X\n", (uint32_t)__DMI_LDRAM_BEGIN);
	//	printf("__DMI_LDRAM_SIZE\t:%08X\n", (uint32_t)__DMI_LDRAM_SIZE);
	//
	//	volatile uint32_t tmp_u32;
	//	tmp_u32 = _mfcr(CPU_ID_ADDR);
	//	printf("CPUID\t%08X\t:%08X\n", &CPU_ID, tmp_u32);
	//	tmp_u32 = _mfcr(SYSCON_ADDR);
	//	printf("SYSCON\t%08X\t:%08X\n", SYSCON_ADDR, tmp_u32);
	//	tmp_u32 = _mfcr(PSW_ADDR);
	//	printf("PSW\t%08X\t:%08X\n", PSW_ADDR, tmp_u32);
	//	tmp_u32 = _mfcr(MMU_CON_ADDR);
	//	printf("MMU_CON\t%08X\t:%08X\n", MMU_CON_ADDR, tmp_u32);
	//	tmp_u32 = _mfcr(FPU_ID_ADDR);
	//	printf("FPU_ID\t%08X\t:%08X\n", FPU_ID_ADDR, tmp_u32);
	//
	//	tmp_u32 = direct_read(PMI_ID_ADDR);
	//	printf("PMI_ID\t%08X\t:%08X\n", PMI_ID_ADDR, tmp_u32);
	//	tmp_u32 = direct_read(PMI_CON0_ADDR);
	//	printf("PMI_CON0\t%08X\t:%08X\n", PMI_CON0_ADDR, tmp_u32);
	//	tmp_u32 = direct_read(PMI_CON1_ADDR);
	//	printf("PMI_CON1\t%08X\t:%08X\n", PMI_CON1_ADDR, tmp_u32);
	//	tmp_u32 = direct_read(PMI_CON2_ADDR);
	//	printf("PMI_CON2\t%08X\t:%08X\n", PMI_CON2_ADDR, tmp_u32);
	//	tmp_u32 = direct_read(PMI_STR_ADDR);
	//	printf("PMI_STR\t%08X\t:%08X\n", PMI_STR_ADDR, tmp_u32);
	//
	//	tmp_u32 = direct_read(DMI_ID_ADDR);
	//	printf("DMI_ID\t%08X\t:%08X\n", DMI_ID_ADDR, tmp_u32);
	//	tmp_u32 = direct_read(DMI_CON_ADDR);
	//	printf("DMI_CON\t%08X\t:%08X\n", DMI_CON_ADDR, tmp_u32);
	//	tmp_u32 = direct_read(DMI_STR_ADDR);
	//	printf("DMI_STR\t%08X\t:%08X\n", DMI_STR_ADDR, tmp_u32);
	//	tmp_u32 = direct_read(DMI_ATR_ADDR);
	//	printf("DMI_ATR\t%08X\t:%08X\n", DMI_ATR_ADDR, tmp_u32);
	//
	//	tmp_u32 = _mfcr(FCX_ADDR);
	//	printf("FCX\t%08X\t:%08X %08X\n", FCX_ADDR, tmp_u32, portCSA_TO_ADDRESS(tmp_u32));
	//	tmp_u32 = _mfcr(LCX_ADDR);
	//	printf("LCX\t%08X\t:%08X %08X\n", LCX_ADDR, tmp_u32, portCSA_TO_ADDRESS(tmp_u32));
	//	tmp_u32 = _mfcr(PCXI_ADDR);
	//	printf("PCXI\t%08X\t:%08X %08X\n", PCXI_ADDR, tmp_u32, portCSA_TO_ADDRESS(tmp_u32));

	dhry_benchmark();

	no_rtos_loop();

	return 0;
}

/*-----------------------------------------------------------*/
