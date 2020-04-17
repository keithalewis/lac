// lac_co.h - coroutines
#pragma once

#define co_start(state) switch(state) { case 0:;
#define co_return default:; }
/*save state and return value*/
#define co_yield(state,value) do { \
    state = __LINE__; return (value); case __LINE__:; } while (0)
