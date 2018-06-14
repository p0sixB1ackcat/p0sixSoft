//
//  PBCMemPool.h
//  memorypool
//
//  Created by rongbei-wuxian on 2018/6/14.
//  Copyright © 2018年 rongbei-wuxian. All rights reserved.
//

#ifndef PBCMemPool_h
#define PBCMemPool_h

typedef struct _MMLinkListNode
{
    struct _MMLinkListNode *next;
    
}MMLinkListNode,*PMMLinkListNode;

typedef struct _ImageObject
{
    char name[20];
    char flag;
    int value;
    int pointImportCount;
    MMLinkListNode *entry;
    
}ImageObject,*PImageObject;


ImageObject *pbc_maloc(int size);

int pbc_free(ImageObject ** p);

#endif /* PBCMemPool_h */
