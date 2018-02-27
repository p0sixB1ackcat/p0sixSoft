//
//  WXUtils.h
//  WXStringLen
//
//  Created by WuXian on 16/6/21.
//  Copyright © 2016年 WuXian. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum {
    
    WXSortFlagAscending = 0,//升序
    WXSortFlagDescendingOrder //降序
    
}WXSortFlag;

@interface WXUtils : NSObject

+ (void)strsort:(id)list flag:(WXSortFlag)flag;

@end
