#include "ampi/ampi.h"

int AMPI_Init(int* argc, 
	      char*** argv) { 
  return MPI_Init(argc,argv);
}

int AMPI_Finalize(void) { 
  return MPI_Finalize();
}

int AMPI_Send(void* buf, 
	      int count, 
	      MPI_Datatype datatype, 
	      char isActive,
	      int dest, 
	      int tag, 
	      MPI_Comm comm) { 
  if (isActive) MPI_Abort(comm, MPI_ERR_TYPE); 
  return MPI_Send(buf,
		  count,
		  datatype,
		  dest,
		  tag,
		  comm);
}

int AMPI_Recv(void* buf, 
	      int count,
	      MPI_Datatype datatype, 
	      char isActive,
	      int src, 
	      int tag, 
	      MPI_Comm comm,
	      MPI_Status* status) { 
  if (isActive) MPI_Abort(comm, MPI_ERR_TYPE); 
  return MPI_Recv(buf,
		  count,
		  datatype,
		  src,
		  tag,
		  comm,
		  status);
}  

int AMPI_Isend (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int dest, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request) { 
  if (isActive) MPI_Abort(comm, MPI_ERR_TYPE); 
  return MPI_Isend(buf,
		   count,
		   datatype,
		   dest,
		   tag,
		   comm,
		   request);
}

int AMPI_Irecv (void* buf, 
		int count, 
		MPI_Datatype datatype, 
		char isActive,
		int src, 
		int tag, 
		MPI_Comm comm, 
		MPI_Request* request) { 
  if (isActive) MPI_Abort(comm, MPI_ERR_TYPE);
  return MPI_Irecv(buf,
		   count,
		   datatype,
		   src,
		   tag,
		   comm,
		   request);
}


int AMPI_Wait(MPI_Request *request, 
	      MPI_Status *status) { 
  return MPI_Wait(request,
		  status);
}

int AMPI_waitall (int count, 
		  MPI_Request requests[], 
		  MPI_Status statuses[]) { 
  return MPI_Waitall(count,
		     requests,
		     statuses);
}

int AMPI_Awaitall (int count, 
		   MPI_Request requests[], 
		   MPI_Status statuses[]) { 
  return MPI_SUCCESS;
}

int AMPI_Reduce (void* sbuf, 
		 void* rbuf, 
		 int count, 
		 MPI_Datatype datatype, 
		 char isActive,
		 MPI_Op op, 
		 int root, 
		 MPI_Comm comm) { 
  if (isActive) MPI_Abort(comm, MPI_ERR_TYPE);
  return MPI_Reduce(sbuf,
		    rbuf,
		    count,
		    datatype,
		    op,
		    root,
		    comm);
}
