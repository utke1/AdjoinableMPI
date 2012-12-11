#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include "ampi/adTool/support.h"

void ADTOOL_AMPI_pushSRinfo(void* buf, 
			    int count,
			    MPI_Datatype datatype, 
			    int src, 
			    int tag,
			    enum AMPI_PairedWith_E pairedWith,
			    MPI_Comm comm) { 
}

void ADTOOL_AMPI_popSRinfo(void** buf, 
			   int* count,
			   MPI_Datatype* datatype, 
			   int* src, 
			   int* tag,
			   enum AMPI_PairedWith_E* pairedWith,
			   MPI_Comm* comm) { 
}

void ADTOOL_AMPI_push_CallCode(enum AMPI_PairedWith_E thisCall) { 
}

void ADTOOL_AMPI_pop_CallCode(enum AMPI_PairedWith_E *thisCall) { 
}

void ADTOOL_AMPI_push_AMPI_Request(struct AMPI_Request_S  *ampiRequest) { 
}

void ADTOOL_AMPI_pop_AMPI_Request(struct AMPI_Request_S  *ampiRequest) { 
}

void ADTOOL_AMPI_mapBufForAdjoint(struct AMPI_Request_S  *ampiRequest,
				  void* buf) { 
}

void ADTOOL_AMPI_setBufForAdjoint(struct AMPI_Request_S  *ampiRequest,
				  void* buf) { 
  /* do nothing */
}

void ADTOOL_AMPI_getAdjointCount(int *count,
				 MPI_Datatype datatype) { 
}

void ADTOOL_AMPI_setAdjointCount(struct AMPI_Request_S  *ampiRequest) { 
  /* for now we keep the count as is but for example in vector mode one would have to multiply by vector length */
}

void ADTOOL_AMPI_setAdjointCountAndTempBuf(struct AMPI_Request_S *ampiRequest) { 
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

void ADTOOL_AMPI_releaseAdjointTempBuf(struct AMPI_Request_S *ampiRequest) { 
}

void ADTOOL_AMPI_adjointIncrement(int adjointCount, void* target, void *source) { 
}

void ADTOOL_AMPI_adjointNullify(int adjointCount, void* buf) { 
}

