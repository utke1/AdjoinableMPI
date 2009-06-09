#include "mpi.h"

void ampi_irecv_bk(void *buf,
		   int count, 
		   MPI_Datatype datatype, 
		   int src, 
		   int tag, 
		   MPI_Comm comm, 
		   MPI_Request *req);

void ampi_isend_bk_(void *buf,
		    int count, 
		    MPI_Datatype datatype, 
		    int dest, 
		    int tag, 
		    MPI_Comm comm, 
		    MPI_Request *req); 

void ampi_HandleRequest_ (MPI_Request r);

int ampi_reduce(void* sbuf, 
		void* rbuf, 
		int count, 
		MPI_Datatype datatype, 
		MPI_Op op, 
		int root, 
		MPI_Comm comm);
