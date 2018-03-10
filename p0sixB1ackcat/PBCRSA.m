//
//  RBSafeEncryptRSA.m
//  RBSafeModule
//
//  Created by rongbei-wuxian on 2018/3/7.
//  Copyright © 2018年 rongbei-wuxian. All rights reserved.
//

#import "PBCRSA.h"
#import <openssl/rsa.h>
#import <openssl/pem.h>
#import <openssl/bio.h>
#import <openssl/evp.h>

@implementation PBCRSA

#define HOMEPATH [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject]

#define PRIVATEKEYNAME @"WXSecKeyPriKey.key"
#define PUBLICKEYNAME @"WXPublicKey.key"

#define PRIVATEKEYPATH [[NSString stringWithFormat:@"%@/%@",HOMEPATH,PRIVATEKEYNAME] UTF8String]

#define PUBLICKEYPATH [[NSString stringWithFormat:@"%@/%@",HOMEPATH,PUBLICKEYNAME] UTF8String]


+ (int)initializePrivate:(NSString *)key
{
    NSMutableString * result = [NSMutableString string];
    
    if(key == nil)
        return -1;
    const char * pstr = [key UTF8String];
    
    [result appendString:@"-----BEGIN RSA PRIVATE KEY-----\n"];
    
    int index = 0;
    int count = 0;
    int len = (int)[key length];
    while(index < len)
    {
        char ch = pstr[index];
        if(ch == '\r' || ch == '\n')
        {
            ++index;
            continue;
        }
        [result appendFormat:@"%c",ch];
        
        if(++count == 79)
        {
            [result appendString:@"\n"];
            count = 0;
        }
        index++;
    }
    [result appendString:@"\n-----END RSA PRIVATE KEY-----"];
    
    NSString * keyPath = [HOMEPATH stringByAppendingPathComponent:PRIVATEKEYNAME];
    if([[NSFileManager defaultManager] fileExistsAtPath:keyPath])
    {
        return -1;
    }
    
    char * ptr = (char *)[result UTF8String];
    
    //    for(int i = 0; i < result.length;i++)
    //    {
    //        ptr[i] = ~(ptr[i]);
    //    }
    
    int fd = open([keyPath UTF8String], O_WRONLY | O_CREAT,0677);
    
    if(fd == -1)
    {
        perror("open");
        close(fd);
        return -1;
    }
    
    if(write(fd, ptr, strlen(ptr)) == -1)
    {
        perror("write");
        close(fd);
        return -1;
    }
    
    return 0;
}

+ (int)initializPublicKey:(NSString *)pubKey
{
    NSMutableString * result = [NSMutableString string];
    
    [result appendString:@"-----BEGIN PUBLIC KEY-----\n"];
    
    int index = 0;
    int count = 0;
    while(index < pubKey.length)
    {
        if([pubKey characterAtIndex:index] == '\r' || [pubKey characterAtIndex:index] == '\n')
        {
            continue;
        }
        
        [result appendFormat:@"%c",[pubKey characterAtIndex:index]];
        
        if(++count == 76)
        {
            [result appendString:@"\n"];
            count = 0;
        }
        
        index++;
    }
    
    [result appendString:@"\n-----END PUBLIC KEY-----"];
    
    NSString * pubKeyPath = [HOMEPATH stringByAppendingPathComponent:PUBLICKEYNAME];
    if([[NSFileManager defaultManager] fileExistsAtPath:pubKeyPath])
    {
        return -1;
    }
    
    int fd = open([pubKeyPath UTF8String], O_WRONLY | O_CREAT,0677);
    if(fd == -1)
    {
        perror("open");
        return -1;
    }
    
    unsigned char * inputbuffer = (unsigned char *)[result UTF8String];
    int inputbuffersize = (int)result.length;
    
    if(write(fd, inputbuffer, inputbuffersize) == -1)
    {
        perror("write");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
    
}

+ (NSString *)getPrivateKey
{
    NSString * priKey = [NSString string];
    
    NSString * keyPath = [NSString stringWithFormat:@"%@/%@",HOMEPATH,PRIVATEKEYNAME];
    int fd = open([keyPath UTF8String], O_RDONLY);
    if(fd == -1)
    {
        perror("open");
        close(fd);
        return nil;
    }
    unsigned char loadput = 0;
    while(read(fd, &loadput, 1) > 0)
    {
        priKey = [priKey stringByAppendingFormat:@"%c",loadput];
        
    }
    return priKey;
}


+ (void)removeHomeKeyPath
{
    NSString * keyPath = [NSString stringWithFormat:@"%@/%@",HOMEPATH,PRIVATEKEYNAME];
    NSString * pubkeyPath = [NSString stringWithFormat:@"%@/%@",HOMEPATH,PUBLICKEYNAME];
    unlink([keyPath UTF8String]);
    unlink([pubkeyPath UTF8String]);
}

+ (NSString *)encryptRSA:(NSString *)content
{
    if(!content)
        return nil;
    
    NSMutableString * encrypt = [NSMutableString string];
    NSMutableData *encryptData = [[NSMutableData alloc] init];
    
    FILE * file = fopen(PUBLICKEYPATH, "r");
    if(file == NULL)
    {
        perror("fopen");
        return nil;
    }
    
    RSA * rsa;
    
    rsa = PEM_read_RSA_PUBKEY(file, NULL, NULL, NULL);
    if(rsa == NULL)
    {
        perror("PEM_read_RSAPrivateKey");
        return nil;
    }
    
    unsigned char *inputbuffer = (unsigned char *)content.UTF8String;
    
    unsigned char * outputbuffer = (unsigned char *)malloc(RSA_size(rsa)+1);
    memset(outputbuffer, 0x0, RSA_size(rsa)+1);
    
    size_t maxLen = RSA_size(rsa);//RSA支持的一次加密最大长度
    size_t len = content.length;//源数据的实际长度
    size_t blockcount = (int)ceil(len / (double)maxLen);
    int i = 0;
    int realsize = 0;
    while(i < blockcount)
    {
        realsize = (int)MIN(maxLen, len - i * maxLen);
        if(RSA_public_encrypt(realsize,(unsigned const char *)inputbuffer,outputbuffer,rsa,RSA_PKCS1_PADDING) == -1)
        {
            perror("RSA_private_encrypt");
            encrypt = nil;
            goto ret;
        }
        
        NSData *data = [NSData dataWithBytes:outputbuffer length:maxLen];
        [encryptData appendData:data];
        
        inputbuffer += maxLen;
        i++;
    }
    
    encrypt = [NSMutableString stringWithCString:[[encryptData base64EncodedDataWithOptions:0] bytes] encoding:NSUTF8StringEncoding];
    
    
//    for(int i = 0;i < RSA_size(rsa);i++)
//    {
//        [encrypt appendFormat:@"%02X",outputbuffer[i]];
//    }
    
ret:
    free(outputbuffer);
    outputbuffer = NULL;
    fclose(file);
    
    return encrypt;
}

+ (NSString *)decryptRSA:(NSString *)encrypt
{
    if(!encrypt)
        return nil;
    
    NSMutableString * decrypt = [NSMutableString string];
    
    FILE * file = fopen(PRIVATEKEYPATH, "r");
    
    unsigned char *inputbuffer = NULL;
    
    NSData *inputdata = [[NSData alloc] initWithBase64EncodedString:encrypt options:NSDataBase64DecodingIgnoreUnknownCharacters];
    
    size_t inputsize = [inputdata length];
    
    inputbuffer = (unsigned char *)[inputdata bytes];
    
    if(!inputbuffer)
    {
        perror("base64 decrypt faild");
        return nil;
    }
    if(file == NULL)
    {
        perror("fopen");
        return nil;
    }
    
    RSA * rsa = NULL;
    
    rsa = PEM_read_RSAPrivateKey(file,NULL,NULL,NULL);
    if(rsa == NULL)
    {
        perror("PEM_read_RSA_PUBKEY");
        fclose(file);
        return nil;
    }
    
    int maxlen = RSA_size(rsa);
    int len = (int)inputsize;
    int blockcount = (int)ceil(len / (double)maxlen);
    int i = 0;
    unsigned char *outputbuffer = (unsigned char *)malloc(maxlen + 1);
    unsigned char *p = inputbuffer;
    int realsize = 0;
    while(i < blockcount)
    {
        realsize = MIN(maxlen, len - i * maxlen);
        int ret = RSA_private_decrypt(realsize,
                                     p,
                                     outputbuffer,
                                     rsa,
                                     RSA_NO_PADDING);
        if(ret== -1)
        {
            encrypt = nil;
            goto ret;
        }
        
        for(int index = 0; index < realsize; index++)
        {
            [decrypt appendFormat:@"%c",outputbuffer[index]];
        }
        
        p += maxlen + 1;
        i++;
    }
    
ret:
    free(outputbuffer);
    outputbuffer = NULL;
    fclose(file);
    
    return decrypt;
}

#ifdef RSA_HEX
static void hextochar(const char * input,unsigned char * output,int size,int base)
{
    while(isxdigit(*input) && size--)
    {
        *output++ = base * (isxdigit(*input) ? *input++ - '0' : tolower(*input++) - 'a' + 10) + (isxdigit(*input) ? *input++ - '0' : tolower(*input++) - 'a' + 10);
    }
}
#endif

@end
