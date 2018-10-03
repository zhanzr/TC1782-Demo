/*====================================================================
* Project:  Board Support Package (BSP)
* Function: BSP configuration header file.
*           (Infineon TriBoard-TC1782 board)
*
* Copyright HighTec EDV-Systeme GmbH 1982-2015
*====================================================================*/

#ifndef __BSPCONFIG_H__
#define __BSPCONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MINIMAL_CODE

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <machine/cint.h>

#include <machine/intrinsics.h>
#include <machine/wdtcon.h>
#include <tc1782/cpu.h>

#define SYSTIME_CLOCK	1000	/* timer event rate [Hz] */

#define BAUDRATE	115200

#define SET_ICACHE(e) {\
	unlock_wdtcon();\
	if ((e)) {\
		PMI_CON0.bits.PCBYP = 0;\
	} else {\
		PMI_CON0.bits.PCBYP = 1;\
	}\
	lock_wdtcon();\
}

/*
	NOTE: This include of <tc1782/port5-struct.h> must be after the extern "C" declaration,
	because <tc1782/port5-struct.h> has not its own extern "C" declaration.
*/
#include <tc1782/port5-struct.h>

static PORT5_t * const portLED = (PORT5_t *)P5_BASE;

/*
 *	number of available LEDs
 */
#define MAX_LED					8
#define MASK_ALL_LEDS			((1 << MAX_LED) - 1)

/* TriBoard-TC1782 : P5.0 ... P5.7 --> LED D601 ... D608 */

#define LED_PIN_NR				0

/* OMR is WO ==> don't use load-modify-store access! */
#define LED_PIN_SET(x)			(1 << (LED_PIN_NR + (x)))
#define LED_PIN_RESET(x)		(1 << (LED_PIN_NR + Pn_OMR_PR0_SHIFT + (x)))
#define LED_ON(x)				(portLED->OMR.reg = LED_PIN_RESET(x))
#define LED_OFF(x)				(portLED->OMR.reg = LED_PIN_SET(x))
#define LED_TOGGLE(x)			(portLED->OMR.reg = LED_PIN_RESET(x) | LED_PIN_SET(x))
#define INIT_LEDS				{\
	/* initialise P5.0 - P5.7 */\
	portLED->IOCR0.reg = 0x80808080;	/* OUT_PPGPIO */\
	portLED->IOCR4.reg = 0x80808080;	/* OUT_PPGPIO */\
	/* all LEDs OFF */\
	portLED->OMR.reg = (MASK_ALL_LEDS << LED_PIN_NR);\
}


/*********************************************************/
/* Common UART settings (interrupt and polling variants) */
/*********************************************************/
#if (defined(MODULE_UART_INT) || defined(MODULE_UART_POLL))

#include <tc1782/port3-struct.h>
#include <tc1782/asc0-struct.h>


static ASC0_t * const asc0 = (ASC0_t *) ASC0_BASE;
static PORT3_t * const port = (PORT3_t *) P3_BASE;


#define UARTBASE				asc0

/* ASC Modes */
#define ASCM_8SYNC				0	/* 8 Bit Data Sync. */
#define ASCM_8ASYNC				1	/* 8 Bit Data Async. */
#define ASCM_8IRDA				2	/* 8 Bit Data IrDA */
#define ASCM_7P					3	/* 7 Bit Data + Parity */
#define ASCM_9					4	/* 9 Bit Data */
#define ASCM_8W					5	/* 8 Bit Data + Wake up Bit */
#define ASCM_8P					7	/* 8 Bit Data + Parity */

/* Port Modes */
#define IN_NOPULL0				0x0	/* Port Input No Pull Device */
#define IN_PULLDOWN				0x1	/* Port Input Pull Down Device */
#define IN_PULLUP				0x2	/* Port Input Pull Up Device */
#define IN_NOPULL3				0x3	/* Port Input No Pull Device */
#define OUT_PPGPIO				0x8	/* Port Output General Purpose Push/Pull */
#define OUT_PPALT1				0x9	/* Port Output Alternate 1 Function Push/Pull */
#define OUT_PPALT2				0xA	/* Port Output Alternate 2 Function Push/Pull */
#define OUT_PPALT3				0xB	/* Port Output Alternate 3 Function Push/Pull */
#define OUT_ODGPIO				0xC	/* Port Output General Purpose Open Drain */
#define OUT_ODALT1				0xD	/* Port Output Alternate 1 Function Open Drain */
#define OUT_ODALT2				0xE	/* Port Output Alternate 2 Function Open Drain */
#define OUT_ODALT3				0xF	/* Port Output Alternate 3 Function Open Drain */

/* definitions for RX error conditions */
#define ASC_ERROR_MASK			(ASCn_CON_PE_MASK | ASCn_CON_FE_MASK | ASCn_CON_OE_MASK)
#define ASC_CLRERR_MASK			(ASCn_WHBCON_CLRPE_MASK | ASCn_WHBCON_CLRFE_MASK | ASCn_WHBCON_CLROE_MASK)

/* UART primitives */
#define PUT_CHAR(u, c)			((u)->TBUF.reg = (c))
#define GET_CHAR(u)				((u)->RBUF.bits.RD_VALUE)
#define GET_ERROR_STATUS(u)		(((u)->CON.reg) & ASC_ERROR_MASK)
#define RESET_ERROR(u)			((u)->WHBCON.reg = ASC_CLRERR_MASK)

#endif /* (defined(MODULE_UART_INT) || defined(MODULE_UART_POLL)) */


/************************/
/* Polling variant UART */
/************************/
#ifdef MODULE_UART_POLL

/* UART primitives */
#define TX_START(u)				((u)->TBSRC.bits.SETR = 1)
#define TX_CLEAR(u)				((u)->TBSRC.bits.CLRR = 1)
#define RX_CLEAR(u)				((u)->RSRC.bits.CLRR = 1)
#define TX_READY(u)				((u)->TBSRC.bits.SRR != 0)
#define RX_READY(u)				((u)->RSRC.bits.SRR != 0)

#endif /* MODULE_UART_POLL */


/**************************/
/* Interrupt variant UART */
/**************************/
#ifdef MODULE_UART_INT

extern void _uart_init_bsp(int baudrate, void (*uart_rx_isr)(int arg), void (*uart_tx_isr)(int arg));

#define XMIT_INTERRUPT			3
#define RECV_INTERRUPT			4

/* UART primitives */
#define TX_INT_START(u)			((u)->TBSRC.reg |= (ASCn_TBSRC_SRE_MASK | ASCn_TBSRC_SETR_MASK))
#define TX_INT_STOP(u)			((u)->TBSRC.bits.SRE = 0)
#define TX_INT_CHECK(u)			((u)->TBSRC.bits.SRE)

#endif /* MODULE_UART_INT */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __BSPCONFIG_H__ */
