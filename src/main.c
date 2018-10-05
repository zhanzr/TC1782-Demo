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
#include <stdbool.h>
#include <stdint.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"

/* Demo application includes. */
#include "partest.h"
#include "flash.h"
#include "integer.h"
#include "PollQ.h"
#include "comtest2.h"
#include "semtest.h"
#include "dynamic.h"
#include "BlockQ.h"
#include "blocktim.h"
#include "countsem.h"
#include "GenQTest.h"
#include "recmutex.h"
#include "serial.h"
#include "death.h"
#include "TimerDemo.h"
#include "InterruptNestTest.h"

#include "led.h"
#include "cpufreq.h"

volatile uint32_t g_ticks;
volatile bool g_blink_flag;
/*----------------------------------------------------------*/

/* Constants for the ComTest tasks. */
#define mainCOM_TEST_BAUD_RATE		( ( uint32_t ) 115200 )

#define mainCOM_TEST_LED			( 5 )

/* Priorities for the demo application tasks. */
#define mainLED_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainCOM_TEST_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_POLL_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY		( tskIDLE_PRIORITY + 4 )
#define mainSEM_TEST_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY	( tskIDLE_PRIORITY + 3 )

/* The rate at which the on board LED will toggle when there is/is not an
error. */
#define mainNO_ERROR_FLASH_PERIOD_MS	( ( TickType_t ) 5000 / portTICK_PERIOD_MS	)
#define mainERROR_FLASH_PERIOD_MS		( ( TickType_t ) 500 / portTICK_PERIOD_MS  )
#define mainON_BOARD_LED_BIT			( ( unsigned long ) 7 )

/* Constant used by the standard timer test functions.  The timers created by
the timer test functions will all have a period that is a multiple of this
value. */
#define mainTIMER_TEST_PERIOD		( 200 )

/* Set mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY to 1 to create a simple demo.
Set mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY to 0 to create a much more
comprehensive test application.  See the comments at the top of this file, and
the documentation page on the http://www.FreeRTOS.org web site for more
information. */
#define mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY		1

/*-----------------------------------------------------------*/

/*
 * Checks that all the demo application tasks are still executing without error
 * - as described at the top of the file.
 */
static long prvCheckOtherTasksAreStillRunning( void );

/*
 * The task that executes at the highest priority and calls
 * prvCheckOtherTasksAreStillRunning().	 See the description at the top
 * of the file.
 */
static void prvCheckTask( void *pvParameters );

/*
 * Configure the processor ready to run this demo.
 */
static void prvSetupHardware( void );

/*
 * Writes to and checks the value of each register that is used in the context
 * of a task.  See the comments at the top of this file.
 */
static void prvRegisterCheckTask1( void *pvParameters );
static void prvRegisterCheckTask2( void *pvParameters );

/*
 * Specific check to see if the register test functions are still operating
 * correctly.
 */
static portBASE_TYPE prvAreRegTestTasksStillRunning( void );

/*
 * This file can be used to create either a simple LED flasher example, or a
 * comprehensive test/demo application - depending on the setting of the
 * mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY constant defined above.  If
 * mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY is set to 1, then the following
 * function will create a lot of additional tasks and timers.  If
 * mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY is set to 0, then the following
 * function will do nothing.
 */
static void prvOptionallyCreateComprehensveTestApplication( void );

/*-----------------------------------------------------------*/

/* Used by the register test tasks to indicated liveness. */
static unsigned long ulRegisterTest1Count = 0;
static unsigned long ulRegisterTest2Count = 0;

/*-----------------------------------------------------------*/


///* timer callback handler */
//static void my_timer_handler(void)
//{
//	++g_ticks;
//
//	if (0 == g_ticks%(2*configTICK_RATE_HZ))
//	{
//		g_blink_flag = true;
//	}
//}

TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

TaskHandle_t LED0Task_Handler;
void led0_task(void *pvParameters);

TaskHandle_t LED1Task_Handler;
void led1_task(void *pvParameters);

TaskHandle_t FLOATTask_Handler;
void print_task(void *pvParameters);

int main(void)
{
	_init_uart(mainCOM_TEST_BAUD_RATE);

	InitLED();

//	/* initialise timer at SYSTIME_CLOCK rate */
//	TimerInit(SYSTIME_CLOCK);
//	/* add own handler for timer interrupts */
//	TimerSetHandler(my_timer_handler);

	/* enable global interrupts */
	_enable();

	printf("Tricore Demo @FPI:%u Hz CPU:%u Hz %u CoreType:%04X\n",
			get_fpi_frequency(),
			get_cpu_frequency(),
			g_ticks,
			__TRICORE_CORE__
	);
	/* Setup the hardware for use with the TriCore evaluation board. */
	prvSetupHardware();

	/* Start standard demo/test application flash tasks.  See the comments at
	the top of this file.  The LED flash tasks are always created.  The other
	tasks are only created if mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY is set to
	0 (at the top of this file).  See the comments at the top of this file for
	more information. */
//	vStartLEDFlashTasks( mainLED_TASK_PRIORITY );

	/* The following function will only create more tasks and timers if
	mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY is set to 0 (at the top of this
	file).  See the comments at the top of this file for more information. */
//	prvOptionallyCreateComprehensveTestApplication();

	xTaskCreate((TaskFunction_t )start_task,
	                (const char*    )"start_task",
	                (uint16_t       )512,
	                (void*          )NULL,
	                (UBaseType_t    )tskIDLE_PRIORITY + 1,
	                (TaskHandle_t*  )&StartTask_Handler);

	/* Now all the tasks have been started - start the scheduler. */
	vTaskStartScheduler();

	/* If all is well then the following line will never be reached.  If
	execution does reach here, then it is highly probably that the heap size
	is too small for the idle and/or timer tasks to be created within
	vTaskStartScheduler(). */

	while(1)
	{
		//		c = _in_uart();
		/* check flag set by timer ISR in every 100 ms */

		if(g_blink_flag)
		{
			printf("Failed. @FPI:%u Hz CPU:%u Hz %u CoreType:%04X\n",
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

#define	LED0_STACK_SIZE	256
StackType_t led0_stack[LED0_STACK_SIZE];
StaticTask_t led0_task_tcb;

#define	LED1_STACK_SIZE	256
StackType_t led1_stack[LED1_STACK_SIZE];
StaticTask_t led1_task_tcb;

static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t IdleTaskTCB;

static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];
static StaticTask_t TimerTaskTCB;

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
								   StackType_t **ppxIdleTaskStackBuffer,
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&IdleTaskTCB;
	*ppxIdleTaskStackBuffer=IdleTaskStack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
									StackType_t **ppxTimerTaskStackBuffer,
									uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer=&TimerTaskTCB;
	*ppxTimerTaskStackBuffer=TimerTaskStack;
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}

/*-----------------------------------------------------------*/
void start_task(void *pvParameters)
{
//    xTaskCreate((TaskFunction_t )led0_task,
//                (const char*    )"led0_task",
//                (uint16_t       )512,
//                (void*          )NULL,
//                (UBaseType_t    )tskIDLE_PRIORITY + 2,
//                (TaskHandle_t*  )&LED0Task_Handler);

    LED0Task_Handler=xTaskCreateStatic((TaskFunction_t	)led0_task,
										(const char* 	)"led0_task",
										(uint32_t 		)LED0_STACK_SIZE,
										(void* 		  	)NULL,
										(UBaseType_t 	)tskIDLE_PRIORITY + 2,
										(StackType_t*   )led0_stack,
										(StaticTask_t*  )&led0_task_tcb);

//    xTaskCreate((TaskFunction_t )led1_task,
//                (const char*    )"led1_task",
//                (uint16_t       )512,
//                (void*          )NULL,
//                (UBaseType_t    )tskIDLE_PRIORITY + 3,
//                (TaskHandle_t*  )&LED1Task_Handler);
    LED1Task_Handler=xTaskCreateStatic((TaskFunction_t	)led1_task,
										(const char* 	)"led1_task",
										(uint32_t 		)LED1_STACK_SIZE,
										(void* 		  	)NULL,
										(UBaseType_t 	)tskIDLE_PRIORITY + 3,
										(StackType_t*   )led1_stack,
										(StaticTask_t*  )&led1_task_tcb);

    xTaskCreate((TaskFunction_t )print_task,
                (const char*    )"print_task",
                (uint16_t       )512,
                (void*          )NULL,
                (UBaseType_t    )tskIDLE_PRIORITY + 4,
                (TaskHandle_t*  )&FLOATTask_Handler);
    vTaskDelete(StartTask_Handler);
}

void led0_task(void *pvParameters)
{
    while(1)
    {
    	vParTestToggleLED(0);
//    	portDISABLE_INTERRUPTS();
    	vTaskDelay(100 / portTICK_PERIOD_MS);
//    	portENABLE_INTERRUPTS();
    }
}

//Block Style Delay
void simple_delay(uint32_t t)
{
	for(uint32_t i=0; i<10000; ++i)
	{
		for(uint32_t j=0; j<t; ++j)
		{
			_nop();
		}
	}
}

void led1_task(void *pvParameters)
{
	uint32_t tmp_num = 0;

    while(1)
    {
        vParTestToggleLED(1);
//    	portDISABLE_INTERRUPTS();
//    	simple_delay(500);
//    	portENABLE_INTERRUPTS();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        tmp_num ++;
        if(10==tmp_num)
        {
        	vTaskDelete(LED0Task_Handler);
        }
    }
}

void print_task(void *pvParameters)
{
	while(1)
	{
		printf("FPI:%u Hz CPU:%u Hz %u CoreType:%04X\n",
				get_fpi_frequency(),
				get_cpu_frequency(),
				xTaskGetTickCount(),
				__TRICORE_CORE__
		);

//    	portDISABLE_INTERRUPTS();
//    	simple_delay(500);
//    	portENABLE_INTERRUPTS();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

static void prvCheckTask( void *pvParameters )
{
TickType_t xDelayPeriod = mainNO_ERROR_FLASH_PERIOD_MS;
TickType_t xLastExecutionTime;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	/* Initialise xLastExecutionTime so the first call to vTaskDelayUntil()
	works correctly. */
	xLastExecutionTime = xTaskGetTickCount();

	/* Cycle for ever, delaying then checking all the other tasks are still
	operating without error.  If an error is detected then the delay period
	is decreased from mainNO_ERROR_FLASH_PERIOD_MS to mainERROR_FLASH_PERIOD_MS so
	the on board LED flash rate will increase.  NOTE:  This task could easily
	be replaced by a software timer callback to remove the overhead of having
	an extra task. */

	for( ;; )
	{
		/* Delay until it is time to execute again. */
		vTaskDelayUntil( &xLastExecutionTime, xDelayPeriod );

		/* Check all the standard demo application tasks are executing without
		error.	*/
		if( prvCheckOtherTasksAreStillRunning() != pdPASS )
		{
			/* An error has been detected in one of the tasks - flash the LED
			at a higher frequency to give visible feedback that something has
			gone wrong (it might just be that the loop back connector required
			by the comtest tasks has not been fitted). */
			xDelayPeriod = mainERROR_FLASH_PERIOD_MS;
		}

		/* The toggle rate of the LED depends on how long this task delays for.
		An error reduces the delay period and so increases the toggle rate. */
		vParTestToggleLED( mainON_BOARD_LED_BIT );
	}
}
/*-----------------------------------------------------------*/

static long prvCheckOtherTasksAreStillRunning( void )
{
long lReturn = pdPASS;
unsigned long ulHighFrequencyTimerTaskIterations, ulExpectedIncFrequency_ms;

	/* Check all the demo tasks (other than the flash tasks) to ensure
	that they are all still running, and that none have detected an error. */

	if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if( xAreComTestTasksStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if( xAreDynamicPriorityTasksStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if( xAreBlockingQueuesStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if ( xAreBlockTimeTestTasksStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if ( xAreGenericQueueTasksStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if ( xAreRecursiveMutexTasksStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if( prvAreRegTestTasksStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if( xIsCreateTaskStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if( xAreTimerDemoTasksStillRunning( mainNO_ERROR_FLASH_PERIOD_MS ) != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if( xArePollingQueuesStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	if( xAreSemaphoreTasksStillRunning() != pdTRUE )
	{
		lReturn = pdFAIL;
	}

	/* Obtain the number of times the task associated with the high frequency
	(interrupt nesting) timer test has increment since the check task last
	executed, and the frequency at which it is expected to execute in ms. */
	ulHighFrequencyTimerTaskIterations = ulInterruptNestingTestGetIterationCount( &ulExpectedIncFrequency_ms );
	if( ( ulHighFrequencyTimerTaskIterations < ( ( mainNO_ERROR_FLASH_PERIOD_MS / ulExpectedIncFrequency_ms ) - 1 ) )
		||
		( ulHighFrequencyTimerTaskIterations > ( ( mainNO_ERROR_FLASH_PERIOD_MS / ulExpectedIncFrequency_ms ) +5 ) )
	  )
	{
		/* Would have expected the high frequency timer task to have
		incremented its execution count more times that reported. */
		lReturn = pdFAIL;
	}

	return lReturn;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
extern void set_cpu_frequency(void);

	/* Set-up the PLL. */
	set_cpu_frequency();

	/* Initialise LED outputs. */
	vParTestInitialise();
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	#if mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY != 1
	{
		/* vApplicationTickHook() will only be called if configUSE_TICK_HOOK is set
		to 1 in FreeRTOSConfig.h.  It is a hook function that will get called during
		each FreeRTOS tick interrupt.  Note that vApplicationTickHook() is called
		from an interrupt context. */

		/* Call the periodic timer test, which tests the timer API functions that
		can be called from an ISR. */
		vTimerPeriodicISRTests();
	}
	#endif /* mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY */
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

static portBASE_TYPE prvAreRegTestTasksStillRunning( void )
{
static unsigned long ulPreviousRegisterTest1Count = 0;
static unsigned long ulPreviousRegisterTest2Count = 0;
portBASE_TYPE xReturn = pdPASS;

	/* Check to see if the Counts have changed since the last check. */
	if( ulRegisterTest1Count == ulPreviousRegisterTest1Count )
	{
		xReturn = pdFAIL;
	}

	if( ulRegisterTest2Count == ulPreviousRegisterTest2Count )
	{
		xReturn = pdFAIL;
	}

	/* Remember the current count for the next time this function is called. */
	ulPreviousRegisterTest1Count = ulRegisterTest1Count;
	ulPreviousRegisterTest2Count = ulRegisterTest2Count;

	return xReturn;
}
/*-----------------------------------------------------------*/

static void prvOptionallyCreateComprehensveTestApplication( void )
{
	#if mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY == 0
	{
		vStartIntegerMathTasks( tskIDLE_PRIORITY );
		vStartDynamicPriorityTasks();
		vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
		vCreateBlockTimeTasks();
		vStartCountingSemaphoreTasks();
		vStartGenericQueueTasks( tskIDLE_PRIORITY );
		vStartRecursiveMutexTasks();
		vAltStartComTestTasks( mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE, mainCOM_TEST_LED );
		vSetupInterruptNestingTest();
		vStartTimerDemoTask( mainTIMER_TEST_PERIOD );
		vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
		vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );

		/* Create the register test tasks, as described at the top of this file. */
		xTaskCreate( prvRegisterCheckTask1, "Reg 1", configMINIMAL_STACK_SIZE, &ulRegisterTest1Count, tskIDLE_PRIORITY, NULL );
		xTaskCreate( prvRegisterCheckTask2, "Reg 2", configMINIMAL_STACK_SIZE, &ulRegisterTest2Count, tskIDLE_PRIORITY, NULL );

		/* Start the check task - which is defined in this file. */
		xTaskCreate( prvCheckTask, "Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );

		/* This task has to be created last as it keeps account of the number of tasks
		it expects to see running. */
		vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );
	}
	#else /* mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY */
	{
		/* Just to prevent compiler warnings when the configuration options are
		set such that these static functions are not used. */
		( void ) prvCheckTask;
		( void ) prvRegisterCheckTask1;
		( void ) prvRegisterCheckTask2;
	}
	#endif /* mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY */
}
/*-----------------------------------------------------------*/

static void prvRegisterCheckTask1( void *pvParameters )
{
	/* Make space on the stack for the parameter and a counter. */
	__asm volatile( " sub.a %sp, 4 			\n"
					" st.a [%sp], %a4		\n"
					" mov %d15, 0			\n"
					" st.w [%sp]4, %d15		\n" );

	/* Change all of the Context sensitive registers (except SP and RA). */
	__asm volatile(
			" mov %d0, 0		\n"
			" mov %d1, 1		\n"
			" mov %d2, 2		\n"
			" mov %d3, 3		\n"
			" mov %d4, 4		\n"
			" mov %d5, 5		\n"
			" mov %d6, 6		\n"
			" mov %d7, 7		\n"
			" mov %d8, 8		\n"
			" mov %d9, 9		\n"
			" mov %d10, 10		\n"
			" mov %d11, 11		\n"
			" mov %d12, 12		\n"
			" mov %d13, 13		\n"
			" mov %d14, 14		\n"
			" mov %d15, 15		\n"
			" mov.a %a2, 2		\n"
			" mov.a %a3, 3		\n"
			" mov.a %a4, 4		\n"
			" mov.a %a5, 5		\n"
			" mov.a %a6, 6		\n"
			" mov.a %a7, 7		\n"
			" mov.a %a12, 12	\n"
			" mov.a %a13, 13	\n"
			" mov.a %a14, 14	\n" );

	/* Check the values of the registers. */
	__asm(	" _task1_loop:							\n" \
			" eq %d1, %d0, 0						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d1, 1						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d2, 2						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d3, 3						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d4, 4						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d5, 5						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d6, 6						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d7, 7						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d8, 8						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d9, 9						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d10, 10						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d11, 11						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d12, 12						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d13, 13						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d14, 14						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" eq %d1, %d15, 15						\n" \
			" jne %d1, 1, _task1_error_loop			\n" \
			" mov.a %a15, 2							\n" \
			" jne.a %a15, %a2, _task1_error_loop	\n" \
			" mov.a %a15, 3							\n" \
			" jne.a %a15, %a3, _task1_error_loop	\n" \
			" mov.a %a15, 4							\n" \
			" jne.a %a15, %a4, _task1_error_loop	\n" \
			" mov.a %a15, 5							\n" \
			" jne.a %a15, %a5, _task1_error_loop	\n" \
			" mov.a %a15, 6							\n" \
			" jne.a %a15, %a6, _task1_error_loop	\n" \
			" mov.a %a15, 7							\n" \
			" jne.a %a15, %a7, _task1_error_loop	\n" \
			" mov.a %a15, 12						\n" \
			" jne.a %a15, %a12, _task1_error_loop	\n" \
			" mov.a %a15, 13						\n" \
			" jne.a %a15, %a13, _task1_error_loop	\n" \
			" mov.a %a15, 14						\n" \
			" jne.a %a15, %a14, _task1_error_loop	\n" \
			" j _task1_skip_error_loop				\n"	\
			"_task1_error_loop:						\n"	/* Hitting this error loop will stop the counter incrementing, allowing the check task to recognise an error. */ \
			"	debug								\n"	\
			" j _task1_error_loop					\n"	\
			"_task1_skip_error_loop:				\n" );

	/* Load the parameter address from the stack and modify the value. */
	__asm volatile(									\
			" ld.w %d1, [%sp]4						\n"	\
			" add %d1, 1							\n"	\
			" st.w [%sp]4, %d1						\n"	\
			" ld.a %a15, [%sp]						\n"	\
			" st.w [%a15], %d1						\n"	\
			" j _task1_loop							\n" );

	/* The parameter is used but in the assembly. */
	(void)pvParameters;
}
/*-----------------------------------------------------------*/

static void prvRegisterCheckTask2( void *pvParameters )
{
	/* Make space on the stack for the parameter and a counter. */
	__asm volatile( " sub.a %sp, 4 		\n" \
					" st.a [%sp], %a4	\n" \
					" mov %d15, 0		\n" \
					" st.w [%sp]4, %d15	\n" );

	/* Change all of the Context sensitive registers (except SP and RA). */
	__asm volatile(	" mov %d0, 7		\n" \
					" mov %d1, 1		\n" \
					" mov %d2, 5		\n" \
					" mov %d3, 4		\n" \
					" mov %d4, 3		\n" \
					" mov %d5, 2		\n" \
					" mov %d6, 1		\n" \
					" mov %d7, 0		\n" \
					" mov %d8, 15		\n" \
					" mov %d9, 14		\n" \
					" mov %d10, 13		\n" \
					" mov %d11, 12		\n" \
					" mov %d12, 11		\n" \
					" mov %d13, 10		\n" \
					" mov %d14, 9		\n" \
					" mov %d15, 8		\n" \
					" mov.a %a2, 14		\n" \
					" mov.a %a3, 13		\n" \
					" mov.a %a4, 12		\n" \
					" mov.a %a5, 7		\n" \
					" mov.a %a6, 6		\n" \
					" mov.a %a7, 5		\n" \
					" mov.a %a12, 4		\n" \
					" mov.a %a13, 3		\n" \
					" mov.a %a14, 2		\n" );

	/* Check the values of the registers. */
	__asm volatile(	" _task2_loop:							\n" \
					" syscall 0								\n" \
					" eq %d1, %d0, 7						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d1, 1						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d2, 5						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d3, 4						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d4, 3						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d5, 2						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d6, 1						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d7, 0						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d8, 15						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d9, 14						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d10, 13						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d11, 12						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d12, 11						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d13, 10						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d14, 9						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" eq %d1, %d15, 8						\n" \
					" jne %d1, 1, _task2_error_loop			\n" \
					" mov.a %a15, 14						\n" \
					" jne.a %a15, %a2, _task2_error_loop	\n" \
					" mov.a %a15, 13						\n" \
					" jne.a %a15, %a3, _task2_error_loop	\n" \
					" mov.a %a15, 12						\n" \
					" jne.a %a15, %a4, _task2_error_loop	\n" \
					" mov.a %a15, 7							\n" \
					" jne.a %a15, %a5, _task2_error_loop	\n" \
					" mov.a %a15, 6							\n" \
					" jne.a %a15, %a6, _task2_error_loop	\n" \
					" mov.a %a15, 5							\n" \
					" jne.a %a15, %a7, _task2_error_loop	\n" \
					" mov.a %a15, 4							\n" \
					" jne.a %a15, %a12, _task2_error_loop 	\n" \
					" mov.a %a15, 3							\n" \
					" jne.a %a15, %a13, _task2_error_loop	\n" \
					" mov.a %a15, 2							\n" \
					" jne.a %a15, %a14, _task2_error_loop	\n" \
					" j _task2_skip_error_loop				\n"	\
					"_task2_error_loop:						\n"	/* Hitting this error loop will stop the counter incrementing, allowing the check task to recognise an error. */ \
					" debug									\n" \
					" j _task2_error_loop					\n"	\
					"_task2_skip_error_loop:				\n"	);

	/* Load the parameter address from the stack and modify the value. */
	__asm volatile(	" ld.w %d1, [%sp]4						\n"	\
					" add %d1, %d1, 1						\n"	\
					" st.w [%sp]4, %d1						\n"	\
					" ld.a %a15, [%sp]						\n"	\
					" st.w [%a15], %d1						\n"	\
					" j _task2_loop                			\n"  );

	/* The parameter is used but in the assembly. */
	(void)pvParameters;
}

/*-----------------------------------------------------------*/
