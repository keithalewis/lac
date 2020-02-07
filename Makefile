LAC_ = lac_dbm.c lac_ffi.c lac_parse.c lac_stack.c
SRCS = lac.c $(LAC_)
OBJS = $(SRCS:.c=.o)
LAC_T = lac_dbm.t.c lac_ffi.t.c lac_parse.t.c lac_stack.t.c
SRCS_T = lac.t.c $(LAC_) $(LAC_T)
OBJS_T = $(SRCS_T:.c=.o)

CFLAGS = -g -Wall -O0
LDLIBS = -ldl -lffi -lgdbm

lac: $(OBJS)

lac.t: $(OBJS_T)

test: lac.t
	./lac.t

.PHONY : clean
clean:
	rm -f lac lac.t *.o *.gdb

valgrind: lac
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./lac

valgrind_t: lac.t
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./lac.t

deps: $(SRCS_T)
	$(foreach c, $(SRCS_T), cc -MM $(c);)

# r!make deps
lac.t.o: lac.t.c lac_dbm.h lac_ffi.h lac_parse.h
lac_dbm.o: lac_dbm.c lac_dbm.h
lac_ffi.o: lac_ffi.c lac_ffi.h
lac_parse.o: lac_parse.c lac_parse.h
lac_stack.o: lac_stack.c ensure.h lac_stack.h
lac_dbm.t.o: lac_dbm.t.c lac_dbm.h
lac_ffi.t.o: lac_ffi.t.c lac_ffi.h
lac_parse.t.o: lac_parse.t.c lac_parse.h
lac_stack.t.o: lac_stack.t.c ensure.h lac_stack.h
