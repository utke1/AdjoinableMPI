#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>
#include "ampi/libCommon/modified.h"
#include "ampi/bookkeeping/support.h"
#include "ampi/adTool/support.h"

MPI_Datatype AMPI_ADOUBLE;
MPI_Datatype AMPI_AFLOAT;

#ifdef AMPI_FORTRANCOMPATIBLE
MPI_Datatype AMPI_ADOUBLE_PRECISION;
MPI_Datatype AMPI_AREAL;
#endif

int FW_AMPI_Recv(void* buf, 
		 int count,
		 MPI_Datatype datatype, 
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
    double* mappedbuf=NULL;
    int dt_idx = derivedTypeIdx(datatype);
    int is_derived = isDerivedType(dt_idx);
    if((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
      mappedbuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(buf,&count);
    }
    else if(is_derived) {
      mappedbuf=(*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,datatype,comm);
    }
    else {
      mappedbuf=buf;
    }
    rc=MPI_Recv(mappedbuf,
		count,
		(*ourADTOOL_AMPI_FPCollection.FW_rawType_fp)(datatype),
		src,
		tag,
		comm,
		&myStatus); /* because status as passed in may be MPI_STATUS_IGNORE */
    if (rc==MPI_SUCCESS && ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE || is_derived)) {
      if (is_derived) {
	(*ourADTOOL_AMPI_FPCollection.unpackDType_fp)(mappedbuf,buf,count,dt_idx);
	(*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(mappedbuf);
      }
      (*ourADTOOL_AMPI_FPCollection.writeData_fp)(buf,&count);
      if(tag==MPI_ANY_TAG) tag=myStatus.MPI_TAG;
      if(src==MPI_ANY_SOURCE) src=myStatus.MPI_SOURCE;
      (*ourADTOOL_AMPI_FPCollection.pushSRinfo_fp)(buf,
						   count,
						   datatype,
						   src,
						   tag,
						   pairedWith,
						   comm);
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_RECV);
    }
    if (status!=MPI_STATUS_IGNORE) *status=myStatus;
  }
  return rc;
}  

int BW_AMPI_Recv(void* buf, 
		 int count,
		 MPI_Datatype datatype, 
		 int src, 
		 int tag,
		 AMPI_PairedWith pairedWith,
		 MPI_Comm comm,
		 MPI_Status* status) {
  int rc;
  void *idx=NULL;
  (*ourADTOOL_AMPI_FPCollection.popSRinfo_fp)(&buf,
					      &count,
					      &datatype,
					      &src,
					      &tag,
					      &pairedWith,
					      &comm,
					      &idx);
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
      MPI_Datatype mappedtype = (*ourADTOOL_AMPI_FPCollection.BW_rawType_fp)(datatype);
      (*ourADTOOL_AMPI_FPCollection.getAdjointCount_fp)(&count,datatype);   
      rc=MPI_Send(buf,
		  count,
		  mappedtype,
		  src,
		  tag,
		  comm);
      (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(count,mappedtype,comm,
				 buf, buf, buf);
      break;
    }
    default:  
      rc=MPI_Abort(comm, MPI_ERR_TYPE);
      break;
    }
  }
  return rc;
}  

int FW_AMPI_Irecv (void* buf,
		   int count,
		   MPI_Datatype datatype,
		   int source,
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm,
		   AMPI_Request* request) {
  int rc;
  if (!(
	pairedWith==AMPI_SEND
        ||
        pairedWith==AMPI_ISEND_WAIT
	||
	pairedWith==AMPI_ISEND_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else {
    double* mappedbuf=NULL;
    if((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
      mappedbuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(buf,&count);
    }
    else {
      mappedbuf=buf;
    }
    rc= MPI_Irecv(mappedbuf,
		  count,
		  datatype,
		  source,
		  tag,
		  comm,
#ifdef AMPI_FORTRANCOMPATIBLE
                  request
#else
                  &(request->plainRequest)
#endif
		  );
    struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
    struct AMPI_Request_S ampiRequestInst;
    ampiRequest=&ampiRequestInst;
    ampiRequest->plainRequest=*request;
#else 
    ampiRequest=request;
#endif
    /* fill in the other info */
    ampiRequest->endPoint=source;
    ampiRequest->tag=tag;
    ampiRequest->count=count;
    ampiRequest->datatype=datatype;
    ampiRequest->comm=comm;
    ampiRequest->origin=AMPI_RECV_ORIGIN;
    ampiRequest->pairedWith=pairedWith;
    (*ourADTOOL_AMPI_FPCollection.mapBufForAdjoint_fp)(ampiRequest,buf);
    ampiRequest->tracedRequest=ampiRequest->plainRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
    BK_AMPI_put_AMPI_Request(ampiRequest);
#endif
    if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_IRECV);
#ifdef AMPI_REQUESTONTRACE
      (*ourADTOOL_AMPI_FPCollection.push_request_fp)(ampiRequest->tracedRequest);
#endif
    }
  }
  return rc;
}

int BW_AMPI_Irecv (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
		   int source, 
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request) {
  int rc;
  MPI_Request *plainRequest;
  struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_REQUESTONTRACE
  MPI_Request tracedRequest;
#endif
#ifdef AMPI_FORTRANCOMPATIBLE
  struct AMPI_Request_S ampiRequestInst;
  ampiRequest=&ampiRequestInst;
  plainRequest=request;
#else
  plainRequest=&(request->plainRequest) ;
  ampiRequest=request;
#endif
#if defined AMPI_FORTRANCOMPATIBLE || defined AMPI_REQUESTONTRACE
#ifdef AMPI_REQUESTONTRACE
  tracedRequest=(*ourADTOOL_AMPI_FPCollection.pop_request_fp)();
  BK_AMPI_get_AMPI_Request(&tracedRequest,ampiRequest,1);
#else 
  BK_AMPI_get_AMPI_Request(plainRequest,ampiRequest,0);
#endif
#endif
  assert(ampiRequest->origin==AMPI_RECV_ORIGIN) ;
  if (!(
	ampiRequest->pairedWith==AMPI_SEND 
	|| 
	ampiRequest->pairedWith==AMPI_ISEND_WAIT
	||
	ampiRequest->pairedWith==AMPI_ISEND_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    switch(ampiRequest->pairedWith) { 
    case AMPI_SEND:
    case AMPI_ISEND_WAIT: {
      rc=MPI_Wait(plainRequest,
		  MPI_STATUS_IGNORE);
      (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(ampiRequest->adjointCount,
                                 ampiRequest->datatype,
                                 ampiRequest->comm,
                                 ampiRequest->buf,
                                 ampiRequest->adjointBuf,
                                 buf);
      break ;
    }
    default:  
      rc=MPI_Abort(ampiRequest->comm, MPI_ERR_TYPE);
      break;
    }
  }
  return rc;
}

int FW_AMPI_Send (void* buf, 
                  int count, 
                  MPI_Datatype datatype, 
                  int dest, 
                  int tag,
                  AMPI_PairedWith pairedWith,
                  MPI_Comm comm) {
  int rc;
  if (!(
	pairedWith==AMPI_RECV 
	|| 
	pairedWith==AMPI_IRECV_WAIT
	||
	pairedWith==AMPI_IRECV_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else {
    double* mappedbuf=NULL;
    int dt_idx = derivedTypeIdx(datatype);
    int is_derived = isDerivedType(dt_idx);
    if((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
      mappedbuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(buf,&count);
    }
    else if(is_derived) {
      mappedbuf=(*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,datatype,comm);
      (*ourADTOOL_AMPI_FPCollection.packDType_fp)(buf,mappedbuf,count,dt_idx);
    }
    else {
      mappedbuf=buf;
    }
    rc=MPI_Send(mappedbuf,
		count,
		(*ourADTOOL_AMPI_FPCollection.FW_rawType_fp)(datatype),
		/* if derived then need to replace typemap */
		dest,
		tag,
		comm);
    if (is_derived) (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(mappedbuf);
    if (rc==MPI_SUCCESS && ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE || is_derived)) {
      (*ourADTOOL_AMPI_FPCollection.pushSRinfo_fp)(buf,
						   count,
						   datatype,
						   dest,
						   tag,
						   pairedWith,
						   comm);
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_SEND);
    }
  }
  return rc;
}

int BW_AMPI_Send (void* buf,
                  int count, 
                  MPI_Datatype datatype, 
                  int dest, 
                  int tag,
                  AMPI_PairedWith pairedWith,
                  MPI_Comm comm) {
  int rc;
  void *idx=NULL;
  (*ourADTOOL_AMPI_FPCollection.popSRinfo_fp)(&buf,
					      &count,
					      &datatype,
					      &dest,
					      &tag,
					      &pairedWith,
					      &comm,
					      &idx);
  if (!(
	pairedWith==AMPI_RECV 
	|| 
	pairedWith==AMPI_IRECV_WAIT
	||
	pairedWith==AMPI_IRECV_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else {
    switch(pairedWith) {
    case AMPI_IRECV_WAIT:
    case AMPI_RECV: { 
      MPI_Datatype mappedtype = (*ourADTOOL_AMPI_FPCollection.BW_rawType_fp)(datatype);
      (*ourADTOOL_AMPI_FPCollection.getAdjointCount_fp)(&count,datatype);
      void *tempBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,mappedtype,comm) ;
      rc=MPI_Recv(tempBuf,
                  count,
                  mappedtype,
                  dest,
                  tag,
                  comm,
                  MPI_STATUS_IGNORE) ;
      (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(count,
                                   mappedtype,
                                   comm,
				   buf,
				   buf,
                                   buf,
                                   tempBuf,
				   idx);
      (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(tempBuf);
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
    double* mappedbuf=NULL;
    if((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
      mappedbuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(buf,&count);
    }
    else {
      mappedbuf=buf;
    }
    rc= MPI_Isend(mappedbuf,
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
    struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
    struct AMPI_Request_S ampiRequestInst;
    ampiRequest=&ampiRequestInst;
    ampiRequest->plainRequest=*request;
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
    (*ourADTOOL_AMPI_FPCollection.mapBufForAdjoint_fp)(ampiRequest,buf);
    ampiRequest->tracedRequest=ampiRequest->plainRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
    BK_AMPI_put_AMPI_Request(ampiRequest);
#endif
    if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_ISEND);
#ifdef AMPI_REQUESTONTRACE
      (*ourADTOOL_AMPI_FPCollection.push_request_fp)(ampiRequest->tracedRequest);
#endif
    }
  }
  return rc;
}

int BW_AMPI_Isend (void* buf, 
		   int count, 
		   MPI_Datatype datatype, 
		   int dest, 
		   int tag,
		   AMPI_PairedWith pairedWith,
		   MPI_Comm comm, 
		   AMPI_Request* request) { 
  int rc;
  MPI_Request *plainRequest;
  struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_REQUESTONTRACE
  MPI_Request tracedRequest;
#endif
#ifdef AMPI_FORTRANCOMPATIBLE
  struct AMPI_Request_S ampiRequestInst;
  ampiRequest=&ampiRequestInst;
  plainRequest=request;
#else 
  ampiRequest=request;
  plainRequest=&(ampiRequest->plainRequest);
#endif
#if defined AMPI_FORTRANCOMPATIBLE || defined AMPI_REQUESTONTRACE
#ifdef AMPI_REQUESTONTRACE
  tracedRequest=(*ourADTOOL_AMPI_FPCollection.pop_request_fp)();
  BK_AMPI_get_AMPI_Request(&tracedRequest,ampiRequest,1);
#else 
  BK_AMPI_get_AMPI_Request(plainRequest,ampiRequest,0);
#endif
#endif
  assert(ampiRequest->origin==AMPI_SEND_ORIGIN) ;
  if (!(
	ampiRequest->pairedWith==AMPI_RECV 
	|| 
	ampiRequest->pairedWith==AMPI_IRECV_WAIT
	||
	ampiRequest->pairedWith==AMPI_IRECV_WAITALL
	)) rc=MPI_Abort(comm, MPI_ERR_ARG);
  else { 
    switch(ampiRequest->pairedWith) { 
    case AMPI_RECV:
    case AMPI_IRECV_WAIT: { 
      rc=MPI_Wait(plainRequest,
		  MPI_STATUS_IGNORE);
      (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(ampiRequest->adjointCount,
                                   ampiRequest->datatype,
                                   ampiRequest->comm,
				   ampiRequest->buf,
				   ampiRequest->adjointBuf,
                                   buf,
                                   ampiRequest->adjointTempBuf,
	                           ampiRequest->idx);
      (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(ampiRequest->adjointTempBuf);
      break;
    }
    default:  
      rc=MPI_Abort(ampiRequest->comm, MPI_ERR_TYPE);
      break;
    }
  }
  return rc;
}

int FW_AMPI_Wait(AMPI_Request *request,
		 MPI_Status *status) { 
  int rc;
  MPI_Request *plainRequest;
  struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
  struct AMPI_Request_S ampiRequestInst;
  ampiRequest=&ampiRequestInst;
  plainRequest=request;
  /*[llh] doubt about the 3rd argument (0?) for the OO traced case: */
  BK_AMPI_get_AMPI_Request(plainRequest,ampiRequest,0);
#else 
  plainRequest=&(request->plainRequest);
  ampiRequest=request;
#endif 
  rc=MPI_Wait(plainRequest,
	      status);
  if (rc==MPI_SUCCESS && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(ampiRequest->datatype)==AMPI_ACTIVE) {
    (*ourADTOOL_AMPI_FPCollection.writeData_fp)(ampiRequest->buf,&ampiRequest->count);
    (*ourADTOOL_AMPI_FPCollection.push_AMPI_Request_fp)(ampiRequest);
    (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_WAIT);
  }
  return rc;
}

int BW_AMPI_Wait(AMPI_Request *request,
		 MPI_Status *status) {
  int rc; 
  struct AMPI_Request_S *ampiRequest;
#ifdef AMPI_FORTRANCOMPATIBLE
  struct AMPI_Request_S ampiRequestInst;
  ampiRequest=&ampiRequestInst;
#else 
  ampiRequest=request;
#endif 
  /* pop request  */
  (*ourADTOOL_AMPI_FPCollection.pop_AMPI_Request_fp)(ampiRequest);
  switch(ampiRequest->origin) { 
  case AMPI_SEND_ORIGIN: { 
    (*ourADTOOL_AMPI_FPCollection.setAdjointCountAndTempBuf_fp)(ampiRequest);   
    rc=MPI_Irecv(ampiRequest->adjointTempBuf,
		 ampiRequest->adjointCount,
		 ampiRequest->datatype,
		 ampiRequest->endPoint,
		 ampiRequest->tag,
		 ampiRequest->comm,
		 &(ampiRequest->plainRequest));
    break;
  }
  case AMPI_RECV_ORIGIN: { 
    (*ourADTOOL_AMPI_FPCollection.setAdjointCount_fp)(ampiRequest);
    rc=MPI_Isend((*ourADTOOL_AMPI_FPCollection.rawAdjointData_fp)(ampiRequest->adjointBuf),
		 ampiRequest->adjointCount,
		 ampiRequest->datatype,
		 ampiRequest->endPoint,
		 ampiRequest->tag,
		 ampiRequest->comm,
		 &(ampiRequest->plainRequest));
    break;
  }
  default:  
    rc=MPI_Abort(ampiRequest->comm, MPI_ERR_TYPE);
    break;
  }
#ifdef AMPI_FORTRANCOMPATIBLE 
  *request=ampiRequest->plainRequest;
#endif
#if defined AMPI_FORTRANCOMPATIBLE || defined AMPI_REQUESTONTRACE
  BK_AMPI_put_AMPI_Request(ampiRequest);
#endif
  return rc;
}

int FW_AMPI_Barrier(MPI_Comm comm){
  int rc;
  rc=MPI_Barrier(comm);
  (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_BARRIER);
  (*ourADTOOL_AMPI_FPCollection.push_comm_fp)(comm);
  return rc;
}

int BW_AMPI_Barrier(MPI_Comm comm){
  int rc;
  comm=(*ourADTOOL_AMPI_FPCollection.pop_comm_fp)();
  rc=MPI_Barrier(comm);
  return rc;
}

int FW_AMPI_Gather(void *sendbuf,
		   int sendcnt,
		   MPI_Datatype sendtype,
		   void *recvbuf,
		   int recvcnt,
		   MPI_Datatype recvtype,
		   int root,
		   MPI_Comm comm) {
  void *rawSendBuf=sendbuf, *rawRecvBuf=recvbuf;
  int rc=MPI_SUCCESS;
  int isInPlace=(sendbuf==MPI_IN_PLACE);
  int myRank, myCommSize;
  MPI_Comm_rank(comm, &myRank);
  MPI_Comm_size(comm, &myCommSize);
  if (!isInPlace && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)!=(*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)) {
    rc=MPI_Abort(comm, MPI_ERR_ARG);
  }
  else {
    if (!isInPlace && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)==AMPI_ACTIVE)  rawSendBuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(sendbuf,&sendcnt);
    if (myRank==root) {
      if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)==AMPI_ACTIVE)  rawRecvBuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(recvbuf,&recvcnt);
    }
    rc=MPI_Gather(rawSendBuf,
		  sendcnt,
		  sendtype,
		  rawRecvBuf,
		  recvcnt,
		  recvtype,
		  root,
		  comm);
    if (rc==MPI_SUCCESS && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)==AMPI_ACTIVE) {
      if (myRank==root) (*ourADTOOL_AMPI_FPCollection.writeData_fp)(recvbuf,&recvcnt);
      (*ourADTOOL_AMPI_FPCollection.pushGSinfo_fp)(((myRank==root)?myCommSize:0),
						   recvbuf,
						   recvcnt,
						   recvtype,
						   sendbuf,
						   sendcnt,
						   sendtype,
						   root,
						   comm);
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_GATHER);
    }
  }
  return rc;
}

int BW_AMPI_Gather(void *sendbuf,
		   int sendcnt,
		   MPI_Datatype sendtype,
		   void *recvbuf,
		   int recvcnt,
		   MPI_Datatype recvtype,
		   int root,
		   MPI_Comm comm) {
  void *idx=NULL;
  int rc=MPI_SUCCESS;
  int commSizeForRootOrNull, rTypeSize,i;
  (*ourADTOOL_AMPI_FPCollection.popGScommSizeForRootOrNull_fp)(&commSizeForRootOrNull);
  (*ourADTOOL_AMPI_FPCollection.popGSinfo_fp)(commSizeForRootOrNull,
					      &recvbuf,
					      &recvcnt,
					      &recvtype,
					      &sendbuf,
					      &sendcnt,
					      &sendtype,
					      &root,
					      &comm);
  (*ourADTOOL_AMPI_FPCollection.getAdjointCount_fp)(&sendcnt,sendtype);
  void *tempBuf = 0;
  if (sendcnt>0) tempBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(sendcnt,sendtype,comm) ;
  else {
    if (commSizeForRootOrNull) 
      tempBuf=MPI_IN_PLACE;
    else 
      tempBuf=0;
  }
  rc=MPI_Scatter(recvbuf,
		 recvcnt,
		 recvtype,
		 tempBuf,
		 sendcnt,
		 sendtype,
		 root,
		 comm);
  (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(sendcnt,
						     sendtype,
						     comm,
						     sendbuf,
						     sendbuf,
						     sendbuf,
						     tempBuf,
						     idx);
  if (commSizeForRootOrNull) {
    MPI_Type_size(recvtype,&rTypeSize);
    for (i=0;i<commSizeForRootOrNull;++i) { 
      if (! (i==root && sendcnt==0)) { /* don't nullify the segment if "in place" on root */
	void *recvbufSegment=recvbuf+(i*recvcnt*rTypeSize);
	(*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(recvcnt,recvtype,comm,
							 recvbufSegment , recvbufSegment, recvbufSegment);
      }
    }
  }
  if (tempBuf!=MPI_IN_PLACE && tempBuf!=0) (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(tempBuf);
  return rc;
}

int FW_AMPI_Scatter(void *sendbuf,
                     int sendcnt,
                     MPI_Datatype sendtype,
                     void *recvbuf,
                     int recvcnt,
                     MPI_Datatype recvtype,
                     int root,
                     MPI_Comm comm) {
  int rc=MPI_SUCCESS;
  int myRank, myCommSize;
  int isInPlace=(recvbuf==MPI_IN_PLACE);
  void *rawSendBuf=sendbuf, *rawRecvBuf=recvbuf;
  MPI_Comm_rank(comm, &myRank);
  MPI_Comm_size(comm, &myCommSize);
  if (!isInPlace && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)!=(*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)) {
    rc=MPI_Abort(comm, MPI_ERR_ARG);
  }
  else {
    if (myRank==root) {
      if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)==AMPI_ACTIVE)  rawSendBuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(sendbuf,&sendcnt);
    }
    if (!isInPlace && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)==AMPI_ACTIVE)  rawRecvBuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(recvbuf,&recvcnt);
    rc=MPI_Scatter(rawSendBuf,
                   sendcnt,
                   sendtype,
                   rawRecvBuf,
                   recvcnt,
                   recvtype,
                   root,
                   comm);
    if (rc==MPI_SUCCESS && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)==AMPI_ACTIVE) {
      (*ourADTOOL_AMPI_FPCollection.writeData_fp)(recvbuf,&recvcnt);
      (*ourADTOOL_AMPI_FPCollection.pushGSinfo_fp)(((myRank==root)?myCommSize:0),
						   sendbuf,
						   sendcnt,
						   sendtype,
						   recvbuf,
						   recvcnt,
						   recvtype,
						   root,
						   comm);
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_SCATTER);
    }
  }
  return rc;
}

int BW_AMPI_Scatter(void *sendbuf,
                     int sendcnt,
                     MPI_Datatype sendtype,
                     void *recvbuf,
                     int recvcnt,
                     MPI_Datatype recvtype,
                     int root,
                     MPI_Comm comm) {
  int rc=MPI_SUCCESS;
  void *idx=NULL;
  int commSizeForRootOrNull;
  (*ourADTOOL_AMPI_FPCollection.popGScommSizeForRootOrNull_fp)(&commSizeForRootOrNull);
  (*ourADTOOL_AMPI_FPCollection.popGSinfo_fp)(commSizeForRootOrNull,
					      &sendbuf,
					      &sendcnt,
					      &sendtype,
					      &recvbuf,
					      &recvcnt,
					      &recvtype,
					      &root,
					      &comm);
  void *tempBuf = NULL;
  if (commSizeForRootOrNull>0) tempBuf=(*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(sendcnt*commSizeForRootOrNull,sendtype,comm);
  rc=MPI_Gather(recvbuf,
		recvcnt,
		recvtype,
		tempBuf,
                sendcnt,
		sendtype,
		root,
		comm);
  (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(recvcnt,recvtype,comm,
                             recvbuf, recvbuf, recvbuf);
  if (commSizeForRootOrNull>0) {
    (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(sendcnt*commSizeForRootOrNull,
				 sendtype,
				 comm,
				 sendbuf,
				 sendbuf,
				 sendbuf,
				 tempBuf,
				 idx);
    (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(tempBuf);
  }
  return rc;
}

int FW_AMPI_Allgather(void *sendbuf,
                      int sendcount,
                      MPI_Datatype sendtype,
                      void *recvbuf,
                      int recvcount,
                      MPI_Datatype recvtype,
                      MPI_Comm comm) {
  void *rawSendBuf=NULL, *rawRecvBuf=NULL;
  int rc=MPI_SUCCESS;
  int myRank, myCommSize;
  MPI_Comm_rank(comm, &myRank);
  MPI_Comm_size(comm, &myCommSize);
  if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)!=(*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)) {
    rc=MPI_Abort(comm, MPI_ERR_ARG);
  }
  else {
    if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)==AMPI_ACTIVE)  rawSendBuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(sendbuf,&sendcount);
    else rawSendBuf=sendbuf;
    if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)==AMPI_ACTIVE)  rawRecvBuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(recvbuf,&recvcount);
    else rawRecvBuf=recvbuf;
    rc=MPI_Allgather(rawSendBuf,
                     sendcount,
                     sendtype,
                     rawRecvBuf,
                     recvcount,
                     recvtype,
                     comm);
    if (rc==MPI_SUCCESS && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)==AMPI_ACTIVE) {
      (*ourADTOOL_AMPI_FPCollection.writeData_fp)(recvbuf,&recvcount);
      (*ourADTOOL_AMPI_FPCollection.pushGSinfo_fp)((myCommSize),
						   recvbuf,
						   recvcount,
						   recvtype,
						   sendbuf,
						   sendcount,
						   sendtype,
						   0,
						   comm);
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_ALLGATHER);
    }
  }
  return rc;
}

int BW_AMPI_Allgather(void *sendbuf,
                      int sendcount,
                      MPI_Datatype sendtype,
                      void *recvbuf,
                      int recvcount,
                      MPI_Datatype recvtype,
                      MPI_Comm comm) {
  void *idx=NULL;
  int rc=MPI_SUCCESS, rootPlaceholder;
  int commSizeForRootOrNull, rTypeSize, *recvcounts,i;
  (*ourADTOOL_AMPI_FPCollection.popGScommSizeForRootOrNull_fp)(&commSizeForRootOrNull);
  (*ourADTOOL_AMPI_FPCollection.popGSinfo_fp)(commSizeForRootOrNull,
					      &recvbuf,
					      &recvcount,
					      &recvtype,
					      &sendbuf,
					      &sendcount,
					      &sendtype,
					      &rootPlaceholder,
					      &comm);
  recvcounts=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
  for (i=0;i<commSizeForRootOrNull;++i) recvcounts[i]=sendcount;
  void *tempBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(sendcount,sendtype,comm);
  /**
   * \todo shortcut taken below by assuming AMPI_ADOUBLE is equivalent to MPI_DOUBLE, need type map.
   */
  rc=MPI_Reduce_scatter(recvbuf,
                        tempBuf,
                        recvcounts,
                        MPI_DOUBLE, /* <<< here is the offending bit */
                        MPI_SUM,
                        comm);
  (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(sendcount,
                               sendtype,
                               comm,
                               sendbuf,
                               sendbuf,
                               sendbuf,
                               tempBuf,
                               idx);
  if (commSizeForRootOrNull) {
    MPI_Type_size(recvtype,&rTypeSize);
    (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(recvcount*commSizeForRootOrNull,recvtype,comm,
                               recvbuf , recvbuf, recvbuf);
  }
  (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(tempBuf);
  return rc;
}

int FW_AMPI_Gatherv(void *sendbuf,
                    int sendcnt,
                    MPI_Datatype sendtype,
                    void *recvbuf,
                    int *recvcnts,
                    int *displs,
                    MPI_Datatype recvtype,
                    int root,
                    MPI_Comm comm) {
  void *rawSendBuf=sendbuf, *rawRecvBuf=recvbuf;
  int rc=MPI_SUCCESS;
  int isInPlace=(sendbuf==MPI_IN_PLACE);
  int myRank, myCommSize;
  MPI_Comm_rank(comm, &myRank);
  MPI_Comm_size(comm, &myCommSize);
  if (!isInPlace && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)!=(*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)) {
    rc=MPI_Abort(comm, MPI_ERR_ARG);
  }
  else {
    if (!isInPlace && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)==AMPI_ACTIVE)  rawSendBuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(sendbuf,&sendcnt);
    if (myRank==root) {
      if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)==AMPI_ACTIVE)  rawRecvBuf=(*ourADTOOL_AMPI_FPCollection.rawDataV_fp)(recvbuf, myCommSize, recvcnts, displs);
    }
    rc=MPI_Gatherv(rawSendBuf,
                   sendcnt,
                   sendtype,
                   rawRecvBuf,
                   recvcnts,
                   displs,
                   recvtype,
                   root,
                   comm);
    if (rc==MPI_SUCCESS && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)==AMPI_ACTIVE) {
      if (myRank==root) (*ourADTOOL_AMPI_FPCollection.writeDataV_fp)(recvbuf,recvcnts, displs);
      (*ourADTOOL_AMPI_FPCollection.pushGSVinfo_fp)(((myRank==root)?myCommSize:0),
						    recvbuf,
						    recvcnts,
						    displs,
						    recvtype,
						    sendbuf,
						    sendcnt,
						    sendtype,
						    root,
						    comm);
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_GATHERV);
    }
  }
  return rc;
}

int BW_AMPI_Gatherv(void *sendbuf,
                    int sendcnt,
                    MPI_Datatype sendtype,
                    void *recvbuf,
                    int *recvcnts,
                    int *displs,
                    MPI_Datatype recvtype,
                    int root,
                    MPI_Comm comm) {
  void *idx=NULL;
  int i;
  int rc=MPI_SUCCESS;
  int myRank, commSizeForRootOrNull, rTypeSize;
  int *tRecvCnts=recvcnts, *tDispls=displs;
  char tRecvCntsFlag=0, tDisplsFlag=0;
  (*ourADTOOL_AMPI_FPCollection.popGScommSizeForRootOrNull_fp)(&commSizeForRootOrNull);
  if (tRecvCnts==NULL) {
    tRecvCnts=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
    tRecvCntsFlag=1;
  }
  if (tDispls==NULL) {
    tDispls=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
    tDisplsFlag=1;
  }
  (*ourADTOOL_AMPI_FPCollection.popGSVinfo_fp)(commSizeForRootOrNull,
					       &recvbuf,
					       tRecvCnts,
					       tDispls,
					       &recvtype,
					       &sendbuf,
					       &sendcnt,
					       &sendtype,
					       &root,
					       &comm);
  MPI_Comm_rank(comm, &myRank);
  (*ourADTOOL_AMPI_FPCollection.getAdjointCount_fp)(&sendcnt,sendtype);
  void *tempBuf = 0;
  if (sendcnt>0) tempBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(sendcnt,sendtype,comm) ;
  else {
    if (commSizeForRootOrNull) 
      tempBuf=MPI_IN_PLACE;
    else 
      tempBuf=0;
  }
  rc=MPI_Scatterv(recvbuf,
                  tRecvCnts,
                  tDispls,
                  recvtype,
                  tempBuf,
                  sendcnt,
                  sendtype,
                  root,
                  comm);
  (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(sendcnt,
						     sendtype,
						     comm,
						     sendbuf,
						     sendbuf,
						     sendbuf,
						     tempBuf,
						     idx);
  if (commSizeForRootOrNull) {
    MPI_Type_size(recvtype,&rTypeSize);
    for (i=0;i<commSizeForRootOrNull;++i) {
      if (! (i==root && sendcnt==0)) { /* don't nullify the segment if "in place" on root */
	void* recvbufSegment=recvbuf+(rTypeSize*tDispls[i]); /* <----------  very iffy! */
	(*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(tRecvCnts[i],recvtype,comm,
							 recvbufSegment , recvbufSegment, recvbufSegment);
      }
    }
  }
  if (tempBuf!=MPI_IN_PLACE && tempBuf!=0) (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(tempBuf);
  if (tRecvCntsFlag) free((void*)(tRecvCnts));
  if (tDisplsFlag) free((void*)(tDispls));
  return rc;
}

int FW_AMPI_Scatterv(void *sendbuf,
                     int *sendcnts,
                     int *displs,
                     MPI_Datatype sendtype,
                     void *recvbuf,
                     int recvcnt,
                     MPI_Datatype recvtype,
                     int root,
                     MPI_Comm comm) {
  int rc=MPI_SUCCESS;
  int myRank, myCommSize;
  int isInPlace=(recvbuf==MPI_IN_PLACE);
  void *rawSendBuf=sendbuf, *rawRecvBuf=recvbuf;
  MPI_Comm_rank(comm, &myRank);
  MPI_Comm_size(comm, &myCommSize);
  if (!isInPlace && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)!=(*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)) {
    rc=MPI_Abort(comm, MPI_ERR_ARG);
  }
  else {
    if (myRank==root) {
      if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)==AMPI_ACTIVE) rawSendBuf=(*ourADTOOL_AMPI_FPCollection.rawDataV_fp)(sendbuf,myCommSize,sendcnts,displs);
    }
    if (!isInPlace && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)==AMPI_ACTIVE)  rawRecvBuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(recvbuf,&recvcnt);
    rc=MPI_Scatterv(rawSendBuf,
                    sendcnts,
                    displs,
                    sendtype,
                    rawRecvBuf,
                    recvcnt,
                    recvtype,
                    root,
                    comm);
    if (rc==MPI_SUCCESS && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)==AMPI_ACTIVE) {
      (*ourADTOOL_AMPI_FPCollection.writeData_fp)(recvbuf,&recvcnt);
      (*ourADTOOL_AMPI_FPCollection.pushGSVinfo_fp)(((myRank==root)?myCommSize:0),
						    sendbuf,
						    sendcnts,
						    displs,
						    sendtype,
						    recvbuf,
						    recvcnt,
						    recvtype,
						    root,
						    comm);
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_SCATTERV);
    }
  }
  return rc;
}

int BW_AMPI_Scatterv(void *sendbuf,
                     int *sendcnts,
                     int *displs,
                     MPI_Datatype sendtype,
                     void *recvbuf,
                     int recvcnt,
                     MPI_Datatype recvtype,
                     int root,
                     MPI_Comm comm) {
  int rc=MPI_SUCCESS;
  void *idx=NULL;
  int sendSize=0,i, typeSize;
  int myRank, commSizeForRootOrNull, *tempDispls;
  int *tSendCnts=sendcnts, *tDispls=displs;
  char tSendCntsFlag=0, tDisplsFlag=0;
  (*ourADTOOL_AMPI_FPCollection.popGScommSizeForRootOrNull_fp)(&commSizeForRootOrNull);
  if (tSendCnts==NULL && commSizeForRootOrNull>0) {
    tSendCnts=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
    tSendCntsFlag=1;
  }
  if (tDispls==NULL && commSizeForRootOrNull>0) {
    tDispls=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
    tDisplsFlag=1;
  }
  (*ourADTOOL_AMPI_FPCollection.popGSVinfo_fp)(commSizeForRootOrNull,
					       &sendbuf,
					       tSendCnts,
					       tDispls,
					       &sendtype,
					       &recvbuf,
					       &recvcnt,
					       &recvtype,
					       &root,
					       &comm);
  MPI_Comm_rank(comm, &myRank);
  tempDispls=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
  for (i=0;i<commSizeForRootOrNull;++i) {
    tempDispls[i]=sendSize;
    sendSize+=tSendCnts[i];
  }
  void *tempBuf = NULL;
  if (commSizeForRootOrNull>0) tempBuf=(*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(sendSize,sendtype,comm);
  rc=MPI_Gatherv(recvbuf,
                 recvcnt,
                 recvtype,
                 tempBuf,
                 tSendCnts,
                 tempDispls,
                 sendtype,
                 root,
                 comm);
  (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(recvcnt,recvtype,comm,
                             recvbuf, recvbuf, recvbuf);
  if (commSizeForRootOrNull>0) {
    MPI_Type_size(sendtype,&typeSize);
    for (i=0;i<commSizeForRootOrNull;++i) {
      void* buf=sendbuf+(typeSize*tDispls[i]); /* <----------  very iffy! */
      void* sourceBuf=tempBuf+(typeSize*tempDispls[i]);
      (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(tSendCnts[i],
                                   sendtype,
                                   comm,
                                   buf,
                                   buf,
                                   buf,
                                   sourceBuf,
                                   idx);
    }
    (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(tempBuf);
  }
  if (tSendCntsFlag) free((void*)(tSendCnts));
  if (tDisplsFlag) free((void*)(tDispls));
  return rc;
}

int FW_AMPI_Allgatherv(void *sendbuf,
                       int sendcnt,
                       MPI_Datatype sendtype,
                       void *recvbuf,
                       int *recvcnts,
                       int *displs,
                       MPI_Datatype recvtype,
                       MPI_Comm comm) {
  void *rawSendBuf=NULL, *rawRecvBuf=NULL;
  int rc=MPI_SUCCESS;
  int myRank, myCommSize;
  MPI_Comm_rank(comm, &myRank);
  MPI_Comm_size(comm, &myCommSize);
  if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)!=(*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)) {
    rc=MPI_Abort(comm, MPI_ERR_ARG);
  }
  else {
    if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(sendtype)==AMPI_ACTIVE)  rawSendBuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(sendbuf,&sendcnt);
    else rawSendBuf=sendbuf;
    if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)==AMPI_ACTIVE)  rawRecvBuf=(*ourADTOOL_AMPI_FPCollection.rawDataV_fp)(recvbuf, myCommSize, recvcnts, displs);
    else rawRecvBuf=recvbuf;
    rc=MPI_Allgatherv(rawSendBuf,
                      sendcnt,
                      sendtype,
                      rawRecvBuf,
                      recvcnts,
                      displs,
                      recvtype,
                      comm);
    if (rc==MPI_SUCCESS && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(recvtype)==AMPI_ACTIVE) {
      (*ourADTOOL_AMPI_FPCollection.writeDataV_fp)(recvbuf,recvcnts, displs);
      (*ourADTOOL_AMPI_FPCollection.pushGSVinfo_fp)(myCommSize,
						    recvbuf,
						    recvcnts,
						    displs,
						    recvtype,
						    sendbuf,
						    sendcnt,
						    sendtype,
						    0,
						    comm);
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_ALLGATHERV);
    }
  }
  return rc;
}

int BW_AMPI_Allgatherv(void *sendbuf,
                       int sendcnt,
                       MPI_Datatype sendtype,
                       void *recvbuf,
                       int *recvcnts,
                       int *displs,
                       MPI_Datatype recvtype,
                       MPI_Comm comm) {
  void *idx=NULL;
  int i;
  int rc=MPI_SUCCESS;
  int myRank, commSizeForRootOrNull, rTypeSize,rootPlaceholder;
  int *tRecvCnts=recvcnts, *tDispls=displs;
  char tRecvCntsFlag=0, tDisplsFlag=0;
  (*ourADTOOL_AMPI_FPCollection.popGScommSizeForRootOrNull_fp)(&commSizeForRootOrNull);
  if (tRecvCnts==NULL) {
    tRecvCnts=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
    tRecvCntsFlag=1;
  }
  if (tDispls==NULL) {
    tDispls=(int*)malloc(sizeof(int)*commSizeForRootOrNull);
    tDisplsFlag=1;
  }
  (*ourADTOOL_AMPI_FPCollection.popGSVinfo_fp)(commSizeForRootOrNull,
					       &recvbuf,
					       tRecvCnts,
					       tDispls,
					       &recvtype,
					       &sendbuf,
					       &sendcnt,
					       &sendtype,
					       &rootPlaceholder,
					       &comm);
  MPI_Comm_rank(comm, &myRank);
  void *tempBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(tRecvCnts[myRank],sendtype,comm) ;
  /**
   * \todo shortcut taken below by assuming AMPI_ADOUBLE is equivalent to MPI_DOUBLE, need type map.
   */
  rc=MPI_Reduce_scatter(recvbuf,
                        tempBuf,
                        tRecvCnts,
                        MPI_DOUBLE, /* <<< here is the offending bit */
                        MPI_SUM,
                        comm);
  (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(sendcnt,
                               sendtype,
                               comm,
                               sendbuf,
                               sendbuf,
                               sendbuf,
                               tempBuf,
                               idx);
  MPI_Type_size(recvtype,&rTypeSize);
  for (i=0;i<commSizeForRootOrNull;++i) {
    void* buf=recvbuf+(rTypeSize*tDispls[i]); /* <----------  very iffy! */
    (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(tRecvCnts[i],recvtype,comm,
                               buf , buf, buf);
  }
  (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(tempBuf);
  if (tRecvCntsFlag) free((void*)(tRecvCnts));
  if (tDisplsFlag) free((void*)(tDispls));
  return rc;
}

int FW_AMPI_Bcast (void* buf,
                   int count,
                   MPI_Datatype datatype,
                   int root,
                   MPI_Comm comm) {
  int rc;
  double* mappedbuf=NULL;
  int dt_idx = derivedTypeIdx(datatype);
  int is_derived = isDerivedType(dt_idx);
  if((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
    mappedbuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(buf,&count);
  }
  else if(is_derived) {
    mappedbuf=(*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,datatype,comm);
    (*ourADTOOL_AMPI_FPCollection.packDType_fp)(buf,mappedbuf,count,dt_idx);
  }
  else {
    mappedbuf=buf;
  }
  rc=MPI_Bcast(mappedbuf,
               count,
               (*ourADTOOL_AMPI_FPCollection.FW_rawType_fp)(datatype),
               root,
               comm);
  if (rc==MPI_SUCCESS && ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE || is_derived )) {
    if (is_derived) {
      (*ourADTOOL_AMPI_FPCollection.unpackDType_fp)(mappedbuf,buf,count,dt_idx);
      (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(mappedbuf);
    }
    (*ourADTOOL_AMPI_FPCollection.pushBcastInfo_fp)(buf,
						    count,
						    datatype,
						    root,
						    comm);
    (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_BCAST);
  }
  return rc;
}

int BW_AMPI_Bcast (void* buf,
                   int count,
                   MPI_Datatype datatype,
                   int root,
                   MPI_Comm comm) {
  int rc,rank;
  void *idx=NULL;
  (*ourADTOOL_AMPI_FPCollection.popBcastInfo_fp)(&buf,
						 &count,
						 &datatype,
						 &root,
						 &comm,
						 &idx);
  MPI_Comm_rank(comm,&rank);
  MPI_Datatype mappedtype = (*ourADTOOL_AMPI_FPCollection.BW_rawType_fp)(datatype);
  (*ourADTOOL_AMPI_FPCollection.getAdjointCount_fp)(&count,datatype);
  void *tempBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,datatype,comm);
  rc=MPI_Reduce(buf,
                tempBuf,
                count,
                mappedtype,
                MPI_SUM,
                root,
                comm);
  (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(count, mappedtype, comm,
                             buf, buf, buf);
  if (rank==root) {
    (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(count, mappedtype, comm,
                                 buf, buf, buf, tempBuf, idx);
  }
  (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(tempBuf);
  return rc;
}

int FW_AMPI_Reduce (void* sbuf,
		    void* rbuf,
		    int count,
		    MPI_Datatype datatype,
		    MPI_Op op,
		    int root,
		    MPI_Comm comm) {
  int rc,rank;
  MPI_Comm_rank(comm,&rank);
  int uop_idx = userDefinedOpIdx(op);
  if (isUserDefinedOp(uop_idx)) {
    int comm_size, is_commutative;
    int mask, relrank, source, lroot;
    int dt_idx = derivedTypeIdx(datatype);
    MPI_Status status;
    MPI_Aint lb;
    void *tmp_buf;
    userDefinedOpData* uopd = getUOpData();
    MPI_User_function* uop = uopd->functions[uop_idx];
    if (count == 0) return MPI_SUCCESS;
    MPI_Comm_size(comm,&comm_size);
    if (isDerivedType(dt_idx)) lb = getDTypeData()->lbs[dt_idx];
    else lb = 0;
    is_commutative = uopd->commutes[uop_idx];
    tmp_buf = (*ourADTOOL_AMPI_FPCollection.allocateTempActiveBuf_fp)(count,datatype,comm);
    tmp_buf = (void*)((char*)tmp_buf - lb);
    if (rank != root) {
      rbuf = (*ourADTOOL_AMPI_FPCollection.allocateTempActiveBuf_fp)(count,datatype,comm);
      rbuf = (void*)((char*)rbuf - lb);
    }
    if ((rank != root) || (sbuf != MPI_IN_PLACE)) {
      (*ourADTOOL_AMPI_FPCollection.copyActiveBuf_fp)(sbuf, rbuf, count, datatype, comm);
    }
    mask = 0x1;
    if (is_commutative)
      lroot = root;
    else
      lroot = 0;
    relrank = (rank - lroot + comm_size) % comm_size;
    while (mask < comm_size) {
      if ((mask & relrank) == 0) {
	source = (relrank | mask);
	if (source < comm_size) {
	  
	  source = (source + lroot) % comm_size;
	  rc = FW_AMPI_Recv(tmp_buf, count, datatype, source,
			 11, AMPI_SEND, comm, &status);
	  assert(rc==MPI_SUCCESS);
	  if (is_commutative) {
	    (*uop)(tmp_buf, rbuf, &count, &datatype);
	  }
	  else {
	    (*uop)(rbuf, tmp_buf, &count, &datatype);
	    (*ourADTOOL_AMPI_FPCollection.copyActiveBuf_fp)(sbuf, rbuf, count, datatype, comm);
	  }
	}
      }
      else {
	source = ((relrank & (~mask)) + lroot) % comm_size;
	rc = FW_AMPI_Send(rbuf, count, datatype, source,
			  11, AMPI_RECV, comm);
	assert(rc==MPI_SUCCESS);
	break;
      }
      mask<<=1;
    }
    if (!is_commutative && (root != 0)) {
      if (rank == 0) rc = FW_AMPI_Send(rbuf, count, datatype, root,
				    11, AMPI_RECV, comm);
      else if (rank==root) rc = FW_AMPI_Recv(rbuf, count, datatype, 0,
					  11, AMPI_SEND, comm, &status);
      assert(rc==MPI_SUCCESS);
    }
    (*ourADTOOL_AMPI_FPCollection.releaseTempActiveBuf_fp)(tmp_buf,count,datatype);
    if (rank != root) {
      (*ourADTOOL_AMPI_FPCollection.releaseTempActiveBuf_fp)(rbuf,count,datatype);
      }
    return 0;
  }
  else {
    double* mappedsbuf=NULL;
    double* mappedrbuf=NULL;
    if((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
      mappedsbuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(sbuf,&count);
      mappedrbuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(rbuf,&count);
    }
    else {
      mappedsbuf=sbuf;
      mappedrbuf=rbuf;
    }
    rc=MPI_Reduce(mappedsbuf,
		  mappedrbuf,
		  count,
		  (*ourADTOOL_AMPI_FPCollection.FW_rawType_fp)(datatype),
		  op,
		  root,
		  comm);
    if (rc==MPI_SUCCESS && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
      (*ourADTOOL_AMPI_FPCollection.pushReduceInfo_fp)(sbuf,
						       rbuf,
						       rbuf,
						       rank==root, /* also push contents of rbuf for root */
						       count,
						       datatype,
						       op,
						       root,
						       comm);
      (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_REDUCE);
    }
    return rc;
  }
}

int BW_AMPI_Reduce (void* sbuf,
		    void* rbuf,
		    int count,
		    MPI_Datatype datatype,
		    MPI_Op op,
		    int root,
		    MPI_Comm comm) {
  int rc,rank;
  void *idx=NULL;
  (*ourADTOOL_AMPI_FPCollection.popReduceCountAndType_fp)(&count,&datatype);
  MPI_Datatype mappedtype = (*ourADTOOL_AMPI_FPCollection.BW_rawType_fp)(datatype);
  (*ourADTOOL_AMPI_FPCollection.getAdjointCount_fp)(&count,datatype);
  void *prevValBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,datatype,comm);
  void *reduceResultBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,datatype,comm);
  (*ourADTOOL_AMPI_FPCollection.popReduceInfo_fp)(&sbuf,
						  &rbuf,
						  &prevValBuf,
						  &reduceResultBuf,
						  &count,
						  &op,
						  &root,
						  &comm,
						  &idx);
  MPI_Comm_rank(comm,&rank);
  rc=MPI_Bcast(reduceResultBuf,
	       count,
	       mappedtype,
	       root,
	       comm);
  if (rc!=MPI_SUCCESS) MPI_Abort(comm, MPI_ERR_ARG);
  void *tempBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,mappedtype,comm);
  if (rank==root) {
    (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(count, mappedtype, comm,
			       tempBuf, tempBuf, tempBuf);
    (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(count, mappedtype, comm,
				 tempBuf, tempBuf, tempBuf, rbuf, idx);
    (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(count, mappedtype, comm,
			       rbuf, rbuf, rbuf);
  }
  rc=MPI_Bcast(tempBuf,
	       count,
	       mappedtype,
	       root,
	       comm);
  if (op==MPI_PROD) {
    (*ourADTOOL_AMPI_FPCollection.adjointMultiply_fp)(count, mappedtype, comm,
				tempBuf, tempBuf, tempBuf, reduceResultBuf, idx);
    (*ourADTOOL_AMPI_FPCollection.adjointDivide_fp)(count, mappedtype, comm,
			      tempBuf, tempBuf, tempBuf, prevValBuf, idx);
  }
  else if (op==MPI_MAX || op==MPI_MIN) {
    void *equalsResultBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,mappedtype,comm);
    (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(count, mappedtype, comm,
			       equalsResultBuf, equalsResultBuf, equalsResultBuf);
    (*ourADTOOL_AMPI_FPCollection.adjointEquals_fp)(count, mappedtype, comm,
			      equalsResultBuf, equalsResultBuf, equalsResultBuf, prevValBuf, reduceResultBuf, idx);
    void *contributionTotalsBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,mappedtype,comm);
    MPI_Allreduce(equalsResultBuf,
		  contributionTotalsBuf,
		  count,
		  mappedtype,
		  MPI_SUM,
		  comm);
    (*ourADTOOL_AMPI_FPCollection.adjointMultiply_fp)(count, mappedtype, comm,
				tempBuf, tempBuf, tempBuf, equalsResultBuf, idx);
    (*ourADTOOL_AMPI_FPCollection.adjointDivide_fp)(count, mappedtype, comm,
			      tempBuf, tempBuf, tempBuf, contributionTotalsBuf, idx);
    (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(equalsResultBuf);
    (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(contributionTotalsBuf);
  }
  else {}
  (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(count, mappedtype, comm,
			       sbuf, sbuf, sbuf, tempBuf, idx);
  (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(tempBuf);
  (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(reduceResultBuf);
  (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(prevValBuf);
  return rc;
}

int FW_AMPI_Allreduce (void* sbuf,
                       void* rbuf,
                       int count,
                       MPI_Datatype datatype,
                       MPI_Op op,
                       MPI_Comm comm) {
  int rc,rank;
  MPI_Comm_rank(comm,&rank);
  double* mappedsbuf=NULL;
  double* mappedrbuf=NULL;
  if((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
    mappedsbuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(sbuf,&count);
    mappedrbuf=(*ourADTOOL_AMPI_FPCollection.rawData_fp)(rbuf,&count);
  }
  else {
    mappedsbuf=sbuf;
    mappedrbuf=rbuf;
  }
  rc=MPI_Allreduce(mappedsbuf,
                   mappedrbuf,
                   count,
                   (*ourADTOOL_AMPI_FPCollection.FW_rawType_fp)(datatype),
                   op,
                   comm);
  if (rc==MPI_SUCCESS && (*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(datatype)==AMPI_ACTIVE) {
    (*ourADTOOL_AMPI_FPCollection.pushReduceInfo_fp)(sbuf,
						     rbuf,
						     rbuf,
						     1,
						     count,
						     datatype,
						     op,
						     0,
						     comm);
    (*ourADTOOL_AMPI_FPCollection.push_CallCode_fp)(AMPI_ALLREDUCE);
  }
  return rc;
}

int BW_AMPI_Allreduce (void* sbuf,
                       void* rbuf,
                       int count,
                       MPI_Datatype datatype,
                       MPI_Op op,
                       MPI_Comm comm) {
  int rc=0,rank, rootPlaceHolder;
  void *idx=NULL;
  (*ourADTOOL_AMPI_FPCollection.popReduceCountAndType_fp)(&count,&datatype);
  MPI_Datatype mappedtype = (*ourADTOOL_AMPI_FPCollection.BW_rawType_fp)(datatype);
  (*ourADTOOL_AMPI_FPCollection.getAdjointCount_fp)(&count,datatype);
  void *prevValBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,mappedtype,comm);
  void *reduceResultBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,mappedtype,comm);
  (*ourADTOOL_AMPI_FPCollection.popReduceInfo_fp)(&sbuf,
						  &rbuf,
						  &prevValBuf,
						  &reduceResultBuf,
						  &count,
						  &op,
						  &rootPlaceHolder,
						  &comm,
						  &idx);
  MPI_Comm_rank(comm,&rank);
  void *tempBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,mappedtype,comm);
  MPI_Allreduce(rbuf,
                tempBuf,
                count,
                mappedtype,
                MPI_SUM,
                comm);
  if (op==MPI_SUM) {
     ; /* nothing extra to be done here */
  }
  else if (op==MPI_PROD) {
    (*ourADTOOL_AMPI_FPCollection.adjointMultiply_fp)(count, mappedtype, comm,
                                tempBuf, tempBuf, tempBuf, reduceResultBuf, idx);
    (*ourADTOOL_AMPI_FPCollection.adjointDivide_fp)(count, mappedtype, comm,
                              tempBuf, tempBuf, tempBuf, prevValBuf, idx);
  }
  else if (op==MPI_MAX || op==MPI_MIN) {
    void *equalsResultBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp)(count,mappedtype,comm);
    (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(count, mappedtype, comm,
                               equalsResultBuf, equalsResultBuf, equalsResultBuf);
    (*ourADTOOL_AMPI_FPCollection.adjointEquals_fp)(count, mappedtype, comm,
                              equalsResultBuf, equalsResultBuf, equalsResultBuf, prevValBuf, reduceResultBuf, idx);
    void *contributionTotalsBuf = (*ourADTOOL_AMPI_FPCollection.allocateTempBuf_fp) (count,mappedtype,comm);
    MPI_Allreduce(equalsResultBuf,
                  contributionTotalsBuf,
                  count,
                  mappedtype,
                  MPI_SUM,
                  comm);
    (*ourADTOOL_AMPI_FPCollection.adjointMultiply_fp)(count, mappedtype, comm,
                                tempBuf, tempBuf, tempBuf, equalsResultBuf, idx);
    (*ourADTOOL_AMPI_FPCollection.adjointDivide_fp)(count, mappedtype, comm,
                              tempBuf, tempBuf, tempBuf, contributionTotalsBuf, idx);
    (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(equalsResultBuf);
    (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(contributionTotalsBuf);
  }
  else {
    assert(0); /* unimplemented */
  }
  (*ourADTOOL_AMPI_FPCollection.adjointIncrement_fp)(count, mappedtype, comm,
                               sbuf, sbuf, sbuf, tempBuf, idx);
  (*ourADTOOL_AMPI_FPCollection.adjointNullify_fp)(count, mappedtype, comm,
                             rbuf, rbuf, rbuf);
  (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(tempBuf);
  (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(reduceResultBuf);
  (*ourADTOOL_AMPI_FPCollection.releaseAdjointTempBuf_fp)(prevValBuf);
  return rc;
}

derivedTypeData* getDTypeData() {
  static derivedTypeData* dat = NULL;
  if (dat==NULL) {
    derivedTypeData* newdat = (derivedTypeData*)malloc(sizeof(derivedTypeData));
    newdat->size = 0;
    newdat->preAlloc = 0;
    newdat->num_actives = NULL;
    newdat->first_active_blocks = NULL;
    newdat->last_active_blocks = NULL;
    newdat->last_active_block_lengths = NULL;
    newdat->derived_types = NULL;
    newdat->counts = NULL;
    newdat->arrays_of_blocklengths = NULL;
    newdat->arrays_of_displacements = NULL;
    newdat->arrays_of_types = NULL;
    newdat->lbs = NULL;
    newdat->extents = NULL;
    newdat->packed_types = NULL;
    newdat->arrays_of_p_blocklengths = NULL;
    newdat->arrays_of_p_displacements = NULL;
    newdat->arrays_of_p_types = NULL;
    newdat->p_extents = NULL;
    dat = newdat;
  }
  return dat;
}

void addDTypeData(derivedTypeData* dat,
		  int count,
		  int array_of_blocklengths[],
		  MPI_Aint array_of_displacements[],
		  MPI_Datatype array_of_types[],
		  MPI_Aint lb,
		  MPI_Aint extent,
		  int array_of_p_blocklengths[],
		  MPI_Aint array_of_p_displacements[],
		  MPI_Datatype array_of_p_types[],
		  MPI_Aint p_extent,
		  MPI_Datatype* newtype,
		  MPI_Datatype* packed_type) {
  assert(dat);
  int i, dt_idx;
  int num_actives=0, fst_ablk_set=0;
  MPI_Aint fst_active_blk=0, lst_active_blk=0, lst_active_blk_len=0;
  for (i=0;i<count;i++) {
    if ((*ourADTOOL_AMPI_FPCollection.isActiveType_fp)(array_of_types[i])==AMPI_ACTIVE) {
      num_actives += array_of_blocklengths[i];
      if (!fst_ablk_set) {
	fst_active_blk = array_of_displacements[i];
	fst_ablk_set = 1;
      }
      lst_active_blk = array_of_displacements[i];
      lst_active_blk_len = array_of_blocklengths[i];
      continue;
    }
    dt_idx = derivedTypeIdx(array_of_types[i]);
    if (isDerivedType(dt_idx)) {
      num_actives += dat->num_actives[dt_idx]*array_of_blocklengths[i];
      if (!fst_ablk_set) {
	fst_active_blk = array_of_displacements[i] + dat->first_active_blocks[dt_idx];
	fst_ablk_set = 1;
      }
      lst_active_blk = array_of_displacements[i] + (array_of_blocklengths[i]-1)*dat->extents[dt_idx] + dat->last_active_blocks[dt_idx];
      lst_active_blk_len = dat->last_active_block_lengths[dt_idx];
    }
  }
  if (num_actives>0) return -1;
  if (dat->preAlloc == dat->size) {
    dat->preAlloc += 16;
    dat->num_actives = realloc(dat->num_actives, (dat->preAlloc)*sizeof(int));
    dat->first_active_blocks = realloc(dat->first_active_blocks, (dat->preAlloc)*sizeof(MPI_Aint));
    dat->last_active_blocks = realloc(dat->last_active_blocks, (dat->preAlloc)*sizeof(MPI_Aint));
    dat->last_active_block_lengths = realloc(dat->last_active_block_lengths, (dat->preAlloc)*sizeof(int));
    dat->derived_types = realloc(dat->derived_types,
				 (dat->preAlloc)*sizeof(MPI_Datatype));
    dat->counts = realloc(dat->counts, (dat->preAlloc)*sizeof(int));
    dat->arrays_of_blocklengths = realloc(dat->arrays_of_blocklengths,
					  (dat->preAlloc)*sizeof(int*));
    dat->arrays_of_displacements = realloc(dat->arrays_of_displacements,
					   (dat->preAlloc)*sizeof(MPI_Aint*));
    dat->arrays_of_types = realloc(dat->arrays_of_types,
				   (dat->preAlloc)*sizeof(MPI_Datatype*));
    dat->lbs = realloc(dat->lbs, (dat->preAlloc)*sizeof(MPI_Aint));
    dat->extents = realloc(dat->extents, (dat->preAlloc)*sizeof(MPI_Aint));
    dat->packed_types = realloc(dat->packed_types,
				(dat->preAlloc)*sizeof(MPI_Datatype));
    dat->arrays_of_p_blocklengths = realloc(dat->arrays_of_p_blocklengths,
					    (dat->preAlloc)*sizeof(int*));
    dat->arrays_of_p_displacements = realloc(dat->arrays_of_p_displacements,
					     (dat->preAlloc)*sizeof(MPI_Aint*));
    dat->arrays_of_p_types = realloc(dat->arrays_of_p_types,
				     (dat->preAlloc)*sizeof(MPI_Datatype*));
    dat->p_extents = realloc(dat->p_extents, (dat->preAlloc)*sizeof(MPI_Aint));
  }
  dat->num_actives[dat->size] = num_actives;
  dat->first_active_blocks[dat->size] = fst_active_blk;
  dat->last_active_blocks[dat->size] = lst_active_blk;
  dat->last_active_block_lengths[dat->size] = lst_active_blk_len;
  dat->derived_types[dat->size] = *newtype;
  dat->counts[dat->size] = count;
  dat->arrays_of_blocklengths[dat->size] = malloc(count*sizeof(int));
  memcpy(dat->arrays_of_blocklengths[dat->size], array_of_blocklengths, count*sizeof(int));
  dat->arrays_of_displacements[dat->size] = malloc(count*sizeof(MPI_Aint));
  memcpy(dat->arrays_of_displacements[dat->size], array_of_displacements, count*sizeof(MPI_Aint));
  dat->arrays_of_types[dat->size] = malloc(count*sizeof(MPI_Datatype));
  memcpy(dat->arrays_of_types[dat->size], array_of_types, count*sizeof(MPI_Datatype));
  dat->lbs[dat->size] = lb;
  dat->extents[dat->size] = extent;
  dat->packed_types[dat->size] = *packed_type;
  dat->arrays_of_p_blocklengths[dat->size] = malloc(count*sizeof(int));
  memcpy(dat->arrays_of_p_blocklengths[dat->size], array_of_p_blocklengths, count*sizeof(int));
  dat->arrays_of_p_displacements[dat->size] = malloc(count*sizeof(MPI_Aint));
  memcpy(dat->arrays_of_p_displacements[dat->size], array_of_p_displacements, count*sizeof(MPI_Aint));
  dat->arrays_of_p_types[dat->size] = malloc(count*sizeof(MPI_Datatype));
  memcpy(dat->arrays_of_p_types[dat->size], array_of_p_types, count*sizeof(MPI_Datatype));
  dat->p_extents[dat->size] = p_extent;
  dat->size += 1;
}

int derivedTypeIdx(MPI_Datatype datatype) {
  int i;
  derivedTypeData* dtdata = getDTypeData();
  for (i=0;i<dtdata->size;i++) {
    if (dtdata->derived_types[i]==datatype) return i;
  }
  return -1;
}

int isDerivedType(int dt_idx) { return dt_idx!=-1; }

userDefinedOpData* getUOpData() {
  static userDefinedOpData* dat = NULL;
  if (dat==NULL) {
    userDefinedOpData* newdat = (userDefinedOpData*)malloc(sizeof(userDefinedOpData));
    newdat->size = 0;
    newdat->preAlloc = 0;
    newdat->ops = NULL;
    newdat->functions = NULL;
    newdat->commutes = NULL;
    dat = newdat;
  }
  return dat;
}

void addUOpData(userDefinedOpData* dat,
		MPI_Op* op,
		MPI_User_function* function,
		int commute) {
  assert(dat);
  if (dat->preAlloc == dat->size) {
    dat->preAlloc += 16;
    dat->ops = realloc(dat->ops,(dat->preAlloc)*sizeof(MPI_Op));
    dat->functions = realloc(dat->functions,(dat->preAlloc)*sizeof(MPI_User_function*));
    dat->commutes = realloc(dat->commutes,(dat->preAlloc)*sizeof(int));
  }
  dat->ops[dat->size] = *op;
  dat->functions[dat->size] = function;
  dat->commutes[dat->size] = commute;
  dat->size += 1;
}

int userDefinedOpIdx(MPI_Op op) {
  int i;
  userDefinedOpData* uopdata = getUOpData();
  for (i=0;i<uopdata->size;i++) {
    if (uopdata->ops[i]==op) return i;
  }
  return -1;
}

int isUserDefinedOp(int uop_idx) {
  return uop_idx!=-1;
}
