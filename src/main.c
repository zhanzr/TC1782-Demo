/*====================================================================
 * Project:  Board Support Package (BSP) examples
 * Function: example using a serial line (polling mode)
 *
 * Copyright HighTec EDV-Systeme GmbH 1982-2015
 *====================================================================*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <tc1782.h>

#include "led.h"
#include "uart_poll.h"
#include "cpufreq.h"

#define BUFSIZE		512

extern volatile uint32_t clock_mode;

volatile uint32_t g_ticks;
volatile bool g_blink_flag;

/* timer callback handler */
static void my_timer_handler(void)
{
	++g_ticks;

	if (0 == g_ticks%(2*SYSTIME_CLOCK))
	{
		g_blink_flag = true;
	}
}

int main(void)
{
	_init_uart(BAUDRATE);

	InitLED();

	/* initialise timer at SYSTIME_CLOCK rate */
	TimerInit(SYSTIME_CLOCK);
	/* add own handler for timer interrupts */
	TimerSetHandler(my_timer_handler);

	/* enable global interrupts */
	_enable();

	printf("Tricore Demo @FPI:%u Hz CPU:%u Hz %u CoreType:%04X\n",
			get_fpi_frequency(),
			get_cpu_frequency(),
			g_ticks,
			__TRICORE_CORE__
	);

	while(1)
	{
		//		c = _in_uart();
		/* check flag set by timer ISR in every 100 ms */

		if(g_blink_flag)
		{
			printf("Tricore Demo @FPI:%u Hz CPU:%u Hz %u CoreType:%04X\n",
					get_fpi_frequency(),
					get_cpu_frequency(),
					g_ticks,
					__TRICORE_CORE__
			);

			LEDTOGGLE(0);
			LEDTOGGLE(1);
			//			LEDTOGGLE(2);
			//			LEDTOGGLE(3);
			//			LEDTOGGLE(4);
			//			LEDTOGGLE(5);
			//			LEDTOGGLE(6);
			//			LEDTOGGLE(7);
			g_blink_flag = false;
		}
	}
	return EXIT_SUCCESS;
}
