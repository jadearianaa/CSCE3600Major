newshell: newshell.o
	gcc -o newshell newshell.o
newshell.o: newshell.c
	gcc -c -Wall newshell.c PathCommand.h SignalHandle.h
clean:
	rm *.o newshell
