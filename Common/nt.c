#include <mpi.h>
#include "ampi/userIF/nt.h"
#include "ampi/adTool/support.h"
#include <assert.h>

int AMPI_Init_NT(int* argc, 
	      char*** argv) { 
  int rc;
  rc=MPI_Init(argc,
              argv);
  ADTOOL_AMPI_setupTypes();
  return rc;
}

int AMPI_Finalize_NT(void) { 
  return MPI_Finalize();
}


int AMPI_Buffer_attach_NT(void *buffer, 
			  int size) { 
  return MPI_Buffer_attach(buffer,
			   size);

}

int AMPI_Buffer_detach_NT(void *buffer, 
			  int *size){ 
  return MPI_Buffer_detach(buffer,
			   size);
}

int AMPI_Type_create_struct_NT(int count,
			       int array_of_blocklengths[],
			       MPI_Aint array_of_displacements[],
			       MPI_Datatype array_of_types[],
			       MPI_Datatype *newtype) {
  int i, rc;
  rc = MPI_Type_create_struct (count,
			       array_of_blocklengths,
			       array_of_displacements,
			       array_of_types,
			       newtype);
  if (!(rc==MPI_SUCCESS)) assert(0);
  MPI_Datatype packed_type;
  int array_of_p_blocklengths[count];
  MPI_Aint array_of_p_displacements[count];
  MPI_Datatype array_of_p_types[count];
  int s=0, is_active, mapsize=0, p_mapsize=0;
  for (i=0;i<count;i++) {
    is_active = ADTOOL_AMPI_isActiveType(array_of_types[i])==AMPI_ACTIVE;
    array_of_p_blocklengths[i] = array_of_blocklengths[i];
    array_of_p_displacements[i] = p_mapsize;
    array_of_p_types[i] = is_active ? MPI_DOUBLE : array_of_types[i];
    if (is_active) s = sizeof(double);
    else if (array_of_types[i]==MPI_DOUBLE) s = sizeof(double);
    else if (array_of_types[i]==MPI_INT) s = sizeof(int);
    else if (array_of_types[i]==MPI_FLOAT) s = sizeof(float);
    else if (array_of_types[i]==MPI_CHAR) s = sizeof(char);
    else assert(0);
    p_mapsize += array_of_blocklengths[i]*s;
  }
  /* we'll take a guess at the struct size, but it's best to specify with MPI_Type_create_resized */
  MPI_Aint lb, extent;
  MPI_Type_get_extent(*newtype,&lb,&extent);
  mapsize = (int)extent - (int)lb;
  rc = MPI_Type_create_struct (count,
			       array_of_p_blocklengths,
			       array_of_p_displacements,
			       array_of_p_types,
			       &packed_type);
  if (!(rc==MPI_SUCCESS)) assert(0);
  derivedTypeData* dat = getDTypeData();  
  addDTypeData(dat,
	       count,
	       array_of_blocklengths,
	       array_of_displacements,
	       array_of_types,
	       mapsize,
	       array_of_p_blocklengths,
	       array_of_p_displacements,
	       array_of_p_types,
	       p_mapsize,
	       newtype,
	       &packed_type);
  return rc;
}

int AMPI_Type_commit_NT(MPI_Datatype *datatype) {
  int dt_idx = derivedTypeIdx(*datatype);
  if (isDerivedType(dt_idx)) MPI_Type_commit(&(getDTypeData()->packed_types[dt_idx]));
  return MPI_Type_commit(datatype);
}

int AMPI_Type_create_resized_NT(MPI_Datatype oldtype,
				MPI_Aint lb,
				MPI_Aint extent,
				MPI_Datatype *newtype) {
  int rc;
  rc = MPI_Type_create_resized(oldtype,
			       lb,
			       extent,
			       newtype);
  int dt_idx = derivedTypeIdx(oldtype);
  if (isDerivedType(dt_idx)) {
    derivedTypeData* dtd = getDTypeData();
    dtd->mapsizes[dt_idx] = (int)(extent-lb);
    dtd->derived_types[dt_idx] = *newtype;
  }
  return rc;
}

int AMPI_Op_create_NT(MPI_User_function *function,
		      int commute,
		      MPI_Op *op) {
  int rc;
  rc = MPI_Op_create(function,
		     commute,
		     op);
  if (!(rc==MPI_SUCCESS)) assert(0);
  userDefinedOpData* dat = getUOpData();
  addUOpData(dat,
	     op,
	     function,
	     commute);
  return rc;
}
