#include "mpi.h"

int AMPI_Init(int* argc, 
	      char*** argv);

int AMPI_Finalize(void);

int AMPI_Send(void* buf, 
	      int count, 
	      MPI_Datatype datatype, 
	      char isActive,
	      int dest, 
	      int tag, 
	      MPI_Comm comm);

int AMPI_Recv(void* buf, 
	      int cont,
	      MPI_Datatype datatype, 
	      char isActive,
	      int src, 
	      int tag, 
	      MPI_Comm comm,
	      MPI_Status* status);

int AMPI_isend (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int dest, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request);

int AMPI_irecv (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int src, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request);

int AMPI_Wait(MPI_Request *request, 
	      MPI_Status *status);

int AMPI_waitall (int count, 
		  MPI_Request requests[], 
		  MPI_Status statuses[]);

int AMPI_awaitall (int count, 
		   MPI_Request requests[], 
		   MPI_Status statuses[]);

int AMPI_reduce (void* sbuf, 
		 void* rbuf, 
		 int count, 
		 MPI_Datatype datatype, 
		char isActive,
		 MPI_Op op, 
		 int root, 
		 MPI_Comm comm); 
