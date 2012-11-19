#ifndef _AMPI_ADTOOL_SUPPORT_H_
#define _AMPI_ADTOOL_SUPPORT_H_

#include "ampi/userIF/request.h"

/**
 * \file 
 * methods that an AD tool needs to implement in order to use the implementation in Common
 */ 

/**
 * the implementation of pushing the required elements of an \ref  AMPI_Request_S
 * to the AD-tool-userIF stack
 */
void ADTOOL_AMPI_push_AMPI_Request(struct AMPI_Request_S  *ampiRequest);

/**
 * the implementation of popping the required elements of an \ref  AMPI_Request_S
 * from the AD-tool-userIF stack
 */
void ADTOOL_AMPI_pop_AMPI_Request(struct AMPI_Request_S  *ampiRequest);

/**
 * an operator overloading tool should not do anything in the implementation but see \ref ADTOOL_AMPI_setBufForAdjoint;
 * an operator overloading tool maps a given buf to the respective adjoint address space (e.g. the locint of Adol-C)
 * and set it on the request; 
 * \param buf is forward sweep buffer (for source transformation tools)
 * \param ampiRequest is the request to be used during the adjoint communication
 */
void ADTOOL_AMPI_mapBufForAdjoint(struct AMPI_Request_S  *ampiRequest,
				  void* buf);

/**
 * an operator overloading tool should not do anything in the implementation but see \ref ADTOOL_AMPI_mapBufForAdjoint;
 * a source transformation tool would receive the adjoint buffer as an argument
 * and set it on the request; 
 * \param buf is the adjoint buffer (for source transformation tools)
 * \param ampiRequest is the request to be used during the adjoint communication
 */
void ADTOOL_AMPI_setBufForAdjoint(struct AMPI_Request_S  *ampiRequest,
				  void* buf);

/**  
 * \param ampiRequest in this instance this method resets the <tt>adjointCount</tt> member to represent the buffer in terms of 
 * elements of the original MPI data type; this is of particular interest for vector mode; 
 * \todo needs to be expanded for the case where the adjoint data is not represented by the same MPI data type as the 
 * original program data
 */
void ADTOOL_AMPI_setAdjointCount(struct AMPI_Request_S  *ampiRequest);

/**  
 * calls \ref ADTOOL_AMPI_setAdjointCount and sets up a temporary buffer into which the adjoint data is received, see e.g. \ref BW_AMPI_Wait
 * \param ampiRequest is the request instance that is modified
 */
void ADTOOL_AMPI_setAdjoinCountAndTempBuf(struct AMPI_Request_S *ampiRequest);

#endif

