//
//  showbits.h
//  WXStringLen
//
//  Created by WuXian on 16/1/21.
//  Copyright © 2016年 WuXian. All rights reserved.
//

#ifndef showbits_h
#define showbits_h

#define BYTESIZE sizeof(int)
#define BITSIZE 8

typedef struct{
    
    char showb_t_list[BYTESIZE][BITSIZE];
    int showb_t_bytesize;
    int showb_t_bitsize;
    
}showb,*pshowb;


showb * showbites(int);


#endif /* showbits_h */
