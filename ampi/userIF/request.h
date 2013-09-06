#ifndef _AMPI_REQUEST_H_
#define _AMPI_REQUEST_H_

/**
 * \file 
 * the request in the AMPI context need to be used to track extra information;
 * the extra information cannot be exposed in Fortran77 but there can be a 
 * Fortran90 equivalent and it can be exposed in C to allow source transformation 
 * tools to use TBR analysis on the tracked information.
 */ 

#include "ampi/userIF/libConfig.h"
#include "ampi/userIF/pairedWith.h"

/**
 * does the request originate with a  send or a receive 
 */
enum AMPI_Request_origin_E { 
  AMPI_SEND_ORIGIN,
  AMPI_RECV_ORIGIN
};

#ifdef AMPI_FORTRANCOMPATIBLE
typedef int AMPI_Request_origin;
#else 
typedef enum AMPI_Request_origin_E AMPI_Request_origin;
#endif 

/**
 * MPI_Request augmented with extra information 
 */ 
struct AMPI_Request_S {

  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  buf  parameter 
   */
  void *buf;

  /**
   * The corresponding adjoint buffer
   */
  void *adjointBuf ;

  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  count  parameter 
   */
  int count;

  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  datatype  parameter 
   */
  MPI_Datatype datatype;

  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  dst or src  parameter 
   */
  int endPoint;

  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  tag parameter 
   */
  int tag;

  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv sets this
   */ 
  enum AMPI_PairedWith_E pairedWith;

  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  comm  parameter 
   */
  MPI_Comm comm;

  /**
   * temporary adjoint buffer; not to be traced
   */
  void *adjointTempBuf;
  
  /**
   * the count of the adjoint buffer size in terms of the original data type; not to be traced;
   */
  int adjointCount;
  
  /**
   * the "plain" request returned by MPI_Isend or MPI_Irecv resp; not to be traced
   */  
  MPI_Request plainRequest;

  /**
   * the "plain" request returned by MPI_Isend or MPI_Irecv resp; 
   */  
  MPI_Request tracedRequest;

  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv sets this
   */ 
  enum AMPI_Request_origin_E origin;

  void *idx;

};

#ifdef AMPI_FORTRANCOMPATIBLE
typedef MPI_Request AMPI_Request;
#else 
typedef struct AMPI_Request_S AMPI_Request;
#endif 

#endif
