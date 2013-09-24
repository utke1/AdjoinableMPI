#ifndef _AMPI_AMPI_H_
#define _AMPI_AMPI_H_

/** \mainpage 
 * \authors Laurent Hascoet
 * \authors Uwe Naumann
 * \authors Michel Schanen
 * \authors Jean Utke 
 *
 * \section links Links to Ressources
 * 
 *  - <a href="https://trac.mcs.anl.gov/projects/AdjoinableMPI/wiki">TRAC  page</a> for bugs and feature tracking
 *  - <a href="http://mercurial.mcs.anl.gov/ad/AdjoinableMPI/">mercurial repository</a> for source code and change history
 *  
 * \section Introduction
 * 
 * The Adjoinable MPI (AMPI) library provides a modified set if MPI subroutines 
 * that are constructed such that and adjoint in the context of algorithmic 
 * differentiation (AD) can be computed. 
 * There are principal recipes for the construction of the adjoint of 
 * a given communication, see \cite Utke2009TAM . 
 * The practical implementation of these recipes, however, faces the following 
 * challenges.
 *  - the target language may prevent some implementation options
 *   - exposing an MPI_Request augmented with extra information as a structured type (not supported by Fortran 77)
 *   - passing an array of buffers (of different length), e.g. to \ref AMPI_Waitall, as a additional argument to  (not supported in any Fortran version)
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
 * The sources can be accessed throug the <a href="http://mercurial.mcs.anl.gov/ad/AdjoinableMPI/">AdjoinableMPI mercurial repository</a>. Bug tracking, feature requests 
 * etc. are done via <a href="http://trac.mcs.anl.gov/projects/AdjoinableMPI">trac</a>.
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
 * It references the header files in <tt>ampi/userIF</tt> which are organized to contain
 *  - unmodified pass through to MPI in <tt>ampi/userIF/passThrough.h</tt> which exists to give the extent of the original MPI we cover  
 *  - variants of routines that in principle need adjoint logic but happen to be called outside of the code section that is adjoined and therefore 
 *    are not transformed / not traced (NT) in  <tt>ampi/userIF/nt.h</tt>
 *  - routines that are modified from the original MPI counterparts because their behavior in the reverse sweep differs from their behavior in the 
 *    forward sweep and they also may have a modified signatyre; in <tt>ampi/userIF/modified.h</tt>
 *  - routines that are specific for some variants of source transformation (ST) approaches in <tt>ampi/userIF/st.h</tt>; 
 *    while these impose a larger burden for moving from MPI to AMPI on the user, they also enable a wider variety of transformations 
 *    currently supported by the tools; we anticipate that the ST specific versions may become obsolete as the source transformation tools evolve to 
 *    support all transformations via the routines in <tt>ampi/userIF/modified.h</tt> 
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
 * the need for the AD tool implementation to tackle them. 
 * For cases where these routines have to be called within the adjointed code section the variants without the <tt>_NT</tt> suffix will ensure the
 * correct adjoint behavior.
 * 
 * \subsection commPat General Assumptions and Notions on Communication Patterns
 * 
 * \subsubsection pairings Pairings
 *
 * Following the explanations in \cite Utke2009TAM it is clear that context information about the 
 * communication pattern, that is the pairing of MPI calls, is needed to achieve 
 * -# correct adjoints, i.e. correct send and receive end points and deadlock free
 * -# if possible retain the efficiency advantages present in the original MPI communication for the adjoint.
 *
 * In AMPI pairings are conveyed via additional <tt>pairedWith</tt> parameters which may be set to \ref AMPI_PairedWith enumeration values , see e.g. \ref AMPI_Send or \ref AMPI_Recv.
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
 * 
 * <b>    Restriction : pairing of send and receive types must be static. </b>
 *
 * Note that this does not prevent the use of wild cards for source, destination, or tag.  
 * 
 * \subsubsection nonblocking Nonblocking Communication and Fortran Compatibility
 * 
 * A central concern is the handling of non-blocking sends and receives in combination with their respective completion,
 * e.g. wait,  waitall, test. 
 * Taking as an example 
 * \code{.cpp}
 * MPI_Irecv(&b,...,&r);
 * // some other code in between 
 * MPI_Wait(&r,MPI_STATUS_IGNORE); 
 * \endcode
 * The adjoint action for <tt>MPI_Wait</tt> will have to be the <tt>MPI_Isend</tt> of the adjoint data associated with 
 * the data in buffer <tt>b</tt>. 
 * The original <tt>MPI_Wait</tt> does not have any of the parameters required for the send and in particular it does not 
 * have the buffer. The latter, however, is crucial in particular in a source transformation context because, absent a correct syntactic 
 * representation for the buffer at the <tt>MPI_Wait</tt> call site one has to map the address <tt>&b</tt> valid during the forward 
 * sweep to the address of the associated adjoint buffer during the reverse sweep. 
 * In some circumstances, e.g. when the buffer refers to stack variable and the reversal mode follows a strict <em>joint</em> scheme 
 * where one does not leave the stack frame of a given subroutine until the reverse sweep has completed, it is possible to predetermine 
 * the address of the respective adjoint buffer even in the source transformation context.  
 * In the general case, e.g. allowing for <em>split</em> mode reversal 
 * or  dynamic memory deallocation before the adjoint sweep commences such predetermination 
 * requires a more elaborate mapping algorithm. 
 * This mapping is subject of ongoing research and currently not supported. 
 * 
 * On the other hand, for operator overloading based tools, the mapping to a reverse sweep address space is an integral part of the 
 * tool because there the reverse sweep is executed as interpretation of  a trace of the execution that is entirely separate from the original program 
 * address space. Therefore all addresses have to be mapped to the new adjoint address space to begin with and no association to some 
 * adjoint program variable is needed. Instead, the buffer address can be conveyed via the request parameter (and AMPI-userIF bookkeeping) 
 * to the <tt>MPI_Wait</tt> call site, traced there and is then recoverable during the reverse sweep.  
 * Nevertheless, to allow a common interface this version of the AMPI library has the buffer as an additional argument to in the source-transformation-specific \ref AMPI_Wait_ST 
 * variant of \ref AMPI_Wait.  
 * In later editions, when source transformation tools can fully support the address mapping, the  of the AMPI library the \ref AMPI_Wait_ST variant  may be dropped.  
 * 
 * Similarly to conveying the buffer address via userIF bookkeeping associated with the request being passed, all other information such as source or destination, tag, 
 * data type, or the distinction if a request originated with a send or receive  will be part of the augmented information attached to the request and be subject to the trace and recovery as the buffer address itself. 
 * In the source transformation context, for cases in which parameter values such as source, destination, or tag are constants or loop indices the question could be asked if these values couldn't be easily recovered in
 * the generated adjoint code without having to store them. 
 * Such recovery following a TBR-like approach would, however, require exposing the augmented request instance as a structured data type to the TBR analysis in the languages other than Fortran77. 
 * This necessitates the introduction of the \ref AMPI_Request, which in Fotran77 still maps to just an integer address. 
 * The switching between these variants is done via  configure flags, see \ref configure.
 * 
 * \subsection bookkeeping Bookkeeping of Requests
 * 
 * As mentioned in \ref nonblocking the target language may prevent the augmented request from being used directly.  
 * In such cases the augmented information has to be kept internal to the library, that is we do some bookkeeping to convey the necessary information between the nonblocking sends or receives and
 * the and respective completion calls. Currently the bookkeeping has a very simple implementation as a doubly-linked list implying linear search costs which is acceptable only as long as the 
 * number of icomplete nonblocking operations per process remains moderate. 
 *
 * Whenever internal handles are used to keep trace (or correspondence) of a given internal object
 * between two distant locations in the source code (e.g. file identifier to keep trace of an opened/read/closed file,
 * or address to keep trace of a malloc/used/freed dynamic memory, or request ID to keep trace of a Isend/Wait...)
 * we may have to arrange the same correspondence during the backward sweep.
 * Keeping the internal identifier in the AD stack is not sufficient because there is no guarantee that
 * the mechanism in the backward sweep will use the same values for the internal handle.
 * The bookkeeping we use to solve this problem goes as follows:
 * -- standard TBR mechanism makes sure that variables that are needed in the BW sweep and are overwritten
 *    are pushed onto the AD stack before they are overwritten
 * -- At the end of its life in the forward sweep, the FW handle is pushed in the AD stack
 * -- At the beginning of its backward life, we obtain a BW handle, we pop the FW handle,
 *    and we keep the pair of those in a table (if an adjoint handle is created too, we keep the triplet).
 * -- When a variable is popped from the AD stack, and it is an internal handle,
 *    the popped handle is re-based using the said table.
 *
 * Simple workaround for the "request" case:
 * This method doesn't rely on TBR.
 * -- Push the FW request upon acquisition (e.g. just after the Isend)
 * -- Push the FW request upon release (e.g. just before the Wait)
 * -- Pop the FW request upon adjoint of resease, and get the BW request from the adjoint of release
 * -- Add the BW request into the bookkeeping, with the FW request as a key.
 * -- Upon ajoint of acquisition, pop the FW request, lookup in the bookkeeping to get the BW request.
 *
 * \subsection Things that don't work
 *
 * The question is whether to piggyback the derivative information on the original or to
 * sending shadow messages for the derivatives.
 * One argument for shadow messages is to avoid type-change from bundles to separate values and
 * separate derivatives.
 * One argument against that is the use of wildcards, which makes it difficult to
 * correctly associate parts of the message.
 * Choice on Tuesday 12th: An association by name tool is required to bundle value with tangent
 * before communication call, and conversely unbundle after communication.
 * The bundling and unbundling routines are left for the user to implement,
 * but eventually should also be generated by the AD tool if types are serializable.
 * The bundling and unbundling are the association-by-name equivalent of the
 * \ref rawData routines for the association-by-address.
 *
 * Specifically for tangent mode, if we don't want to modify the count value of
 * the original communication call, we need to replace the MPI datatype
 * with a new one where REAL*8 become MPI_ACTIVE (a bundle of one REAL*8 plus any sort of derivative info.)
 * For association-by-name tools, this implies that the bundling routine
 * really abides with this structure.
 *
 * About MPI_Types and the "active" boolean:
 * One cannot get away with just an "active" boolean to indicate the structure of
 * the MPI_Type of the bundle. Since the MPI_Type definition of the bundle type
 * has to be done anyway in the differentiated application code, and is passed
 * to the communication call, the AMPI communication implementation will
 * check this bundle MPI_Type to discover activity and trace/not trace accordingly.
 *
 * For the operator overloading, the tool needs to supply the active MPI types
 * for the built-in MPI_datatypes and using the active types, one can achieve
 * type conformance between the buffer and the type parameter passed.
 *
 * \subsection onesided One-Sided Active Targets
 * Idea - use an <tt>AMPI_Win</tt> instance (similar to the \ref AMPI_Request ) to attach more 
 * information about the things that are applied to the window and completed on the fence; 
 * we execute/trace/collect-for-later-execution operations on the window in the following fashion
 * 
 * forward: 
 * - MPI_Get record op/args on the window (buffer called 'x')  
 * - MPI_Put/MPI_Accumulate(z,,...): record op/args on the window; during forward: replace with MPI_Get of the remote target value into temporary 't' ; postpone to the  fence;
 * 
 * upon hitting a fence in the forward sweep:
 * 1. put all ops on the stack 
 * 2. run the fence 
 * 3. for earch accum/put:
 * 3.1:  push 't'
 * 3.2: do the postponed accumulate/put
 * 4. run a fence for 3.2
 * 5. for each accum*: 
 * 5.1 get accumlation result 'r'
 * 6. run a fence for 5.1 
 * 7. for each accum*:
 * 7.1 push 'r'
 * 
 * for the adjoint of a fence :
 * 0. for each operation on the window coming from the previous fence: 
 * 0.1 op isa GET then x_bar=0.0
 * 0.2 op isa PUT/accum= then x_bar+=t21
 * 0.3 op isa accum+ then x_bar+=t22
 * 1. run a fence
 * 2. pop  op from the stack and put onto adjoint window
 * 2.1 op isa PUT/accum=: then   GET('t21')
 * 2.2 op isa accum+; then get('t22') from adjoint target 
 * 2.3 op isa accum*, then pop('r'),  GET('t23') from adjoint target
 * 3. run a fence
 * 4. for each op on the adjoint window
 * 4.1 op isa GET, then accum+ into remote
 * 4.2 op isa PUT/accum: pop(t); accu(t,'=') to the value in the target
 * 4.3 op isa PUT/accum=; then acc(0.0,'=') to adjoint target
 * 4.4 op isa accum*: then accumulate( r*t23/t,'=', to the target) AND do z_bar+=r*t23/z  (this is the old local z ); 
 * 
 * \subsubsection derived Handling of Derived Types
 * (Written mostly in the context of ADOL-C.) MPI allows the user to create typemaps for arbitrary structures in terms of a block
 * count and arrays of block lengths, block types, and displacements. For sending an array of active variables, we could get by with
 * a pointer to their value array; in the case of a struct, we may want to send an arbitrary collection of data as well as some active
 * variables which we'll need to "dereference". If a struct contains active data, we must manually pack it into a new array because
 * -# the original datamap alignment is destroyed when we convert active data to real values
 * -# we would like to send completely contiguous messages
 * 
 * When received, the struct is unpacked again.
 * 
 * When the user calls the \ref AMPI_Type_create_struct wrapper with a datamap, the map is stored in a structure of type
 * \ref derivedTypeData; the wrapper also generates an internal typemap that describes the packed data. The packed typemap is used
 * whenver a derived type is sent and received; it's also used in conjunction with the user-provided map to pack and unpack data.
 * This typemap is invisible to the user, so the creation of derived datatypes is accomplished entirely with calls to the
 * \ref AMPI_Type_create_struct and \ref AMPI_Type_commit wrappers.
 * 
 * \image html dtype_illustration.png
 * \image latex dtype_illustration.png
 * 
 * AMPI currently supports sending structs with active elements and structs with embedded structs. Packing is called recursively.
 * 
 * \subsubsection reduction Reduction operations
 * 
 * Since operator overloading can't enter MPI routines, other AMPI functions extract the double values from active variables,
 * transfer those, and have explicit adjoint code that replaces the automated transformation. This is possible because we know the
 * partial derivative of the result. For reductions, we can also do this with built-in reduction ops (e.g., sum, product). But
 * we can't do this for user-defined ops because we don't know the partial derivative of the result.
 * 
 * (Again explained in the context of ADOL-C.) So we have to make the tracing machinery enter the Reduce and perform taping every
 * time the reduction op is applied. As it turns out, MPICH implements Reduce for derived types as a binary tree of Send/Recv pairs,
 * so we can make our own Reduce by replicating the code with AMPI_Send/Recv functions. (Note that derived types are necessarily
 * reduced with user-defined ops because MPI doesn't know how to accumulate them with its built-in ops.) So AMPI_Reduce is implemented
 * for derived types as the aforementioned binary tree with active temporaries used between steps for applying the reduction op.
 * See \ref AMPI_Op_create_NT.
 * 
 * 
 */


/**
 * \file 
 * One-stop header file for all AD-tool-independent AMPI routines; this is the file to replace mpi.h in the user code.
 */ 

#include <mpi.h>
#if defined(__cplusplus)
extern "C" {
#endif

#include "ampi/userIF/passThrough.h"
#include "ampi/userIF/nt.h"
#include "ampi/userIF/modified.h"
#include "ampi/userIF/st.h"

#if defined(__cplusplus)
}
#endif

#endif
