CC := gcc
CFLAGS := -std=c99 -Wall -Werror -pedantic
SRC := src


libfunc.o: $(SRC)/libfunc.c
	$(CC) -I$(SRC) $(CFLAGS) -fPIC -lm -c $<

libfunc.so: libfunc.o
	$(CC) $(CFLAGS) libfunc.o -shared -lm -o $@


app.exe: $(OBJS) main.o libfunc.so
	$(CC) -L. $(CFLAGS) -o $@ main.o -lm -lfunc

%.o: $(SRC)/%.c libfunc.so
	$(CC) -L. -I$(SRC) -I/usr/include/python3.10/ $(CFLAGS) -lm -lfunc -c $<

run:
	LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH ./app.exe
