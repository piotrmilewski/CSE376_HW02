CC= gcc 
CFLAGS= -g -O2 -Wall -Werror 
LDFLAGS= -L. -llkmalloc 
SRC= lkmalloc.c memBST.c recordsLL.c 
OBJS= lkmalloc.o memBST.o recordsLL.o

all: test01 test02 test03

buildLibrary: liblkmalloc.a

run_tests: all
	./runTests.sh

clean:
	rm -f *.o *.a *.d test01 test02 test03

-include (OBJS:.o=.d)

depend:
	$(CC) -MMD $(CFLAGS) $(SRC) test01.c
	$(CC) -MMD $(CFLAGS) $(SRC) test02.c
	$(CC) -MMD $(CFLAGS) $(SRC) test03.c
	rm a.out

test01: test01.o liblkmalloc.a
	$(CC) -o test01 $(CFLAGS) test01.o $(LDFLAGS)

test02: test02.o liblkmalloc.a
	$(CC) -o test02 $(CFLAGS) test02.o $(LDFLAGS)

test03: test03.o liblkmalloc.a
	$(CC) -o test03 $(CFLAGS) test03.o $(LDFLAGS)

test01.o: test01.c
	$(CC) -c $(CFLAGS) test01.c

test02.o: test02.c
	$(CC) -c $(CFLAGS) test02.c

test03.o: test03.c
	$(CC) -c $(CFLAGS) test03.c

liblkmalloc.a: lkmalloc.o memBST.o recordsLL.o
	ar rcs liblkmalloc.a lkmalloc.o memBST.o recordsLL.o

lkmalloc.o: lkmalloc.c lkmalloc.h
	$(CC) -c $(CFLAGS) lkmalloc.c

memBST.o: memBST.c memBST.h
	$(CC) -c $(CFLAGS) memBST.c

recordsLL.o: recordsLL.c recordsLL.h
	$(CC) -c $(CFLAGS) recordsLL.c
