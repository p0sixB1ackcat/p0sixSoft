//
//  PBCEncrypt.m
//  PBCEncrypt
//
//  Created by p0sixB1ackcat on 2018/3/10.
//  Copyright © 2018年 p0sixB1ackcat. All rights reserved.
//

#import "PBCEncrypt.h"
#import "PBCRSA.h"

#define RSAPublicKey @"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDMp5gplfi930PQwVb8+iQAtBSqPsszgiqbAPb8bH+ByulVJnxw8LslSUOxHhnyBhFc2eFmZ49xLFg+odRve4mynhZsMqknQgR3uiwnCKueeX+LJyZK7IswHGUyXU5xUHq2l2oKeEgtxh8aKJ+HwEYthOzLheRuH4p5fpg4rWFqiwIDAQAB"

#define RSAPrivateKey @"MIICXAIBAAKBgQDMp5gplfi930PQwVb8+iQAtBSqPsszgiqbAPb8bH+ByulVJnxw8LslSUOxHhnyBhFc2eFmZ49xLFg+odRve4mynhZsMqknQgR3uiwnCKueeX+LJyZK7IswHGUyXU5xUHq2l2oKeEgtxh8aKJ+HwEYthOzLheRuH4p5fpg4rWFqiwIDAQABAoGBAKrR4LIlWb+yEXq3RhJV/6MOrIkFbLqLlWBXx9AeB7egVG4cd5YVeT/ADV5TD7jx06A6xGajXhlKuEVe8mAzL1SHambDoxnDYI/JmXwPnTJYJ1aPPYOvhtMj56L4wMO2TDLq9Abvzh7gcgwv1GHMphHaKdcAtX1/Pfz+rA/OJ2lRAkEA6cwauFO7fERwns8ySU3RTY9nvKcr3ABqsRQDX/Q9l7n6Lt5oxrMpu8ZxeCD4qQJt9bFXRhiJr2ZebkjCnXOwtQJBAOAW/2Rnf2EHGcUDUZxAfJsAwg14GN93Wxaipa30UYJMRo2/CUK9BDJm5s+fVrZCn20bm0lQJTkiyO2ny/mP9j8CQA0RQmERFQaSELVfoVDlj5BFNqgds97dCxmQo8umLBch5vDdr3+Dng6HvenX9e4EW78CxXz+fBuYniiN2orQX40CQBTfD3pyiVKoLqZC4N8EwpZNQ8WtYeOuYjWBgJzrHIxePlQFbv9gS3vGSckcth7TpbWr1JLHfDAOJVi9QCpGVlECQHqksB3nXlWFC+AEr9nFTZRj4TaYrIMWeutOwTvCMTkcB7/vvahB2mll+xHyFjC+HMv9psLOsUGjUym5YBtb/ks="

@implementation PBCEncrypt

+ (NSString *)RSAEncryptStr:(NSString *)source key:(NSString *)key
{
    if(!source)
        return nil;
    
    if(!key)
        key = RSAPublicKey;
    
    [PBCRSA initializPublicKey:key];
    
    NSString *enc = [PBCRSA encryptRSA:source];
    
    return enc;
}

+ (NSString *)RSADecryptStr:(NSString *)source key:(NSString *)key
{
    if(!source)
        return nil;
    
    if(!key)
        key = RSAPrivateKey;
    
    [PBCRSA initializePrivate:key];
    
    NSString *dec = [PBCRSA decryptRSA:source];
    
    return dec;
}

@end
