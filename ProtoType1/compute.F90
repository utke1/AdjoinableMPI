#include "ampi.inc"

subroutine ring(x)
!$openad xxx template oad_t_split.f

 ! use mpi
 implicit none ! after 'use' before 'include'
 include 'mpif.h'

 double precision  x, lx, rx
 integer myid, numprocs, i, rc, ierr, leftId, rightId,req(4)
 call MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)
 call MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierr)
 do i=1,numprocs
    ! compute something
    x=sin(x)
    ! get the neighbor id
    leftId=MODULO(myId-1,numprocs)
    rightId=MODULO(myId+1,numprocs)
    ! antiwait for all non-blocking requests
    call AMPI_AWAITALL(4,req,MPI_STATUSES_IGNORE,ierr)
    ! send to left
    call AMPI_ISEND_TO_I(x,1,MPI_DOUBLE_PRECISION,leftId,0,& 
         MPI_COMM_WORLD,req(1),ierr) 
    ! send to right
    call AMPI_ISEND_TO_I(x,1,MPI_DOUBLE_PRECISION,rightId,0,& 
         MPI_COMM_WORLD,req(2),ierr)
    ! recv from left
    call AMPI_IRECV_FROM_I(lx,1,MPI_DOUBLE_PRECISION,leftId,0,& 
         MPI_COMM_WORLD,req(3),ierr) 
    ! recv from right
    call AMPI_IRECV_FROM_I(rx,1,MPI_DOUBLE_PRECISION,rightId,0,& 
         MPI_COMM_WORLD,req(4),ierr)
    ! wait for all non-blocking requests
    call AMPI_WAITALL(4,req,MPI_STATUSES_IGNORE,ierr)
    ! add the neighbors contributions
    x=x+rx+lx
 end do
end

subroutine compute(x,f)
 ! use mpi
 implicit none ! after 'use' before 'include'
 include 'mpif.h'

 double precision  x, f
 integer ierr
 call ring(x) 
 call AMPI_REDUCE(x,f,1,MPI_DOUBLE_PRECISION,MPI_SUM,0,&
      MPI_COMM_WORLD,ierr)
end
