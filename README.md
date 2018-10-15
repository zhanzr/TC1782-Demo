FreeRTOS Demo for TC1782.

FreeRTOS Example 25.

Content:

	Demo for Division Instruction.

	//Divide
Ifx_Div:
    dvinit result64, inputA, inputB
    dvstep result64, result64, inputB
    dvstep result64, result64, inputB
    dvstep result64, result64, inputB
    dvstep result64, result64, inputB
    dvadj result64, result64, inputB
    ret

typedef union union_pack64
{
	uint64_t u64;
	int64_t i64;
	uint32_t u32[2];
	int32_t i32[2];
	uint16_t u16[4];
	int16_t i16[4];
	uint8_t u8[8];
	int8_t i8[8];
}pack64;

pack64 Ifx_Div(int32_t inputA, int32_t inputB);

	{
		int32_t ai_32 = 2000;
		int32_t bi_32 = 3;
		int32_t ci_32 = ai_32/bi_32;
		int32_t di_32 = ai_32%bi_32;
		printf("%i / %i  = %i, %i\n",
				ai_32, bi_32, ci_32, di_32);
	}

	{
		int16_t ai_16 = 1000;
		int16_t bi_16 = 6;
		int16_t ci_16 = ai_16/bi_16;
		int16_t di_16 = ai_16%bi_16;
		printf("%i / %i  = %i, %i\n",
				ai_16, bi_16, ci_16, di_16);
	}

	{
		int32_t ai_32 = 2000;
		int32_t bi_32 = 3;
		pack64 res_p_64;
		res_p_64 = Ifx_Div(ai_32, bi_32);
		printf("%i / %i  = %i, %i\n",
				ai_32, bi_32, res_p_64.i32[0], res_p_64.i32[1]);
	}

	{
		int32_t ai_32 = 1000;
		int32_t bi_32 = 6;
		pack64 res_p_64;
		res_p_64 = Ifx_Div(ai_32, bi_32);
		printf("%i / %i  = %i, %i\n",
				ai_32, bi_32, res_p_64.i32[0], res_p_64.i32[1]);
	}



	2000 / 3  = 666, 2
1000 / 6  = 166, 4
2000 / 3  = 666, 2
1000 / 6  = 166, 4



