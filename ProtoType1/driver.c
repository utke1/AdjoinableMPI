#include "mpi.h"

int main(int argc, char **argv){
  double x, f;
  int myId, ierr;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  printf("I am %d\n",myId);
  init(&x);
  compute(x,&f);
  if (myId == 0) { 
    printf("final result: %lf\n",f);
  }
  MPI_Finalize();
  return 0;
}
