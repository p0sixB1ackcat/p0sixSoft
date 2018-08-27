#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "PBCMemoryPool.h"
#include "hashmap.h"

#define MemoryBlockKeyLength 0x0c
#define MemoryPoolLinkListNodeTagLength 5
#define MemoryPossessInfoNameLength 0x10

typedef struct _Memoryblock
{
    //内存块标识
    char *key;
    
    //实际使用的内存
    void *buffer;
    
}Memoryblock,*PMemoryblock;

typedef struct _InfoEntry
{
    //内存块标记，用来区分不同内存对象
    char tags[MemoryPoolLinkListNodeTagLength];
    
    //内存池的数据结构，用链表来管理，每个结点对应一块指定结构体、内存对象大小倍数的内存块，用于分配时切割
    struct _InfoEntry *next;
    
    //当前使用内存块的数量
    int currentObjCount;
    
    //最大内容容量，要DataObject的长度相乘
    int maxObjCount;
    
    //单个对象的大小
    int objSize;
    
    //存放当前tag下的上次被标记为空闲的地址，初始化为NULL
    long lastFree;
    
    //内存块
    Memoryblock *bufferblock;
    
}InfoEntry,*PInfoEntry;

/**
    内存池结构
 */
typedef struct _MemoryPool
{
    
    PInfoEntry header;
    PInfoEntry tail;
    
    //用来将每个内存块和所属内存池链表结点做一个映射
    map_t *hashmap;
    
}MemoryPool,*PMemoryPool;

/**
    持有者信息
 */
typedef struct _PossessInfo
{
    char FunctionName[MemoryPossessInfoNameLength];
    pthread_t ThreadID;
    struct _PossessInfo *Flink;
    struct _PossessInfo *Blink;
    
}PossessInfo,*PPossessInfo;

/**
 实际使用的结构体，内存对象
 */
typedef struct _DataObject
{
    //引用计数
    int retainCount;
    
    //...上面的参数可以按照实际需要进行填充
    long long place;
    
    //内存对象的链表结点，存放当前引用该对象的函数名称，线程id
    PossessInfo *possessInfoHeader;
    
}DataObject,*PDataObject;

PMemoryPool _globalMemoryPool = NULL;

int verstr(const char *dest,const char *source);

void initialize()
{
    _globalMemoryPool = (PMemoryPool)malloc(sizeof(MemoryPool));
    if(!_globalMemoryPool)
    {
        printf("malloc faild!\n");
        return;
    }
    memset((void *)_globalMemoryPool,0x00,sizeof(MemoryPool));
    
    _globalMemoryPool->hashmap = hashmap_new();
    
    _globalMemoryPool->header = _globalMemoryPool->tail = NULL;
    
}

int verstr(const char *dest,const char *source)
{
    if((!dest && source) || (dest && !source))
        return 0;
    if(strlen(dest) != strlen(source))
    {
        return 0;
    }
    
    char *p,*q;
    p = (char *)dest;
    q = (char *)source;
    int result = 1;
    while(*p != '\0')
    {
        if(*p++ != *q++)
        {
            result = 0;
            break;
        }
    }
    
    return result;
    
}

void InsertNodeFromHeader(InfoEntry *node)
{
    if(!_globalMemoryPool->header)
    {
        _globalMemoryPool->header = node;
    }
    else
    {
        node->next = _globalMemoryPool->header;
        _globalMemoryPool->header = node;
    }
}

InfoEntry *createPoolNode(int size,int maxobjcount,const char*tag)
{
    InfoEntry *newInfo = (InfoEntry *)malloc(sizeof(InfoEntry));
    newInfo->bufferblock = (Memoryblock *)malloc((size + (MemoryBlockKeyLength * maxobjcount)) * maxobjcount);
    
    newInfo->bufferblock->key = (char *)newInfo->bufferblock;
    newInfo->bufferblock->buffer = (((char *)newInfo->bufferblock) + MemoryBlockKeyLength);
    newInfo->currentObjCount = 0;
    newInfo->maxObjCount = maxobjcount;
    newInfo->objSize = size;
    newInfo->lastFree = 0x00;
    newInfo->next = NULL;
    strlcpy(newInfo->tags, tag, MemoryPoolLinkListNodeTagLength);
    snprintf(newInfo->bufferblock->key, MemoryBlockKeyLength, "%s-%d",tag,newInfo->currentObjCount);
    return newInfo;
}

void *PBCAllocatePoolWithTag(int size,int maxobjcount, const char *tag)
{
    if(!tag)
        return NULL;
    if(!_globalMemoryPool)
        initialize();
    
    InfoEntry *info = _globalMemoryPool->header;
    void *buffer = NULL;
    int ishave = 0;
    
    while(info)
    {
        if(verstr(tag, info->tags) && info->currentObjCount < info->maxObjCount)  //去链表中找tag
        {
            ishave = 1;
            break;
        }
        info = info->next;
    }
    if(!ishave)
    {
        //没找到，要重新生成一个和该tag对应的结点，并根据size大小生成其整数倍的内存块，并进行切割
        info = createPoolNode(size, maxobjcount, tag);
        buffer = (&(info->bufferblock->buffer) + (info->currentObjCount * info->objSize));
        snprintf((char *)buffer, MemoryBlockKeyLength, "%s-%d",info->tags,info->currentObjCount);
        info->bufferblock->key = (char *)buffer;
        buffer += MemoryBlockKeyLength;
        
        hashmap_put(_globalMemoryPool->hashmap, info->bufferblock->key, info);
        ++info->currentObjCount;
        
        InsertNodeFromHeader(info);
    }
    else
    {
        //到这里，说明内存池中对应tag内存块有空闲
        if(info->lastFree)
        {
            //根据lastFree一级一级的取地址
            buffer = (void *)info->lastFree;
            info->lastFree = *((long *)info->lastFree);
        }
        else
        {
            //先切割，在增加currentObjCount
            //注意这里，要取buffer的地址，直接用buffer，如果该buffer内容被填充，那么buffer的值不是地址，而是实际的内容
            buffer = &info->bufferblock->buffer;
            buffer += ((info->currentObjCount * info->objSize));
            snprintf((char *)buffer, MemoryBlockKeyLength,  "%s-%d",info->tags,info->currentObjCount);
            info->bufferblock->key = (char *)buffer;
            buffer += MemoryBlockKeyLength;
#ifdef __WATCHLOG__
            printf("key address is %p\n",info->bufferblock->key);
#endif
            ++info->currentObjCount;
            //切割之后,将该内存块的地址和所处链表结点地址做一个映射，存放到hash表中
            hashmap_put(_globalMemoryPool->hashmap, info->bufferblock->key, info);
            
            
        }
    }
    
    return buffer;
}

//从链表中摘掉目标结点
void removePossessInfoInLink(PossessInfo *header,PossessInfo *target)
{
    if(header == target)
    {
        free(target);
        target = NULL;
    }
    else if(target->Blink)
    {
        target->Blink->Flink = target->Flink;
        target->Flink->Blink = target->Blink;
        free(target);
        target = NULL;
    }
    else
    {
        target->Flink->Blink = target->Blink;
        free(target);
        target = NULL;
    }
}

//将lastFree指针写入到dataObject->value域，并将lastFree重新指向当前的空闲内存地址
void flagMemoryUnoccupied(void *buffer)
{
    if(!buffer)
        return;
    PDataObject dataObject = (PDataObject)buffer;
    int count = 0;
    do
    {
        if(dataObject->retainCount > 1)
        {
            sleep(3);
            continue;
        }
        
        //到了这里，说明可以正常被标记为空闲了
        goto safeflag;
        
    }while(++count < 3);
    
    --dataObject->retainCount;
    if(dataObject->retainCount > 0)
    {
        //此时说明内存泄露，将possessInfo信息发给服务器，或写入到磁盘中
        PossessInfo *possessInfo = dataObject->possessInfoHeader;
        while(possessInfo)
        {
            //write()
            possessInfo = possessInfo->Blink;
        }
        return;
    }
    else
    {
        free(dataObject->possessInfoHeader);
        dataObject->possessInfoHeader = NULL;//如果引用计数为0，说明此时链表只剩一个结点，直接释放
    }
    
safeflag:
    {
        //求得该内存块所在结点
        InfoEntry *inNode;
        //根据void *类型的buffer，找到与之对应的info

        char *key = ((char *)(long)dataObject - MemoryBlockKeyLength);
#ifdef __WATCHLOG__
        printf("buffer address is %p,key address is %p\n",dataObject,key);
#endif
        if(!key)
        {
            printf("error:key is NULL");
        }
        hashmap_get(_globalMemoryPool->hashmap, key, (void **)&inNode);
        if(!inNode)
        {
            printf("hashmap_get inNode faild\n");
            return;
        }
        
        *((long *)dataObject) = inNode->lastFree;
        inNode->lastFree = (long)dataObject;
    }
    
    
}

void checkGlobalLinkListCount(void)
{
    int count = 0;
    InfoEntry *p = _globalMemoryPool->header;
    while(p)
    {
        count++;
        p = p->next;
    }
    
    printf("current link list count is %d\n",count);
}

//填写持有者信息
void fillPossessInfo(PossessInfo *possessInfo, char *funcName,pthread_t threadId)
{
    int realLen = 0x10 * sizeof(char);
    int funcNameLen = strlen(funcName);
    int len = realLen > funcNameLen ? funcNameLen : realLen;
    strlcpy(possessInfo->FunctionName, funcName, len + 1);
    possessInfo->ThreadID = pthread_self();
}


void *PBCMalloc()
{
    PDataObject dataObject = (PDataObject)PBCAllocatePoolWithTag(sizeof(DataObject), 0x10, "PDAT");
    if(!dataObject)
    {
        printf("PBCAllocatePoolWithTag failed:PDATA!\n");
        return NULL;
    }
    
    dataObject->retainCount = 1;
    dataObject->possessInfoHeader = PBCAllocatePoolWithTag(sizeof(PossessInfo),0x10,"PEIH");
    if(!dataObject->possessInfoHeader)
    {
        printf("PBCAllocatePoolWithTag possessInfo failed:PEIH!\n");
        flagMemoryUnoccupied(dataObject);//将该内存块放回到内存池
        return NULL;
    }
    
    
    fillPossessInfo(dataObject->possessInfoHeader,(char *)__func__, pthread_self());
    dataObject->possessInfoHeader->Blink = dataObject->possessInfoHeader->Flink = NULL;
    return dataObject;
}


void PBCFreePool()
{
    if(_globalMemoryPool)
    {
        if(_globalMemoryPool->header)
        {
            InfoEntry *p = _globalMemoryPool->header;
            while(p)
            {
                InfoEntry *tmp = p;
                if(tmp->bufferblock)
                {
                    free(tmp->bufferblock);
                    tmp->bufferblock = NULL;
                }
                p = p->next;
                free(tmp);
                tmp = NULL;
            }
        }
        free(_globalMemoryPool);
        _globalMemoryPool = NULL;
    }
    
}
