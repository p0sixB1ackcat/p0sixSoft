#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct _InfoEntry
{
    
    struct _InfoEntry *next;    //内存池的数据结构，用链表来管理，每个结点对应一块指定结构体、内存对象大小倍数的内存块，用于分配时切割
    void *buffer;               //内存块
    
}InfoEntry,*PInfoEntry;

/**
    内存池结构
 */
typedef struct _MemoryPool
{
    
    PInfoEntry header;
    PInfoEntry tail;
    int currentObjCount;   //当前使用内存块的数量
    int maxObjCount;       //最大内容容量，要DataObject的长度相乘
    int objSize;            //单个对象的大小
    long lastFree;
}MemoryPool,*PMemoryPool;

/**
    持有者信息
 */
typedef struct _PossessInfo
{
    pthread_t ThreadID;
    char *FunctionName[30];
    struct _PossessInfo *Flink;
    struct _PossessInfo *Blink;
}PossessInfo,*PPossessInfo;

/**
    实际使用的结构体，内存对象
 */
typedef struct _DataObject
{
    int retainCount;    //引用计数
    PossessInfo *possessInfoHeader;//内存对象的hash表结点，存放当前引用该对象的函数名称，线程id
    long value;         //预留字段，用来保存指针用的
    //...
    
}DataObject,*PDataObject;

PMemoryPool _globalMemoryPool = NULL;

void initialize(int objCount,int objSize)
{
    _globalMemoryPool = (PMemoryPool)malloc(sizeof(MemoryPool));
    if(!_globalMemoryPool)
    {
        printf("malloc faild!\n");
        return;
    }
    memset((void *)_globalMemoryPool,0x00,sizeof(MemoryPool));
    _globalMemoryPool->currentObjCount = 0;
    _globalMemoryPool->objSize = objSize;
    _globalMemoryPool->maxObjCount = objCount;
    _globalMemoryPool->lastFree = 0x00;
    
    PInfoEntry info = (PInfoEntry)malloc(sizeof(InfoEntry));
    info->buffer = (PDataObject)malloc(objCount * objSize);
    info->next = NULL;
    _globalMemoryPool->header = _globalMemoryPool->tail = info;
    
}


PDataObject PBCAllocatePool()
{
    if(_globalMemoryPool->lastFree) //如果内存池中有未使用的内存
    {
        //去空闲内存中去取
        PDataObject dataObject = (PDataObject)_globalMemoryPool->lastFree;
        //下一层寻址，找到该指针之前被标记为空闲的内存块地址
        _globalMemoryPool->lastFree = dataObject->value;
        
        return dataObject;
    }
    
    PInfoEntry info = NULL;
    PDataObject buffer = NULL;
    if(_globalMemoryPool->currentObjCount >= _globalMemoryPool->maxObjCount)
    {
        //如果大于最大内存数，则需要重新生成链表结点
        info = (PInfoEntry)malloc(sizeof(InfoEntry));
        info->buffer = (PDataObject)malloc(_globalMemoryPool->objSize * _globalMemoryPool->maxObjCount);
        info->next = NULL;
        _globalMemoryPool->tail->next = info;
        _globalMemoryPool->tail = info;
        _globalMemoryPool->currentObjCount = 0;
    }
    
    //切割内存块
    if(!info)
    {
        info = _globalMemoryPool->tail;
    }
    buffer = (PDataObject)info->buffer + (_globalMemoryPool->currentObjCount * _globalMemoryPool->objSize);
    _globalMemoryPool->currentObjCount++;
    
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
void flagMemoryUnoccupied(PDataObject dataObject)
{
    if(!dataObject)
        return;
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
    if(dataObject->retainCount)
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
    dataObject->value = (long)_globalMemoryPool->lastFree;
    _globalMemoryPool->lastFree = (long)dataObject;
    
}

//填写持有者信息
void fillPossessInfo(PossessInfo *possessInfo, char *funcName,pthread_t threadId)
{
    int realLen = 30 * sizeof(char);
    int funcNameLen = strlen(funcName);
    int len = realLen > funcName ? funcName : realLen;
    strlcpy(possessInfo->FunctionName, funcName, len);
    possessInfo->ThreadID = pthread_self();
    
}


PDataObject PBCMalloc()
{
    PDataObject dataObject = PBCAllocatePool();
    if(!dataObject)
    {
        printf("PBCAllocatePool failed!\n");
        return NULL;
    }
    
    dataObject->retainCount = 1;
    dataObject->possessInfoHeader = malloc(sizeof(PossessInfo));
    if(!dataObject->possessInfoHeader)
    {
        printf("malloc possessInfo failed!\n");
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
                if(tmp->buffer)
                {
                    free(tmp->buffer);
                    tmp->buffer = NULL;
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
