

%ifdef PREFIX
global _x265_get_timer_state

_x265_get_timer_state:
	RDTSC
	ret


%else
global x265_get_timer_state

x265_get_timer_state:
	RDTSC
	ret



%endif

