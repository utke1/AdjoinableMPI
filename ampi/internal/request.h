#ifndef _AMPI_REQUEST_H_
#define _AMPI_REQUEST_H_

/**
 * \file 
 * the request in the AMPI context need to be used to track extra information;
 * the extra information cannot be exposed in Fortran77 but there can be a 
 * Fortran90 equivalent and it can be exposed in C to allow source transformation 
 * tools to use TBR analysis on the tracked information.
 */ 

#include <mpi.h>

/**
 * does the request originate with a  send or a receive 
 */
enum AMPI_Request_origin { 
  AMPI_SEND_ORIGIN,
  AMPI_RECV_ORIGIN
};

/**
 * MPI_Request augmented with extra information 
 */ 
struct AMPI_Request {
  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  dst or src  parameter 
   */
  int endPoint;
  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  tag parameter 
   */
  int tag;
  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  buf  parameter 
   */
  void *buf;
  
  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  count  parameter 
   */
  int count;
  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  datatype  parameter 
   */
  MPI_Datatype datatype;
  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv  comm  parameter 
   */
  MPI_Comm comm;
  /**
   * the "plain" request returned by MPI_Isend or MPI_Irecv resp. 
   */  
  MPI_Request plainRequest;
  /**
   * \ref AMPI_Isend / \ref AMPI_Irecv sets this
   */ 
  enum AMPI_Request_origin origin;
};

#endif
