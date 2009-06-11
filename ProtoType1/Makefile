ifndef MPIROOT
 $(error "Error:  environment variable MPIROOT not defined!")
endif

FC       = $(MPIROOT)/bin/mpif90
CC       = $(MPIROOT)/bin/mpicc
FFLAGS   = -g -O0
CFLAGS   = -g -O0

FILES=driver.o compute.o init.o ampi.o ampi_bookKeeping.o

ADJFILES= driverAdj.o compute.o computeAdj.o init.o ampi.o ampi_bookKeeping.o

all: driver driverAdj

driver: $(FILES)
	$(CC) $(CFLAGS) -o $@ $^ -lm 

driverAdj: $(ADJFILES)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o : %.c
	$(CC) $(CFLAGS) -c $<


clean:
	rm -f *.o driver driverAdj
