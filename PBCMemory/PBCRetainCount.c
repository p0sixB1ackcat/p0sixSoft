#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "PBCRetainCount.h"

MMLinkListNode *plinkListHeader = NULL;

void pushWithPool(ImageObject *p)
{
    if(!p)
        return;
    
    p->entry->next = plinkListHeader;
    plinkListHeader = p->entry;
}

void popWithPool(ImageObject **p)
{
    if(!p)
        return;
    
    MMLinkListNode *tmpNode = NULL;
    
    if((*p)->entry == plinkListHeader)
    {
        tmpNode = plinkListHeader;
        plinkListHeader = plinkListHeader->next;
        free(tmpNode);
        free(*p);
        tmpNode = NULL;
        *p = NULL;
    }
    else if((*p)->entry->next)
    {
        while(plinkListHeader->next)
        {
            if(plinkListHeader->next == (*p)->entry)
            {
                tmpNode = (*p)->entry->next;
                plinkListHeader->next = tmpNode;
                free((*p)->entry);
                (*p)->entry = NULL;
                free(*p);
                *p = NULL;
                break;
            }
        }
    }
    else
    {
        free((*p)->entry);
        (*p)->entry = NULL;
        free(p);
        p = NULL;
    }
    
}


ImageObject *pbc_maloc(int size)
{
    ImageObject *p = NULL;
    p = (ImageObject *)malloc(size);
    if(!p)
        return NULL;
    p->pointImportCount = 1;
    p->entry = (MMLinkListNode *)malloc(sizeof(MMLinkListNode));
    if(!p->entry)
    {
        free(p);
        p = NULL;
        return NULL;
    }
    
    pushWithPool(p);
    return p;
}

int pbc_free(ImageObject ** p)
{
    if(!p)
        return -1;
    if((*p)->pointImportCount)
    {
        printf("point import count not zero!\n");
        return -1;
    }
    popWithPool(p);
    return 1;
}


