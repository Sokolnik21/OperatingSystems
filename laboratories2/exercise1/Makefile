RECORDS = 6000

compile:
	gcc FileOperationsParser.c libraries/FileOperations.c -o FileOperationsParser.out

gen4:
	touch test.txt
	rm test.txt
	./FileOperationsParser.out generate test.txt $(RECORDS) 4 sys
gen512:
	touch test.txt
	rm test.txt
	./FileOperationsParser.out generate test.txt $(RECORDS) 512 sys
gen4096:
	touch test.txt
	rm test.txt
	./FileOperationsParser.out generate test.txt $(RECORDS) 4096 sys
gen8192:
	touch test.txt
	rm test.txt
	./FileOperationsParser.out generate test.txt $(RECORDS) 8192 sys

# System functions

sysTimeSort4:
	cp test.txt copy.txt
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out sort copy.txt $(RECORDS) 4 sys
sysTimeSort512:
	cp test.txt copy.txt
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out sort copy.txt $(RECORDS) 512 sys
sysTimeSort4096:
	cp test.txt copy.txt
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out sort copy.txt $(RECORDS) 4096 sys
sysTimeSort8192:
	cp test.txt copy.txt
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out sort copy.txt $(RECORDS) 8192 sys

sysTestSort4:
	make compile
	make sysTimeSort4
	cat TmpTime.txt >> results.text
sysTestSort512:
	make compile
	make sysTimeSort512
	cat TmpTime.txt >> results.text
sysTestSort4096:
	make compile
	make sysTimeSort4096
	cat TmpTime.txt >> results.text
sysTestSort8192:
	make compile
	make sysTimeSort8192
	cat TmpTime.txt >> results.text

sysTimeCopy4:
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out copy test.txt copy.txt 4 sys
sysTimeCopy512:
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out copy test.txt copy.txt 512 sys
sysTimeCopy4096:
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out copy test.txt copy.txt 4096 sys
sysTimeCopy8192:
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out copy test.txt copy.txt 8192 sys

sysTestCopy4:
	make compile
	make sysTimeCopy4
	cat TmpTime.txt >> results.text
sysTestCopy512:
	make compile
	make sysTimeCopy512
	cat TmpTime.txt >> results.text
sysTestCopy4096:
	make compile
	make sysTimeCopy4096
	cat TmpTime.txt >> results.text
sysTestCopy8192:
	make compile
	make sysTimeCopy8192
	cat TmpTime.txt >> results.text

# Standard library function

libTimeSort4:
	cp test.txt copy.txt
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out sort copy.txt $(RECORDS) 4 lib
libTimeSort512:
	cp test.txt copy.txt
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out sort copy.txt $(RECORDS) 512 lib
libTimeSort4096:
	cp test.txt copy.txt
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out sort copy.txt $(RECORDS) 4096 lib
libTimeSort8192:
	cp test.txt copy.txt
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out sort copy.txt $(RECORDS) 8192 lib

libTestSort4:
	make compile
	make libTimeSort4
	cat TmpTime.txt >> results.text
libTestSort512:
	make compile
	make libTimeSort512
	cat TmpTime.txt >> results.text
libTestSort4096:
	make compile
	make libTimeSort4096
	cat TmpTime.txt >> results.text
libTestSort8192:
	make compile
	make libTimeSort8192
	cat TmpTime.txt >> results.text

libTimeCopy4:
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out copy test.txt copy.txt 4 lib
libTimeCopy512:
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out copy test.txt copy.txt 512 lib
libTimeCopy4096:
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out copy test.txt copy.txt 4096 lib
libTimeCopy8192:
	time -f "%E\tREAL\n%U\tUSER\n%S\tSYS" -o TmpTime.txt ./FileOperationsParser.out copy test.txt copy.txt 8192 lib

libTestCopy4:
	make compile
	make libTimeCopy4
	cat TmpTime.txt >> results.text
libTestCopy512:
	make compile
	make libTimeCopy512
	cat TmpTime.txt >> results.text
libTestCopy4096:
	make compile
	make libTimeCopy4096
	cat TmpTime.txt >> results.text
libTestCopy8192:
	make compile
	make libTimeCopy8192
	cat TmpTime.txt >> results.text

result:
	make compile
	touch results.text
	rm results.text
	echo "<><><><><><><><><><><><>" >> results.text
	echo "\tSort" >> results.text
	echo "<><><><><><><><><><><><>" >> results.text
	make gen4
	echo "4-size record" >> results.text
	echo "SystemFunctionsTest" >> results.text
	make sysTestSort4
	echo "StandardLibraryTest" >> results.text
	make libTestSort4
	make gen512
	echo "512-size record" >> results.text
	echo "SystemFunctionsTest" >> results.text
	make sysTestSort512
	echo "StandardLibraryTest" >> results.text
	make libTestSort512
	make gen4096
	echo "4096-size record" >> results.text
	echo "SystemFunctionsTest" >> results.text
	make sysTestSort4096
	echo "StandardLibraryTest" >> results.text
	make libTestSort4096
	make gen8192
	echo "8192-size record" >> results.text
	echo "SystemFunctionsTest" >> results.text
	make sysTestSort8192
	echo "StandardLibraryTest" >> results.text
	make libTestSort8192
	echo >> results.text
	echo "<><><><><><><><><><><><>" >> results.text
	echo "\tCopy" >> results.text
	echo "<><><><><><><><><><><><>" >> results.text
	make gen4
	echo "4-size record" >> results.text
	echo "SystemFunctionsTest" >> results.text
	make sysTestCopy4
	echo "StandardLibraryTest" >> results.text
	make libTestCopy4
	make gen512
	echo "512-size record" >> results.text
	echo "SystemFunctionsTest" >> results.text
	make sysTestCopy512
	echo "StandardLibraryTest" >> results.text
	make libTestCopy512
	make gen4096
	echo "4096-size record" >> results.text
	echo "SystemFunctionsTest" >> results.text
	make sysTestCopy4096
	echo "StandardLibraryTest" >> results.text
	make libTestCopy4096
	make gen8192
	echo "8192-size record" >> results.text
	echo "SystemFunctionsTest" >> results.text
	make sysTestCopy8192
	echo "StandardLibraryTest" >> results.text
	make libTestCopy8192
	echo >> results.text
	make remove

remove:
	rm *.out *.txt
