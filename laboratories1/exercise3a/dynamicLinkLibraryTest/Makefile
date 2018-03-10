compileSharedLoadedByProgram:
	gcc UsingLibraryLoadedByProgramTest.c -ldl -o UsingLibraryLoadedByProgramTest.out

test:
	./UsingLibraryLoadedByProgramTest.out 5 15 d c:0:a:2:f:31

testCreate:
	./UsingLibraryLoadedByProgramTest.out 5 15 d c:0:c:2:c:31

testAdd:
	./UsingLibraryLoadedByProgramTest.out 5 15 d c:0:a:2:a:4

testDelete:
	./UsingLibraryLoadedByProgramTest.out 5 15 d c:0:a:5:d:3

testDeleteEmptyTable:
	./UsingLibraryLoadedByProgramTest.out 5 15 d c:0:d:5:a:3

testFind:
	./UsingLibraryLoadedByProgramTest.out 5 15 d c:0:a:2:f:31

testMix:
	./UsingLibraryLoadedByProgramTest.out 5 15 d c:0:m:3:f:31

testStatic:
	./UsingLibraryLoadedByProgramTest.out 5 15 s c:0:a:2:f:31

clean:
	rm *.out
