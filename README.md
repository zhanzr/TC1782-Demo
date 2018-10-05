FreeRTOS Demo for TC1782.

FreeRTOS Example 2.

Content:
        configMAX_SYSCALL_INTERRUPT_PRIORITY

        Defines the maximum interrupt priority from which FreeRTOS API functions can be called. Interrupts at or below this priority can call FreeRTOS API functions provided that the API function ends in 'FromISR'. Interrupts above this priority cannot call any FreeRTOS API functions, but will not be disabled by the RTOS kernel (over an above the limitations imposed by the architecture mentioned at the top of this file). Interrupts that have a priority above configMAX_SYSCALL_INTERRUPT_PRIORITY are therefore suitable for functionality that requires very high timing accuracy.
