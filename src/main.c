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
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

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

/* TriCore specific includes. */
#include <tc1782.h>
#include <machine/intrinsics.h>
#include <machine/cint.h>
#include <machine/wdtcon.h>

#define MESSAGE_Q_NUM   2
QueueHandle_t Message_Queue;

SemaphoreHandle_t BinarySemaphore;

SemaphoreHandle_t CountSemaphore;

SemaphoreHandle_t MutexSemaphore;

TimerHandle_t 	AutoReloadTimer_Handle;
TimerHandle_t	OneShotTimer_Handle;

EventGroupHandle_t EventGroupHandler;
#define EVENTBIT_0	(1<<0)
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)
#define EVENTBIT_ALL	(EVENTBIT_0|EVENTBIT_1|EVENTBIT_2)
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

TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

TaskHandle_t g_task0_handler;
void led0_task(void *pvParameters);

TaskHandle_t g_task1_handler;
void led1_task(void *pvParameters);

TaskHandle_t g_info_task_handler;
void print_task(void *pvParameters);

void enable_performance_cnt(void)
{
	unlock_wdtcon();
	{
		CCTRL_t tmpCCTRL;
		tmpCCTRL.reg = _mfcr(CCTRL_ADDR);

		//Instruction Cache Hit Count
		//		tmpCCTRL.bits.M1 = 2;
		//Data Cache Hit Count.
		tmpCCTRL.bits.M1 = 3;
		//Instruction Cache Miss Count

		//		tmpCCTRL.bits.M2 = 1;
		//Data Cache Clean Miss Count
		tmpCCTRL.bits.M2 = 3;

		//Data Cache Dirty Miss Count
		tmpCCTRL.bits.M3 = 3;

		//Normal Mode
		tmpCCTRL.bits.CM = 0;
		//Task Mode
		//		tmpCCTRL.bits.CM = 1;

		tmpCCTRL.bits.CE = 1;

		_mtcr(CCTRL_ADDR, tmpCCTRL.reg);
	}
	lock_wdtcon();
}


int main(void)
{
	_init_uart(mainCOM_TEST_BAUD_RATE);

	InitLED();

	enable_performance_cnt();

	//	/* initialise timer at SYSTIME_CLOCK rate */
	//	TimerInit(SYSTIME_CLOCK);
	//	/* add own handler for timer interrupts */
	//	TimerSetHandler(my_timer_handler);

	/* enable global interrupts */
	_enable();

	printf("Tricore %04X %s @CPU:%u MHz Core:%04X\n",
			__TRICORE_NAME__,
			tskKERNEL_VERSION_NUMBER,
			get_cpu_frequency()/1000000,
			__TRICORE_CORE__
	);

	/* Setup the hardware for use with the TriCore evaluation board. */
	prvSetupHardware();

	//	ConfigureTimeForRunTimeStats();

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
		//		//		c = _in_uart();
		//		/* check flag set by timer ISR in every 100 ms */
		//
		//		if(g_blink_flag)
		//		{
		//			printf("Failed. @FPI:%u Hz CPU:%u Hz %u CoreType:%04X\n",
		//					get_fpi_frequency(),
		//					get_cpu_frequency(),
		//					g_ticks,
		//					__TRICORE_CORE__
		//			);
		//
		//			LEDTOGGLE(0);
		//			LEDTOGGLE(1);
		//			//			LEDTOGGLE(2);
		//			//			LEDTOGGLE(3);
		//			//			LEDTOGGLE(4);
		//			//			LEDTOGGLE(5);
		//			//			LEDTOGGLE(6);
		//			//			LEDTOGGLE(7);
		//			g_blink_flag = false;
		//		}
	}
	return EXIT_SUCCESS;
}
/*-----------------------------------------------------------*/
void AutoReloadCallback(TimerHandle_t xTimer)
{
	if(NULL != OneShotTimer_Handle)
	{
		xTimerStart(OneShotTimer_Handle,0);
	}

	if(EventGroupHandler!=NULL)
	{
		xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);
	}

	//	xTaskNotify( g_task0_handler, 1, eSetValueWithOverwrite);
	//	xTaskNotify( g_task1_handler, 2, eSetValueWithOverwrite);
	xTaskNotify( g_info_task_handler, 3, eSetValueWithOverwrite);
}

void OneShotCallback(TimerHandle_t xTimer)
{
	char info_buf[256];

	vParTestToggleLED(0);
	vParTestToggleLED(1);

	if(EventGroupHandler!=NULL)
	{
		xEventGroupSetBits(EventGroupHandler,EVENTBIT_0);
	}

	if(NULL != BinarySemaphore)
	{
		if(pdTRUE == xSemaphoreTake(BinarySemaphore, portMAX_DELAY))
		{
			vTaskList(info_buf);
			if(NULL != MutexSemaphore)
			{
				if(pdTRUE == xSemaphoreTake(MutexSemaphore, portMAX_DELAY))
				{
					//					printf("%s,TaskList Len:%d\r\n",
					//							pcTaskGetName(NULL),
					//							strlen(info_buf));
					//					printf("%s\r\n",info_buf);

					xSemaphoreGive(MutexSemaphore);
				}
			}

			xSemaphoreGive(BinarySemaphore);
		}
	}
}

void start_task(void *pvParameters)
{
	Message_Queue = xQueueCreate(MESSAGE_Q_NUM, sizeof(uint32_t));

	CountSemaphore = xSemaphoreCreateCounting(2, 2);

	MutexSemaphore = xSemaphoreCreateMutex();

	AutoReloadTimer_Handle=xTimerCreate((const char*		)"AT",
			(TickType_t			)1000 / portTICK_PERIOD_MS,
			(UBaseType_t		)pdTRUE,
			(void*				)1,
			(TimerCallbackFunction_t)AutoReloadCallback);

	OneShotTimer_Handle=xTimerCreate((const char*			)"OT",
			(TickType_t			)500 / portTICK_PERIOD_MS,
			(UBaseType_t			)pdFALSE,
			(void*					)2,
			(TimerCallbackFunction_t)OneShotCallback);

	EventGroupHandler = xEventGroupCreate();

	if(NULL != AutoReloadTimer_Handle)
	{
		xTimerStart(AutoReloadTimer_Handle, 0);
	}

	xTaskCreate((TaskFunction_t )led0_task,
			(const char*    )"led0_task",
			(uint16_t       )768,
			(void*          )NULL,
			(UBaseType_t    )tskIDLE_PRIORITY + 2,
			(TaskHandle_t*  )&g_task0_handler);

	xTaskCreate((TaskFunction_t )led1_task,
			(const char*    )"led1_task",
			(uint16_t       )768,
			(void*          )NULL,
			(UBaseType_t    )tskIDLE_PRIORITY + 2,
			(TaskHandle_t*  )&g_task1_handler);

	xTaskCreate((TaskFunction_t )print_task,
			(const char*    )"print_task",
			(uint16_t       )2048,
			(void*          )NULL,
			(UBaseType_t    )tskIDLE_PRIORITY + 2,
			(TaskHandle_t*  )&g_info_task_handler);
	vTaskDelete(StartTask_Handler);
}

void led0_task(void *pvParameters)
{
	char info_buf[512];
	EventBits_t EventValue;

	while(1)
	{
		//    	vParTestToggleLED(0);
		//		vTaskDelay(4000 / portTICK_PERIOD_MS);
		if(EventGroupHandler!=NULL)
		{
			EventValue = xEventGroupGetBits(EventGroupHandler);
			if(NULL != MutexSemaphore)
			{
				if(pdTRUE == xSemaphoreTake(MutexSemaphore, portMAX_DELAY))
				{
					//					printf("<%s,ev:%08X\n",
					//							pcTaskGetName(NULL),
					//							(uint32_t)EventValue);
					xSemaphoreGive(MutexSemaphore);
				}
			}

			EventValue = xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler,
					(EventBits_t			)EVENTBIT_0,
					(BaseType_t			)pdTRUE,
					(BaseType_t			)pdTRUE,
					(TickType_t			)portMAX_DELAY);

			EventValue = xEventGroupGetBits(EventGroupHandler);
			if(NULL != MutexSemaphore)
			{
				if(pdTRUE == xSemaphoreTake(MutexSemaphore, portMAX_DELAY))
				{
					//					printf(">%s,ev:%08X\n",
					//							pcTaskGetName(NULL),
					//							(uint32_t)EventValue);
					xSemaphoreGive(MutexSemaphore);
				}
			}
		}

		uint32_t NotifyValue=ulTaskNotifyTake( pdTRUE, /* Clear the notification value on exit. */
				portMAX_DELAY );/* Block indefinitely. */

		vTaskList(info_buf);
		if(NULL != MutexSemaphore)
		{
			if(pdTRUE == xSemaphoreTake(MutexSemaphore, portMAX_DELAY))
			{
				printf("%s,TaskList Len:%d, %08X\r\n",
						pcTaskGetName(NULL),
						strlen(info_buf),
						NotifyValue);
				printf("%s\r\n",info_buf);

				xSemaphoreGive(MutexSemaphore);
			}
		}
	}
}

void led1_task(void *pvParameters)
{
	char info_buf[512];
	EventBits_t EventValue;

	while(1)
	{
		//    	vParTestToggleLED(1);
		//		vTaskDelay(4000 / portTICK_PERIOD_MS);
		if(EventGroupHandler!=NULL)
		{
			EventValue = xEventGroupGetBits(EventGroupHandler);
			if(NULL != MutexSemaphore)
			{
				if(pdTRUE == xSemaphoreTake(MutexSemaphore, portMAX_DELAY))
				{
					//					printf("<%s,ev:%08X\n",
					//							pcTaskGetName(NULL),
					//							(uint32_t)EventValue);
					xSemaphoreGive(MutexSemaphore);
				}
			}

			EventValue = xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler,
					(EventBits_t			)EVENTBIT_1,
					(BaseType_t			)pdTRUE,
					(BaseType_t			)pdTRUE,
					(TickType_t			)portMAX_DELAY);

			EventValue = xEventGroupGetBits(EventGroupHandler);
			if(NULL != MutexSemaphore)
			{
				if(pdTRUE == xSemaphoreTake(MutexSemaphore, portMAX_DELAY))
				{
					//					printf(">%s,ev:%08X\n",
					//							pcTaskGetName(NULL),
					//							(uint32_t)EventValue);
					xSemaphoreGive(MutexSemaphore);
				}
			}
		}

		if(NULL != Message_Queue)
		{
			uint32_t tmpTicks = xTaskGetTickCount();
			xQueueSend(Message_Queue, &tmpTicks, 0);
		}

		uint32_t NotifyValue=ulTaskNotifyTake( pdTRUE, /* Clear the notification value on exit. */
				portMAX_DELAY );/* Block indefinitely. */

		vTaskList(info_buf);
		if(NULL != MutexSemaphore)
		{
			if(pdTRUE == xSemaphoreTake(MutexSemaphore, portMAX_DELAY))
			{
				printf("%s,TaskList Len:%d, %08X\r\n",
						pcTaskGetName(NULL),
						strlen(info_buf),
						NotifyValue);
				printf("%s\r\n",info_buf);

				xSemaphoreGive(MutexSemaphore);
			}
		}


	}
}

typedef struct _Hnd_arg
{
	void (*hnd_handler) (int);
	int hnd_arg;
} Hnd_arg;

void print_task(void *pvParameters)
{
	char info_buf[512];

	while(1)
	{
		//		vTaskDelay(4000 / portTICK_PERIOD_MS);
		//		if(NULL != Message_Queue)
		//		{
		//			uint32_t tmpU32;
		//			if(xQueueReceive(Message_Queue, &tmpU32, portMAX_DELAY))
		//			{
		//				mutex_printf("%08X\n", (uint32_t)&Message_Queue);
		//				mutex_printf("CPU:%u Hz %u %u\n",
		//						get_cpu_frequency(),
		//						xTaskGetTickCount(),
		//						tmpU32
		//				);
		//			}
		//		}
		//		else
		//		{
		//			mutex_printf("%08X\n", (uint32_t)&Message_Queue);
		//		    vTaskDelay(1000 / portTICK_PERIOD_MS);
		//		}

		uint32_t NotifyValue = ulTaskNotifyTake( pdTRUE, /* Clear the notification value on exit. */
				portMAX_DELAY );/* Block indefinitely. */

		vTaskList(info_buf);
		if(NULL != MutexSemaphore)
		{
			if(pdTRUE == xSemaphoreTake(MutexSemaphore, portMAX_DELAY))
			{
				printf("%s,TaskList Len:%d, %08X\r\n",
						pcTaskGetName(NULL),
						strlen(info_buf),
						NotifyValue);
				printf("%s\r\n",info_buf);

				vTaskGetRunTimeStats(info_buf);
				printf("RunTimeStats Len:%d\r\n", strlen(info_buf));
				printf("%s\r\n",info_buf);

				//				uint8_t* buffer;
				//				uint32_t tmpA, tmpB, tmpC, tmpD;
				//				tmpA = xPortGetFreeHeapSize();
				//				buffer = pvPortMalloc(1024);
				//				tmpB = xPortGetFreeHeapSize();
				//				if(buffer!=NULL)
				//				{
				//					vPortFree(buffer);
				//					tmpC = xPortGetFreeHeapSize();
				//					buffer = NULL;
				//				}
				//				printf("FreeMem3:\t %u %u %u\n", tmpA, tmpB, tmpC);

				//				printf("STM_CLC\t%08X\t:%08X\n\n", &STM_CLC, STM_CLC.reg);
				//				printf("STM_ID\t%08X\t:%08X\n\n", &STM_ID, STM_ID.reg);
				//				printf("STM_TIM0\t%08X\t:%08X\n\n", &STM_TIM0, STM_TIM0.reg);
				//				printf("STM_TIM1\t%08X\t:%08X\n\n", &STM_TIM1, STM_TIM1.reg);
				//				printf("STM_TIM2\t%08X\t:%08X\n\n", &STM_TIM2, STM_TIM2.reg);
				//				printf("STM_TIM3\t%08X\t:%08X\n\n", &STM_TIM3, STM_TIM3.reg);
				//				printf("STM_TIM4\t%08X\t:%08X\n\n", &STM_TIM4, STM_TIM4.reg);
				//				printf("STM_TIM5\t%08X\t:%08X\n\n", &STM_TIM5, STM_TIM5.reg);
				//				printf("STM_TIM6\t%08X\t:%08X\n\n", &STM_TIM6, STM_TIM6.reg);
				//				printf("STM_CAP\t%08X\t:%08X\n\n", &STM_CAP, STM_CAP.reg);
				//				printf("STM_CMP0\t%08X\t:%08X\n\n", &STM_CMP0, STM_CMP0.reg);
				//				printf("STM_CMP1\t%08X\t:%08X\n\n", &STM_CMP1, STM_CMP1.reg);
				//				printf("STM_CMCON\t%08X\t:%08X\n\n", &STM_CMCON, STM_CMCON.reg);
				//				printf("STM_ICR\t%08X\t:%08X\n\n", &STM_ICR, STM_ICR.reg);
				//				printf("STM_ISRR\t%08X\t:%08X\n\n", &STM_ISRR, STM_ISRR.reg);
				//				printf("STM_SRC1\t%08X\t:%08X\n\n", &STM_SRC1, STM_SRC1.reg);
				//				printf("STM_SRC0\t%08X\t:%08X\n\n", &STM_SRC0, STM_SRC0.reg);

				printf("CPUID\t%08X\t:%08X\n\n", &CPU_ID, _mfcr(CPU_ID_ADDR));
				printf("CCTRL\t%08X\t:%08X\n\n", &CCTRL, _mfcr(CCTRL_ADDR));
				printf("CCNT\t%08X\t:%08X\n\n", &CCNT, _mfcr(CCNT_ADDR));
				printf("ICNT\t%08X\t:%08X\n\n", &ICNT, _mfcr(ICNT_ADDR));
				printf("M1CNT\t%08X\t:%08X\n\n", &M1CNT, _mfcr(M1CNT_ADDR));
				printf("M2CNT\t%08X\t:%08X\n\n", &M2CNT, _mfcr(M2CNT_ADDR));
				printf("M3CNT\t%08X\t:%08X\n\n", &M3CNT, _mfcr(M3CNT_ADDR));

				extern void _start(void);
				printf("_start\t:%08X\n\n", (uint32_t)_start);
				extern void __EBMCFG(void);
				printf("__EBMCFG\t:%08X\n\n", (uint32_t)__EBMCFG);

				extern void __USTACK_BEGIN(void);
				printf("__USTACK_BEGIN\t:%08X\n\n", (uint32_t)__USTACK_BEGIN);
				extern void __USTACK(void);
				printf("__USTACK\t:%08X\n\n", (uint32_t)__USTACK);
				extern void __USTACK_SIZE(void);
				printf("__USTACK_SIZE\t:%08X\n\n", (uint32_t)__USTACK_SIZE);
				extern void __USTACK_END(void);
				printf("__USTACK_END\t:%08X\n\n", (uint32_t)__USTACK_END);

				extern void __ISTACK_BEGIN(void);
				printf("__ISTACK_BEGIN\t:%08X\n\n", (uint32_t)__ISTACK_BEGIN);
				extern void __ISTACK(void);
				printf("__ISTACK\t:%08X\n\n", (uint32_t)__ISTACK);
				extern void __ISTACK_SIZE(void);
				printf("__ISTACK_SIZE\t:%08X\n\n", (uint32_t)__ISTACK_SIZE);
				extern void __ISTACK_END(void);
				printf("__ISTACK_END\t:%08X\n\n", (uint32_t)__ISTACK_END);

				extern void __HEAP_BEGIN(void);
				printf("__HEAP_BEGIN\t:%08X\n\n", (uint32_t)__HEAP_BEGIN);
				extern void __HEAP(void);
				printf("__HEAP\t:%08X\n\n", (uint32_t)__HEAP);
				extern void __HEAP_SIZE(void);
				printf("__HEAP_SIZE\t:%08X\n\n", (uint32_t)__HEAP_SIZE);
				extern void __HEAP_END(void);
				printf("__HEAP_END\t:%08X\n\n", (uint32_t)__HEAP_END);

				extern void __CSA_BEGIN(void);
				printf("__CSA_BEGIN\t:%08X\n\n", (uint32_t)__CSA_BEGIN);
				extern void __CSA(void);
				printf("__CSA\t:%08X\n\n", (uint32_t)__CSA);
				extern void __CSA_SIZE(void);
				printf("__CSA_SIZE\t:%08X\n\n", (uint32_t)__CSA_SIZE);
				extern void __CSA_END(void);
				printf("__CSA_END\t:%08X\n\n", (uint32_t)__CSA_END);

				extern void _SMALL_DATA_(void);
				printf("_SMALL_DATA_\t:%08X\n\n", (uint32_t)_SMALL_DATA_);
				extern void _SMALL_DATA2_(void);
				printf("_SMALL_DATA2_\t:%08X\n\n", (uint32_t)_SMALL_DATA2_);

				extern void boardSetupTab(void);
				printf("boardSetupTab\t:%08X\n\n", (uint32_t)boardSetupTab);
				extern void boardSetupTabSize(void);
				printf("boardSetupTabSize\t:%08X\n\n", (uint32_t)boardSetupTabSize);
				extern void __board_init(void);
				printf("__board_init\t:%08X\n\n", (uint32_t)__board_init);
				extern void first_trap_table(void);
				printf("first_trap_table\t:%08X\n\n", (uint32_t)first_trap_table);

				printf("BIV\t%08X\t:%08X\n\n", &BIV, _mfcr(BIV_ADDR));
				extern void TriCore_int_table(void);
				printf("TriCore_int_table\t:%08X\n\n", (uint32_t)TriCore_int_table);

				extern void __interrupt_1(void);
				printf("__interrupt_1\t:%08X\n\n", (uint32_t)__interrupt_1);
				extern void ___interrupt_1(void);
				printf("___interrupt_1\t:%08X\n\n", (uint32_t)___interrupt_1);
				extern void __interrupt_2(void);
				printf("__interrupt_2\t:%08X\n\n", (uint32_t)__interrupt_2);

				extern Hnd_arg Cdisptab[MAX_INTRS];
				printf("Soft Interrupt vector table %08X:%u * %u = %u\n",
						(uint32_t)Cdisptab,
						sizeof(Cdisptab[0]),
						MAX_INTRS,
						sizeof(Cdisptab));

				printf("BTV\t%08X\t:%08X\n\n", &BTV, _mfcr(BTV_ADDR));
				extern void TriCore_trap_table(void);
				printf("TriCore_trap_table\t:%08X\n\n", (uint32_t)TriCore_trap_table);

				extern void __trap_0(void);
				printf("__trap_0\t:%08X\n\n", (uint32_t)__trap_0);
				extern void __trap_1(void);
				printf("__trap_1\t:%08X\n\n", (uint32_t)__trap_1);
				extern void __trap_6(void);
				printf("__trap_6\t:%08X\n\n", (uint32_t)__trap_6);
				extern void ___trap_6(void);
				printf("___trap_6\t:%08X\n\n", (uint32_t)___trap_6);

				extern void (*Tdisptab[MAX_TRAPS]) (int tin);
				printf("Soft Trap vector table %08X:%u * %u = %u\n",
						(uint32_t)Tdisptab,
						sizeof(Tdisptab[0]),
						MAX_TRAPS,
						sizeof(Tdisptab));

				xSemaphoreGive(MutexSemaphore);
			}
		}

		//        if(NULL != CountSemaphore)
		//        {
		//        	if(pdTRUE == xSemaphoreTake(CountSemaphore, portMAX_DELAY))
		//        	{
		//        		vTaskList(info_buf);
		//        		printf("TaskList Len:%d\r\n", strlen(info_buf));
		//        		printf("%s\r\n",info_buf);
		//
		//        		vTaskGetRunTimeStats(info_buf);
		//        		printf("RunTimeStats Len:%d\r\n", strlen(info_buf));
		//        		printf("%s\r\n",info_buf);
		//
		//        		uint32_t tmp_sema_cnt = uxSemaphoreGetCount(CountSemaphore);
		//        		printf("SemaCnt %08X: %d\n", (uint32_t)&CountSemaphore, tmp_sema_cnt);
		//                xSemaphoreGive(CountSemaphore);
		//                tmp_sema_cnt = uxSemaphoreGetCount(CountSemaphore);
		//        		printf("SemaCnt %08X: %d\n", (uint32_t)&CountSemaphore, tmp_sema_cnt);
		//        	}
		//        }
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
