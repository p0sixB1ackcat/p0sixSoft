//
//  WXSQLiteDatabase.m
//  poinner
//
//  Created by WuXian on 16/7/7.
//  Copyright © 2016年 QianEn payment technology co., LTD. All rights reserved.
//

#import "WXSQLiteDatabase.h"
#import <sqlite3.h>

@implementation WXSQLiteDatabase

#if TARGET_IPHONE_SIMULATOR

#define FILEPATH [@"/Users/wuxian/Desktop"stringByAppendingPathComponent: @"YXDATABASE.sqlite"]

#else

#define FILEPATH [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject] stringByAppendingPathComponent:@"YXDATABASE.sqlite"]

#endif

static sqlite3 * _db;

static NSString * _tableName = @"USERDATA";

- (instancetype)initWithTableName:(NSString *)tableName
{
    if(tableName && tableName.length > 0)
    {
        _tableName = tableName;
    }
    return [self init];
}

- (instancetype)init
{
    if(self == [super init])
    {
        int result = sqlite3_open([FILEPATH UTF8String], &_db);
        
        if(result != SQLITE_OK)
        {
            NSLog(@"%s",sqlite3_errmsg(_db));
        }
        
        NSString * sqlstr = [NSString stringWithFormat:@"create table if not exists %@ (id integer primary key autoincrement, key, value)",_tableName];
        
        const char * sql = [sqlstr UTF8String];
        
        char * error;
        
        result = sqlite3_exec(_db, sql, NULL, NULL, &error);
        
        if(result != SQLITE_OK)
        {
            sqlite3_errmsg(_db);
            sqlite3_close(_db);
            
        }
        
        sqlite3_close(_db);
        
    }
    return self;
}

/**
  插入一条数据，当key存在时，先删除，在更新
 */
- (void)addDataKey:(NSString *)key value:(NSString *)value
{
    NSString * checkKeyStr = [self queryDataKey:key];
    //如果当前数据存在，先删除，在更新
    if(checkKeyStr.length>0)
    {
        if(![self removeDataKey:key])
        {
            NSLog(@"删除失败");
            sqlite3_close(_db);
            return;
        }
        
    }
    
    int result = sqlite3_open([FILEPATH UTF8String], &_db);
    if(result != SQLITE_OK)
    {
        NSLog(@"%s",sqlite3_errmsg(_db));
        sqlite3_close(_db);
        return;
    }
    
    NSString * sql = [NSString stringWithFormat:@"insert into %@ (key,value) values('%@','%@')",_tableName,key,value];
    result = sqlite3_exec(_db, [sql UTF8String], NULL, NULL, NULL);
    if(result != SQLITE_OK)
    {
        NSLog(@"%s",sqlite3_errmsg(_db));
        sqlite3_close(_db);
        return;
    }
    
}

/**
 更改数据
 */
- (void)changeDataKey:(NSString *)key value:(NSString *)value
{

    if(sqlite3_open([FILEPATH UTF8String], &_db) != SQLITE_OK)
    {
        perror("qlite_open");
        return;
    }
    
    NSString * sql = @"select key from USERDATA";
    
    sqlite3_stmt * stmt;
    
    if(sqlite3_prepare_v2(_db, [sql UTF8String], -1, &stmt, NULL) == SQLITE_OK)
    {
        while(sqlite3_step(stmt) == SQLITE_ROW)
        {
            NSLog(@"0 is %s",sqlite3_column_text(stmt, 0));
            if([key isEqualToString:
                [NSString stringWithCString:(const char *)sqlite3_column_text(stmt, 0) encoding:NSUTF8StringEncoding]])
            {
                NSString * updatesql = [NSString stringWithFormat:@"update %@ set value = '%@' where key = '%@'",_tableName,value,key];
                
                if(sqlite3_exec(_db, [updatesql UTF8String], NULL, NULL, NULL) != SQLITE_OK)
                {
                    NSLog(@"%s",sqlite3_errmsg(_db));
                }
                
            }
            
        }
    }
    else
    {
        NSLog(@"%s",sqlite3_errmsg(_db));
    }
    sqlite3_close(_db);
}

/**
 查询数据
 */
- (NSString *)queryDataKey:(NSString *)key
{
    if(sqlite3_open([FILEPATH UTF8String], &_db) != SQLITE_OK)
    {
        sqlite3_errmsg(_db);
        sqlite3_close(_db);
        return @"";
    }
    
    NSString * sql = [NSString stringWithFormat:@"select * from %@ where key='%@' order by value",_tableName,key];
    
    sqlite3_stmt * pstmt;
    if(sqlite3_prepare_v2(_db, [sql UTF8String], -1, &pstmt, NULL)
        != SQLITE_OK)
    {
        NSLog(@"%s",sqlite3_errmsg(_db));
        sqlite3_close(_db);
        return @"";
    }
    
    NSString * value = @"";
    
    while(sqlite3_step(pstmt) == SQLITE_ROW)
    {
        NSString * currentKey = [NSString stringWithCString:(const char *)sqlite3_column_text(pstmt, 0) encoding:NSUTF8StringEncoding];
        NSLog(@"0 is %@",currentKey);
        NSLog(@"1 is %@", [NSString stringWithFormat:@"%s",sqlite3_column_text(pstmt, 1)]);
        NSLog(@"2 is %@", [NSString stringWithFormat:@"%s",sqlite3_column_text(pstmt, 2)]);
        value = [NSString stringWithFormat:@"%s",sqlite3_column_text(pstmt, 2)];
    }
    sqlite3_close(_db);
    return value;
}

/**
 删除一条数据
 */
- (BOOL)removeDataKey:(NSString *)key
{
    if(sqlite3_open([FILEPATH UTF8String], &_db) != SQLITE_OK)
    {
        NSLog(@"%s",sqlite3_errmsg(_db));
        
        return NO;
    }
    
    NSString * sql = [NSString stringWithFormat:@"delete from %@ where key = '%@'",_tableName,key];
    
    int result = sqlite3_exec(_db, [sql UTF8String], NULL, NULL, NULL);
    if(result != SQLITE_OK)
    {
        NSLog(@"%s",sqlite3_errmsg(_db));
        sqlite3_close(_db);
        return NO;
    }
    
    sqlite3_close(_db);
    return YES;
}


@end
