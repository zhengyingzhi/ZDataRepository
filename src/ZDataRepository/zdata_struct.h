/*
* Copyright (C) Yingzhi Zheng.
* Copyright (C) <zhengyingzhi112@163.com>
*/

#ifndef _ZDATA_STRUCT_H_
#define _ZDATA_STRUCT_H_

#include <stdint.h>
#include <stdbool.h>

#define ZDATA_BAR_FIELDS_N  8


#pragma pack(push,1)

/* zdata bar 数据
 * 日线/分钟
 */
struct zdatabar_s
{
    char        Instrument[32];
    double      OpenPrice;
    double      ClosePrice;
    double      HighPrice;
    double      LowPrice;
    uint64_t    Volume;
    uint64_t    Turnover;
    uint64_t    Time;
    uint64_t    Reserve;
    float       AdjustFactor;
    uint32_t    Flag;
};

typedef struct zdatabar_s zdatabar_t;

struct zdatabar_index_s
{
    uint32_t    IndexInstrument;
    uint32_t    IndexOpenPrice;
    uint32_t    IndexClosePrice;
    uint32_t    IndexHighPrice;
    uint32_t    IndexLowPrice;
    uint32_t    IndexVolume;
    uint32_t    IndexTurnover;
    uint32_t    IndexAdjustFactor;
    uint32_t    IndexTime;
};
typedef struct zdatabar_index_s zdatabar_index_t;


#pragma pack(pop)


//////////////////////////////////////////////////////////////////////////
/* 行情数组
 */
struct zdatavec_s
{
    zdatabar_t**    Datas;      // array object
    uint32_t        Count;      // array size
    uint32_t        Capacity;
    uint32_t        Flag;
    //ztl_pool_t*     Pool;
    void*           Pool;       // ztl_pool_t type
};
typedef struct zdatavec_s zdatavec_t;

#include <ZToolLib/ztl_palloc.h>
struct zdatatable_s
{
    zdatavec_t* Table;
    uint32_t    Size;
    //ztl_pool_t* Pool;
    void*       Pool;           // ztl_pool_t type
};
typedef struct zdatatable_s zdatatable_t;


void zdata_set_csv_index(zdatabar_index_t* indexs);


#endif//_ZDATA_STRUCT_H_
