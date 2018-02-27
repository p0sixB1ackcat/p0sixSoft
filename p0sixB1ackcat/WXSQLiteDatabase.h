//
//  WXSQLiteDatabase.h
//  poinner
//
//  Created by WuXian on 16/7/7.
//  Copyright © 2016年 QianEn payment technology co., LTD. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface WXSQLiteDatabase : NSObject

/**
 插入一条数据，当key存在时，先删除，在更新
 */
- (void)addDataKey:(NSString *)key value:(NSString *)value;

/**
 删除一条数据
 */
- (BOOL)removeDataKey:(NSString *)key;

/**
 更改数据
 */
- (void)changeDataKey:(NSString *)key value:(NSString *)value;

/**
 查询数据
 */
- (NSString *)queryDataKey:(NSString *)key;

@end
