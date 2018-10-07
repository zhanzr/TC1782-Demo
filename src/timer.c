/*====================================================================
 * Project:  Board Support Package (BSP)
 * Function: Hardware-dependent module providing a time base
 *           by programming a system timer (TriCore TC1782 version).
 *
 * Copyright HighTec EDV-Systeme GmbH 1982-2015
 *====================================================================*/

#include <stdint.h>
#include <stdio.h>

#include <tc1782.h>
#include <machine/intrinsics.h>
#include <machine/cint.h>
#include <machine/wdtcon.h>
#include <tc1782/stm.h>

#include "timer.h"
#include "cpufreq.h"
#include "portmacro.h"
#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern int g_cmp1_ret;

volatile uint64_t FreeRTOSRunTimeTicks;

#define CMP1_MATCH_VAL	( configPERIPHERAL_CLOCK_HZ /  configRUN_TIME_STATS_RATE_HZ)

const uint64_t GetFreeRTOSRunTimeTicks(void)
{
	return FreeRTOSRunTimeTicks;
}

static void prvStatTickHandler(int iArg) __attribute__((longcall));

static void prvStatTickHandler(int iArg)
{
	++ FreeRTOSRunTimeTicks;

	/* Clear the interrupt source. */
	STM_ISRR.bits.CMP1IRR = 0x01UL;
//	STM_CMP1.reg += (uint32_t)iArg;
}

void ConfigureTimeForRunTimeStats(void)
{
	FreeRTOSRunTimeTicks = 0;

	taskENTER_CRITICAL();
	{
		/* Determine how many bits are used without changing other bits in the CMCON register. */
		STM_CMCON.bits.MSIZE1 &= ~0x1fUL;
		STM_CMCON.bits.MSIZE1 |= (0x1f - __builtin_clz( CMP1_MATCH_VAL));
		/* Take into account the current time so a tick doesn't happen immediately. */
		STM_CMP1.reg = CMP1_MATCH_VAL + STM_TIM0.reg;

		int cmp1_ret = _install_int_handler( configRUNTIME_STAT_INTERRUPT_PRIORITY,
				prvStatTickHandler,
				CMP1_MATCH_VAL );
		if( 0 != cmp1_ret )
		{
			/* Set-up the interrupt. */
			STM_SRC1.bits.SRPN = configRUNTIME_STAT_INTERRUPT_PRIORITY;
			STM_SRC1.bits.CLRR = 1UL;
			STM_SRC1.bits.SRE = 1UL;

			/* Enable the Interrupt. */
			STM_ISRR.reg &= ~( 0x0CUL );
			STM_ISRR.bits.CMP1IRR = 1;
			STM_ICR.reg &= ~( 0x70UL );
			STM_ICR.bits.CMP1EN = 1;
			STM_ICR.bits.CMP1OS = 1;
		}
		else
		{
			/* Failed to install the interrupt. */
			configASSERT( ( ( volatile void * ) NULL ) );
		}
	}
	taskEXIT_CRITICAL();
}
