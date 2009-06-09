#include "mpi.h"

void init(double * x) { 
  int myid, ierr;
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
 *x=(myid+1)*0.5;
}
