#ifndef PBCMemoryPool_h
#define PBCMemoryPool_h



void *PBCMalloc(void);

void *PBCAllocatePoolWithTag(int size,int maxobjcount,const char *tag);

void flagMemoryUnoccupied(void *dataObject);

void checkGlobalLinkListCount(void);

#endif /* PBCMemoryPool_h */
