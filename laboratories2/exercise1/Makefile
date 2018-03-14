compile:
	gcc FileOperationsParser.c libraries/FileOperations.c -o FileOperationsParser.out

sysGen:
	./FileOperationsParser.out generate systest.txt 100 512 sys

sysCopy:
	./FileOperationsParser.out copy systest.txt syscopy.txt 512 sys

sysSort:
	./FileOperationsParser.out sort syscopy.txt 100 512 sys

libGen:
	./FileOperationsParser.out generate libtest.txt 100 512 lib

libCopy:
	./FileOperationsParser.out copy libtest.txt libcopy.txt 512 lib

libSort:
	./FileOperationsParser.out sort libcopy.txt 100 512 lib

sysAll:
	./FileOperationsParser.out generate systest.txt 100 512 sys
	./FileOperationsParser.out copy systest.txt syscopy.txt 512 sys
	./FileOperationsParser.out sort syscopy.txt 100 512 sys

libAll:
	./FileOperationsParser.out generate libtest.txt 100 512 lib
	./FileOperationsParser.out copy libtest.txt libcopy.txt 512 lib
	./FileOperationsParser.out sort libcopy.txt 100 512 lib

all:
	./FileOperationsParser.out generate systest.txt 100 512 sys
	./FileOperationsParser.out copy systest.txt syscopy.txt 512 sys
	./FileOperationsParser.out sort syscopy.txt 100 512 sys
	./FileOperationsParser.out generate libtest.txt 100 512 lib
	./FileOperationsParser.out copy libtest.txt libcopy.txt 512 lib
	./FileOperationsParser.out sort libcopy.txt 100 512 lib

remove:
	rm *.out *.txt
