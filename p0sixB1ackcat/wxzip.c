//
//  wxzip.c
//  WXStringLen
//
//  Created by WuXian on 16/4/27.
//  Copyright © 2016年 WuXian. All rights reserved.
//

#include "wxzip.h"
//#include <zconf.h>该头文件不能再下面的STDC里面导入，因为。。。你可以点STDC进去看一看

#ifdef STDC

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#endif

#ifdef NO_ERRNO_H
extern int errno;
#else
#include <errno.h>
#endif

#ifndef local
#define local static
#endif

#ifndef UNZ_BUFSIZE
#define UNZ_BUFSIZE (16384)
#endif


#ifndef ALLOC
#define ALLOC(size) (malloc(size))
#endif
#ifndef TRYFREE
#define TRYFREE(p) {if (p) free(p);}
#endif

#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)

const char unz_copyright[] =                                                        " unzip 1.01 Copyright 1998-2004 Gilles Vollant - http://www.winimage.com/zLibDll";

/*
    包含这个zip文件中的一个文件的内部信息
 */
typedef struct unz_file_info64_internal_s
{
    ZPOS64_T offset_curfile;
    
} unz_file_info64_internal;

/*
    当读取或解压的时候，包含这个zip文件中的一个子文件的内部信息
 */
typedef struct {
    
    char * read_buffer;         //被解压数据的缓冲区
    
    z_stream stream;
    
#ifdef HAVE_BZIP2
    bz_stream bstream;
#endif
    ZPOS64_T pos_in_zipfile;    //为fseek函数定位zip的文件指针的位置
    
    uLong stream_initialised;   //初始化结构时设置的flag
    
    ZPOS64_T offset_local_extrafield;   //临时字段,偏移量
    
    uInt size_local_extrafield;         //临时字段，大小
    
    ZPOS64_T pos_local_extrafield;      //在读取时本地临时字段的位置
    
    ZPOS64_T total_out_64;
    
    uLong crc32;
    
    uLong crc32_wait;            //在crc32之后，我们必须获得所有解压的文件
    
    ZPOS64_T rest_read_compressed;  //得到压缩之后的字节编号
    
    ZPOS64_T rest_read_uncompressed;    //压缩之前的字节编号
    
    zlib_filefunc64_32_def z_filefunc;
    
    voidpf filestream;          //这个zip文件的io结构
    
    uLong compression_method;   //压缩方法(0 == store)
    
    ZPOS64_T byte_before_the_zipfile;   //代表这个zip文件之前的字节
    
    int raw;
    
} file_in_zip64_read_info_s;

//unz64_s结构体包含这个zip文件的内部信息
typedef struct
{
    zlib_filefunc64_32_def z_filefunc;
    
    int is64bitOpenFunction;
    
    voidpf filestream;          //zip文件的io结构
    
    unz_global_info64 gi;       //公有的全局信息
    
    ZPOS64_T byte_before_the_zipfile;   //得到这个zip文件之前的字节
    
    ZPOS64_T num_file;          //当前文件在zip文件里的编号
    
    ZPOS64_T pos_in_central_dir;        //当前文件在他所在的文件夹的位置
    
    ZPOS64_T current_file_ok;   //表示当前文件是合法的
    
    ZPOS64_T central_pos;       //当前文件在他所在目录中央的起点位置
    
    ZPOS64_T size_central_dir;  //目录中央的大小
    
    ZPOS64_T offset_central_dir;//在目录中央的偏移量，开始位置在所在的磁盘上
    
    unz_file_info64 cur_file_info;//关于当前文件的公有信息
    
    unz_file_info64_internal cur_file_info_internal;//关于他的私有信息
    
    file_in_zip64_read_info_s * pfile_in_zip_read;  //关于我们要解压的一个文件的结构信息
    int encrypted;
    
    int isZip64;
    
#ifndef NOUNCRYPT
    unsigned long keys[3]; //keys表示伪随机数的顺序
    const unsigned long * pcrc_32_tab;
#endif
    
    
}unz64_s;

#ifndef NOUNCRYPT
#include "wxcrypt.h"
#endif

#ifndef BUFERADCOMMENT
#define BUFERADCOMMENT (0x400)//1024
#endif

/**
 *  内部函数，读取文件流中1个元素
 *
 *  @param pzlib_filefunc_def zlib_filefunc64_32_def结构，调度函数指针用的
 *  @param filestream         目标文件流
 *  @param pi                 将读取到的二进制转换成int
 *
 *  @return UNZ_OK表示成功，否则失败。
 */
local int unz64local_getByte        OF((const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream,int * pi));


local int unz64local_getByte (const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream,int * pi)
{
    unsigned char c;
    //从filestream中读取1个元素放到c里面
    int err = (int)ZREAD64(*pzlib_filefunc_def, filestream, &c, 1);
    
    if (err == 1)//如果读取成功
    {
        *pi = (int)c;
        return UNZ_OK;
    }
    
    else//否则关闭文件并返回错误信息
    {
        if (ZERROR64(*pzlib_filefunc_def, filestream)
        
            return UNZ_ERRNO;
        
        else
            
            return UNZ_EOF;
            
        
    }
    
}

/**
 *  内部函数，读取文件流2个字节
 *
 *  @param pzlib_filefunc_def zlib_filefunc64_64_32_def结构
 *  @param filestream         文件流
 *  @param pX                 保存读取
 *
 *  @return 成功返回UNZ_OK。
 */
local int unz64local_getShort   OF((const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream,uLong * pX));

local int unz64local_getShort (const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream,uLong * pX)
{
    
    uLong x;
    int i = 0;
    int err;
    
    err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x = (uLong)i;//将第一次读取的int类型二进制保存到x中
    
    if(err == UNZ_OK){
        //如果第一次读取成功,继续从当前偏移量读取下一字节
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    }
    
    //x=x和第二次读取的字节的上一字节进行按位或，如果两个数相同，x不变
    x |= ((uLong)i) << 8;
    
    if(err == UNZ_OK){
        
        *pX = x;//如果第二次读取成功,赋值
        
    }else{
        
        *pX = 0;//否则置为0
    }
    return err;
    
}
/*
    内部函数，获取目标文件流4个字节的二进制位
 */
local int unz64local_getLong        OF((const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream,uLong * pX));

local int unz64local_getLong (const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream,uLong * pX)
{
    uLong x;
    int i = 0;
    int err;
    
    err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x = (uLong)i;
    
    if(err == UNZ_OK)
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x |= ((uLong)i) <<8;
    
    if(err == UNZ_OK)
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x |= ((uLong)i) << 16;
    
    if(err == UNZ_OK)
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x += ((uLong)i) << 24;
    
    if(err == UNZ_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}

/*
   内部函数，在64位CPU上获取目标文件流8个字节的二进制位
*/
local int unz64local_getLong64      OF((const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream,ZPOS64_T * pX));
         
local int unz64local_getLong64(const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream,ZPOS64_T * pX)
{
    ZPOS64_T x;
    int i = 0;
    int err;
    
    err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x = (ZPOS64_T)i;
    
    if(err == UNZ_OK)
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x |= ((ZPOS64_T)i) << 8;
    
    if(err == UNZ_OK)
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x |= ((ZPOS64_T)i) << 16;
    
    if(err == UNZ_OK)
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x |= ((ZPOS64_T)i) << 24;
    
    if(err == UNZ_OK)
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x |= ((ZPOS64_T)i) << 32;
    
    if(err == UNZ_OK)
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x |= ((ZPOS64_T)i) << 40;
    
    if(err == UNZ_OK)
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x |= ((ZPOS64_T)i) << 48;
    
    if(err == UNZ_OK)
        err = unz64local_getByte(pzlib_filefunc_def, filestream, &i);
    x |= ((ZPOS64_T)i) << 56;
    
    if(err == UNZ_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
    
}
            
local ZPOS64_T unz64local_SearchCentralDir  OF((const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream));
local ZPOS64_T unz64local_SearchCentralDir (const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream)
{
    unsigned char * buf;
    ZPOS64_T uSizeFile;
    ZPOS64_T uBackRead;
    ZPOS64_T uMaxBack = 0xffff;//内容默认设置最大长度
    ZPOS64_T uPosFound = 0;
    
    if(ZSEEK64(*pzlib_filefunc_def, filestream, 0, ZLIB_FILEFUNC_SEEK_END) != 0)
       return 0;//下面也是，只要出错，就返回0
       
    uSizeFile = ZTELL64(*pzlib_filefunc_def, filestream);
    
    if(uMaxBack > uSizeFile)
        uMaxBack = uSizeFile;
    
    buf = (unsigned char *)ALLOC(BUFERADCOMMENT+4);//开启1k+4字节空间
    if(buf == NULL)
        return 0;
    
    uBackRead = 4;
    while(uBackRead < uMaxBack)
    {
        uLong uReadSize;
        ZPOS64_T uReadPos;
        int i;
        if(uBackRead + BUFERADCOMMENT > uMaxBack)
            uBackRead = uMaxBack;
        else
            
            uBackRead += uSizeFile - uBackRead;
        
        uReadPos = uSizeFile - uBackRead;
        
        uReadSize = ((BUFERADCOMMENT+4)<(uSizeFile-uReadPos))?(BUFERADCOMMENT + 4) : (uLong)(uSizeFile - uReadPos);
        
        if(ZSEEK64(*pzlib_filefunc_def, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0)
            break;
        
        if(ZREAD64(*pzlib_filefunc_def, filestream, buf, uReadSize) != uReadSize)
            break;
        
        for(i = (int)uReadSize-3;(i--)>0;)
            if(((*buf+i))==0x50 && ((*buf+i+1) == 0x4b) && ((*(buf+i+2)) == 0x05) && ((*(buf+i+3)) == 0x06))
            {
                uPosFound = uReadPos + i;
                break;
            }
        
        if(uPosFound != 0)
            break;
    }
    TRYFREE(buf);
    return uPosFound;
    
}

local ZPOS64_T unz64local_SearchCentralDir64    OF((const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream));

local ZPOS64_T unz64local_SearchCentralDir64(const zlib_filefunc64_32_def * pzlib_filefunc_def,voidpf filestream)
{
    unsigned char * buf;
    ZPOS64_T uSizeFile;
    ZPOS64_T uBackRead;
    ZPOS64_T uMaxBack = 0xffff;//定义一个大数
    ZPOS64_T uPosFound = 0;
    uLong uL;
    ZPOS64_T relativeOffset;
    if (ZSEEK64(*pzlib_filefunc_def, filestream, 0, ZLIB_FILEFUNC_SEEK_END) != 0) {
        return 0;
    }
    
    uSizeFile = ZTELL64(*pzlib_filefunc_def, filestream);//获取当前文件指针在文件头的偏移字节数
    
    if (uMaxBack > uSizeFile) {
        uMaxBack = uSizeFile;//记录一下当前的偏移量
    }
    
    buf = (unsigned char *)ALLOC(BUFERADCOMMENT + 4);//开1k+4个字节的空间
    if (buf == NULL) {
        return 0;
    }
    
    uBackRead = 4;
    
    while (uBackRead < uMaxBack)
    {
        uLong uReadSize;//读取位置
        ZPOS64_T uReadPos;//读取长度
        int i;
        if (uBackRead + BUFERADCOMMENT > uMaxBack) {//如果当前偏移量小于1024+4个字节
            
            uBackRead = uMaxBack;
            
        }else{
            
            uBackRead += BUFERADCOMMENT;
        }
        
        uReadPos = uSizeFile-uBackRead;
        
        uReadSize = ((BUFERADCOMMENT + 4) < (uSizeFile - uReadPos)) ? (BUFERADCOMMENT + 4):(uLong)(uSizeFile - uReadPos);
        
        if (ZSEEK64(*pzlib_filefunc_def, filestream, uReadPos, ZLIB_FILEFUNC_SEEK_SET) != 0) {//将文件指针移动到开始位置
            break;
        }
        
        if (ZREAD64(*pzlib_filefunc_def, filestream, buf, uReadSize) != uReadSize) {
            break;
        }
        
        for (i = (int)uReadSize-3; (i--)>0;) {
            if (((*(buf + i)) == 0x50) && ((*(buf+i+1)) == 0x4b) && ((*(buf+i+2)) == 0x06) && ((*(buf+i+3)) == 0x07)) {
                uPosFound = uReadPos + i;
                break;
            }
        }
        
        if (uPosFound != 0) {
            break;
        }
    }
    
        TRYFREE(buf);
        
        if (uPosFound == 0) {
            return 0;
        }
        
        if (ZSEEK64(*pzlib_filefunc_def, filestream, uPosFound, ZLIB_FILEFUNC_SEEK_SET) != 0) {
            return 0;
        }
        //检查是否已经签名
        if(unz64local_getLong(pzlib_filefunc_def, filestream, &uL)!=UNZ_OK)
            return 0;
        
        //zip文件的开始和结束的中心目录的磁盘编号
        if(unz64local_getLong(pzlib_filefunc_def, filestream, &uL))
        return 0;
        if(uL != 0)
            return 0;
        
        if(unz64local_getLong64(pzlib_filefunc_def, filestream, &relativeOffset) != UNZ_OK)
            return 0;
        
        if(unz64local_getLong(pzlib_filefunc_def, filestream, &uL) != UNZ_OK)
            return 0;
    
        if(uL != 1)
            return 0;
        
        if(ZSEEK64(*pzlib_filefunc_def, filestream, relativeOffset, ZLIB_FILEFUNC_SEEK_SET) != 0)
            return 0;
    
        if(unz64local_getLong(pzlib_filefunc_def, filestream, &uL) != UNZ_OK)
            return 0;
    
        if(uL != 0x06064b50)
            return 0;
    
    
    return relativeOffset;
}


            
            
local unzFile unzOpenInternal   OF((const void * path,zlib_filefunc64_32_def * pzlib_filefunc64_32_def,int is64bitOpenFunction));

            
/**
 *  打开一个zip文件
 *  入口函数，解压缩的整体在这处理
 *  @param path                    文件路径
 *  @param pzlib_filefunc64_32_def 可以为空
 *  @param is64bitOpenFunction     ？？？
 *
 *  @return 打开成功返回该zip文件的句柄，否则（zip不能被编辑或打开失败）返回NULL
 */
local unzFile unzOpenInternal (const void * path,zlib_filefunc64_32_def * pzlib_filefunc64_32_def,int is64bitOpenFunction)
{
    unz64_s us;
    unz64_s * s;
    ZPOS64_T central_pos;
    uLong uL;
    
    uLong number_disk;  //当前距离的数，使用spaning，如果不支持，就为0
    
    uLong number_disk_with_CD;
    
    ZPOS64_T number_entry_CD;
    
    int err = UNZ_OK;
    
    if (unz_copyright[0] != ' ') {
        return NULL;
    }
    
    us.z_filefunc.zseek32_file = NULL;
    us.z_filefunc.ztell32_file = NULL;
    if (pzlib_filefunc64_32_def == NULL) {
        fill_fopen64_filefunc(&us.z_filefunc.zfile_func64);
    }else{
        us.z_filefunc = *pzlib_filefunc64_32_def;
    }
    
    us.is64bitOpenFunction = is64bitOpenFunction;
    
    us.filestream = ZOPEN64(us.z_filefunc, path, ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_EXISTING);
    
    if (us.filestream == NULL) {
        return NULL;
    }
    
    central_pos = unz64local_SearchCentralDir64(&us.z_filefunc, us.filestream);
    
    if(central_pos)
    {
        uLong uS;
        ZPOS64_T uL64;
        
        us.isZip64 = 1;
        
        if(ZSEEK64(us.z_filefunc, us.filestream, central_pos, ZLIB_FILEFUNC_SEEK_SET) != 0){
            
            err = UNZ_ERRNO;
        }
        
        //检查之前是否已经签名
        if(unz64local_getLong(&us.z_filefunc, us.filestream, &uL) != UNZ_OK){
            err = UNZ_ERRNO;
        }
        
        //64位zip文件在他所在目录的大小
        if(unz64local_getLong64(&us.z_filefunc, us.filestream, &uL64) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        //版本由谁制作？
        if(unz64local_getShort(&us.z_filefunc, us.filestream, &uS) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        //提取版本信息
        if(unz64local_getShort(&us.z_filefunc, us.filestream, &uS) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        //在磁盘上的编号
        if(unz64local_getLong(&us.z_filefunc, us.filestream, &number_disk) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        //在主目录的磁盘编号
        if(unz64local_getLong(&us.z_filefunc, us.filestream, &number_disk_with_CD)!= UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        //
        if(unz64local_getLong64(&us.z_filefunc, us.filestream, &us.gi.number_entry) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        if(unz64local_getLong64(&us.z_filefunc, us.filestream, &number_entry_CD) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        if((number_entry_CD != us.gi.number_entry) || (number_disk_with_CD != 0) || (number_disk != 0)){
            
            err = UNZ_BADZIPFILE;
        }
        
        if(unz64local_getLong64(&us.z_filefunc, us.filestream, &us.size_central_dir) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        if(unz64local_getLong64(&us.z_filefunc, us.filestream, &us.offset_central_dir)){
            
            err = UNZ_ERRNO;
        }
        us.gi.size_comment = 0;
        
        
    }
    else
    {
        central_pos = unz64local_SearchCentralDir(&us.z_filefunc, us.filestream);
        if(central_pos == 0){
            
            err = UNZ_ERRNO;
        }
        us.isZip64 = 0;
        
        if(ZSEEK64(us.z_filefunc, us.filestream, central_pos, ZLIB_FILEFUNC_SEEK_SET) != 0){
            
            err = UNZ_ERRNO;
        }
        
        //检查之前是否已经签名
        if(unz64local_getLong(&us.z_filefunc, us.filestream, &uL) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        //磁盘编号
        if(unz64local_getShort(&us.z_filefunc, us.filestream, &number_disk)!= UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        //主目录在磁盘的编号
        if(unz64local_getShort(&us.z_filefunc, us.filestream, &number_disk_with_CD) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        if(unz64local_getShort(&us.z_filefunc, us.filestream, &uL) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        us.gi.number_entry = uL;
        
        if(unz64local_getShort(&us.z_filefunc, us.filestream, &uL) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        number_entry_CD = uL;
        
        if((number_entry_CD != us.gi.number_entry) || (number_disk_with_CD != 0) || (number_disk != 0)){
            
            err = UNZ_BADZIPFILE;
        }
        
        //主目录大小
        if(unz64local_getLong(&us.z_filefunc, us.filestream, &uL) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        us.size_central_dir = uL;
        
        //
        if(unz64local_getLong(&us.z_filefunc, us.filestream, &uL) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
        us.offset_central_dir = uL;
        
        //zip文件的注释长度
        if(unz64local_getShort(&us.z_filefunc, us.filestream, &us.gi.size_comment) != UNZ_OK){
            
            err = UNZ_ERRNO;
        }
        
    }
    
    if((central_pos<us.offset_central_dir + us.size_central_dir) && (err == UNZ_OK)){
        
        err = UNZ_BADZIPFILE;
    }
    
    if(err != UNZ_OK){
        
        ZCLOSE64(us.z_filefunc, us.filestream);
        return NULL;
    }
    
    us.byte_before_the_zipfile = central_pos - (us.offset_central_dir + us.size_central_dir);
    
    us.central_pos = central_pos;
    us.pfile_in_zip_read = NULL;
    us.encrypted = 0;
    
    s = (unz64_s *)ALLOC(sizeof(unz64_s));
    
    if(s != NULL){
        
        *s = us;
        unzGoToFirstFile((unzFile)s);
    }
    
    return (unzFile)s;
}

/*
    将磁盘时间格式转换成date格式tm_unz（可读性高）
 */
local void unz64local_DosDateToTmuDate (ZPOS64_T ulDosDate,tm_unz * ptm)
{
    ZPOS64_T uDate;
    uDate = (ZPOS64_T)(ulDosDate >> 16);
    ptm->tm_mday = (uInt)(uDate & 0x1f);
    ptm->tm_mon = (uInt)((((uDate)&0x1E0)/0x20)-1);
    ptm->tm_year = (uInt)(((uDate & 0x0FE00)/0x0200)+1980);
    ptm->tm_hour = (uInt)((ulDosDate & 0xF800)/0x800);
    ptm->tm_min = (uInt)((ulDosDate & 0x7E0)/0x20);
    ptm->tm_sec = (uInt)(2*(ulDosDate & 0x1f));
    
}
            
/**
 *  内部函数，得到当前文件的内部信息
 *
 *  @param file                 文件句柄 void *
 *  @param pfile_info           文件信息结构
 *  @param pfile_info_internal  文件内部信息结构
 *  @param szFileName           文件名
 *  @param fileNameBufferSize   文件名缓冲区大小
 *  @param extraField           临时字段
 *  @param extraFieldBufferSize 临时字段缓冲区大小
 *  @param szComment
 *  @param commentBufferSize
 *
 *  @return UNZ_OK代表成功。
 */
local int unz64local_GetCurrentFileInfoInternal (unzFile file,unz_file_info64 * pfile_info,unz_file_info64_internal * pfile_info_internal,char * szFileName,uLong fileNameBufferSize,void * extraField,uLong extraFieldBufferSize,char * szComment,uLong commentBufferSize)
{
    
    unz64_s * s;
    unz_file_info64 file_info;
    unz_file_info64_internal file_info_internal;
    int err = UNZ_OK;
    uLong uMagic;
    long lSeek = 0;
    uLong uL;
    
    if(file == NULL)
        return UNZ_PARAMERROR;
    
    s = (unz64_s *)file;
    
    if(ZSEEK64(s->z_filefunc, s->filestream, s->pos_in_central_dir, ZLIB_FILEFUNC_SEEK_SET) != 0){
        
        err = UNZ_ERRNO;
    }
    
    if(err == UNZ_OK){
        
        if(unz64local_getLong(&s->z_filefunc, s->filestream, &uMagic) != UNZ_OK){
            
            err = UNZ_ERRNO;
            
        }else if(uMagic != 0x02014b50){
            
            err = UNZ_BADZIPFILE;
        }
    }
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.version) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.version_needed) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.compression_method) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    if(unz64local_getLong(&s->z_filefunc, s->filestream, &file_info.dosDate) != UNZ_OK){
        
        err = UNZ_OK;
    }
    
    unz64local_DosDateToTmuDate(file_info.dosDate, &file_info.tmu_date);
    
    if(unz64local_getLong(&s->z_filefunc, s->filestream, &file_info.crc)!= UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    if(unz64local_getLong(&s->z_filefunc, s->filestream, &uL) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    file_info.compressed_size = uL;
    
    if(unz64local_getLong(&s->z_filefunc, s->filestream, &uL) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    file_info.uncompressed_size = uL;
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.size_filename) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.size_file_extra) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.size_file_comment) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.disk_num_start) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &file_info.internal_fa) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    if(unz64local_getLong(&s->z_filefunc, s->filestream, &file_info.external_fa) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    
    if(unz64local_getLong(&s->z_filefunc, s->filestream, &uL) != UNZ_OK){
        
        err = UNZ_ERRNO;
    }
    file_info_internal.offset_curfile = uL;
    
    lSeek+=file_info.size_filename;
    
    if((err == UNZ_OK) && (szFileName != NULL)){
        
        uLong uSizeRead;
        if(file_info.size_filename < fileNameBufferSize){
            
            *(szFileName + file_info.size_filename) = '\0';
            
            uSizeRead = file_info.size_filename;
            
        }else{
            
            uSizeRead = fileNameBufferSize;
        }
        if((file_info.size_filename > 0) && (fileNameBufferSize > 0)){
            
            if(ZREAD64(s->z_filefunc, s->filestream, szFileName, uSizeRead) != uSizeRead){
                
                err = UNZ_ERRNO;
            }
        }
        lSeek -= uSizeRead;
    }
    
    if((err == UNZ_OK) && (extraField!=NULL)){
        
        ZPOS64_T uSizeRead;
        if(file_info.size_file_extra < extraFieldBufferSize){
            
            uSizeRead = file_info.size_file_extra;
        }else{
            
            uSizeRead = extraFieldBufferSize;
        }
        
        if(lSeek!=0){
            
            if(ZSEEK64(s->z_filefunc, s->filestream, lSeek, ZLIB_FILEFUNC_SEEK_CUR)==0){
                
                lSeek = 0;
            }else{
                
                err = UNZ_ERRNO;
            }
            
        }
        if((file_info.size_file_extra > 0) && (extraFieldBufferSize > 0)){
            
            if(ZREAD64(s->z_filefunc, s->filestream, extraField, (uLong)uSizeRead) != uSizeRead){
                
                err = UNZ_ERRNO;
            }
        }
        lSeek += file_info.size_file_extra - (uLong)uSizeRead;
        
    }
    else
    {
        lSeek += file_info.size_file_extra;
        
    }
    
    if((err == UNZ_OK) && (file_info.size_file_extra != 0)){
        
        uLong acc = 0;
        
        //在现在的查找位置之后我们要把临时字段移动回来
        lSeek -= file_info.size_file_extra;
        
        if(lSeek != 0){
            
            if(ZSEEK64(s->z_filefunc, s->filestream, lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0){
                
                lSeek = 0;
            }else{
                
                err = UNZ_ERRNO;
            }
        }
        
        while(acc < file_info.size_file_extra)
        {
            uLong headerId;
            uLong dataSize;
            
            if(unz64local_getShort(&s->z_filefunc, s->filestream, &headerId) != UNZ_OK){
                
                err = UNZ_ERRNO;
            }
            if(unz64local_getShort(&s->z_filefunc, s->filestream, &dataSize) != UNZ_OK){
                
                err = UNZ_ERRNO;
            }
            
            //ZIP64临时字段
            if(headerId == 0x0001)
            {
                uLong uL;
                if(file_info.uncompressed_size == (ZPOS64_T)(unsigned long) - 1){
                    
                    if(unz64local_getLong64(&s->z_filefunc, s->filestream, &file_info.uncompressed_size) != UNZ_OK)
                        err = UNZ_ERRNO;
                }
                if(file_info.compressed_size == (ZPOS64_T)(unsigned long) -1){
                    
                    if(unz64local_getLong64(&s->z_filefunc, s->filestream, &file_info.compressed_size)!=UNZ_OK)
                        err = UNZ_ERRNO;
                }
                if(file_info_internal.offset_curfile == (ZPOS64_T)(unsigned long)-1){
                    
                    if(unz64local_getLong64(&s->z_filefunc, s->filestream, &file_info_internal.offset_curfile) != UNZ_OK)
                        err = UNZ_ERRNO;
                    
                }
                if(file_info.disk_num_start == (unsigned long) -1){
                    
                    //开始位置在磁盘的索引
                    if(unz64local_getLong(&s->z_filefunc, s->filestream, &uL) != UNZ_OK)
                        err = UNZ_ERRNO;
                }
                    
            }
            else
            {
                if(ZSEEK64(s->z_filefunc, s->filestream, dataSize, ZLIB_FILEFUNC_SEEK_CUR) != 0)
                    err = UNZ_ERRNO;
            }
            acc += 2+2+dataSize;
            
        }
    }
        if((err == UNZ_OK) && (szComment!=NULL))
        {
            uLong uSizeRead;
            if(file_info.size_file_comment < commentBufferSize){
                
                *(szComment + file_info.size_file_comment) = '\0';
                uSizeRead = file_info.size_file_comment;
            }else{
                uSizeRead = commentBufferSize;
            }
            if(lSeek != 0){
                
                if(ZSEEK64(s->z_filefunc, s->filestream, lSeek, ZLIB_FILEFUNC_SEEK_CUR) == 0){
                    
#ifndef __clang_analyzer__
                    lSeek = 0;
#endif
                }else{
                    
                    err = UNZ_ERRNO;
                }
            }
                if((file_info.size_file_comment > 0)&& (commentBufferSize > 0))
                    if(ZREAD64(s->z_filefunc, s->filestream, szComment, uSizeRead) != uSizeRead)
                        err = UNZ_ERRNO;
#ifndef __clang_analyzer__
                lSeek+=file_info.size_file_comment-uSizeRead;
#endif
                
            }
#ifndef __clang_analyzer__
            else
                lSeek += file_info.size_file_comment;
#endif
    
    if((err == UNZ_OK) && (pfile_info != NULL))
        *pfile_info = file_info;
    
    if((err ==UNZ_OK) && (pfile_info_internal != NULL))
        *pfile_info_internal = file_info_internal;
        
    return err;
    
    
}
           

extern unzFile ZEXPORT unzOpen(const char * path)
{
    return unzOpenInternal(path, NULL, 0);
}

            
extern int ZEXPORT unzGoToFirstFile(unzFile file)
{
    int err = UNZ_OK;
    unz64_s * s;
    
    if(file == NULL)
        return UNZ_PARAMERROR;
    s = (unz64_s *)file;
    s->pos_in_central_dir = s->offset_central_dir;
    s->num_file = 0;
    err = unz64local_GetCurrentFileInfoInternal(file, &s->cur_file_info, &s->cur_file_info_internal, NULL, 0, NULL, 0, NULL, 0);
    s->current_file_ok = (err == UNZ_OK);
    return err;
    
}

extern int ZEXPORT unzGoToNextFile (unzFile file)
{
    unz64_s * s;
    int err;
    
    if(file == NULL)
        return UNZ_PARAMERROR;
    s = (unz64_s *)file;
    
    if(!s->current_file_ok)
        return UNZ_END_OF_LIST_OF_FILE;
    
    if(s->gi.number_entry != 0xffff)
        if(s->num_file+1 == s->gi.number_entry)
            return UNZ_END_OF_LIST_OF_FILE;
    
    s->pos_in_central_dir += SIZECENTRALDIRITEM + s->cur_file_info.size_filename + s->cur_file_info.size_file_extra + s->cur_file_info.size_file_comment;
    
    s->num_file++;
    
    err = unz64local_GetCurrentFileInfoInternal(file, &s->cur_file_info, &s->cur_file_info_internal, NULL, 0, NULL, 0, NULL, 0);
    
    s->current_file_ok = (err == UNZ_OK);
    
    return err;
}


extern int ZEXPORT unzGetCurrentFileInfo (unzFile file,unz_file_info * pfile_info,char * szFileName,uLong fileNameBufferSize,void * extraField,uLong extraFieldBufferSize,char * szComment,uLong commentBufferSize)
{
    int err;
    unz_file_info64 file_info64;
    err = unz64local_GetCurrentFileInfoInternal(file, &file_info64, NULL, szFileName, fileNameBufferSize, extraField, extraFieldBufferSize, szComment, commentBufferSize);
    
    if(err == UNZ_OK)
    {
        pfile_info->version = file_info64.version;
        pfile_info->version_needed = file_info64.version_needed;
        pfile_info->flag = file_info64.flag;
        pfile_info->compression_method = file_info64.compression_method;
        pfile_info->dosDate = file_info64.dosDate;
        pfile_info->crc = file_info64.crc;
        pfile_info->size_filename = file_info64.size_filename;
        pfile_info->size_file_extra = file_info64.size_file_extra;
        pfile_info->size_file_comment = file_info64.size_file_comment;
        
        pfile_info->disk_num_start = file_info64.disk_num_start;
        pfile_info->internal_fa = file_info64.internal_fa;
        pfile_info->external_fa = file_info64.external_fa;
        pfile_info->tmu_date = file_info64.tmu_date,                                               pfile_info->compressed_size = (uLong)file_info64.compressed_size;
        pfile_info->uncompressed_size = (uLong)file_info64.uncompressed_size;
        
    }
    return err;
}

extern int ZEXPORT unzOpenCurrentFile (unzFile file)
{
    return unzOpenCurrentFile3(file, NULL, NULL, 0, NULL);
}

            
extern int ZEXPORT unzOpenCurrentFilePassword (unzFile file,const char * password)
{
    
    return unzOpenCurrentFile3(file, NULL, NULL, 0, password);
    
}
local int unz64local_CheckCurrentFileCoherencyHeader (unz64_s * s,uInt * piSizeVar,ZPOS64_T * poffset_local_extrafield,uInt * psize_local_extrafield)
{
    uLong uMagic,uData,uFlags;
    uLong size_filename;
    uLong size_extra_field;
    int err = UNZ_OK;
    
    *piSizeVar = 0;
    *poffset_local_extrafield = 0;
    *psize_local_extrafield = 0;
    
    if(ZSEEK64(s->z_filefunc, s->filestream, s->cur_file_info_internal.offset_curfile+s->byte_before_the_zipfile, ZLIB_FILEFUNC_SEEK_SET)!=0)
        return UNZ_ERRNO;
    
    if(err == UNZ_OK)
    {
        if(unz64local_getLong(&s->z_filefunc, s->filestream, &uMagic) != UNZ_OK)
            err = UNZ_ERRNO;
        else if (uMagic!=0x04034b50)
            err = UNZ_BADZIPFILE;
    }
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &uData) != UNZ_OK)
        err = UNZ_ERRNO;
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &uFlags)!= UNZ_OK)
        err = UNZ_ERRNO;
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &uData)!= UNZ_OK)
        err = UNZ_ERRNO;
    else if ((err == UNZ_OK) && (uData != s->cur_file_info.compression_method))
        err = UNZ_BADZIPFILE;
    
    if((err == UNZ_OK) && (s->cur_file_info.compression_method!=0) && (s->cur_file_info.compression_method!=Z_BZIP2ED) && (s->cur_file_info.compression_method!=Z_DEFLATED))
        err = UNZ_BADZIPFILE;
    
    if(unz64local_getLong(&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* data/time */
        err = UNZ_ERRNO;
    
    if(unz64local_getLong(&s->z_filefunc, s->filestream, &uData)!= UNZ_OK) /* crc */
        err = UNZ_ERRNO;
    else if ((err == UNZ_OK) && (uData!=s->cur_file_info.crc) && ((uFlags & 8)==0))
        err = UNZ_BADZIPFILE;
    
    if(unz64local_getLong(&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* size compr*/
        err = UNZ_ERRNO;
    else if (uData != 0xFFFFFFFF && (err == UNZ_OK) && (uData!=s->cur_file_info.compressed_size) && ((uFlags & 8) == 0))
        err = UNZ_BADZIPFILE;
    
    if(unz64local_getLong(&s->z_filefunc, s->filestream, &uData) != UNZ_OK) /* size uncompr*/
        err = UNZ_ERRNO;
    else if(uData != 0xFFFFFFFF && (err == UNZ_OK) && (uData != s->cur_file_info.uncompressed_size) && ((uFlags & 8) ==0))
        err = UNZ_BADZIPFILE;
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &size_filename) != UNZ_OK)
        err = UNZ_ERRNO;
    else if ((err == UNZ_OK) && (size_filename != s->cur_file_info.size_filename))
        err = UNZ_BADZIPFILE;
    
    *piSizeVar += (uInt)size_filename;
    
    if(unz64local_getShort(&s->z_filefunc, s->filestream, &size_extra_field) != UNZ_OK)
        err = UNZ_ERRNO;
    *poffset_local_extrafield = s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER+size_filename;
    
    *psize_local_extrafield = (uInt)size_extra_field;
    
    *piSizeVar += (uInt)size_extra_field;
    
    return err;
}
    
extern int ZEXPORT unzOpenCurrentFile3 (unzFile file,int *method,int *level,int raw,const char * password)
{
    int err = UNZ_OK;
    uInt iSizeVar;
    unz64_s * s;
    file_in_zip64_read_info_s * pfile_in_zip_read_info;
    
    ZPOS64_T offset_local_extrafield;//本地临时字段的偏移量
    
    uInt size_local_extrafield;//本地临时字段的大小
    
#ifndef NOUNCRYPT
    char source[12];
#else
    if(password != NULL)
        return UNZ_PARAMERROR;
#endif
    if(file == NULL)
        return UNZ_PARAMERROR;
    s = (unz64_s *)file;
    if(!s->current_file_ok)
        return UNZ_PARAMERROR;
    
    if(s->pfile_in_zip_read != NULL)
        
        unzCloseCurrentFile(file);
    
    if(unz64local_CheckCurrentFileCoherencyHeader(s, &iSizeVar, &offset_local_extrafield, &size_local_extrafield) != UNZ_OK)
        return UNZ_BADZIPFILE;
    
    pfile_in_zip_read_info = (file_in_zip64_read_info_s *)ALLOC(sizeof(file_in_zip64_read_info_s));
    
    if(pfile_in_zip_read_info == NULL)
        return UNZ_INTERNALERROR;
    
    pfile_in_zip_read_info->read_buffer = (char *)ALLOC(UNZ_BUFSIZE);
    
    pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
    pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
    pfile_in_zip_read_info->pos_local_extrafield = 0;
    pfile_in_zip_read_info->raw = raw;
    pfile_in_zip_read_info->byte_before_the_zipfile = 0;
    
    if(pfile_in_zip_read_info->read_buffer == NULL)
    {
        TRYFREE(pfile_in_zip_read_info);
        return UNZ_INTERNALERROR;
    }
    
    pfile_in_zip_read_info->stream_initialised = 0;
    
    if(method != NULL)
        *method = (int)s->cur_file_info.compression_method;
    if(level != NULL)
    {
        *level = 6;
        switch(s->cur_file_info.flag & 0x06)
        {
            case 6:*level = 1; break;
            case 4:*level = 2; break;
            case 2:*level = 9; break;
        }
        
    }
    
    if((s->cur_file_info.compression_method != 0) && (s->cur_file_info.compression_method != Z_BZIP2ED) && (s->cur_file_info.compression_method!= Z_DEFLATED))
    {
#ifndef __clang_analyzer__
        err = UNZ_BADZIPFILE;
#endif
        
    }
    
    pfile_in_zip_read_info->crc32_wait = s->cur_file_info.crc;
    pfile_in_zip_read_info->crc32 = 0;
    pfile_in_zip_read_info->total_out_64 = 0;
    pfile_in_zip_read_info->compression_method = s->cur_file_info.compression_method;
    pfile_in_zip_read_info->filestream = s->filestream;
    pfile_in_zip_read_info->z_filefunc = s->z_filefunc;
#ifndef __clang_analyzer__
    pfile_in_zip_read_info->byte_before_the_zipfile = s->byte_before_the_zipfile;
#endif
    
    pfile_in_zip_read_info->stream.total_out = 0;
    
    if((s->cur_file_info.compression_method == Z_BZIP2ED) && (!raw))
    {
        
#ifdef HAVE_BZIP2
        pfile_in_zip_read_info->bstream.bzalloc = (void * (*) (void *,int,int))0;
        pfile_in_zip_read_info->bstream.bzfree = (free_func)0;
        pfile_in_zip_read_info->bstream.opaque = (voidpf)0;
        pfile_in_zip_read_info->bstream.state = (voidpf)0;
        pfile_in_zip_read_info->bstream.zalloc = (alloc_func)0;
        pfile_in_zip_read_info->bstream.zfree = (free_func)0;
        pfile_in_zip_read_info->bstream.opaque = (voidpf)0;
        pfile_in_zip_read_info->bstream.next_in = (voidpf)0;
        pfile_in_zip_read_info->bstream_avail_in = 0;
        
        err = BZ2_bzDecompressInit(&pfile_in_zip_read_info->bstream,0,0);
        if(err == Z_OK)
            pfile_in_zip_read_info->stream_initialised = Z_BZIP2ED;
        else
        {
            TRYFREE(pfile_in_zip_read_info);
            return err;
        }
#else
        pfile_in_zip_read_info->raw = 1;
#endif
    }
    else if ((s->cur_file_info.compression_method == Z_DEFLATED) && (!raw))
    {
        pfile_in_zip_read_info->stream.zalloc = (alloc_func)0;
        pfile_in_zip_read_info->stream.zfree = (free_func)0;
        pfile_in_zip_read_info->stream.opaque = (voidpf)0;
        pfile_in_zip_read_info->stream.next_in = 0;
        pfile_in_zip_read_info->stream.avail_in = 0;
        
        err = inflateInit2(&pfile_in_zip_read_info->stream, -MAX_WBITS);
        if(err == Z_OK)
            pfile_in_zip_read_info->stream_initialised = Z_DEFLATED;
        else
        {
            TRYFREE(pfile_in_zip_read_info);
            return err;
        }
    }
        pfile_in_zip_read_info->rest_read_compressed = s->cur_file_info.compressed_size;
        pfile_in_zip_read_info->rest_read_uncompressed = s->cur_file_info.uncompressed_size;
        
        pfile_in_zip_read_info->pos_in_zipfile = s->cur_file_info_internal.offset_curfile+SIZEZIPLOCALHEADER + iSizeVar;
        pfile_in_zip_read_info->stream.avail_in = (uInt)0;
        
        s->pfile_in_zip_read = pfile_in_zip_read_info;
        s->encrypted = 0;

        
#ifndef NOUNCRYPT
    if(password != NULL)
    {
        int i;
        s->pcrc_32_tab = (const unsigned long *)get_crc_table();
        init_keys(password, s->keys, s->pcrc_32_tab);
        if(ZSEEK64(s->z_filefunc, s->filestream, s->pfile_in_zip_read->pos_in_zipfile + s->pfile_in_zip_read->byte_before_the_zipfile, SEEK_SET)  != 0)
            return UNZ_INTERNALERROR;
        if(ZREAD64(s->z_filefunc, s->filestream, source, 12) < 12)
            return UNZ_INTERNALERROR;
        
        for(i = 0;i < 12;i++)
            zdecode(s->keys, s->pcrc_32_tab, source[i]);
        s->pfile_in_zip_read->pos_in_zipfile += 12;
        s->encrypted = 1;
            
    }
#endif
    
    return UNZ_OK;
}

extern int ZEXPORT unzReadCurrentFile (unzFile file,voidp buf,unsigned len)
{
    int err = UNZ_OK;
    uInt iRead = 0;
    
    unz64_s * s;
    
    file_in_zip64_read_info_s * pfile_in_zip_read_info;
    
    if(file == NULL)
        return UNZ_PARAMERROR;
    s=(unz64_s *)file;
    
    pfile_in_zip_read_info = s->pfile_in_zip_read;
    
    if(pfile_in_zip_read_info == NULL)
        return UNZ_PARAMERROR;
    
    if(pfile_in_zip_read_info->read_buffer == NULL)
        return UNZ_END_OF_LIST_OF_FILE;
    if(len == 0)
        return 0;
    
    pfile_in_zip_read_info->stream.next_out = (Bytef *)buf;
    
    pfile_in_zip_read_info->stream.avail_out = (uInt)len;
    
    if((len > pfile_in_zip_read_info->rest_read_compressed+pfile_in_zip_read_info->stream.avail_in) && (pfile_in_zip_read_info->raw))
        
        pfile_in_zip_read_info->stream.avail_out = (uInt)pfile_in_zip_read_info->rest_read_compressed + pfile_in_zip_read_info->stream.avail_in;
    
    while(pfile_in_zip_read_info->stream.avail_out > 0)
    {
        if((pfile_in_zip_read_info->stream.avail_in == 0) && (pfile_in_zip_read_info->rest_read_compressed > 0))
        {
            uInt uReadThis = UNZ_BUFSIZE;
            
            if(pfile_in_zip_read_info->rest_read_compressed < uReadThis)
                
                uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;
            
            if(uReadThis == 0)
                return UNZ_EOF;
            if(ZSEEK64(pfile_in_zip_read_info->z_filefunc,pfile_in_zip_read_info->filestream, pfile_in_zip_read_info->pos_in_zipfile+pfile_in_zip_read_info->byte_before_the_zipfile,ZLIB_FILEFUNC_SEEK_SET) != 0)
                
                return UNZ_ERRNO;
            
            if(ZREAD64(pfile_in_zip_read_info->z_filefunc, pfile_in_zip_read_info->filestream, pfile_in_zip_read_info->read_buffer, uReadThis) != uReadThis)
                
                return UNZ_ERRNO;
            
#ifndef NOUNCRYPT
            if(s->encrypted)
            {
                uInt i;
                for(i = 0; i < uReadThis; i++)
                    pfile_in_zip_read_info->read_buffer[i] = zdecode(s->keys, s->pcrc_32_tab, pfile_in_zip_read_info->read_buffer[i]);

            }
#endif
            pfile_in_zip_read_info->pos_in_zipfile += uReadThis;
            
            pfile_in_zip_read_info->rest_read_compressed -= uReadThis;
            
            pfile_in_zip_read_info->stream.next_in = (Bytef *)pfile_in_zip_read_info->read_buffer;
            
            pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
        }
        
        if((pfile_in_zip_read_info->compression_method == 0) || (pfile_in_zip_read_info->raw))
        {
            uInt uDoCopy,i;
            
            if((pfile_in_zip_read_info->stream.avail_in == 0) && (pfile_in_zip_read_info->rest_read_compressed == 0))
                
                return (iRead == 0)? UNZ_EOF : iRead;
            
            if(pfile_in_zip_read_info->stream.avail_out < pfile_in_zip_read_info->stream.avail_in)
                uDoCopy = pfile_in_zip_read_info->stream.avail_out;
            else
                uDoCopy = pfile_in_zip_read_info->stream.avail_in;
            for(i = 0;i < uDoCopy;i++)
                *(pfile_in_zip_read_info->stream.next_out+i) = *(pfile_in_zip_read_info->stream.next_in + i);
            
            pfile_in_zip_read_info->total_out_64 = pfile_in_zip_read_info->total_out_64 + uDoCopy;
            
            pfile_in_zip_read_info->crc32 = crc32(pfile_in_zip_read_info->crc32, pfile_in_zip_read_info->stream.next_out, uDoCopy);
            
            pfile_in_zip_read_info->rest_read_uncompressed -= uDoCopy;
            
            pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
            pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
            pfile_in_zip_read_info->stream.next_out += uDoCopy;
            pfile_in_zip_read_info->stream.next_in += uDoCopy;
            pfile_in_zip_read_info->stream.total_out += uDoCopy;
            iRead += uDoCopy;
            
        }
        else if (pfile_in_zip_read_info->compression_method == Z_BZIP2ED)
        {
#ifdef HAVE_BZIP2
            uLong uTotalOutBefore,uTotalOutAfter;
            const Bytef * bufBefore;
            uLong uOutThis;
            
            pfile_in_zip_read_info->bstream.next_in = (char *)pfile_in_zip_read_info->stream.next_in;
            
            pfile_in_zip_read_info->bstream.avail_in = pfile_in_zip_read_info->stream.avail_in;
            
            pfile_in_zip_read_info->bstream.total_in_lo32 = pfile_in_zip_read_info->stream.total_in;
            
            pfile_in_zip_read_info->bstream.total_in_hi32 = 0;
            
            pfile_in_zip_read_info->bstream.next_out = (char *)pfile_in_zip_read_info->stream.next_out;
            
            pfile_in_zip_read_info->bstream.avail_out = pfile_in_zip_read_info->stream.avail_out;
            
            pfile_in_zip_read_info->bstream.total_out_lo32 = pfile_in_zip_read_info->stream.total_out;
            
            pfile_in_zip_read_info->bstream.total_out_hi32 = 0;
            
            uTotalOutBefore = pfile_in_zip_read_info->bstream.total_out_lo32;
            
            bufBefore = (const Bytef *)pfile_in_zip_read_info->bstream.next_out;
            
            err = BZ2_bzDecompress(&pfile_in_zip_read_info->bstream);
            
            uTotalOutAfter = pfile_in_zip_read_info->bstream.total_out_lo32;
            uOutThis = uTotalOutAfter-uTotalOutBefore;
            
            pfile_in_zip_read_info->stream.next_in = (Bytef*)pfile_in_zip_read_info->bstream.next_in;
            pfile_in_zip_read_info->stream.avail_in = pfile_in_zip_read_info->bstream.avail_in;
            pfile_in_zip_read_info->stream.total_in = pfile_in_zip_read_info->bstream.total_in_lo32;
            pfile_in_zip_read_info->stream.next_out = (Bytef *)pfile_in_zip_read_info->bstream.next_out;
            pfile_in_zip_read_info->stream.avail_out = pfile_in_zip_read_info->bstream.avail_out;
            pfile_in_zip_read_info->stream.total_out = pfile_in_zip_read_info->bstream.total_out_lo32;
            
            if(err = BZ_STREAM_END)
                return (iRead == 0) ? UNZ_EOF : iRead;
            if(err!=bZ_OK)
                break;
#endif
            
            
        }
        else
        {
            ZPOS64_T uTotalOutBefore,uTotalOutAfter;
            const Bytef * bufBefore;
            ZPOS64_T uOutThis;
            int flush = Z_SYNC_FLUSH;
            
            uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
            bufBefore = pfile_in_zip_read_info->stream.next_out;
            err = inflate(&pfile_in_zip_read_info->stream, flush);
            
            if((err >= 0) && (pfile_in_zip_read_info->stream.msg!=NULL))
                err = Z_DATA_ERROR;
            
            uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
            uOutThis = uTotalOutAfter-uTotalOutBefore;
            
            pfile_in_zip_read_info->total_out_64 = pfile_in_zip_read_info->total_out_64 + uOutThis;
            pfile_in_zip_read_info->crc32 = crc32(pfile_in_zip_read_info->crc32, bufBefore, (uInt)(uOutThis));
            pfile_in_zip_read_info->rest_read_uncompressed -= uOutThis;
            
            iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);
            if(err == Z_STREAM_END)
                return (iRead == 0)?UNZ_EOF:iRead;
            if(err != Z_OK)
                break;
        }
        
    }
    if(err == Z_OK)
        return iRead;
    
    return err;
    
}
        
extern int ZEXPORT unzCloseCurrentFile (unzFile file)
 {
     int err = UNZ_OK;
     
     unz64_s * s;
     file_in_zip64_read_info_s * pfile_in_zip_read_info;
     if(file == NULL)
         return UNZ_PARAMERROR;
     s = (unz64_s *)file;
     pfile_in_zip_read_info=s->pfile_in_zip_read;
     
     if(pfile_in_zip_read_info == NULL)
         return UNZ_PARAMERROR;
     
     if((pfile_in_zip_read_info->rest_read_uncompressed == 0) && (!pfile_in_zip_read_info->raw))
     {
         if(pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32)
             err = UNZ_CRCERROR;
     }
     
     TRYFREE(pfile_in_zip_read_info->read_buffer);
     
     pfile_in_zip_read_info->read_buffer = NULL;
     
     if(pfile_in_zip_read_info->stream_initialised == Z_DEFLATED)
         inflateEnd(&pfile_in_zip_read_info->stream);
     
#ifdef HAVE_BZIP2
     else if(pfile_in_zip_read_info->stream_initialised == Z_BZIP2ED)
         BZ2_bzDecompressEnd(&pfile_in_zip_read_info->bstream);
#endif
     pfile_in_zip_read_info->stream_initialised = 0;
     TRYFREE(pfile_in_zip_read_info);
     
     s->pfile_in_zip_read = NULL;
     
     return err;
}
     
     
extern int ZEXPORT unzClose (unzFile file)
{
    unz64_s * s;
    if(file == NULL)
        return UNZ_PARAMERROR;
    
    s=(unz64_s *)file;
    
    if(s->pfile_in_zip_read!=NULL)
        unzCloseCurrentFile(file);
    ZCLOSE64(s->z_filefunc, s->filestream);
    TRYFREE(s);
    
    return UNZ_OK;
    
}
            
            
            
            
            
            
            

