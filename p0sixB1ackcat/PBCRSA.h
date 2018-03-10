//
//  RBSafeEncryptRSA.h
//  RBSafeModule
//
//  Created by rongbei-wuxian on 2018/3/7.
//  Copyright © 2018年 rongbei-wuxian. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PBCRSA : NSObject

+ (int)initializePrivate:(NSString *)key;

+ (int)initializPublicKey:(NSString *)pubKey;

+ (NSString *)getPrivateKey;

+ (NSString *)encryptRSA:(NSString *)content;

+ (NSString *)decryptRSA:(NSString *)encrypt;

+ (void)removeHomeKeyPath;

@end
