LAC_ = lac_parse.c
SRCS = lac.c $(LAC_)
OBJS = $(SRCS:.c=.o)

LAC_T = lac_parse.t.c
SRCS_T = lac.t.c $(LAC_) $(LAC_T)
OBJS_T = $(SRCS_T:.c=.o)

all: lac lac.t

# Get value of VAR from libffi Makefile
.PHONY: printvar
printvar:
	@make -f printvar.mk -f libffi/Makefile print-$(VAR)

# Get value of build variable from libffi Makefile
BUILD = $(eval $(shell make -s printvar VAR=build))$(build)
FFI_DIR = ./libffi/$(BUILD)

CFLAGS = -g -Wall -I $(FFI_DIR)/include
# link to static lib
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


deps: $(SRCS_T)
	@$(foreach c, $(SRCS_T), cc -MM $(c);)

# r!make deps
lac.t.o: lac.t.c lac.h ensure.h lac_ffi.h lac_variant.h lac_init.h \
 lac_map.h lac_eval.h
lac_parse.o: lac_parse.c ensure.h lac_parse.h
lac_parse.t.o: lac_parse.t.c ensure.h lac_parse.h
