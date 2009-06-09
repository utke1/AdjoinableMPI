ifndef MPIROOT
 $(error "Error:  environment variable MPIROOT not defined!")
endif

FC       = $(MPIROOT)/bin/mpif90
CC       = $(MPIROOT)/bin/mpicc
FFLAGS   = -g -O0
CFLAGS   = -g -O0

FILES=driver.o compute.o init.o ampi.o ampi_bookKeeping.o

all: driver 

driver: $(FILES)
	$(CC) $(CFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<


clean:
	rm -f *.o 
