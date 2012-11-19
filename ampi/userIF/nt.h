#ifndef _AMPI_NT_H_
#define _AMPI_NT_H_

/**
 * \file 
 * NT= "no tansformation" or "no trace" prototypes for wrapper routines that could have an adjoint action but in particular contexts do not because they are called outside of the transformed or traced section of the source code;  the signatures are identical to their MPI originals and they pass the parameters through to the MPI routines
 */ 

#include "mpi.h"

/**
 * this wrapper variant has no adjoint transformation / trace functionality; to be used outside of the transformed/traced code section
 */
int AMPI_Init_NT(int* argc, 
		 char*** argv);

/**
 * this wrapper variant has no adjoint transformation / trace functionality; to be used outside of the transformed/traced code section
 */
int AMPI_Finalize_NT(void);

/**
 * this wrapper variant has no adjoint transformation / trace functionality; to be used outside of the transformed/traced code section
 */
int AMPI_Buffer_attach_NT(void *buffer, 
			  int size); 

/**
 * this wrapper variant has no adjoint transformation / trace functionality; to be used outside of the transformed/traced code section
 */
int AMPI_Buffer_detach_NT(void *buffer, 
			  int *size);

#endif
