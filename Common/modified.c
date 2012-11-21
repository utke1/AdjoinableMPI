#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include "ampi/libCommon/modified.h"
#include "ampi/bookkeeping/support.h"
#include "ampi/adTool/support.h"

int FW_AMPI_Recv(void* buf, 
		 int count,
		 MPI_Datatype datatype, 
		 AMPI_Activity isActive,
		 int src, 
		 int tag,
		 AMPI_PairedWith pairedWith,
		 MPI_Comm comm,
		 MPI_Status* status) { 
  int rc;
  if (!(
	pairedWith==AMPI_SEND 
	|| 
	pairedWith==AMPI_BSEND 
	||
	pairedWith==AMPI_RSEND 
	||
	pairedWith==AMPI_ISEND_WAIT
	||
	pairedWith==AMPI_ISEND_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    MPI_Status myStatus;
    rc=MPI_Recv(buf,
		count,
		datatype,
		src,
		tag,
		comm,
		&myStatus); /* because status as passed in may be MPI_STATUS_IGNORE */
    if (rc==MPI_SUCCESS && isActive==AMPI_ACTIVE) {
      if(tag==MPI_ANY_TAG) tag=myStatus.MPI_TAG;
      if(src==MPI_ANY_SOURCE) src=myStatus.MPI_SOURCE;
      ADTOOL_AMPI_pushSRinfo(buf,
			     count,
			     datatype,
			     src,
			     tag,
			     pairedWith,
			     comm);
      ADTOOL_AMPI_push_CallCode(AMPI_RECV);
    }
    if (status!=MPI_STATUS_IGNORE) *status=myStatus;
  }
  return rc;
}  

int BW_AMPI_Recv(void* buf, 
		 int count,
		 MPI_Datatype datatype, 
		 AMPI_Activity isActive,
		 int src, 
		 int tag,
		 AMPI_PairedWith pairedWith,
		 MPI_Comm comm,
		 MPI_Status* status) { 
  int rc;
  ADTOOL_AMPI_popSRinfo(buf, /* ST ignores it, OO had mapped it already */
			&count,
			&datatype,
			&src,
			&tag,
			&pairedWith,
			&comm);
  if (!(
	pairedWith==AMPI_SEND 
	|| 
	pairedWith==AMPI_BSEND 
	||
	pairedWith==AMPI_RSEND 
	||
	pairedWith==AMPI_ISEND_WAIT
	||
	pairedWith==AMPI_ISEND_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    switch(pairedWith) { 
    case AMPI_ISEND_WAIT:
    case AMPI_SEND: { 
      ADTOOL_AMPI_getAdjointCount(&count,datatype);   
      rc=MPI_Send(buf,
		  count,
		  datatype,
		  src,
		  tag,
		  comm);
      ADTOOL_AMPI_adjointNullify(count,
				 buf);
      break;
    }
    default:  
      rc=MPI_Abort(comm, MPI_ERR_TYPE);
      break;
    }
  }
  return rc;
}  

int FW_AMPI_Isend (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
		   AMPI_Activity isActive,
		   int dest, 
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request) { 
  int rc;
  if (!(
	pairedWith==AMPI_RECV 
	|| 
	pairedWith==AMPI_IRECV_WAIT
	||
	pairedWith==AMPI_IRECV_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    rc= MPI_Isend(buf,
		  count,
		  datatype,
		  dest,
		  tag,
		  comm,
#ifdef AMPI_FORTRANCOMPATIBLE
		  request
#else 
		  &(request->plainRequest)
#endif 
		  );
    if (isActive==AMPI_ACTIVE) {
      struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
      struct AMPI_Request_S ampiRequestInst;
      ampiRequest=&ampiRequestInst;
      ampiRequest->plainRequest=request;
#else 
      ampiRequest=request;
#endif
      /* fill in the other info */
      ampiRequest->endPoint=dest;
      ampiRequest->tag=tag;
      ampiRequest->count=count;
      ampiRequest->datatype=datatype;
      ampiRequest->comm=comm;
      ampiRequest->origin=AMPI_SEND_ORIGIN;
      ampiRequest->pairedWith=pairedWith;
      ADTOOL_AMPI_mapBufForAdjoint(ampiRequest,buf);
#ifdef AMPI_FORTRANCOMPATIBLE
      BK_AMPI_put_AMPI_Request(ampiRequest);
#endif
      ADTOOL_AMPI_push_CallCode(AMPI_ISEND);
    }
  }
  return rc;
}

int BW_AMPI_Isend (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
		   AMPI_Activity isActive,
		   int dest, 
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request) { 
  int rc;
  MPI_Request *plainRequest;
  struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
  struct AMPI_Request_S ampiRequestInst;
  ampiRequest=&ampiRequestInst;
  ampiRequest->plainRequest=request;
  plainRequest=request;
  BK_AMPI_get_AMPI_Request(plainRequest,ampiRequest);
#else 
  ampiRequest=request;
  plainRequest=&(ampiRequest->plainRequest);
#endif
  if (!(
	ampiRequest->pairedWith==AMPI_RECV 
	|| 
	ampiRequest->pairedWith==AMPI_IRECV_WAIT
	||
	ampiRequest->pairedWith==AMPI_IRECV_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    switch(ampiRequest->pairedWith) { 
    case AMPI_RECV: { 
      rc=MPI_Wait(plainRequest,
		  MPI_STATUS_IGNORE);
      ADTOOL_AMPI_adjointIncrement(ampiRequest->adjointCount,
				   ampiRequest->buf,
				   ampiRequest->adjointTempBuf);
      ADTOOL_AMPI_releaseAdjointTempBuf(&ampiRequest);
      break;
    }
    default:  
      rc=MPI_Abort(ampiRequest->comm, MPI_ERR_TYPE);
      break;
    }
  }
  return rc;
}
