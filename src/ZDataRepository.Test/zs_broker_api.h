#pragma once

#include <stdint.h>

typedef struct zs_data_head_s zs_data_head_t;

/* 平台统一数据头部 */
struct zs_data_head_s
{
    int         type;
    int         size;
    uint32_t    refcount;
    uint32_t    flag;

    void      (*cleanup)(zs_data_head_t* self);

    //char        buf[1];
};

uint32_t zs_data_addref(zs_data_head_t* zdh);

uint32_t zs_data_decref_auto_release(zs_data_head_t* zdh);

#define zs_data_body(zdh) (char*)(zdh + 1)


/* 各api的出入口 */
struct zs_trade_api_s
{
    void* hlib;
    char api_name[16];

    // request
    int(*reqlogin)(const char* account, const char* password);
    int(*order)(const char* account, const char* instrument, double price, int volume, const char* strategyname);
    int(*cancel)(int orderid);
    int(*query)(const char* account, int category);

    // returned
    void(*trade_handler)(void* userdata, zs_data_head_t* data);
};
typedef struct zs_trade_api_s zs_trade_api_t;

struct zs_md_api_s
{
    void* hlib;
    char api_name[16];

    void(*register_front)();

    // request
    int(*reqlogin)(const char* account, const char* password);
    int(*subscribe)(const char* instruments[], int count);
    int(*unsubscribe)(const char* instruments[], int count);

    // returned
    void(*md_handler)(void* userdata, zs_data_head_t* data);
};
typedef struct zs_md_api_s zs_md_api_t;


/* 全局交易核心对象 */
typedef struct zs_algorithm_s
{
    char start_date[16];
    char end_date[16];
    void* tc;
};
typedef struct zs_algorithm_s zs_algorithm_t;

/* 事件引擎对象，负责事件的分发与回调 */
typedef struct zs_event_engine
{
    // 需要一个保存事件与回测函数的关系
    ztl_producer_consumer_t* zpc;
}zs_event_engine_t;

int zs_event_register(zs_event_engine_t* zee, void* handler);
int zs_event_post(zs_event_engine_t* zee, zs_data_head_t* zdh);


#include <ZToolLib/ztl_producer_consumer.h>
/* 平台对于各api的管理和对接 */
struct zs_brokerage_s
{
    zs_algorithm_t*     algo;
    zs_trade_api_t*     tdapi;
    zs_md_api_t*        mdapi;
    ztl_producer_consumer_t* zpc;
};
typedef struct zs_brokerage_s zs_brokerage_t;

/* api的回调处理函数 */
void trade_handler(void* userdata, zs_data_head_t* data)
{
    // 从api收到交易数据，异步通知给事件引擎，最后调用到 交易核心，上层客户策略
    // 连接、登录、订单回报、查询数据等
}
void md_handler(void* userdata, zs_data_head_t* data)
{
    // 从api收到行情，异步通知给事件引擎，最后调用到 交易核心，上层客户策略
    // 连接、登录、行情数据
}
