all: libsalloc.a app

libsalloc.a: 	salloc.c
	gcc -Wall -c salloc.c -lpthread
	ar -cvq libsalloc.a salloc.o
	ranlib libsalloc.a

app: 	app.c
	gcc -Wall -o app app.c  -L. -lsalloc -lpthread
	gcc -Wall -o app2 app2.c  -L. -lsalloc -lpthread
	gcc -Wall -o app3 app3.c  -L. -lsalloc -lpthread


clean:
	rm -fr *.o *.a *~ a.out app app2 app3
