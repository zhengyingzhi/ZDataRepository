/*
* Copyright (C) Yingzhi Zheng.
* Copyright (C) <zhengyingzhi112@163.com>
*/

#ifndef _ZDATA_LOAD_H_INCLUDED_
#define _ZDATA_LOAD_H_INCLUDED_

#include "zdata_struct.h"


#define ZDATA_Ok        0       // ok
#define ZDATA_Error    -1       // 错误
#define ZDATA_Ignore    1       // 忽略

/* 自定义数据读取函数
 * result   处理结果指针对象，内存默认由底层分配(可能是zdatabar/zdatatick等)
 * context  用户上下文数据
 * number   回调次数
 * data     待处理数据
 * length   待处理数据的长度
 * return   ZDATA_Ok-成功 
 */
typedef int(*zdata_proc_func_ptr)(void* result, void* context, int number, const char* data, int length);

/* 加载数据时，每个数据的对象的内存分配函数
 * context  用户上下文数据
 * size     分配大小
 * return   指针地址
 */
typedef void*(*zdata_alloc_ptr)(void* context, int size);


/* 灵活的数据加载方式
 * 1. 框架可只负责数据快速读取，再将数据按行或按二进制块大小回调通知到上层
 * 2. 也可以指定数据字段关系，由于底层自动处理
 * 数据加载解析后，各数据对象默认由底层自动管理
 */
typedef struct  
{
    zdata_proc_func_ptr proc;       // 可以使用系统默认的
    void*               context;   // 用户上下文数据
    const char*         filepath;   // 文件路径(可支持多个文件)
    zdatabar_index_t    indexs;     // 若没指定 proc 时，需要定义好各字段的索引位置
    int                 headline;   // 是否有表头行
    int                 binary_usz; // 加载binary数据时的各块数据的大小
    void*               alloc_ptr;  // 内存分配函数
}zdata_load_conf_t;

/* 加载csv文件中的数据到内在中
 * 传入csv 各字段对应的索引位置
 */
int zdata_load_from_csv_file(zdatavec_t** ppdvec, const char* filepath, zdatabar_index_t* indexs);

/* 加载binary文件中的数据到内在中
 */
int zdata_load_from_binary_file(zdatavec_t** ppdvec, const char* filepath);

/* 保存binary行情数据到文件中
 */
int zdata_save_to_binary_file(const char* datapath, const zdatavec_t* dvec);


/* 重置 zdatavec_t 对象
 */
void zdata_dvec_reset(zdatavec_t* dvec);

/* 释放 zdatavec_t 对象的内存
 */
void zdata_dvec_release(zdatavec_t* dvec);

#endif//_ZDATA_LOAD_H_INCLUDED_
