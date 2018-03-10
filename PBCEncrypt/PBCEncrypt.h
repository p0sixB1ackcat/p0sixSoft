//
//  PBCEncrypt.h
//  PBCEncrypt
//
//  Created by p0sixB1ackcat on 2018/3/10.
//  Copyright © 2018年 p0sixB1ackcat. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PBCEncrypt : NSObject

+ (NSString *)RSAEncryptStr:(NSString *)source key:(NSString *)key;

+ (NSString *)RSADecryptStr:(NSString *)source key:(NSString *)key;

@end
