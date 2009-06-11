#include "ampi.h"
#include "active.h"
#include "math.h"

int main(int argc, char **argv){
  struct active x, f;
  int myId, numprocs, ierr;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  init(&(x.v));
  compute(x.v,&(f.v));
  f.d=0.0;
  x.d=0.0;
  if (myId == 0) { 
    printf("final result forward: %lf\n",f.v);
    f.d=1.0;
  } 
  ampi_reverse=1;
  MPI_Barrier(MPI_COMM_WORLD);
  /* re-init */
  init(&(x.v));
  computeAdj(&x,&f);
  MPI_Barrier(MPI_COMM_WORLD);
  printf("%d : adjoint result: %lf == %lf ?\n",myId,x.d, cos(x.v)*3);
  MPI_Finalize();
  return 0;
}
