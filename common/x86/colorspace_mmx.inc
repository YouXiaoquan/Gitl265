


%define					PTR_SIZE					4
%define					_EAX						EAX
%define					_EBX						EBX
%define					_ECX						ECX
%define					_EDX						EDX
%define					_ESI						ESI
%define					_EDI						EDI
%define					_ESP						ESP
%define					_EBP						EBP


%define					TMP1						_EDX
%define					u_ptr						_EBX




%ifdef WINDOWS
	%define PREFIX
%endif


%macro cglobal 1
%ifdef PREFIX
	global _%1
	%define %1 _%1
	%define ENDFUNC
%else
	global %1
	%define ENDFUNC
%endif
%endmacro




%macro MAKE_COLORSPACE 8

%define					NAME					%1
%define					STACK					%2
%define					BYTES					%3
%define					PIXELS					%4
%define					VPIXELS					%5
%define					FUNC					%6
%define					ARG1					%7
%define					ARG2					%8


%define					localsize				5*PTR_SIZE + STACK
%define					pushsize				4*PTR_SIZE

%define					y_stride				_EAX
%define					u_ptr   				_EBX
%define					v_ptr			   		_ECX
%define					x_stride				_EDX
%define					x_stride_d				_EDX


%define					y_ptr					_ESI
%define					x_ptr					_EDI


%define					prm_vflip				[ _ESP + localsize + pushsize + 10*PTR_SIZE]
%define					prm_height				[ _ESP + localsize + pushsize + 9*PTR_SIZE]
%define					prm_width				[ _ESP + localsize + pushsize + 8*PTR_SIZE]
%define					prm_uv_stride			[ _ESP + localsize + pushsize + 7*PTR_SIZE]
%define					prm_y_stride			[ _ESP + localsize + pushsize + 6*PTR_SIZE]
%define					prm_v_ptr				[ _ESP + localsize + pushsize + 5*PTR_SIZE]
%define					prm_u_ptr				[ _ESP + localsize + pushsize + 4*PTR_SIZE]
%define					prm_y_ptr				[ _ESP + localsize + pushsize + 3*PTR_SIZE]
%define					prm_x_stride			[ _ESP + localsize + pushsize + 2*PTR_SIZE]
%define					prm_x_ptr				[ _ESP + localsize + pushsize + 1*PTR_SIZE]

%define					y_dif					[ _ESP + localsize - 1*4]
%define					uv_dif					[ _ESP + localsize - 2*4]
%define					fixed_width				[ _ESP + localsize - 3*4]
%define					tmp_height				[ _ESP + localsize - 4*4]
%define					x_dif					[ _ESP + localsize - 5*4]

%define					TEMP_Y1					_ESP
%define					TEMP_Y2					_ESP + 8
%define					TEMP_G1					_ESP + 16
%define					TEMP_G2					_ESP + 24
%define					TEMP_B1					_ESP + 32
%define					TEMP_B2					_ESP + 40


cglobal NAME
NAME:

	PUSH _EBX
	PUSH _ESI
	PUSH _EDI
	PUSH _EBP

	SUB _ESP, localsize

	MOV _EAX, prm_width
	ADD _EAX, 15
	AND _EAX, ~15
	MOV fixed_width, _EAX

	MOV _EBX, prm_x_stride
	%rep BYTES
		SUB _EBX, _EAX
	%endrep
	MOV x_dif, _EBX

	MOV _EBX, prm_y_stride
	SUB _EBX, _EAX
	MOV y_dif, _EBX

	MOV _EBX, prm_uv_stride
	MOV TMP1, _EAX
	SHR TMP1, 1
	SUB _EBX, TMP1
	MOV uv_dif, _EBX


	MOV _ESI, prm_y_ptr
	MOV _EDI, prm_x_ptr
	MOV _EDX, prm_x_stride
	MOV _EBP, prm_height

	MOV _EBX, prm_vflip
	OR  _EBX, _EBX
	JZ .dont_flip

	XOR _EBX, _EBX
	%rep BYTES
		SUB _EBX, _EAX
	%endrep
	SUB _EBX, _EDX
	MOV x_dif, _EBX

	LEA _EAX, [_EBP-1]
	PUSH _EDX
	MUL _EDX
	POP _EDX
	ADD _EDI, _EAX
	NEG _EDX

	.dont_flip:
	MOV _EAX, prm_y_stride
	MOV _EBX, prm_u_ptr
	MOV _ECX, prm_v_ptr

	FUNC %+ _INIT ARG1, ARG2
.y_loop:
	MOV tmp_height, _EBP
	MOV _EBP, fixed_width

.x_loop:
	FUNC ARG1, ARG2

	ADD _EDI, BYTES*PIXELS
	ADD _ESI, PIXELS
	ADD _EBX, PIXELS/2
	ADD _ECX, PIXELS/2

	SUB _EBP, PIXELS
	JG .x_loop

	MOV _EBP, tmp_height
	ADD _EDI, x_dif
	ADD _ESI, y_dif

	%rep VPIXELS-1
	ADD _EDI, _EDX
	ADD _ESI, _EAX
	%endrep

	ADD _EBX, uv_dif
	ADD _ECX, uv_dif
	%rep VPIXELS/2-1
	ADD _EBX, prm_uv_stride
	ADD _ECX, prm_uv_stride
	%endrep

	SUB _EBP, VPIXELS
	JG .y_loop



	ADD _ESP, localsize


	POP _EBP
	POP _EDI
	POP _ESI
	POP _EBX

	RET
%endmacro
