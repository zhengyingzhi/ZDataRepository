#pragma once

#include <stdint.h>

typedef struct zs_data_head_s zs_data_head_t;

/* ƽ̨ͳһ����ͷ�� */
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


/* ��api�ĳ���� */
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


/* ȫ�ֽ��׺��Ķ��� */
typedef struct zs_algorithm_s
{
    char start_date[16];
    char end_date[16];
    void* tc;
};
typedef struct zs_algorithm_s zs_algorithm_t;

/* �¼�������󣬸����¼��ķַ���ص� */
typedef struct zs_event_engine
{
    // ��Ҫһ�������¼���ز⺯���Ĺ�ϵ
    ztl_producer_consumer_t* zpc;
}zs_event_engine_t;

int zs_event_register(zs_event_engine_t* zee, void* handler);
int zs_event_post(zs_event_engine_t* zee, zs_data_head_t* zdh);


#include <ZToolLib/ztl_producer_consumer.h>
/* ƽ̨���ڸ�api�Ĺ���ͶԽ� */
struct zs_brokerage_s
{
    zs_algorithm_t*     algo;
    zs_trade_api_t*     tdapi;
    zs_md_api_t*        mdapi;
    ztl_producer_consumer_t* zpc;
};
typedef struct zs_brokerage_s zs_brokerage_t;

/* api�Ļص������� */
void trade_handler(void* userdata, zs_data_head_t* data)
{
    // ��api�յ��������ݣ��첽֪ͨ���¼����棬�����õ� ���׺��ģ��ϲ�ͻ�����
    // ���ӡ���¼�������ر�����ѯ���ݵ�
}
void md_handler(void* userdata, zs_data_head_t* data)
{
    // ��api�յ����飬�첽֪ͨ���¼����棬�����õ� ���׺��ģ��ϲ�ͻ�����
    // ���ӡ���¼����������
}
