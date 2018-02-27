//
//  wxioapi.h
//  WXStringLen
//
//  Created by WuXian on 16/4/27.
//  Copyright © 2016年 WuXian. All rights reserved.
//

#ifndef wxioapi_h
#define wxioapi_h

#include <stdio.h>
#include <zconf.h>

#define USE_FILE32API
#if defined(USE_FILE32API)
#define fopen64 fopen
#define ftello64 ftell
#define fseeko64 fseek
#else
#ifdef _MSC_VER
 #define fopen64 fopen
 #if (_MSC_VER >= 1400) && (!(defined(NO_MSCVER_FILE64_FUNC)))
  #define ftello64 _ftelli64
  #define fseeko64 _fseeki64
 #else
  #define ftello64 ftell
  #define fseeko64 fseek
 #endif
#endif
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 ZPOS64_T;
#else
typedef unsigned long long int ZPOS64_T;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ZLIB_FILEFUNC_SEEK_CUR (1)
#define ZLIB_FILEFUNC_SEEK_END (2)
#define ZLIB_FILEFUNC_SEEK_SET (0)
    
#define ZLIB_FILEFUNC_MODE_READ            (1)
#define ZLIB_FILEFUNC_MODE_WRITE           (2)
#define ZLIB_FILEFUNC_MODE_READWRITEFILTER (3)
    
#define ZLIB_FILEFUNC_MODE_EXISTING        (4)
#define ZLIB_FILEFUNC_MODE_CREATE          (8)
    
    
    
#ifndef ZCALLBACK
#if (defined(WIN32) || defined(_WIN32) || defined(WINDOWS) || defined(_WINDOWS)) && defined(CALLBACK) && defined (USEWINDOWS_CALLBACK)
#define ZCALLBACK CALLBACK
#else
#define ZCALLBACK
#endif
#endif
    
typedef voidpf (*open_file_func)        OF((voidpf opaque,const char * filename,int mode));


typedef uLong (*read_file_func)         OF((voidpf opaque,voidpf stream,void * buf,uLong size));

typedef uLong (*write_file_func)        OF((voidpf opaque,voidpf stream,const void * buf,uLong size));

typedef int (*close_file_func)          OF((voidpf qpaque,voidpf stream));

typedef int (*testerror_file_func)      OF((voidpf opaque,voidpf stream));

typedef long (*tell_file_func)          OF((voidpf opaque,voidpf stream));

typedef long (*seek_file_func)          OF((voidpf opaque,voidpf stream,uLong offset,int origin));

/*这是在32位架构CPU上使用的*/
typedef struct zlib_filefunc_def_s
{
    open_file_func      zopen_file;
    read_file_func      zread_file;
    write_file_func     zwrite_file;
    tell_file_func      ztell_file;
    seek_file_func      zseek_file;
    close_file_func     zclose_file;
    testerror_file_func zerror_file;
    voidpf              opaque;
    
} zlib_filefunc_def;

typedef voidpf (* open64_file_func)     OF((voidpf opaque,const void * filename,int mode));

typedef ZPOS64_T (*tell64_file_func)    OF((voidpf opaque,voidpf stream));

typedef long (*seek64_file_func)        OF((voidpf opaque,voidpf stream,ZPOS64_T offset,int origin));



typedef struct zlib_filefunc64_def_s
{
    open64_file_func    zopen64_file;
    read_file_func      zread_file;
    write_file_func     zwrite_file;
    tell64_file_func    ztell64_file;
    seek64_file_func    zseek64_file;
    close_file_func     zclose_file;
    testerror_file_func zerror_file;
    voidpf              opaque;
    
}zlib_filefunc64_def;

/**
 *  函数结构，用于处理一些操作
 */
typedef struct zlib_filefunc64_32_def_s
{
    zlib_filefunc64_def zfile_func64;
    open_file_func      zopen32_file;
    tell_file_func      ztell32_file;
    seek_file_func      zseek32_file;
    
}zlib_filefunc64_32_def;
    
void fill_fopen64_filefunc          OF((zlib_filefunc64_def * pzlib_filefunc_def));

voidpf call_zopen64  OF((const zlib_filefunc64_32_def * pfilefunc,const void * filename,int mode));

ZPOS64_T call_ztell64   OF((const zlib_filefunc64_32_def * pfilefunc,voidpf filestream));
    
long call_zseek64       OF((const zlib_filefunc64_32_def * pfilefunc,voidpf filestream,ZPOS64_T offset,int origin));

#define ZREAD64(filefunc,filestream,buf,size)       ((*((filefunc).zfile_func64.zread_file))  ((filefunc).zfile_func64.opaque,filestream,buf,size))


#define ZCLOSE64(filefunc,filestream)               ((*((filefunc).zfile_func64.zclose_file))    ((filefunc).zfile_func64.opaque,filestream))
    
    
#define ZERROR64(filefunc,filestream)               ((*(filefunc).zfile_func64.zclose_file)) ((filefunc).zfile_func64.opaque,filestream))
    
#define ZOPEN64(filefunc,filename,mode)             (call_zopen64((&(filefunc)),(filename),(mode)))

#define ZTELL64(filefunc,filestream)                (call_ztell64((&(filefunc)),(filestream)))
    
#define ZSEEK64(filefunc,filestream,pos,mode)           (call_zseek64((&(filefunc)),(filestream),(pos),(mode)))
    
#ifdef __cplusplus
}
#endif
















#endif /* wxioapi_h */
