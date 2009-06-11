#include "ampi.h"
#include "math.h"
#include "active.h"

void ringAdj(struct active * x) { 
  double lx, rx;
  int myId, numprocs, i, rc, ierr, leftId, rightId,req[4];
  MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  /* adjoint for     *x=*x+rx+lx;  */
  rx=(*x).d;
  lx=(*x).d;
  /* recompute (or tape) */
  leftId=(myId-1)%numprocs;
  if (leftId<0)
    leftId=leftId+numprocs;
  rightId=(myId+1)%numprocs;
  if (rightId<0)
    rightId=rightId+numprocs;
  /* reversed order of AMPI calls */
  AMPI_waitall(4,req,MPI_STATUSES_IGNORE);
  AMPI_irecv(&rx,1,MPI_DOUBLE_PRECISION,rightId,0,MPI_COMM_WORLD,&req[3]);
  AMPI_irecv(&lx,1,MPI_DOUBLE_PRECISION,leftId,0,MPI_COMM_WORLD,&req[2]); 
  AMPI_isend(&((*x).d),1,MPI_DOUBLE_PRECISION,rightId,0,MPI_COMM_WORLD,&req[1]);
  AMPI_isend(&((*x).d),1,MPI_DOUBLE_PRECISION,leftId,0,MPI_COMM_WORLD,&req[0]); 
  AMPI_awaitall(4,req,MPI_STATUSES_IGNORE);
  /* adjoint of *x=sin(*x);  */
  (*x).d=cos((*x).v)*(*x).d;
}

void computeAdj(struct active * x, struct active * f) { 
  int myId=0;
  MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  AMPI_reduce(&((*x).d),&((*f).d),1,MPI_DOUBLE_PRECISION,MPI_SUM,0,MPI_COMM_WORLD);
  ringAdj(x);
}
