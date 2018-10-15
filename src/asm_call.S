//*****************************************************************************
// File: asm_call.S
// Compiler: HIGHTEC
//******************************************************************************
// uint32_t* Ifx_Get_A10(void);
// pack64 Ifx_Div(int32_t inputA, int32_t inputB);
//******************************************************************************

//External Reference
    .global Ifx_Get_A10
    .global Ifx_Div

//Code Segment Declaration
 //    .section .text.dsplib, "ax"
#define     inputA       %d4
#define     inputB       %d5
#define     result_pointer    	%A2
#define     result64    	%E2

//Executable Code
    .align 8
Ifx_Get_A10:
	mov.aa result_pointer, %A10
    ret

//Divide
Ifx_Div:
	dvinit result64, inputA, inputB
	dvstep result64, result64, inputB
	dvstep result64, result64, inputB
	dvstep result64, result64, inputB
	dvstep result64, result64, inputB
	dvadj result64, result64, inputB
	ret
