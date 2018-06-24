#ifndef PBCRetainCount_h
#define PBCRetainCount_h

typedef struct _MMLinkListNode
{
    struct _MMLinkListNode *next;
    
}MMLinkListNode,*PMMLinkListNode;

typedef struct _ImageObject
{
    char name[0x04];
    char flag;
    int value;
    int pointImportCount;
    MMLinkListNode *entry;
    
}ImageObject,*PImageObject;


ImageObject *pbc_maloc(int size);

int pbc_free(ImageObject ** p);

#endif /* PBCRetainCount_h */
