staticLibrary:
	gcc -c LibraryToWorkOnTable.c -o LibraryToWorkOnTable.o
	ar rcs libLibraryToWorkOnTable.a LibraryToWorkOnTable.o

sharedLibrary:
	gcc -fPIC -shared LibraryToWorkOnTable.c -o libLibraryToWorkOnTable.so

clean:
	rm *.out *.o
