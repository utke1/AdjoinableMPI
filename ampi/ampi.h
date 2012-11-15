#ifndef _AMPI_AMPI_H_
#define _AMPI_AMPI_H_

/** \mainpage 
 *
 * \section Introduction
 * 
 * The Adjoinable MPI (AMPI) library provides a modified set if MPI subroutines 
 * that are constructed such that and adjoint in the context of algorithmic 
 * differentiation (AD) can be computed. 
 * There are principal recipes for the construction of the adjoint of 
 * a given communication, see \cite AMPIpaper . 
 * The practical implementation of these recipes, however, faces the following 
 * challenges.
 *  - the target language may prevent some implementation options
 *   - exposing an MPI_Request augmented with extra information as a structured type (not supported by Fortran 77)
 *   - passing an array of buffers (of different length) as a formal argument (not supported in Fortran)
 *  - the AD tool implementation could be based on 
 *   - operator overloading 
 *   - source transformation
 *    - association by address
 *    - association by name
 * 
 * The above choices imply certain consequences on the complexity for implementing  
 * the adjoint action and this would imply differences in the ANPI design while 
 * on the other hand there is a clear advantage to present a single, AD tool implementation independent
 * variant of the AMPI library to permit switching AD tools and a common understanding of the 
 * adjoining of MPI calls. 
 * Some overarching design decisions are discussed in \ref design. Concerns applicable to specific 
 * routines are covered in their respective sections.
 * We assume the reader is familiar with MPI and AD. 
 *
 * \section sources Getting the source files
 * 
 * In the following we assume the sources are installed in a directory 
 * \code
 * AdjoinableMPI
 * \endcode
 * 
 * \section configure Configure,  Build, and Install 
 * 
 * Configuration, build, and install follows the typical GNU autotools chain. 
 * Assuming the installation is done into a directory called
 * \code 
 * /opt/AMPI_inst
 * \endcode 
 * then the user should add that directory to the preprocessor include path as
 * \code
 * -I/opt/AMPI_inst/include
 * \endcode 
 * and include the top level header file 
 * as  
 * \code{.cpp}
 * #include "ampi/ampi.h"
 * \endcode
 * There is a simple pass-through-to-MPI library build without any AD tool involvement which, following the example above, weould be linked as 
 * \code
 * -L /opt/AMPI_inst/lib -lampiPlainC
 * \endcode
 * 
 * \section design Libary Design
 * In this section we discuss the directory structure of the implementation, 
 * the distinction between different subroutine variants in the context of 
 * the source code to be adjoined, and general assumptions on the communication patterns. 
 * 
 * \subsection dirStruct Directory and File Structure
 * All locations discussed below are relative to the top level source directory. 
 * The top level header file to be included in place of the usual  "mpi.h" is located in  
 * ampi/ampi.h
 *
 * It references the header files in <tt>ampi/internal</tt> which are organized to contain
 *  - unmodified pass through to MPI in <tt>ampi/internal/passThrough.h</tt> which exists to give the extent of the original MPI we cover  
 *  - variants of routines that in principle need adjoint logic but happen to be called outside of the code section that is adjoined and therefore 
 *    are not transformed / not traced (NT) in  <tt>ampi/internal/nt.h</tt>
 *  - routines that are modified from the original MPI counterparts because their behavior in the reverse sweep differs from their behavior in the 
 *    forward sweep and they also may have a modified signatyre; in <tt>ampi/internal/modified.h</tt>
 *
 * Additional header files contain enumerations used as arguments to AMPI routines.
 * 
 * A library that simply passes through all AMPI calls to their MPI counterparts for a test compilation and execution without any involvement of 
 * and AD tool is implemented in the source files under <tt>plainC</tt>. 
 * 
 * \subsection adjoinableSection Subroutine variants relative to the adjoinable section
 * 
 * The typical assumption of a program to be differentiated is that there is some top level routine <tt>head</tt> which does the numerical computation 
 * and communication which is called from some main <tt>driver</tt> routine. The <tt>driver</tt> routine would have to be manually adjusted to initiate 
 * the derivative computation, retrieve, and use the derviative values. 
 * Therefore only <tt>head</tt> and everything it references would be <em>adjointed</em> while <tt>driver</tt> would not. Typically, the <tt>driver</tt>
 * routine also includes the basic setup and teardown with MPI_Init and MPI_Finalize and consequently these calls (for consistency) should be replaced 
 * with their AMPI "no trace/transformation"  (NT) counterparts \ref AMPI_Init_NT and \ref AMPI_Finalize_NT. 
 * The same approach should be taken for all resource allocations/deallocations (e.g. \ref AMPI_Buffer_attach_NT and \ref AMPI_Buffer_detach_NT) 
 * that can exist in the scope enclosing the adjointed section alleviating 
 + The need for the AD tool implementation to tackle them. 
 * For cases where these routines have to be called within the adjointed code section the variants without the <tt>_NT</tt> suffix will ensure the
 * correct adjoint behavior.
 * 
 * \subsection commPat General Assumptions and Notions on Communication Patterns
 * 
 * Following the explanations in \cite AMPIpaper it is clear that context information about the 
 * communication pattern, that is the pairing of MPI calls, is needed to achieve 
 * -# correct adjoints, i.e. correct send and receive end points and deadlock free
 * -# if possible retain the efficiency advantages present in the original MPI communication for the adjoint.
 *
 * The need to convey the pairing imposes restrictions because in a given code the pairing may not be static.
 * For a example a given <tt>MPI_Recv</tt> may be paired with 
 * \code{.cpp}
 * if (doBufferedSends)  
 *   MPI_Bsend(...); 
 * else  
 *   MPI_Ssend(...);
 * \endcode 
 *
 * but the AD tool has to decide on the send mode once the reverse sweep needs to adjoin the orginal <tt>MPI_Recv</tt>.  
 * Tracing such information in a global data structure is not scalable and piggybacking the send type onto the message 
 * so it can be traced on the receiving side is conceivable but not trivial and currently not implemented.
 */


/**
 * \file 
 * One-stop header file for all AD-tool-independent AMPI routines; this is the file to replace mpi.h in the user code.
 */ 



#include "ampi/internal/passThrough.h"
#include "ampi/internal/nt.h"
#include "ampi/internal/modified.h"

#endif
