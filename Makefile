all: mikolaj.c
	gcc mikolaj.c -pthread -o run -Wall

clean:
	rm -f *.o run
