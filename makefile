CC=cc
CFLAGS=-Wall

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

t-clock: t-clock.o
	$(CC) -o t-clock t-clock.o

clean:
	rm -f t-clock t-clock.o

run: t-clock
	./t-clock