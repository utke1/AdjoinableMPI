#include <malloc.h>
#include <assert.h>
#include "ampi/bookkeeping/support.h"

struct RequestListItem { 
  struct AMPI_Request_S ampiRequest;
  struct RequestListItem *next_p;
  struct RequestListItem *prev_p;
};

static struct RequestListItem* requestListHead=0;
static struct RequestListItem* requestListTail=0;
static struct RequestListItem* unusedRequestStack=0;

/**
 * very simple implementation for now 
 */
static struct RequestListItem* addToList() { 
  struct RequestListItem* returnItem_p=0;
  if (! unusedRequestStack) { 
    unusedRequestStack=(struct RequestListItem*)malloc(sizeof(struct RequestListItem));
    assert(unusedRequestStack);
    unusedRequestStack->next_p=0; 
    unusedRequestStack->prev_p=0; 
  }
  /* get it from the unused stack */
  returnItem_p=unusedRequestStack;
  unusedRequestStack=returnItem_p->prev_p;
  returnItem_p->prev_p=0;
  /* add it to the list */
  if (requestListTail) { 
    requestListTail->next_p=returnItem_p;
    returnItem_p->prev_p=requestListTail;
  }
  requestListTail=returnItem_p;
  return returnItem_p;
}

/**
 * very simple implementation for now 
 */
static void dropFromList(struct RequestListItem* toBoDropped) { 
  /* remove it from the list */
  if (requestListHead==toBoDropped) { 
    requestListHead=toBoDropped->next_p;
    requestListHead->prev_p=0;
    toBoDropped->next_p=0;
  }
  if (requestListTail==toBoDropped) { 
    requestListTail=toBoDropped->prev_p;
    requestListTail->next_p=0;
    toBoDropped->prev_p=0;
  }
  if (toBoDropped->next_p && toBoDropped->prev_p) {
    toBoDropped->prev_p->next_p=toBoDropped->next_p;
    toBoDropped->next_p->prev_p=toBoDropped->prev_p;
    toBoDropped->next_p=0; 
    toBoDropped->prev_p=0;
  }
  /* add it to the unused stack */
  if (unusedRequestStack) { 
    toBoDropped->prev_p=unusedRequestStack;
  }
  unusedRequestStack=toBoDropped;
}

static struct RequestListItem* findInList(MPI_Request *request) { 
  struct RequestListItem* current_p=requestListHead;
  while(current_p) { 
    if (current_p->ampiRequest.plainRequest==*request) break;
    current_p=current_p->next_ppp;
  }
  assert(current_p);
  return current_p;
}

void BK_AMPI_put_AMPI_Request(struct AMPI_Request_S  *ampiRequest) { 
  struct RequestListItem *inList_p;
  inList_p=addToList();
  inList_p->ampiRequest=*ampiRequest;
}

void BK_AMPI_get_AMPI_Request(MPI_Request *request, struct AMPI_Request_S  *ampiRequest) { 
  struct RequestListItem *inList_p=findInList(request);
  *ampiRequest=inList_p->ampiRequest;
  dropFromList(inList_p);
}

void BK_AMPI_read_AMPI_Request(MPI_Request *request, struct AMPI_Request_S  *ampiRequest) { 
  struct RequestListItem *inList_p=findInList(request);
  *ampiRequest=inList_p->ampiRequest;
}

