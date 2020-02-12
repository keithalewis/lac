#!!! add git submodule for libffi
LAC_ = lac_ffi.c lac_map.c lac_parse.c lac_stack.c lac_stream.c lac_variant.c
SRCS = lac.c $(LAC_)
OBJS = $(SRCS:.c=.o)
LAC_T = lac_ffi.t.c lac_map.t.c lac_parse.t.c lac_stack.t.c lac_stream.t.c lac_variant.t.c

FFI_LIB_DIR = ./libffi/x86_64-pc-linux-gnu/.libs/

SRCS_T = lac.t.c $(LAC_) $(LAC_T)
OBJS_T = $(SRCS_T:.c=.o)

CFLAGS = -g -Wall
LDLIBS = -ldl -L $(FFI_LIB_DIR) -l:libffi.a

lac: $(OBJS)

lac.t: $(OBJS_T)

test: lac.t
	./lac.t

.PHONY : libffi
libffi: libffi/configure
	(cd libffi; make)

libffi/configure: libffi/autogen.sh
	(cd libffi; ./autogen.sh)
	(cd libffi; ./configure --enable-debug --disable-docs)

.PHONY : clean
clean:
	rm -f lac lac.t *.o

valgrind: lac
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./lac

valgrind_t: lac.t
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./lac.t

deps: $(SRCS_T)
	$(foreach c, $(SRCS_T), cc -MM $(c);)

# r!make deps
lac.t.o: lac.t.c ensure.h lac_ffi.h lac_variant.h lac_parse.h \
 lac_stream.h lac_map.h
lac_ffi.o: lac_ffi.c ensure.h lac_ffi.h lac_variant.h
lac_parse.o: lac_parse.c ensure.h lac_stream.h lac_parse.h
lac_stack.o: lac_stack.c ensure.h lac_stack.h
lac_map.o: lac_map.c lac_parse.h lac_stream.h
lac_stream.o: lac_stream.c lac_stream.h
lac_variant.o: lac_variant.c lac_variant.h
lac_ffi.t.o: lac_ffi.t.c ensure.h lac_ffi.h lac_variant.h
lac_parse.t.o: lac_parse.t.c ensure.h lac_parse.h lac_stream.h
lac_stack.t.o: lac_stack.t.c ensure.h lac_stack.h
lac_map.t.o: lac_map.t.c ensure.h lac_map.h lac_parse.h lac_stream.h
lac_stream.t.o: lac_stream.t.c ensure.h lac_parse.h lac_stream.h
lac_variant.t.o: lac_variant.t.c ensure.h lac_variant.h
