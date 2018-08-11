#ifndef _ZDATA_LOAD_H_INCLUDED_
#define _ZDATA_LOAD_H_INCLUDED_

#include <stdint.h>

#include <ZToolLib/ztl_array.h>
#include <ZToolLib/ztl_map.h>


struct zs_trading_calendar_s
{
    char        calendar_name[16];  // 交易日历名字
    char        market_name[16];    // 交易日历名字
    char        start_date[16];     // 开始时间
    char        end_date[16];       // 结束时间
    int64_t     start_session;      // 
    int64_t     end_session;        // 
    int         include_weekend;    // 是否包含周末
    ztl_set_t*  holidays;           // 节假日 value为 int64 时间
    ztl_array_t all_days;           // 所有交易日
    ztl_array_t all_minutes;        // 所有交易日的分钟
    ztl_array_t open_close;         // 各个交易日的开收盘时间
};

typedef struct zs_trading_calendar_s zs_trading_calendar_t;



#endif
