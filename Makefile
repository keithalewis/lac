LAC_ = lac_ffi.c lac_map.c lac_parse.c lac_variant.c lac_init.c lac_tree.c
SRCS = lac.c $(LAC_)
OBJS = $(SRCS:.c=.o)
LAC_T = lac_ffi.t.c lac_map.t.c lac_parse.t.c lac_variant.t.c lac_tree.t.c lac_data.t.c

BUILD = $(eval $(shell make -s printvar VAR=build))$(build)
FFI_DIR = ./libffi/$(BUILD)

SRCS_T = lac.t.c $(LAC_) $(LAC_T)
OBJS_T = $(SRCS_T:.c=.o)

CFLAGS = -g -Wall -I $(FFI_DIR)/include
LDLIBS = -ldl -L $(FFI_DIR)/.libs -l:libffi.a

lac: $(OBJS)

lac.t: $(OBJS_T)

test: lac.t
	./lac.t

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
	rm -f lac lac.t *.o

valgrind: lac
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./lac

valgrind_t: lac.t
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./lac.t

.PHONY: printvar
printvar:
	@make -f printvar.mk -f libffi/Makefile print-$(VAR)

deps: $(SRCS_T)
	@$(foreach c, $(SRCS_T), cc -MM $(c);)

# r!make deps
lac.t.o: lac.t.c lac.h ensure.h lac_ffi.h lac_variant.h lac_init.h \
 lac_map.h
lac_ffi.o: lac_ffi.c ensure.h lac_ffi.h lac_variant.h
lac_map.o: lac_map.c ensure.h
lac_parse.o: lac_parse.c ensure.h lac_variant.h
lac_variant.o: lac_variant.c lac_variant.h ensure.h
lac_init.o: lac_init.c lac_init.h lac_ffi.h lac_variant.h ensure.h \
 lac_map.h
lac_tree.o: lac_tree.c lac_tree.h
lac_ffi.t.o: lac_ffi.t.c ensure.h lac_ffi.h lac_variant.h
lac_map.t.o: lac_map.t.c ensure.h lac_map.h
lac_parse.t.o: lac_parse.t.c ensure.h lac_variant.h
lac_variant.t.o: lac_variant.t.c ensure.h lac_variant.h
lac_tree.t.o: lac_tree.t.c ensure.h lac_tree.h
lac_data.t.o: lac_data.t.c ensure.h lac_data.h
