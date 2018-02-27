//
//  WXUtils.m
//  WXStringLen
//
//  Created by WuXian on 16/6/21.
//  Copyright © 2016年 WuXian. All rights reserved.
//

#import "WXUtils.h"

@implementation WXUtils


+ (WXUtils *)SharedInstance
{
    static dispatch_once_t  t;
    static WXUtils * _selfClass;
    
    dispatch_once(&t, ^{
       
        _selfClass = [[WXUtils alloc] init];
        
    });
    return _selfClass;
}

- (instancetype)init
{
    if (self == [super init])
    {
        
    }
    return self;
}

+ (void)strsort:(id)list flag:(WXSortFlag)flag
{
    NSMutableArray * arr = (NSMutableArray *)list;
    if (arr == nil || arr.count == 0)
    {
        
        return;
    }
    
    //获取数组元素个数
    int count = (int)arr.count;
    
    //冒泡排序
    //外层循环
    for (int i = 0; i < count-1; i++) {
        
        if (!([arr[i] isKindOfClass:[NSString class]]))
        {
            return;
        }
        
        //升序，内层循环，比较当前关键值的右面数据
        for (int j = 0; j < count -1-i; j++)
        {
            if (!([arr[j] isKindOfClass:[NSString class]])) {
                continue;
            }
            
            NSString * str1 = arr[j];
            NSString * str2 = arr[j+1];
            
            //比较两个字符串，从前往后一字节一字节比较，比较长度取两个字符串中长度较小的那个就好，如果长度长的包含这个小的，则长的字符串大
            NSInteger compareLen = str1.length > str2.length ? str2.length : str1.length;
            //NSString * temp = @"";
            for (int z = 0; z < compareLen; z++) {
                
                char c1 = [str1 characterAtIndex:z];
                char c2 = [str2 characterAtIndex:z];
                
                if (c1 == c2) {
                    continue;
                }
                
                
                if (c1 > c2) {
                            
                    [arr replaceObjectAtIndex:j withObject:str2];
                    [arr replaceObjectAtIndex:j+1 withObject:str1];
                            
                    break;
                            
                }else {
                            
                    break;
                }

            }

        if (compareLen == str2.length && [(str1 = [str1 substringWithRange:NSMakeRange(0, compareLen)]) isEqualToString:str2]) {
                    
            [arr replaceObjectAtIndex:j withObject:str2];
            [arr replaceObjectAtIndex:j+1 withObject:str1];
                    
        }
        
        
      }
        
        
    }
    
}


@end
