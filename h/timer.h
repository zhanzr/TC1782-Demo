/*====================================================================
* Project:  Board Support Package (BSP)
* Function: Hardware-dependent module providing a time base
*           by programming a system timer
*
* Copyright HighTec EDV-Systeme GmbH 1982-2015
*====================================================================*/

#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* type of a timer callback function */
typedef void (*TCF)(void);

/* Initialise timer at rate <hz> */
void TimerInit(unsigned int hz);

/* Install <handler> as timer callback function */
void TimerSetHandler(TCF handler);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIMER_H__ */
