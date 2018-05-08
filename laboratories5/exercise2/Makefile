all:
	make cmaster
	make cslave
	gnome-terminal --tab -e 'make master'
	gnome-terminal --tab -e 'make slave'

cmaster:
	gcc Master.c -o Master.out

cslave:
	gcc Slave.c -o Slave.out

master:
	./Master.out fifo

slave:
	./Slave.out fifo 2

delete:
	rm *.out
