#include "ampi.h"
#include "math.h"

void ring(double* x) { 
  double lx, rx;
  int myId, numprocs, i, rc, ierr, leftId, rightId,req[4];
  MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  *x=sin(*x);
  leftId=(myId-1)%numprocs;
  if (leftId<0)
    leftId=leftId+numprocs;
  rightId=(myId+1)%numprocs;
  if (rightId<0)
    rightId=rightId+numprocs;
  /*
    printf("I: %d left: %d right: %d \n",myId,leftId,rightId);
  */
  AMPI_awaitall(4,req,MPI_STATUSES_IGNORE);
  AMPI_isend(x,1,MPI_DOUBLE_PRECISION,leftId,0,MPI_COMM_WORLD,&req[0]); 
  AMPI_isend(x,1,MPI_DOUBLE_PRECISION,rightId,0,MPI_COMM_WORLD,&req[1]);
  AMPI_irecv(&lx,1,MPI_DOUBLE_PRECISION,leftId,0,MPI_COMM_WORLD,&req[2]); 
  AMPI_irecv(&rx,1,MPI_DOUBLE_PRECISION,rightId,0,MPI_COMM_WORLD,&req[3]);
  AMPI_waitall(4,req,MPI_STATUSES_IGNORE);
  /* add the neighbors contributions */
  *x=*x+rx+lx;
}

void compute(double x, double * f) { 
  ring(&x);
  AMPI_reduce((&x),f,1,MPI_DOUBLE_PRECISION,MPI_SUM,0,MPI_COMM_WORLD);
}
