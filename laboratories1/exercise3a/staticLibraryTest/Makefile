compileStatic:
	gcc UsingLibraryTest.c -static -L. -lLibraryToWorkOnTable -o UsingLibraryTest.out

test:
	./UsingLibraryTest.out 5 15 d c:0:a:2:f:31

testCreate:
	./UsingLibraryTest.out 5 15 d c:0:c:2:c:31

testAdd:
	./UsingLibraryTest.out 5 15 d c:0:a:2:a:4

testDelete:
	./UsingLibraryTest.out 5 15 d c:0:a:5:d:3

testDeleteEmptyTable:
	./UsingLibraryTest.out 5 15 d c:0:d:5:a:3

testFind:
	./UsingLibraryTest.out 5 15 d c:0:a:2:f:31

testMix:
	./UsingLibraryTest.out 5 15 d c:0:m:3:f:31

testStatic:
	./UsingLibraryTest.out 5 15 s c:0:a:2:f:31

clean:
	rm *.out
