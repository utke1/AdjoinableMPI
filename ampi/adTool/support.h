#ifndef _AMPI_ADTOOL_SUPPORT_H_
#define _AMPI_ADTOOL_SUPPORT_H_

#include <mpi.h>
#if defined(__cplusplus)
extern "C" {
#endif

#include "ampi/userIF/request.h"

/**
 * \file 
 * \brief methods that an AD tool needs to implement in order to use the implementation in Common
 */ 

/**
 * the implementation of pushing the required elements for send/recv
 * to the AD-tool-internal stack;
 * the operator overloading implementation maps <tt>buf</tt> to the adjoint address space
 * the source transformation implementation ignores <tt>buf</tt> 
 */
void ADTOOL_AMPI_pushSRinfo(void* buf, 
			    int count,
			    MPI_Datatype datatype, 
			    int src, 
			    int tag,
			    enum AMPI_PairedWith_E pairedWith,
			    MPI_Comm comm);

/**
 * the implementation of popping the required elements for send/recv
 * from the AD-tool-internal stack;
 * the source transformation implementation will ignore <tt>buf</tt>
 */
void ADTOOL_AMPI_popSRinfo(void** buf, 
			   int* count,
			   MPI_Datatype* datatype, 
			   int* src, 
			   int* tag,
			   enum AMPI_PairedWith_E* pairedWith,
			   MPI_Comm* comm);

/**
 * the implementation of pushing an operation code to the 
 * to the AD-tool-internal stack for an operator overloading tool;
 * the source transformation implementation will leave this empty;
 * this method is called in the respective <tt>FW_</tt>  variant 
 * implemented in <tt>Common</tt>
 */
void ADTOOL_AMPI_push_CallCode(enum AMPI_PairedWith_E thisCall);

/**
 * the implementation of pushing an operation code to the 
 * to the AD-tool-internal stack for an operator overloading tool;
 * the source transformation implementation will leave this empty;
 * the operator overloading tool needs to pop the code from its operation 
 * stack first and then call (with dummy parameters) the respect <tt>BW_</tt>
 * variant of the operatiorn represented by <tt>thisCall</tt> 
 */
void ADTOOL_AMPI_pop_CallCode(enum AMPI_PairedWith_E *thisCall);

/**
 * the implementation of pushing the required elements of an \ref  AMPI_Request_S
 * to the AD-tool-internal stack
 */
void ADTOOL_AMPI_push_AMPI_Request(struct AMPI_Request_S  *ampiRequest);

/**
 * the implementation of popping the required elements of an \ref  AMPI_Request_S
 * from the AD-tool-internal stack
 */
void ADTOOL_AMPI_pop_AMPI_Request(struct AMPI_Request_S  *ampiRequest);
  
/**
 * map active data to raw data; this is to be implemented for the forward 
 * execution by tools using association-by-address; 
 * for tools using association-by-name the same address should be returned;   
 */
void * ADTOOL_AMPI_rawData(void* activeData);

/**
 * set it on the request; 
 * \param buf is forward sweep buffer (for source transformation tools)
 * \param ampiRequest is the request to be pushed and popped for the adjoint communication
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
 * this method resets \param count member to represent the buffer in terms of 
 * elements of the original MPI data type; this is of particular interest for vector mode; 
 * \todo needs to be expanded for the case where the adjoint data is not represented by the same MPI data type as the 
 * original program data
 */
void ADTOOL_AMPI_getAdjointCount(int *count,
				 MPI_Datatype datatype);

/**  
 * \param ampiRequest in this instance this method resets the <tt>adjointCount</tt> member; 
 */
void ADTOOL_AMPI_setAdjointCount(struct AMPI_Request_S  *ampiRequest);

/**  
 * calls \ref ADTOOL_AMPI_setAdjointCount and sets up a temporary buffer into which the adjoint data is received, see e.g. \ref BW_AMPI_Wait
 * \param ampiRequest is the request instance that is modified
 */
void ADTOOL_AMPI_setAdjointCountAndTempBuf(struct AMPI_Request_S *ampiRequest);

/**  
 * releases the temporary buffer (allocated by \ref ADTOOL_AMPI_setAdjoinCountAndTempBuf)  into which the adjoint data was received 
 */
void ADTOOL_AMPI_releaseAdjointTempBuf(struct AMPI_Request_S *ampiRequest);

/** 
 * adjoint increment 
 * \param count number of elements
 * \param target values to be incremented
 * \param source increment values
 */
void ADTOOL_AMPI_adjointIncrement(int adjointCount, void* target, void *source);

/** 
 * adjoint nullify the values in buf
 */ 
void ADTOOL_AMPI_adjointNullify(int adjointCount, void* buf);

#if defined(__cplusplus)
}
#endif

#endif

