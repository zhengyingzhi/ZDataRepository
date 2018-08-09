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
#include <ZToolLib/ztl_array.h>
#include <ZToolLib/ztl_map.h>
#include <ZToolLib/ztl_palloc.h>

/* 行情数组，一组指针bardata的指针
 */
struct zdatavec_s
{
    ztl_array_t     Datas;      // array object zdatabar_t*
    uint32_t        Count;      // array size
    uint32_t        Capacity;
    uint32_t        RefCount;   // reference count
    uint32_t        Flag;
    //ztl_pool_t*     Pool;
    void*           Pool;       // ztl_pool_t type
};
typedef struct zdatavec_s zdatavec_t;


/* 时间与该时间对应的所有产品的行情map
 */
struct ztime2assets_s
{
    int64_t     bartime;        // the time of the bar data
    ztl_map_t*  asset2bars;     // <asset_hashid, zdatabar*>
};
typedef struct ztime2assets_s ztime2assets_t;

/* 品种与该品种对应的所有时间序列的数据
 */
struct zassets2bars_s
{
    ztl_map_t*  asset2bars;     // <asset_hashid, ztl_array_t>
};
typedef struct zassets2bars_s zassets2bars_t;



/* 表示所有的bar数据，并提供丰富的接口访问数据
 * a. 按时间访问
 * b. 按品种访问
 */
struct zbars_s
{
    char            filename[256];
    int64_t         start_time;
    int64_t         end_time;
    ztl_pool_t*     pool;

    ztl_array_t     raw_bars;               // 原始数据
    ztl_array_t     time_assets_map;        // 数组每个元素为时间和该时间对应的所有产品的bar数据
    ztl_map_t*      assets_time_map;        // 每个产品和该产品对应的所有bar数据(一个时间序列的array)
};
typedef struct zbars_s zbars_t;


/* 数据访问获取，每次获取数据时，进行一些指针拷贝(非数据拷贝)
 */
struct zdataframe_s
{
    int64_t         start_time;
    int64_t         end_time;
    ztl_pool_t*     pool;

    uint32_t        frequency;      // 数据频率
    uint32_t        refcount;       // 引用计数，对象不使用是自动删除
    ztl_map_t*      assets_bars;    // 按时间访问时的
    ztl_array_t     bars;           // 一系列数据指针(若按时间访问，则是一堆产品的bar指针，若按产品访问，则是一堆时间序列的bar指针)
};

/* 对产品代码进行hash转换，得到 ztl_map_t 的 key */
uint64_t zlookup_symbol(const char* apstr, int len);


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




//////////////////////////////////////////////////////////////////////////
/* 处理回测结果
 * 1. 以时间序列记录持仓、订单、成交  [map<time,[positions, orders, transactions]>]
 * 2. 以时间序列记录 Cumulative Risks
 */
struct zperformance_s
{
    int64_t time;
};


#endif//_ZDATA_STRUCT_H_
