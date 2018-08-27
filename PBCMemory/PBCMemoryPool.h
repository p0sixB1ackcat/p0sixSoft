#ifndef PBCMemoryPool_h
#define PBCMemoryPool_h

//#ifndef __WATCHLOG__
//#define __WATCHLOG__
//#endif

void *PBCMalloc(void);

void *PBCAllocatePoolWithTag(int size,int maxobjcount,const char *tag);

void flagMemoryUnoccupied(void *dataObject);

void checkGlobalLinkListCount(void);

#endif /* PBCMemoryPool_h */
