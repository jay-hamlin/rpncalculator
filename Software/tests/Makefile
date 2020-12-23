
IDIR =../Core/Inc/
SDIR =../Core/Src/
ODIR=/obj
CFLAGS=-I$(IDIR) -DMAKE_FOR_MAC=1 -c

tests: main.o calcMaths.o calcUtilities.o
	gcc main.o calcMaths.o calcUtilities.o -o tests
	
main.o: main.c
	gcc $(CFLAGS) main.c
	
calcMaths.o: $(SDIR)/calcMaths.c $(IDIR)/calcMaths.h
	gcc $(CFLAGS) $(SDIR)/calcMaths.c

calcUtilities.o: $(SDIR)/calcUtilities.c $(IDIR)/calcUtilities.h
	gcc $(CFLAGS) $(SDIR)/calcUtilities.c

.PHONY: clean

clean:
	rm *.o tests
	
