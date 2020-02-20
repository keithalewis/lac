#!!! add git submodule for libffi
LAC_ =  lac_parse.c lac_variant.c lac_map.c lac_ffi.c lac_init.c
SRCS = lac.c $(LAC_)
OBJS = $(SRCS:.c=.o)
LAC_T = lac_parse.t.c lac_variant.t.c lac_map.t.c lac_ffi.t.c

FFI_LIB_DIR = ./libffi/x86_64-pc-linux-gnu/.libs/

SRCS_T = lac.t.c $(LAC_) $(LAC_T)
OBJS_T = $(SRCS_T:.c=.o)

CFLAGS = -g -Wall # -ftest-coverage -fprofile-arcs
LDLIBS = -ldl -L $(FFI_LIB_DIR) -l:libffi.a # -lgcov

lac: $(OBJS)

lac.t: $(OBJS_T)

test: lac.t
	./lac.t

# Linux kernel style
INDENT_FLAGS = -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 \
-cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai \
-saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1

indent: *.c *.h
	indent $(INDENT_FLAGS) $^

.PHONY : libffi
libffi: libffi/configure
	(cd libffi; make)

libffi/configure: libffi/autogen.sh
	(cd libffi; ./autogen.sh)
	(cd libffi; CFLAGS="-g -O0"  ./configure --enable-debug --disable-docs)

libffi/autogen.sh: 
	git submodule update

.PHONY : clean
clean:
	rm -f lac lac.t *.o *.gcno *.gcov *.gcda

valgrind: lac
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./lac

valgrind_t: lac.t
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./lac.t

deps: $(SRCS) $(SRCS_T)
	@$(foreach c, $^, cc -MM $(c);)

# r!make deps
lac.o: lac.c ensure.h lac.h lac_ffi.h lac_variant.h lac_init.h lac_map.h
lac_parse.o: lac_parse.c ensure.h lac_variant.h
lac_variant.o: lac_variant.c lac_variant.h ensure.h
lac_map.o: lac_map.c ensure.h
lac_ffi.o: lac_ffi.c ensure.h lac_ffi.h lac_variant.h
lac_init.o: lac_init.c lac_init.h lac_ffi.h lac_variant.h ensure.h \
 lac_map.h
lac.t.o: lac.t.c lac.h ensure.h lac_ffi.h lac_variant.h lac_init.h \
 lac_map.h
lac_parse.t.o: lac_parse.t.c ensure.h lac_variant.h
lac_variant.t.o: lac_variant.t.c ensure.h lac_variant.h
lac_map.t.o: lac_map.t.c ensure.h lac_map.h
lac_ffi.t.o: lac_ffi.t.c ensure.h lac_ffi.h lac_variant.h
