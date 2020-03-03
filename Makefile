LAC_ = lac_parse.c lac_variant.c lac_map.c lac_cif.c lac_init.c lac_eval.c
SRCS = lac.c $(LAC_)
OBJS = $(SRCS:.c=.o)

LAC_T  = lac_parse.t.c lac_variant.t.c lac_map.t.c lac_cif.t.c
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

# -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused
# static analysis flags
SFLAGS = -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization \
	-Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs \
	-Wredundant-decls -Wsign-conversion \
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


deps: $(SRCS_T)
	@$(foreach c, $(SRCS_T), cc -MM $(c);)

# r!make deps
