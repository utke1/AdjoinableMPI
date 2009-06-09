#include "mpi.h"

extern int ampi_reverse;

int  AMPI_awaitall (int count, 
		    MPI_Request * requests, 
		    MPI_Status * statuses);

int  AMPI_isend (void* buf, 
		 int count, 
		 MPI_Datatype datatype, 
		 int dest, 
		 int tag, 
		 MPI_Comm comm, 
		 MPI_Request* request);

int  AMPI_irecv (void* buf, 
		 int count, 
		 MPI_Datatype datatype, 
		 int src, 
		 int tag, 
		 MPI_Comm comm, 
		 MPI_Request* request);

int  AMPI_waitall (int count, 
		   MPI_Request * requests, 
		   MPI_Status * statuses);

int AMPI_reduce(void* sbuf, 
		void* rbuf, 
		int count, 
		MPI_Datatype datatype, 
		MPI_Op op, 
		int root, 
		MPI_Comm comm); 
