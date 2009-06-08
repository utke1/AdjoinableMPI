!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
subroutine  AMPI_awaitall ( & 
     count, &
     requests, &
     statuses, &
     ierror)
  include 'mpif.h'

  integer count
  integer requests(*)
  integer statuses(MPI_STATUS_SIZE,*)
  integer ierror  

end 

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
subroutine  AMPI_isend_from_i ( & 
     buf, & 
     count, &
     datatype, &
     dest, &
     tag, &
     comm, &
     request, &
     ierror)

  double precision buf(*)
  integer count
  integer datatype
  integer dest
  integer tag
  integer comm
  integer request
  integer ierror  

  call mpi_isend( &
     buf, & 
     count, &
     datatype, &
     dest, &
     tag, &
     comm, &
     request, &
     ierror)

end 

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
subroutine  AMPI_irecv_from_i ( & 
     buf, & 
     count, &
     datatype, &
     src, &
     tag, &
     comm, &
     request, &
     ierror)

  double precision buf(*)
  integer count
  integer datatype
  integer src
  integer tag
  integer comm
  integer request
  integer ierror  

  call mpi_irecv( &
     buf, & 
     count, &
     datatype, &
     src, &
     tag, &
     comm, &
     request, &
     ierror)

end 

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
subroutine  AMPI_waitall ( & 
     count, &
     requests, &
     statuses, &
     ierror)
  include 'mpif.h'

  integer count
  integer requests(*)
  integer statuses(MPI_STATUS_SIZE,*)
  integer ierror  

  call mpi_waitall( &
     count, &
     requests, &
     statuses, &
     ierror)

end 

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
subroutine  AMPI_reduce ( & 
     sbuf, & 
     rbuf, & 
     count, &
     datatype, &
     op, &
     root, &
     comm, &
     ierror)

  double precision sbuf(*)
  double precision rbuf(*)
  integer count
  integer datatype
  integer op
  integer root
  integer comm
  integer ierror  
  call mpi_reduce( &
     sbuf, & 
     rbuf, & 
     count, &
     datatype, &
     op, &
     root, &
     comm, &
     ierror)

end 
