/*====================================================================
 * Project:  Board Support Package (BSP)
 * Function: LEDs
 *
 * Copyright HighTec EDV-Systeme GmbH 1982-2015
 *====================================================================*/

#ifndef __LED_H__
#define __LED_H__

#include "bspconfig.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static __inline void LEDON(int nr)
{
	LED_ON(nr);
}

static __inline void LEDOFF(int nr)
{
	LED_OFF(nr);
}

static __inline void LEDTOGGLE(int nr)
{
	LED_TOGGLE(nr);
}

static __inline void InitLED(void)
{
	INIT_LEDS;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LED_H__ */
