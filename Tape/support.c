#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "ampi/tape/support.h"

static void* myTapeStorage=0;
static size_t myTapeStorageSize=0;
static void* myStack_p=0;
static void* myRead_p=0;

void TAPE_AMPI_init() {
  /* reset things */
  if (myTapeStorage) {
    free(myTapeStorage);
  }
  myTapeStorage=0;
  myTapeStorageSize=0;
  myStack_p=myTapeStorage;
  myRead_p=myTapeStorage;
}


static void writeBlob(void*, size_t);
static void readBlob(void*,size_t);

void writeBlob(void * aBlob,size_t aSize) {
  assert(aBlob);
  /* make some space*/
  if (aSize>myTapeStorage+myTapeStorageSize-myStack_p) {
    size_t increment=myTapeStorageSize;
    void *newTapeStorage=0;
    if (increment<aSize) increment=aSize;
    if (increment<1024) increment=1024;
    myTapeStorageSize=myTapeStorageSize+increment;
    newTapeStorage=realloc(myTapeStorage,myTapeStorageSize);
    assert(newTapeStorage);
    if (newTapeStorage!=myTapeStorage) {
      myStack_p=newTapeStorage+(myStack_p-myTapeStorage);
      myRead_p=newTapeStorage+(myRead_p-myTapeStorage);
      myTapeStorage=newTapeStorage;
    }
  }
  memcpy(myStack_p,aBlob,aSize);
  myStack_p+=aSize;
}

void readBlob(void* aBlob,size_t aSize) {
  assert(aSize<=myTapeStorage+myTapeStorageSize-myRead_p);
  memcpy(aBlob,myRead_p,aSize);
  myRead_p+=aSize;
}

void popBlob(void* aBlob,size_t aSize) {
  assert(aSize<=myStack_p-myTapeStorage);
  myStack_p-=aSize;
  memcpy(aBlob,myStack_p,aSize);
}

void TAPE_AMPI_resetRead() {
  myRead_p=myTapeStorage;
}

void TAPE_AMPI_push_int(int an_int)  { writeBlob((void*)(&an_int),sizeof(int)); }
void TAPE_AMPI_pop_int(int *an_int)  { popBlob((void*)(an_int),sizeof(int)); }
void TAPE_AMPI_read_int(int* an_int) { readBlob((void*)(an_int),sizeof(int)); }

void TAPE_AMPI_push_MPI_Datatype(MPI_Datatype an_MPI_Datatype)  { writeBlob((void*)(&an_MPI_Datatype),sizeof(MPI_Datatype)); }
void TAPE_AMPI_pop_MPI_Datatype(MPI_Datatype *an_MPI_Datatype)  { popBlob((void*)(an_MPI_Datatype),sizeof(MPI_Datatype)); }
void TAPE_AMPI_read_MPI_Datatype(MPI_Datatype* an_MPI_Datatype) { readBlob((void*)(an_MPI_Datatype),sizeof(MPI_Datatype)); }

void TAPE_AMPI_push_MPI_Comm(MPI_Comm an_MPI_Comm)  { writeBlob((void*)(&an_MPI_Comm),sizeof(MPI_Comm)); }
void TAPE_AMPI_pop_MPI_Comm(MPI_Comm *an_MPI_Comm)  { popBlob((void*)(an_MPI_Comm),sizeof(MPI_Comm)); }
void TAPE_AMPI_read_MPI_Comm(MPI_Comm* an_MPI_Comm) { readBlob((void*)(an_MPI_Comm),sizeof(MPI_Comm)); }

void TAPE_AMPI_push_MPI_Request(MPI_Request an_MPI_Request)  { writeBlob((void*)(&an_MPI_Request),sizeof(MPI_Request)); }
void TAPE_AMPI_pop_MPI_Request(MPI_Request *an_MPI_Request)  { popBlob((void*)(an_MPI_Request),sizeof(MPI_Request)); }
void TAPE_AMPI_read_MPI_Request(MPI_Request* an_MPI_Request) { readBlob((void*)(an_MPI_Request),sizeof(MPI_Request)); }
