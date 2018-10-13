//*****************************************************************************
// File: asm_call.S
// Compiler: HIGHTEC
//******************************************************************************
// uint32_t* Ifx_Get_A10(void);
//******************************************************************************

//External Reference
    .global Ifx_Get_A10

//Code Segment Declaration
 //    .section .text.dsplib, "ax"
#define     inputA       %d4
#define     inputB       %d5
#define     result_pointer    	%A2

//Executable Code
    .align 8
Ifx_Get_A10:
	mov.aa result_pointer, %A10
    ret
