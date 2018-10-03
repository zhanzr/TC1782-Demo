/*====================================================================
* Project:  Board Support Package (BSP)
* Function: Hardware-dependent module providing a time base
*           by programming a system timer (TriCore TC1782 version).
*
* Copyright HighTec EDV-Systeme GmbH 1982-2015
*====================================================================*/

#include <machine/cint.h>
#include <machine/wdtcon.h>

#include <tc1782/stm.h>

#include "timer.h"

#define TICK_INTERRUPT  4

extern unsigned int get_cpu_frequency(void);

/* timer reload value (needed for subtick calculation) */
static unsigned int reload_value = 0;

/* pointer to user specified timer callback function */
static TCF user_handler = (TCF)0;

/* timer interrupt routine */
static void tick_irq(int reload_value)
{
	/* set new compare value */
	STM_CMP0.reg += (unsigned int)reload_value;
	if (user_handler)
	{
		user_handler();
	}
}

static int STM_enable(void)
{
	unlock_wdtcon();
	STM_CLC.reg = 0x100;
	(void)STM_CLC.reg;
	lock_wdtcon();

	return 0;
}

/* Initialise timer at rate <hz> */
void TimerInit(unsigned int hz)
{
	unsigned int frequency;

	/* Compute CPU frequency and timer reload value. */
	frequency = get_cpu_frequency();
	reload_value = frequency / hz;

	/* Install handler for timer interrupt. */
	_install_int_handler(TICK_INTERRUPT, tick_irq, (int)reload_value);

	/*
		initialise STM
		first must set clock for STM and enable module
		register is endinit protected
	 */
	STM_enable();

	STM_CMP0.reg = STM_TIM0.reg + reload_value;
	STM_CMCON.bits.MSIZE0 = 31;

	STM_ICR.reg = STM_ICR_CMP0EN_MASK;

	/* enable service request node 0 of STM and assign priority */
	STM_SRC0.bits.SRPN = TICK_INTERRUPT;
	STM_SRC0.bits.SRE = 1;
}

/* Install <handler> as timer callback function */
void TimerSetHandler(TCF handler)
{
	user_handler = handler;
}
