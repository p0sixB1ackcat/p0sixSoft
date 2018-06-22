#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _InfoEntry
{
    
    struct _InfoEntry *next;
    void *buffer;
    
}InfoEntry,*PInfoEntry;


typedef struct _MemoryPool
{
    
    PInfoEntry header;
    PInfoEntry tail;
    int currentObjCount;   //当前使用内存块的数量
    int maxObjCount;       //最大内容容量，要DataObject的长度相乘
    int objSize;            //单个对象的大小
    long lastFree;
}MemoryPool,*PMemoryPool;

typedef struct _DataObject
{
    long value;     //这里只使用value，保存指针用的
    
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
        //下一层寻址，找到该指针之前释放的内存地址
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

//将lastFree指针写入到dataObject->value域，并将lastFree重新指向当前的空闲内存地址
void PBCIdleMemory(PDataObject dataObject)
{
    if(!dataObject)
        return;
    
    dataObject->value = (long)_globalMemoryPool->lastFree;
    _globalMemoryPool->lastFree = (long)dataObject;
    
}

void PBCFree(void)
{
    
}
