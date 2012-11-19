#include "ampi/libCommon/st.h"

int FW_AMPI_Wait_ST(AMPI_Request *request,
		    void*  buf,
		    MPI_Status *status) { 
  MPI_Request *plainRequest;
  struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
  plainRequest=request;
  ampiRequest=PR_AMPI_get_AMPI_Request(request);
#else 
  plainRequest=&(request->plainRequest);
  ampiRequest=request;
#endif 
  /* push request  */
  ADTOOL_AMPI_push_AMPI_Request(ampiRequest);
  return MPI_Wait(plainRequest,
		  status);
}

void ADTOOL_AMPI_push_AMPI_Request(struct AMPI_Request_S  *ampiRequest) { 
}
void ADTOOL_AMPI_pop_AMPI_Request(struct AMPI_Request_S  *ampiRequest) { 
}

void ADTOOL_AMPI_setBufForAdjoint(struct AMPI_Request_S  *ampiRequest,
				  void* buf) { 
  /* an overloading tool would not do this but rather leave the buffer as traced 
     because the memory mapping happens already at FW time */
  ampiRequest->buf=buf;
}

void ADTOOL_AMPI_setAdjointCount(struct AMPI_Request_S  *ampiRequest) { 
  /* for now we keep the count as is but for example in vector mode one would have to multiply by vector length */
}

void ADTOOL_AMPI_setAdjoinCountAndTempBuf(struct AMPI_Request_S *ampiRequest) { 
  ADTOOL_AMPI_setAdjointCount(ampiRequest);
  size_t s=0;
  switch(ampiRequest->datatype) { 
  case MPI_DOUBLE: 
    s=sizeof(double);
    break;
  case MPI_FLOAT: 
    s=sizeof(float);
    break;
  default:
    MPI_Abort(ampiRequest->comm, MPI_ERR_TYPE);
    break;
  }
  request->adjointTempBuf=(void*)malloc(ampiRequest->adjointCount*s);
  assert(request->adjointTempBuf);
}

int BW_AMPI_Wait_ST(AMPI_Request *request,
		    void*  buf,
		    MPI_Status *status) {
  int rc; 
  MPI_Request *plainRequest;
  struct AMPI_Request_S ampiRequest;
  /* pop request  */
  ADTOOL_AMPI_pop_AMPI_Request(&ampiRequest);
  ADTOOL_AMPI_setBufForAdjoint(&ampiRequest,buf);   
  switch(ampiRequest.origin) { 
  case AMPI_SEND_ORIGIN: { 
    ADTOOL_AMPI_setAdjoinCountAndTempBuf(&ampiRequest);   
    rc=MPI_Irecv(ampiRequest.adjointTempBuf,
	      ampiRequest.adjointCount,
	      ampiRequest.datatype,
	      ampiRequest.endPoint,
	      ampiRequest.tag,
	      ampiRequest.comm,
	      &(ampiRequest.plainRequest));
    break;
  }
  case AMPI_RECV_ORIGIN: { 
    ADTOOL_AMPI_setAdjoinCount(&ampiRequest);
    rc=MPI_Isend(ampiRequest.buf,
	      ampiRequest.adjointCount,
	      ampiRequest.datatype,
	      ampiRequest.endPoint,
	      ampiRequest.tag,
	      ampiRequest.comm,
	      &(ampiRequest.plainRequest));
    break;
  }
  default:  
    rc=MPI_Abort(ampiRequest.comm, MPI_ERR_TYPE);
    break;
  }
#ifdef AMPI_FORTRANCOMPATIBLE
  *request=ampiRequest.plainRequest;
  PR_AMPI_set_AMPI_Request(&ampiRequest);
#endif
  return rc;
}
