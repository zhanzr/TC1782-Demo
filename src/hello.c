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

volatile uint32_t g_Ticks;
extern volatile uint32_t clock_mode;

/* timer callback handler */
static void my_timer_handler(void)
{
	++g_Ticks;
}

static void my_printf(const char *fmt, ...)
{
	char buffer[BUFSIZE];
	char *ptr;
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buffer, fmt, ap);
	va_end(ap);

	for (ptr = buffer; *ptr; ++ptr)
		_out_uart((const unsigned char) *ptr);
}

int original_main(void)
{
	unsigned char c;

	_init_uart(BAUDRATE);
	InitLED();

	/* initialise timer at SYSTIME_CLOCK rate */
	TimerInit(SYSTIME_CLOCK);
	/* add own handler for timer interrupts */
	TimerSetHandler(my_timer_handler);

	/* enable global interrupts */
	_enable();

//	while (1)
//	{
//		//		c = _in_uart();
//		if (0 == g_Ticks%(10*SYSTIME_CLOCK))
//		{
//			printf("Tricore Dhrystone @FPI:%u MHz CPU:%u MHz %u CoreType:%04X\n",
//					get_fpi_frequency()/1000000,
//					get_cpu_frequency()/1000000,
//					g_Ticks,
//					__TRICORE_CORE__
//			);
//
//			LEDTOGGLE(0);
//
//			LEDTOGGLE(1);
//			//			LEDTOGGLE(2);
//			//			LEDTOGGLE(3);
//			//			LEDTOGGLE(4);
//			//			LEDTOGGLE(5);
//			//			LEDTOGGLE(6);
//			//			LEDTOGGLE(7);
//		}
//	}

	return EXIT_SUCCESS;
}
