/*====================================================================
* Project:  Board Support Package (BSP)
* Function: Transmit and receive characters via TriCore's serial line
*           (Infineon controller TC1782)
*           (polling variant)
*
* Copyright HighTec EDV-Systeme GmbH 1982-2014
*====================================================================*/
#define MODULE_UART_POLL

#include <machine/wdtcon.h>

#include "bspconfig.h"
#include "cpufreq.h"

/* Initialise asynchronous interface to operate at baudrate,8,n,1 */
void _init_uart(int baudrate)
{
	unsigned int frequency, reload_value, fdv;
	unsigned int dfreq;

	/* Set TXD to "output" and "high" */
	/* set P3.1 to output and high */
	port->IOCR0.bits.PC1 = OUT_PPALT1;
	port->OMR.bits.PS1 = 1;

	/* Compute system frequency and reload value for ASC0 */
	frequency = get_fpi_frequency();

	switch (baudrate)
	{
		case   9600 :
		case  19200 :
		case  38400 :
		case  57600 :
		case 115200 :
			break;
		default     :
			baudrate = 38400;
			break;
	}

	/*  reload_value = fdv/512 * freq/16/baudrate -1  ==>
		reload_value = (512*freq)/(baudrate * 512*16) - 1
		fdv = (reload_value + 1) * (baudrate*512*16/freq)
		reload_value = (frequency / 32) / baudrate - 1;
	*/
	reload_value = (frequency / (baudrate * 16)) - 1;
	dfreq = frequency / (16*512);
	fdv = (reload_value + 1) * (unsigned int)baudrate / dfreq;

	/* Enable ASCn */
	unlock_wdtcon();
	UARTBASE->CLC.bits.RMC = 1;
	UARTBASE->CLC.bits.DISR = 0;
	lock_wdtcon();

	/* Program ASCn */
	UARTBASE->CON.reg = 0;
	UARTBASE->BG.reg  = reload_value;
	UARTBASE->FDV.reg = fdv;

	UARTBASE->CON.bits.M = ASCM_8ASYNC;
	UARTBASE->CON.bits.R = 1;
	UARTBASE->CON.bits.REN = 1;
	UARTBASE->CON.bits.FDE = 1;

	TX_START(UARTBASE);
}
