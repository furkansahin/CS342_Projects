all: librwlock.a app  ts_indexgen

ts_indexgen: ts_indexgen.c
	gcc -Wall -o ts_indexgen ts_indexgen.c -lpthread

librwlock.a: 	rwlock.c
	gcc -Wall -c rwlock.c
	ar -cvq librwlock.a rwlock.o
	ranlib librwlock.a

app: 	app.c
	gcc -Wall -o app app.c  -L. -lrwlock -lpthread

clean: 
	rm -fr *.o *.a *~ a.out app ts_indexgen  rwlock.o librwlock.a
