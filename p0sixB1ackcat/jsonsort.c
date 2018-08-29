//
//  jsonsort.c
//  OpenChainModule
//
//  Created by p0sixB1ackcat on 2018/8/28.
//  Copyright © 2018年 p0sixB1ackcat. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _ListEntry
{
    char *key;
    char *value;
    long klen;
    long vlen;
    struct _ListEntry *next;
    
}ListEntry,*PListEntry;

static ListEntry *header = NULL;

static void InsertLinkList(ListEntry *le)
{
    if(!header)
    {
        header = le;
    }
    else
    {
        le->next = header;
        header = le;
    }
}

void emptyLinkList(void)
{
    ListEntry *p = header;
    while(p)
    {
        ListEntry *q = p;
        p = p->next;
        if(q->key)
        {
            free(q->key);
            q->key = NULL;
        }
        if(q->value)
        {
            free(q->value);
            q->value = NULL;
        }
        free(q);
        q = NULL;
    }
}

void traver(void)
{
    ListEntry *p = header;
    while(p)
    {
        printf("p->key is %s,p->value is %s\n",p->key,p->value);
        p = p->next;
    }
}

int compstr(ListEntry *p,ListEntry *q)
{
    if(!p || !q)
        return 0;
    char *str1,*str2;
    str1 = p->key;
    str2 = q->key;
    
    while(*str1 != '\0' && *str2 != '\0')
    {
        if(*str1 > *str2)
        {
            return 1;
        }
        else if (*str1 < *str2)
        {
            return 0;
        }
        else
        {
            ++str1;
            ++str2;
        }
    }
    
    return strlen(p->key) - strlen(q->key) > 0 ? 0 : 1;
}

void sortlist(void)
{
    ListEntry *p,*q,*h;
    p = header;
    while(p->next)
    {
        h = p;
        q = p->next;
        
        while(q)
        {
            if(compstr(h, q))
                h = q;
            q = q->next;
        }
        if(h != p)
        {
            p->key = (char *)((long)p->key ^ (long)h->key);
            h->key=  (char *)((long)p->key ^ (long)h->key);
            p->key = (char *)((long)p->key ^ (long)h->key);
            
            p->value = (char *)((long)p->value ^ (long)h->value);
            h->value = (char *)((long)p->value ^ (long)h->value);
            p->value = (char *)((long)p->value ^ (long)h->value);
            
            p->klen = (long)p->klen ^ (long)h->klen;
            h->klen = (long)p->klen ^ (long)h->klen;
            p->klen = (long)p->klen ^ (long)h->klen;
            
            p->vlen = (long)p->vlen ^ (long)h->vlen;
            h->vlen = (long)p->vlen ^ (long)h->vlen;
            p->vlen = (long)p->vlen ^ (long)h->vlen;
        }
        p = p->next;
    }
    return;
}

char *formatListEntryToJson(void)
{
    ListEntry *p = header;
    int totallen = 0;
    char *buffer = NULL;
loop:
    while(p)
    {
        if(!buffer)
        {
            totallen += p->klen; //"\"key\":\"value\",\"key\":\"value\""
            totallen += p->vlen;
            totallen += 6;
            p = p->next;
        }
        else
        {
            size_t keylen = p->klen + 3;
            size_t valuelen = p->vlen + 3;
            char *str = (char *)malloc(keylen + valuelen);
            
            snprintf(str, keylen + valuelen + 1, "\"%s\":\"%s\",",p->key,p->value);
            strcat(buffer, str);
            free(str);
            str = NULL;
            p = p->next;
        }
        
    }
next:
    if(!buffer)
    {
        totallen -= sizeof(char) * 1; //last ','
        totallen += sizeof(char) * 2; //{}
        buffer = (char *)malloc(totallen + 2);
        buffer[0] = '{';
        p = header;
        goto loop;
    }
    buffer[totallen - 1] = '}';
    
    return buffer;
}

void pullAwayJsonToListEntry(unsigned char *json)
{
    char *p,*q;
    p = q = ((char *)json + 1);
    int pindex = 0,qindex = 0;
    int before = 0;
    while(p[pindex] != '\0')
    {
        if(p[pindex] == ',' || p[pindex] == '}')
        {
            ListEntry *le = (ListEntry *)malloc(sizeof(ListEntry));
            memset(le, 0x00, sizeof(ListEntry));
            before = qindex;
            while(qindex < pindex)
            {
                if(q[qindex] == ':')
                {
                    int ksize = sizeof(char) * (qindex - 1 - before - 1);
                    le->key = (char *)malloc(ksize);
                    memset(le->key, 0x00, ksize);
                    le->klen = ksize;
                    for(int i = 0; i < ksize; i++)
                    {
                        *(le->key + i) = q[before + 1 + i];
                    }
                    *(le->key + le->klen) = '\0';
                    int vsize = sizeof(char) * (pindex - 1 - qindex - 1 - 1);
                    le->value = (char *)malloc(vsize);
                    memset(le->value, 0x00, vsize);
                    le->vlen = vsize;
                    for(int i = 0; i < vsize; i++)
                    {
                        *(le->value + i) = q[qindex + 2 + i];
                    }
                    *(le->value + le->vlen) = '\0';
                    
                }
                ++qindex;
            }
            
            InsertLinkList(le);
            
            qindex = pindex + 1;
        }
        ++pindex;
    }
    
}
