//
//  wxzip.h
//  WXStringLen
//
//  Created by WuXian on 16/4/27.
//  Copyright © 2016年 WuXian. All rights reserved.
//

#ifndef wxzip_h
#define wxzip_h

#include <stdio.h>
#include <zlib.h>
#include "wxioapi.h"

#define Z_BZIP2ED 12

#if defined(STRICTUNZIP) || defined(STRICTZIPUNZIP)

typedef struct TagunzFile__ {int unused; } unzFile__;
typedef unzFile__ * unzFile;
#else
typedef voidp unzFile;
#endif

#define UNZ_OK                          (0)
#define UNZ_END_OF_LIST_OF_FILE         (-100)
#define UNZ_ERRNO                       (Z_ERRNO)
#define UNZ_EOF                         (0)
#define UNZ_PARAMERROR                  (-102)
#define UNZ_BADZIPFILE                  (-103)
#define UNZ_INTERNALERROR               (-104)
#define UNZ_CRCERROR                    (-105)

/*
    时间信息的结构
 */
typedef struct tm_unz_s
{
    uInt tm_sec;            //秒[0,59]
    uInt tm_min;            //分钟[0,59]
    uInt tm_hour;           //小时[0,23]
    uInt tm_mday;           //天[1,31]
    uInt tm_mon;            //月[0,11]
    uInt tm_year;           //年[1980..2044]
}tm_unz;

/*
 这个结构包含关于这个zip文件的数据，是全局的
 这些数据来自目录中央的结束位置；ps：现在还不了解文件系统，所以不是很懂
 */
typedef struct unz_global_info64_s
{
    ZPOS64_T number_entry;
    uLong size_comment;
    
} unz_global_info64;

/*
 这个结构包含这个zip文件里面的一个文件信息
 */
typedef struct unz_file_info64_s
{
    uLong version;                  //版本2字节
    uLong version_needed;           //2字节
    uLong flag;                     //标志2字节
    uLong compression_method;       //压缩方法2字节
    uLong dosDate;                  //在磁盘上最后修改的时间4字节
    uLong crc;                      //crc-32，4字节
    ZPOS64_T compressed_size;       //被压缩的长度，8字节
    ZPOS64_T uncompressed_size;     //解压缩的长度，8字节
    uLong size_filename;            //文件名字长度，2字节
    uLong size_file_extra;          //临时文件长度，2字节
    uLong size_file_comment;        //文件评论长度，2字节
    uLong disk_num_start;           //所在的磁盘号码，2字节
    uLong internal_fa;              //内部文件属性，2字节
    uLong external_fa;              //外部文件属性，4字节
    
    tm_unz tmu_date;
    
} unz_file_info64;


typedef struct unz_file_info_s
{
    uLong version;                  //版本，2字节
    uLong version_needed;           //版本提取，2字节
    uLong flag;                     //通用标志，2字节
    uLong compression_method;       //压缩方法，2字节
    uLong dosDate;                  //文件在磁盘上的最后修改时间，4字节
    uLong crc;                      //crc-32，4字节
    uLong compressed_size;          //压缩大小，4字节
    uLong uncompressed_size;        //解压大小，4字节
    uLong size_filename;            //文件名长度，2字节
    uLong size_file_extra;          //临时文件长度，2字节
    uLong size_file_comment;        //文件评论长度，2字节
    uLong disk_num_start;           //在磁盘的编号
    uLong internal_fa;              //内部文件富文本，2字节
    uLong external_fa;              //外部文件富文本，4字节
    tm_unz tmu_date;
    
}unz_file_info;


/**
 *  外部函数接口：打开文件
 *
 *  @param path zip文件路径
 *
 *  @return 成功返回这个zip文件的句柄，失败返回NULL
 */
extern unzFile ZEXPORT unzOpen  OF((const char * path));

/**
 *  外部函数接口：开始处理第一个文件
 *
 *  @param file 文件句柄
 *
 *  @return UNZ_OK表示成功，其他表示失败
 */
extern int ZEXPORT unzGoToFirstFile(unzFile file);


/**
 *  外部API函数，设置zip文件当前文件的下一个文件
 *
 *  @param file 文件句柄
 *
 *  @return 成功返回UNZ_OK，UNZ_END_OF_LIST_OF_FILE表示当前文件在zip里属于最后一个文件
 */
extern int ZEXPORT unzGoToNextFile  OF((unzFile file));


extern int ZEXPORT unzGetCurrentFileInfo    OF((unzFile file,unz_file_info * pfile_info,char * szFileName,uLong fileNameBufferSize,void * extraField,uLong extraFieldBufferSize,char * szComment,uLong commentBufferSize));



/**
 *  外部函数接口，读取当前文件的上下文，可以打开并读取数据，也可以在读取所有数据之前关闭他
 *
 *  @param file 文件路径
 */
extern int ZEXPORT unzOpenCurrentFile   OF((unzFile file));

/**
 *  外部API函数，读取打开该zip文件里当前文件的数据
 *
 *  @param file     文件路径
 *  @param password 密码
 */
extern int ZEXPORT unzOpenCurrentPassword   OF((unzFile file,const char * password));

/**
 *  外部AIP函数，读取当前文件的字节
 *
 *  @param file 文件句柄
 *  @param buf  保存数据的缓冲区
 *  @param len  缓冲区大小
 *
 *  @return 成功返回读取到的字节数，0表示读取结束或完成，小于0表示出错，UNZ_ERRNO表示IO错误，zLib错误码表示解压缩出错
 */
extern int ZEXPORT unzReadCurrentFile   OF((unzFile file,voidp buf,unsigned len));


extern int ZEXPORT unzOpenCurrentFile3  OF((unzFile file,int *method,int *level,int raw,const char * password));

/**
 *  外部API函数，关闭文件
 *
 *  @param file 目标文件
 */
extern int ZEXPORT unzCloseCurrentFile  OF((unzFile file));


/**
 *  外部API函数，关闭一个已经打开过的解压缩文件
 *
 *  @param file 文件句柄
 *
 *  @return UNZ_OK表示成功
 */
extern int ZEXPORT  unzClose    OF((unzFile file));






#endif /* wxzip_h */
