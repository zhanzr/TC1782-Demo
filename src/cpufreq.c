/*====================================================================
* Project:  Board Support Package (BSP)
* Function: Determine the frequency the CPU is running at (TC 1.3.1)
*
* Copyright HighTec EDV-Systeme GmbH 1982-2014
*====================================================================*/

#include <stdint.h>
#include <machine/wdtcon.h>

#include "cpufreq.h"

#ifdef __TC16__
/* Required for __MTCR, because PCON0 is part of CFSR. */
#include <machine/intrinsics.h>
#endif /* __TC16__ */


/* prototypes for global functions */
void set_cpu_frequency(void);
unsigned int get_cpu_frequency(void);

/* initialisation flag: prevent multiple initialisation of PLL_CLC */
static int freq_init = 0;

volatile uint32_t clock_mode;

/* Set the frequency the CPU is running at */

void set_cpu_frequency(void)
{
	SCU_PLLCON0_t_nonv pllcon0;
	SCU_PLLCON1_t_nonv pllcon1;

	if (freq_init)
	{
		return;
	}

	freq_init = 1;

	/* check whether we are already running at desired clockrate */
	pllcon0 = SCU_PLLCON0;
	pllcon1 = SCU_PLLCON1;
	if (	((SYS_CFG_NDIV - 1)  == pllcon0.bits.NDIV)
		&&	((SYS_CFG_PDIV - 1)  == pllcon0.bits.PDIV)
		&&	((SYS_CFG_K1DIV - 1) == pllcon1.bits.K1DIV)
		&&	((SYS_CFG_K2DIV - 1) == pllcon1.bits.K2DIV)
		&&	SCU_PLLSTAT.bits.VCOLOCK)
	{
		return;
	}

	if (!SCU_PLLSTAT.bits.PWDSTAT)
	{
		/* set speed to selected value */
		pllcon0.reg = 0;
		pllcon1.reg = 0;
		pllcon0.bits.NDIV  = SYS_CFG_NDIV - 1;
		pllcon0.bits.PDIV  = SYS_CFG_PDIV - 1;
		pllcon1.bits.K2DIV = SYS_CFG_K2DIV - 1;
		pllcon1.bits.K1DIV = SYS_CFG_K1DIV - 1;
		pllcon0.bits.VCOBYP = 1;
		pllcon0.bits.CLRFINDIS = 1;
#ifdef __TC131__
		pllcon0.bits.PLLPWD = 1;
#endif
		pllcon0.bits.RESLD = 1;

		unlock_wdtcon();
		/* FPI at half CPU speed */
		SCU_CCUCON0.bits.FPIDIV = SYS_CFG_FPIDIV;

		/* force prescaler mode */
		SCU_PLLCON0.bits.VCOBYP = 1;

		/* wait for prescaler mode */
		while (!SCU_PLLSTAT.bits.VCOBYST)
			;

		/* write new control values */
		SCU_PLLCON1 = pllcon1;
		SCU_PLLCON0 = pllcon0;
		lock_wdtcon();

		/* wait for stable VCO frequency */
		while (!SCU_PLLSTAT.bits.VCOLOCK)
			;

		unlock_wdtcon();
		/* leave prescaler mode */
		SCU_PLLCON0.bits.VCOBYP = 0;
		lock_wdtcon();
	}
}

/* Determine the frequency the CPU is running at */

unsigned int get_fpi_frequency(void)
{
	unsigned int frequency;
	unsigned int fpidiv;

	SCU_PLLCON0_t_nonv pllcon0;
	SCU_PLLCON1_t_nonv pllcon1;
	SCU_PLLSTAT_t_nonv pllstat;

	if (!freq_init)
	{
		/* set speed to selected clock frequency */
		set_cpu_frequency();

#ifdef ENABLE_ICACHE
		unlock_wdtcon();
		/* enable instruction cache */
#if defined(__TC131__)
		PMI_CON0.bits.PCBYP = 0;
#elif defined(__TC16__)
		__MTCR(PCON0_ADDR, 0);
#endif /* __TC131__ */
		lock_wdtcon();
#endif /* ENABLE_ICACHE */
	}

	pllcon0 = SCU_PLLCON0;
	pllcon1 = SCU_PLLCON1;
	pllstat = SCU_PLLSTAT;

	/* read FPI divider value */
	fpidiv = SCU_CCUCON0.bits.FPIDIV;

	if (pllstat.bits.VCOBYST)
	{
		/* prescaler mode */
		unsigned int k_div;

		k_div = pllcon1.bits.K1DIV + 1;
		frequency = DEF_FRQ / k_div;

		clock_mode = 11;
	}
	else if (pllstat.bits.FINDIS)
	{
		/* freerunning mode */
		unsigned int k_div;

		k_div = pllcon1.bits.K2DIV + 1;
		frequency = VCOBASE_FREQ / k_div;

		clock_mode = 12;
	}
	else
	{
		/* normal mode */
		unsigned int k_div, n_div, p_div;

		n_div = pllcon0.bits.NDIV + 1;
		p_div = pllcon0.bits.PDIV + 1;
		k_div = pllcon1.bits.K2DIV + 1;

		frequency = DEF_FRQ * n_div / (k_div * p_div);

		clock_mode = 13;
	}

	frequency /= (fpidiv + 1);

	return frequency;
}

unsigned int get_cpu_frequency(void)
{
	unsigned int frequency;
	unsigned int fpidiv;

	SCU_PLLCON0_t_nonv pllcon0;
	SCU_PLLCON1_t_nonv pllcon1;
	SCU_PLLSTAT_t_nonv pllstat;

	if (!freq_init)
	{
		/* set speed to selected clock frequency */
		set_cpu_frequency();

#ifdef ENABLE_ICACHE
		unlock_wdtcon();
		/* enable instruction cache */
#if defined(__TC131__)
		PMI_CON0.bits.PCBYP = 0;
#elif defined(__TC16__)
		__MTCR(PCON0_ADDR, 0);
#endif /* __TC131__ */
		lock_wdtcon();
#endif /* ENABLE_ICACHE */
	}

	pllcon0 = SCU_PLLCON0;
	pllcon1 = SCU_PLLCON1;
	pllstat = SCU_PLLSTAT;

	if (pllstat.bits.VCOBYST)
	{
		/* prescaler mode */
		unsigned int k_div;

		k_div = pllcon1.bits.K1DIV + 1;
		frequency = DEF_FRQ / k_div;
	}
	else if (pllstat.bits.FINDIS)
	{
		/* freerunning mode */
		unsigned int k_div;

		k_div = pllcon1.bits.K2DIV + 1;
		frequency = VCOBASE_FREQ / k_div;
	}
	else
	{
		/* normal mode */
		unsigned int k_div, n_div, p_div;

		n_div = pllcon0.bits.NDIV + 1;
		p_div = pllcon0.bits.PDIV + 1;
		k_div = pllcon1.bits.K2DIV + 1;

		frequency = DEF_FRQ * n_div / (k_div * p_div);
	}

	return frequency;
}
