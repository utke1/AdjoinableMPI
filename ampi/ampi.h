#include "mpi.h"

enum AMPI_activity { 
  AMPI_PASSIVE=0,
  AMPI_ACTIVE=1
};

int AMPI_Init(int* argc, 
	      char*** argv);

int AMPI_Finalize(void);

int AMPI_Comm_size(MPI_Comm comm, 
		   int *size);

int AMPI_Comm_rank(MPI_Comm comm, 
		   int *rank);

int AMPI_Get_processor_name(char *name, 
			    int *resultlen );

int AMPI_Send(void* buf, 
	      int count, 
	      MPI_Datatype datatype, 
	      char isActive,
	      int dest, 
	      int tag, 
	      MPI_Comm comm);

int AMPI_Recv(void* buf, 
	      int count,
	      MPI_Datatype datatype, 
	      char isActive,
	      int src, 
	      int tag, 
	      MPI_Comm comm,
	      MPI_Status* status);

int AMPI_Isend (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int dest, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request);

int AMPI_Irecv (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int src, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request);

int AMPI_Wait(MPI_Request *request, 
	      MPI_Status *status);

int AMPI_Waitall (int count, 
		  MPI_Request requests[], 
		  MPI_Status statuses[]);

int AMPI_Awaitall (int count, 
		   MPI_Request requests[], 
		   MPI_Status statuses[]);

int AMPI_Reduce (void* sbuf, 
		 void* rbuf, 
		 int count, 
		 MPI_Datatype datatype, 
		char isActive,
		 MPI_Op op, 
		 int root, 
		 MPI_Comm comm); 
