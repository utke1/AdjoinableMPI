#include <stdlib.h>
#include <assert.h>
#include "ampi/adTool/support.h"

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
  ampiRequest->adjointTempBuf=(void*)malloc(ampiRequest->adjointCount*s);
  assert(ampiRequest->adjointTempBuf);
}

