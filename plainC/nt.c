#include "ampi/internal/nt.h"

int AMPI_Init_NT(int* argc, 
	      char*** argv) { 
  return MPI_Init(argc,argv);
}

int AMPI_Finalize_NT(void) { 
  return MPI_Finalize();
}

