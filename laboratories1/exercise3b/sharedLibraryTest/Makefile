compileSharedLoadedAtStart0:
	gcc -o0 UsingLibraryTest.c -L. -Wl,-rpath=. -lLibraryToWorkOnTable -o UsingLibraryTest.out
	./UsingLibraryTest.out 5 15 d c:0:a:2:f:31 > compileSharedLoadedAtStart0.txt

compileSharedLoadedAtStart1:
	gcc -o1 UsingLibraryTest.c -L. -Wl,-rpath=. -lLibraryToWorkOnTable -o UsingLibraryTest.out
	./UsingLibraryTest.out 5 15 d c:0:a:2:f:31 > compileSharedLoadedAtStart1.txt

compileSharedLoadedAtStart2:
	gcc -o2 UsingLibraryTest.c -L. -Wl,-rpath=. -lLibraryToWorkOnTable -o UsingLibraryTest.out
	./UsingLibraryTest.out 5 15 d c:0:a:2:f:31 > compileSharedLoadedAtStart2.txt

compileSharedLoadedAtStart3:
	gcc -o3 UsingLibraryTest.c -L. -Wl,-rpath=. -lLibraryToWorkOnTable -o UsingLibraryTest.out
	./UsingLibraryTest.out 5 15 d c:0:a:2:f:31 > compileSharedLoadedAtStart3.txt

test:
	./UsingLibraryTest.out 5 15 d c:0:a:2:f:31

clean:
	rm raport2.txt
	rm *.out
