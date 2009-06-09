#include "ampi.h"
#include "active.h"

int main(int argc, char **argv){
  struct active x, f;
  int myId, ierr;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  printf("I am %d\n",myId);
  init(&(x.v));
  compute(x.v,&(f.v));
  if (myId == 0) { 
    printf("final result: %lf\n",f.v);
    f.v=1.0;
  }
  ampi_reverse=1;
  MPI_Barrier(MPI_COMM_WORLD);
  printf("%d starting adjoint\n",myId);
  /* re-init */
  init(&(x.v));
  printf("%d after init \n",myId);
  computeAdj(&x,&f);
  if (myId == 0) { 
    printf("final adjoint result: %lf\n",x.d);
  }
  MPI_Finalize();
  return 0;
}
