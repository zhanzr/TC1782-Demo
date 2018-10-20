FreeRTOS Demo for TC1782.

FreeRTOS Example 28.

Content:

	Demo for Dhrystone benchmark.

	PSPR optimization.
	Cache optimization.

Benchmark code in .text section:

Program compiled with 'register' attribute

Please give the number of runs through the benchmark:
Execution starts, 600000 runs through Dhrystone
Execution ends

Final values of the variables used in the benchmark:

Int_Glob:            5
        should be:   5
Bool_Glob:           1
        should be:   1
Ch_1_Glob:           A
        should be:   A
Ch_2_Glob:           B
        should be:   B
Arr_1_Glob[8]:       7
        should be:   7
Arr_2_Glob[8][7]:    600010
        should be:   Number_Of_Runs + 10
Ptr_Glob->
  Ptr_Comp:          -805278496
        should be:   (implementation-dependent)
  Discr:             0
        should be:   0
  Enum_Comp:         2
        should be:   2
  Int_Comp:          17
        should be:   17
  Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
        should be:   DHRYSTONE PROGRAM, SOME STRING
Next_Ptr_Glob->
  Ptr_Comp:          -805278496
        should be:   (implementation-dependent), same as above
  Discr:             0
        should be:   0
  Enum_Comp:         1
        should be:   1
  Int_Comp:          18
        should be:   18
  Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
        should be:   DHRYSTONE PROGRAM, SOME STRING
Int_1_Loc:           5
        should be:   5
Int_2_Loc:           13
        should be:   13
Int_3_Loc:           7
        should be:   7
Enum_Loc:            1
        should be:   1
Str_1_Loc:           DHRYSTONE PROGRAM, 1'ST STRING
        should be:   DHRYSTONE PROGRAM, 1'ST STRING
Str_2_Loc:           DHRYSTONE PROGRAM, 2'ND STRING
        should be:   DHRYSTONE PROGRAM, 2'ND STRING

Microseconds for one run through Dhrystone[14-2601]:     2.6 
Dhrystones per Second:                      386548.1 

Benchmark code in PSPR:
Tricore 1782 @CPU:180 MHz Core:0131

Dhrystone Benchmark, Version 2.1 (Language: C)

Program compiled with 'register' attribute

Please give the number of runs through the benchmark: 
Execution starts, 1000000 runs through Dhrystone
Execution ends

Final values of the variables used in the benchmark:

Int_Glob:            5
        should be:   5
Bool_Glob:           1
        should be:   1
Ch_1_Glob:           A
        should be:   A
Ch_2_Glob:           B
        should be:   B
Arr_1_Glob[8]:       7
        should be:   7
Arr_2_Glob[8][7]:    1000010
        should be:   Number_Of_Runs + 10
Ptr_Glob->
  Ptr_Comp:          -805278496
        should be:   (implementation-dependent)
  Discr:             0
        should be:   0
  Enum_Comp:         2
        should be:   2
  Int_Comp:          17
        should be:   17
  Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
        should be:   DHRYSTONE PROGRAM, SOME STRING
Next_Ptr_Glob->
  Ptr_Comp:          -805278496
        should be:   (implementation-dependent), same as above
  Discr:             0
        should be:   0
  Enum_Comp:         1
        should be:   1
  Int_Comp:          18
        should be:   18
  Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
        should be:   DHRYSTONE PROGRAM, SOME STRING
Int_1_Loc:           5
        should be:   5
Int_2_Loc:           13
        should be:   13
Int_3_Loc:           7
        should be:   7
Enum_Loc:            1
        should be:   1
Str_1_Loc:           DHRYSTONE PROGRAM, 1'ST STRING
        should be:   DHRYSTONE PROGRAM, 1'ST STRING
Str_2_Loc:           DHRYSTONE PROGRAM, 2'ND STRING
        should be:   DHRYSTONE PROGRAM, 2'ND STRING

Microseconds for one run through Dhrystone[14-3063]:     3.0 
Dhrystones per Second:                      327976.4 


Tricore 1782 @CPU:180 MHz Core:0131
PMI_CON0        F87FFD10        :00000002
PMI_CON1        F87FFD14        :00000000
PMI_CON2        F87FFD18        :02800284
PMI_STR F87FFD20        :00000000
Icache_en 16
PMI_CON0        F87FFD10        :00000000
PMI_CON1        F87FFD14        :00000000
PMI_CON2        F87FFD18        :01840284
PMI_STR F87FFD20        :00000000
DMI_ID  F87FFC08        :0008C006
DMI_CON F87FFC10        :08000802
DMI_STR F87FFC18        :00000000
DMI_ATR F87FFC20        :00000000
Dcache_en 4
DMI_ID  F87FFC08        :0008C006
DMI_CON F87FFC10        :07C20802
DMI_STR F87FFC18        :00000000
DMI_ATR F87FFC20        :00000000

Dhrystone Benchmark, Version 2.1 (Language: C)

Program compiled with 'register' attribute

Please give the number of runs through the benchmark:
Execution starts, 2000000 runs through Dhrystone
Execution ends

Final values of the variables used in the benchmark:

Int_Glob:            5
        should be:   5
Bool_Glob:           1
        should be:   1
Ch_1_Glob:           A
        should be:   A
Ch_2_Glob:           B
        should be:   B
Arr_1_Glob[8]:       7
        should be:   7
Arr_2_Glob[8][7]:    2000010
        should be:   Number_Of_Runs + 10
Ptr_Glob->
  Ptr_Comp:          -805278496
        should be:   (implementation-dependent)
  Discr:             0
        should be:   0
  Enum_Comp:         2
        should be:   2
  Int_Comp:          17
        should be:   17
  Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
        should be:   DHRYSTONE PROGRAM, SOME STRING
Next_Ptr_Glob->
  Ptr_Comp:          -805278496
        should be:   (implementation-dependent), same as above
  Discr:             0
        should be:   0
  Enum_Comp:         1
        should be:   1
  Int_Comp:          18
        should be:   18
  Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
        should be:   DHRYSTONE PROGRAM, SOME STRING
Int_1_Loc:           5
        should be:   5
Int_2_Loc:           13
        should be:   13
Int_3_Loc:           7
        should be:   7
Enum_Loc:            1
        should be:   1
Str_1_Loc:           DHRYSTONE PROGRAM, 1'ST STRING
        should be:   DHRYSTONE PROGRAM, 1'ST STRING
Str_2_Loc:           DHRYSTONE PROGRAM, 2'ND STRING
        should be:   DHRYSTONE PROGRAM, 2'ND STRING

Microseconds for one run through Dhrystone[41-3231]:     1.6
Dhrystones per Second:                      626959.2
PMI_CON0        F87FFD10        :00000000
PMI_CON1        F87FFD14        :00000000
PMI_CON2        F87FFD18        :01840284
PMI_STR F87FFD20        :00000000
Icache_en 0
PMI_CON0        F87FFD10        :00000002
PMI_CON1        F87FFD14        :00000000
PMI_CON2        F87FFD18        :02800284
PMI_STR F87FFD20        :00000000
DMI_ID  F87FFC08        :0008C006
DMI_CON F87FFC10        :07C20802
DMI_STR F87FFC18        :00000000
DMI_ATR F87FFC20        :00000000
Dcache_en 0
DMI_ID  F87FFC08        :0008C006
DMI_CON F87FFC10        :08000802
DMI_STR F87FFC18        :00000000
DMI_ATR F87FFC20        :00000000

Dhrystone Benchmark, Version 2.1 (Language: C)

Program compiled with 'register' attribute

Please give the number of runs through the benchmark:
Execution starts, 2000000 runs through Dhrystone
Execution ends

Final values of the variables used in the benchmark:

Int_Glob:            5
        should be:   5
Bool_Glob:           1
        should be:   1
Ch_1_Glob:           A
        should be:   A
Ch_2_Glob:           B
        should be:   B
Arr_1_Glob[8]:       7
        should be:   7
Arr_2_Glob[8][7]:    2000010
        should be:   Number_Of_Runs + 10
Ptr_Glob->
  Ptr_Comp:          -805278088
        should be:   (implementation-dependent)
  Discr:             0
        should be:   0
  Enum_Comp:         2
        should be:   2
  Int_Comp:          17
        should be:   17
  Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
        should be:   DHRYSTONE PROGRAM, SOME STRING
Next_Ptr_Glob->
  Ptr_Comp:          -805278088
        should be:   (implementation-dependent), same as above
  Discr:             0
        should be:   0
  Enum_Comp:         1
        should be:   1
  Int_Comp:          18
        should be:   18
  Str_Comp:          DHRYSTONE PROGRAM, SOME STRING
        should be:   DHRYSTONE PROGRAM, SOME STRING
Int_1_Loc:           5
        should be:   5
Int_2_Loc:           13
        should be:   13
Int_3_Loc:           7
        should be:   7
Enum_Loc:            1
        should be:   1
Str_1_Loc:           DHRYSTONE PROGRAM, 1'ST STRING
        should be:   DHRYSTONE PROGRAM, 1'ST STRING
Str_2_Loc:           DHRYSTONE PROGRAM, 2'ND STRING
        should be:   DHRYSTONE PROGRAM, 2'ND STRING

Microseconds for one run through Dhrystone[3364-8431]:     2.5
Dhrystones per Second:                      394710.9

