LAC_ = lac_parse.c lac_variant.c lac_map.c lac_cif.c lac_init.c lac_eval.c
SRCS = lac.c $(LAC_)
OBJS = $(SRCS:.c=.o)

LAC_T  = lac_parse.t.c lac_variant.t.c lac_map.t.c lac_cif.t.c
SRCS_T = lac.t.c $(LAC_) $(LAC_T)
OBJS_T = $(SRCS_T:.c=.o)

HDRS = ensure.h lac.h lac_cif.h lac_eval.h lac_init.h lac_map.h \
	lac_parse.h lac_variant.h

all: lac lac.t

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
	-Wswitch-default -Wundef -Wno-unused-function
CFLAGS = -g $(SFLAGS) -I $(FFI_DIR)/include
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

IFLAGS = -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 \
         -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai \
         -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1

indent: $(SRCS_T) $(HDRS)
	indent $(IFLAGS) $^

deps: $(SRCS_T)
	@$(foreach c, $(SRCS_T), cc -MM $(c);)

# r!make deps
lac.t.o: lac.t.c lac.h ensure.h lac_parse.h lac_variant.h lac_map.h \
 lac_init.h lac_cif.h lac_eval.h
lac_parse.o: lac_parse.c lac_parse.h
lac_variant.o: lac_variant.c lac_variant.h
lac_map.o: lac_map.c ensure.h lac_map.h
lac_cif.o: lac_cif.c ensure.h lac_cif.h lac_variant.h
lac_init.o: lac_init.c lac_map.h lac_cif.h lac_variant.h lac_init.h
lac_eval.o: lac_eval.c ensure.h lac_parse.h lac_map.h lac_cif.h \
 lac_variant.h lac_eval.h
lac_parse.t.o: lac_parse.t.c ensure.h lac_parse.h
lac_variant.t.o: lac_variant.t.c ensure.h lac_variant.h
lac_map.t.o: lac_map.t.c ensure.h lac_map.h
lac_cif.t.o: lac_cif.t.c ensure.h lac_cif.h lac_variant.h
