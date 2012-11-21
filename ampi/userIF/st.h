#ifndef _AMPI_ST_H_
#define _AMPI_ST_H_

/**
 * \file 
 * ST = "source tansformation" specific versions of routines that exist because generic memory mapping is not yet implemented in any source transformation tool   
 */ 

#include "ampi/userIF/request.h"


/**
 * variant of \ref AMPI_Wait with an extra parameter 
 * \param request follows the semantics of  <tt>MPI_Wait</tt>
 * \param buf is the buffer that was passed to the corresponding \ref AMPI_Isend or \ref AMPI_Irecv call; 
 * see also \ref nonblocking for a discussion of the necessity of the buf parameter.
 * \param status follows the semantics of  <tt>MPI_Wait</tt>
 */
int AMPI_Wait_ST(AMPI_Request *request, 
		 void *buf,
		 MPI_Status *status);

#endif
