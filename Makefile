SRCS = lac_dbm.c lac_ffi.c lac_parse.c
TSTS = lac_dbm.t.c lac_ffi.t.c lac_parse.t.c

CFLAGS = -g
LDLIBS = -ldl -lffi -lgdbm

lac: $(SRCS)
	$(CC) $(CFLAGS) -o $@ lac.c $^ $(LDLIBS)

lac.t: $(TSTS) $(SRCS)
	$(CC) $(CFLAGS) -o $@ lac.t.c $^ $(LDLIBS)

test: lac.t
	./lac.t

clean:
	@rm lac lac.t

# r!cc -MM lac.c
lac.o: lac.c $(SRCS) lac.h lac_dbm.h lac_ffi.h lac_parse.h

# r!cc -MM lac.t.c
lac.t.o: lac.t.c $(SRCS) $(TSTS) lac_dbm.h lac_ffi.h lac_parse.h
