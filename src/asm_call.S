//*****************************************************************************
// File: asm_call.S
// Compiler: HIGHTEC
//******************************************************************************
// uint32_t* Ifx_Get_A10(void);
// uint32_t Ifx_Get_D5(void);
// div_t Ifx_Div(int32_t inputA, int32_t inputB);

// div_t Ifx_Div_test1(int32_t inputA, int32_t inputB);
// div_t Ifx_Div_test2(int32_t inputA, int32_t inputB);
// div_t Ifx_Div_test3(int32_t inputA, int32_t inputB);
// div_t Ifx_Div_test4(int32_t inputA, int32_t inputB);
// div_t Ifx_Div_test5(int32_t inputA, int32_t inputB);
//******************************************************************************

//External Reference
    .global Ifx_Get_A10
    .global Ifx_Get_D5

    .global Ifx_Div
    .global Ifx_Div_test1
    .global Ifx_Div_test2
    .global Ifx_Div_test3
    .global Ifx_Div_test4
    .global Ifx_Div_test5

//Code Segment Declaration
 //    .section .text.dsplib, "ax"
#define     inputA       %d4
#define     inputB       %d5
#define     result       %d2

#define     result_pointer    	%A2
#define     result64    	%E2

//Executable Code
    .align 8
Ifx_Get_A10:
	mov.aa result_pointer, %A10
    ret

Ifx_Get_D5:
	mov result, inputB
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

Ifx_Div_test1:
	dvinit result64, inputA, inputB
	ret

Ifx_Div_test2:
	dvinit result64, inputA, inputB
	dvstep result64, result64, inputB
	ret

Ifx_Div_test3:
	dvinit result64, inputA, inputB
	dvstep result64, result64, inputB
	dvstep result64, result64, inputB
	ret

Ifx_Div_test4:
	dvinit result64, inputA, inputB
	dvstep result64, result64, inputB
	dvstep result64, result64, inputB
	dvstep result64, result64, inputB
	ret

Ifx_Div_test5:
	dvinit result64, inputA, inputB
	dvstep result64, result64, inputB
	dvstep result64, result64, inputB
	dvstep result64, result64, inputB
	dvstep result64, result64, inputB
	ret
