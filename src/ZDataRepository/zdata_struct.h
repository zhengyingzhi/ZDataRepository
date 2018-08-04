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
    float       OpenPrice;
    float       HighPrice;
    float       LowPrice;
    float       ClosePrice;
    float       SettlePrice;        // for Future
    float       OpenInterest;       // for Future
    float       AdjustFactor;       // for Equity
    uint32_t    Flag;
    int64_t     Volume;
    int64_t     Turnover;
    int64_t     DateTime;
    int64_t     Reserve;
};

typedef struct zdatabar_s zdatabar_t;


/* zdata tick Level数据
 */
struct zdatatick_s
{
    char        Instrument[32];
    float       LastPrice;
    float       OpenPrice;
    float       HighPrice;
    float       LowPrice;
    float       PreClosePrice;
    float       PreOpenInterest;    // for Future
    float       ClosePrice;
    float       SettlePrice;        // for Future
    float       OpenInterest;       // for Future
    float       AdjustFactor;       // for Equity
    int64_t     Volume;
    int64_t     Turnover;
    int64_t     DateTime;
    float       bidVolume1;
    float       bidPrice1;
    float       bidVolume2;
    float       bidPrice2;
    float       bidVolume3;
    float       bidPrice3;
    float       bidVolume4;
    float       bidPrice4;
    float       bidVolume5;
    float       bidPrice5;
    float       askVolume1;
    float       askPrice1;
    float       askVolume2;
    float       askPrice2;
    float       askVolume3;
    float       askPrice3;
    float       askVolume4;
    float       askPrice4;
    float       askVolume5;
    float       askPrice5;
};

typedef struct zdatatick_s zdatatick_t;


struct zdatabar_index_s
{
    int32_t     IndexInstrument;
    int32_t     IndexOpenPrice;
    int32_t     IndexHighPrice;
    int32_t     IndexLowPrice;
    int32_t     IndexClosePrice;
    int32_t     IndexSettlePrice;
    int32_t     IndexOpenInterest;
    int32_t     IndexAdjustFactor;
    int32_t     IndexVolume;
    int32_t     IndexTurnover;
    int32_t     IndexDateTime;
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
    uint32_t        RefCount;   // reference count
    uint32_t        Flag;
    //ztl_pool_t*     Pool;
    void*           Pool;       // ztl_pool_t type
};
typedef struct zdatavec_s zdatavec_t;

#include <ZToolLib/ztl_palloc.h>
struct zdatatable_s
{
    zdatavec_t**    Table;
    uint32_t        Size;
    //ztl_pool_t*    Pool;
    void*           Pool;       // ztl_pool_t type
};
typedef struct zdatatable_s zdatatable_t;


void zdata_csv_index_init(zdatabar_index_t* indexs);

// 获取数据的接口
// 可按索引(包括负数)、某段数据、某几行几列的数据
zdatavec_t* zdata_get_byindex(int index);
zdatavec_t* zdata_get_bystartend(int start, int end);


#endif//_ZDATA_STRUCT_H_
