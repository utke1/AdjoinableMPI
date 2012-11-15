#include "ampi/internal/st.h"

int AMPI_Wait_ST(struct AMPI_Request *request,
		 void*  buf,
		 MPI_Status *status) { 
  return MPI_Wait(&(request->plainRequest),
		  status);
}
