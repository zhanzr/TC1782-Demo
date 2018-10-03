#ifndef CPUFREQ_H_
#define CPUFREQ_H_

#include <tc1782/scu.h>
#include <tc1782/cpu.h>

#define DEF_FRQ			20000000U	/* TriBoard-TC1782 quartz frequency is 20 MHz */

#define VCOBASE_FREQ	400000000U	/* ?? */

/* divider values for 150 MHz */
#define SYS_CFG_PDIV	 2
#define SYS_CFG_NDIV	30
#define SYS_CFG_K1DIV	 2
#define SYS_CFG_K2DIV	 2
#define SYS_CFG_FPIDIV	 1

void set_cpu_frequency(void);

unsigned int get_fpi_frequency(void);

unsigned int get_cpu_frequency(void);


#endif /* CPUFREQ_H_ */
