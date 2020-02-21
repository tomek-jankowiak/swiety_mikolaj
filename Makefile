all: mikolaj.c
	gcc main.c -pthread -o run -Wall

clean:
	rm -f *.o run
