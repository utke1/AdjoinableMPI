#include "ampi/ampi.h"

int AMPI_Init(int* argc, 
	      char*** argv) { 
  return MPI_init(argc,argv);
}

int AMPI_Finalize(void) { 
  return MPI_finalize();
}

int AMPI_Send(void* buf, 
	      int count, 
	      MPI_Datatype datatype, 
	      char isActive,
	      int dest, 
	      int tag, 
	      MPI_Comm comm) { 
  if (isActive) 
    MPI_Abort(MPI_ERR_TYPE); 
  return MPI_send(buf,
		  count,
		  datatype,
		  dest,
		  tag,
		  comm);
}

int AMPI_Recv(void* buf, 
	      int cont,
	      MPI_Datatype datatype, 
	      char isActive,
	      int src, 
	      int tag, 
	      MPI_Comm comm,
	      MPI_Status* status) { 
  if (isActive) 
    MPI_Abort(MPI_ERR_TYPE); 
  return MPI_recv(buf,
		  count,
		  datatype,
		  src,
		  tag,
		  comm);
}  

int AMPI_isend (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int dest, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request) { 
  if (isActive) 
    MPI_Abort(MPI_ERR_TYPE); 
  return MPI_isend(buf,
		   count,
		   datatype,
		   dest,
		   tag,
		   comm,
		   request);
}

int AMPI_irecv (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int src, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request) { 
  if (isActive) 
    MPI_Abort(MPI_ERR_TYPE);
  return MPI_irecv(buf,
		   count,
		   datatype,
		   src,
		   tag,
		   comm,
		   request);
}


int AMPI_Wait(MPI_Request *request, 
	      MPI_Status *status) { 
  return MPI_wait(request,
		  status);
}

int AMPI_waitall (int count, 
		  MPI_Request requests[], 
		  MPI_Status statuses[]) { 
  return MPI_waitall(count,
		     requests,
		     statuses);
}

int AMPI_awaitall (int count, 
		  MPI_Request requests[], 
		  MPI_Status statuses[]);
  return MPI_SUCCESS;
}

int AMPI_reduce (void* sbuf, 
		 void* rbuf, 
		 int count, 
		 MPI_Datatype datatype, 
		 char isActive,
		 MPI_Op op, 
		 int root, 
		 MPI_Comm comm) { 
  if (isActive) 
    MPI_Abort(MPI_ERR_TYPE);
  return MPI_reduce(sbuf,
		    rbuf,
		    count,
		    datatype,
		    op,
		    root,
		    comm);
}
