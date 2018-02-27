//
//  wxcrypt.h
//  WXStringLen
//
//  Created by WuXian on 16/5/4.
//  Copyright © 2016年 WuXian. All rights reserved.
//

#ifndef wxcrypt_h
#define wxcrypt_h

#define CRC32(c,b) ((*(pcrc_32_tab+(((int)(c) ^ (b)) & 0xff))) ^ ((c) >> 8))

static int update_keys(unsigned long * pkeys,const unsigned long * pcrc_32_tab,int c)
{
    (*(pkeys+0)) = CRC32((*(pkeys+0)),c);
    (*(pkeys+1)) += (*(pkeys+0)) & 0xff;
    (*(pkeys+1)) = (*(pkeys+1)) * 134775813L + 1;
    {
        register int keyshift = (int)((*(pkeys+1)) >> 24);
        (*(pkeys+2)) = CRC32((*(pkeys+2)),keyshift);
    }
    return c;
    
}


static void init_keys(const char * passwd,unsigned long * pkeys,const unsigned long * pcrc_32_tab)
{
    *(pkeys+0) = 305419896L;
    *(pkeys+1) = 591751049L;
    *(pkeys+2) = 878082192L;
    while (*passwd != '\0') {
        update_keys(pkeys,pcrc_32_tab,(int *)passwd);
        passwd++;
    }
}

static int decrypt_byte(unsigned long * pkeys,const unsigned long *pcrc_32_tab)
{
    unsigned temp;
    temp = ((unsigned)(*(pkeys + 2)) & 0xffff) | 2;
    return (int)(((temp * (temp ^ 1)) >> 8) & 0xff);
    
}

#define zdecode(pkeys,pcrc_32_tab,c) \
    (update_keys(pkeys,pcrc_32_tab,c ^= decrypt_byte(pkeys,pcrc_32_tab)))




#endif /* wxcrypt_h */
