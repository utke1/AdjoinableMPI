#include "ampi/userIF/nt.h"

int AMPI_Init_NT(int* argc, 
	      char*** argv) { 
  return MPI_Init(argc,
		  argv);
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
