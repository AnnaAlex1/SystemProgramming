CC=gcc
CFLAGS=-Wall -g -pthread


travelMonitorClient: main.o threads_imp.o requests.o signal_handl.o sh_sockets.o console.o citizens.o records.o virus.o bloomfilter.o skiplist.o countries.o
	$(CC) $(CFLAGS) -o travelMonitorClient main.o threads_imp.o requests.o signal_handl.o sh_sockets.o console.o citizens.o records.o virus.o bloomfilter.o skiplist.o countries.o -lm

monitorServer: monitor.o threads_imp.o requests.o signal_handl.o sh_sockets.o console.o citizens.o records.o virus.o bloomfilter.o skiplist.o countries.o
	$(CC) $(CFLAGS) -o monitorServer threads_imp.o monitor.o requests.o signal_handl.o sh_sockets.o console.o citizens.o records.o virus.o bloomfilter.o skiplist.o countries.o -lm



threads_imp.o: threads_imp.c
	$(CC) $(CFLAGS) -c threads_imp.c

monitor.o: monitor.c
	$(CC) $(CFLAGS) -c monitor.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

requests.o: requests.c
	$(CC) $(CFLAGS) -c requests.c

signal_handl.o: signal_handl.c
	$(CC) $(CFLAGS) -c signal_handl.c

sh_sockets.o: sh_sockets.c
	$(CC) $(CFLAGS) -c sh_sockets.c -lm


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
	rm -f *.o travelMonitorClient monitorServer
