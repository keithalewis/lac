LAC_ = lac_variant.c lac_map.c lac_cif.c lac_init.c lac_eval.c lac_token.c lac_array.c
SRCS = lac.c $(LAC_)
OBJS = $(SRCS:.c=.o)

LAC_T  = lac_variant.t.c lac_map.t.c lac_cif.t.c lac_token.t.c \
    lac_eval.t.c lac_init.t.c lac_array.t.c
SRCS_T = lac.t.c $(LAC_) $(LAC_T)
OBJS_T = $(SRCS_T:.c=.o)

CFLAGS = -g -Wall -I $(FFI_DIR)/include

HDRS = ensure.h lac.h lac_cif.h lac_eval.h lac_init.h lac_map.h \
	lac_variant.h lac_token.h

all: libffi lac lac.t

# Get value of VAR from libffi Makefile
.PHONY: printvar
printvar:
	@make -f printvar.mk -f libffi/Makefile print-$(VAR)

# Get value of build variable from libffi Makefile
BUILD = $(eval $(shell make -s printvar VAR=build))$(build)
FFI_DIR = ./libffi/$(BUILD)

# static analysis flags
SFLAGS = -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization \
	-Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
	-Wmissing-include-dirs -Wredundant-decls -Wsign-conversion \
	-Wswitch-default -Wundef -Wno-unused-function -fno-strict-aliasing
CFLAGS = -g $(SFLAGS) -I $(FFI_DIR)/include
lac.t: CFLAGS += -D_DEBUG
# link to static lib
LDLIBS = -ldl -L $(FFI_DIR)/.libs -l:libffi.a

lac: $(OBJS)

lac.t: $(OBJS_T)

test: lac.t
	./lac.t

libffi: libffi/configure
	(cd libffi; make)

libffi/configure: libffi/autogen.sh
	(cd libffi; ./autogen.sh)
	(cd libffi; CFLAGS="-g -O0"  ./configure --enable-debug --disable-docs)

libffi/autogen.sh: 
	git submodule update --init --recursive

.PHONY : clean
clean:
	rm -f lac lac.t *.o *~

valgrind: lac
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./lac

valgrind_t: lac.t
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./lac.t

IFLAGS = -kr

indent: $(SRCS_T) $(HDRS)
	indent $(IFLAGS) $^

clang-format: $(SRCS_T) $(HDRS)
	clang-format -i $^


print: $(SRCS_T) $(HDRS)
	enscript -i4 -p - $^ | ps2pdf - lac.pdf

deps: $(SRCS_T)
	@$(foreach c, $(SRCS_T), cc -MM $(c);)

cmake:
	@rm -rf build
	cmake . -B build
	(cd build; make)

# r!make deps
lac.t.o: lac.t.c lac.h ensure.h lac_cif.h lac_variant.h lac_eval.h \
 lac_init.h lac_map.h lac_array.h
lac_variant.o: lac_variant.c lac_variant.h ensure.h
lac_map.o: lac_map.c lac_map.h ensure.h
lac_cif.o: lac_cif.c lac_cif.h lac_variant.h ensure.h
lac_init.o: lac_init.c lac_init.h ensure.h lac_cif.h lac_variant.h \
 lac_eval.h lac_map.h
lac_eval.o: lac_eval.c debug.h ensure.h lac_cif.h lac_variant.h \
 lac_eval.h lac_map.h lac_token.h
lac_token.o: lac_token.c lac_token.h
lac_array.o: lac_array.c lac_array.h
lac_variant.t.o: lac_variant.t.c ensure.h lac_variant.h
lac_map.t.o: lac_map.t.c ensure.h lac_map.h
lac_cif.t.o: lac_cif.t.c ensure.h lac_cif.h lac_variant.h
lac_token.t.o: lac_token.t.c ensure.h lac_token.h
lac_eval.t.o: lac_eval.t.c ensure.h lac_cif.h lac_variant.h lac_eval.h
lac_init.t.o: lac_init.t.c lac_init.h
lac_array.t.o: lac_array.t.c lac_array.h
