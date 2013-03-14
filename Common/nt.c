#include <mpi.h>
#include "ampi/userIF/nt.h"

extern void ADTOOL_AMPI_setupTypes();

int AMPI_Init_NT(int* argc, 
	      char*** argv) { 
  int rc;
  rc=MPI_Init(argc,
              argv);
  ADTOOL_AMPI_setupTypes();
  return rc;
}

int AMPI_Finalize_NT(void) { 
  return MPI_Finalize();
}


int AMPI_Buffer_attach_NT(void *buffer, 
			  int size) { 
  return MPI_Buffer_attach(buffer,
			   size);

}

int AMPI_Buffer_detach_NT(void *buffer, 
			  int *size){ 
  return MPI_Buffer_detach(buffer,
			   size);
}
