!##########################################################
!# This file is part of the AdjoinableMPI library         #
!# released under the MIT License.                        #
!# The full COPYRIGHT notice can be found in the top      #
!# level directory of the AdjoinableMPI distribution.     #
!########################################################## 

       include 'mpif.h'

       INTEGER AMPI_ACTIVE
       PARAMETER (AMPI_ACTIVE=1)

       INTEGER AMPI_RECV
       PARAMETER (AMPI_RECV=0)
       INTEGER AMPI_SEND
       PARAMETER (AMPI_SEND=1)
       INTEGER AMPI_IRECV_WAIT
       PARAMETER(AMPI_IRECV_WAIT=2)
       INTEGER AMPI_IRECV_WAITALL
       PARAMETER(AMPI_IRECV_WAITALL=3)
       INTEGER AMPI_ISEND_WAIT
       PARAMETER(AMPI_ISEND_WAIT=4)
       INTEGER AMPI_ISEND_WAITALL
       PARAMETER(AMPI_ISEND_WAITALL=5)
       INTEGER AMPI_BSEND
       PARAMETER(AMPI_BSEND=6)
       INTEGER AMPI_RSEND
       PARAMETER(AMPI_RSEND=7)
       INTEGER AMPI_ISEND
       PARAMETER(AMPI_ISEND=8)
       INTEGER AMPI_IRECV
       PARAMETER(AMPI_IRECV=9)
       INTEGER AMPI_WAIT
       PARAMETER(AMPI_WAIT=10)

       COMMON /ACTIVE_TYPES/ AMPI_ADOUBLE_PRECISION, AMPI_AREAL
       
       INTEGER AMPI_ADOUBLE_PRECISION, AMPI_AREAL

       EXTERNAL AMPI_INIT_NT, AMPI_COMM_RANK, AMPI_FINALIZE_NT
       EXTERNAL ADTOOL_AMPI_TURN
       EXTERNAL FW_AMPI_SEND, FW_AMPI_RECV
       EXTERNAL FW_AMPI_ISEND, FW_AMPI_IRECV, FW_AMPI_WAIT
       EXTERNAL BW_AMPI_SEND, BW_AMPI_RECV
       EXTERNAL BW_AMPI_ISEND, BW_AMPI_IRECV, BW_AMPI_WAIT
