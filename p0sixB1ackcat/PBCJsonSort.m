//
//  PBCJsonSort.m
//  p0sixB1ackcat
//
//  Created by p0sixB1ackcat on 2018/8/28.
//  Copyright © 2018年 p0sixB1ackcat. All rights reserved.
//

#import "PBCJsonSort.h"
#import "jsonsort.h"

@implementation PBCJsonSort

+ (NSString *)JsonSort:(NSString *)jsonStr
{
    pullAwayJsonToListEntry((unsigned char *)jsonStr.UTF8String);
    
    traver();
    
    sortlist();
    char *buffer = formatListEntryToJson();
    
    NSString *newJson = @"";
    for(int i = 0; i < strlen(buffer); i++)
    {
        newJson = [newJson stringByAppendingFormat:@"%c",buffer[i]];
    }
    
    
    emptyLinkList();
    free(buffer);
    buffer = NULL;
    return newJson;
}

@end
