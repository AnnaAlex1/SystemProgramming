CC=gcc
CFLAGS=-Wall -g


vaccineMonitor: main.o console.o citizens.o records.o virus.o bloomfilter.o skiplist.o countries.o
	$(CC) $(CFLAGS) -o vaccineMonitor main.o console.o citizens.o records.o virus.o bloomfilter.o skiplist.o countries.o



main.o: main.c
	$(CC) $(CFLAGS) -c main.c

console.o: console.c
	$(CC) $(CFLAGS) -c console.c


records.o: records.c
	$(CC) $(CFLAGS) -c records.c

citizens.o: citizens.c
	$(CC) $(CFLAGS) -c citizens.c

virus.o: virus.c
	$(CC) $(CFLAGS) -c virus.c
	
skiplist.o: skiplist.c
	$(CC) $(CFLAGS) -c skiplist.c

bloomfilter.o: bloomfilter.c
	$(CC) $(CFLAGS) -c bloomfilter.c

countries.o: countries.c
	$(CC) $(CFLAGS) -c countries.c

	
.PHONY: clean

clean:
	rm -f *.o vaccineMonitor
