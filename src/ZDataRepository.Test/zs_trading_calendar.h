#ifndef _ZDATA_LOAD_H_INCLUDED_
#define _ZDATA_LOAD_H_INCLUDED_

#include <stdint.h>

#include <ZToolLib/ztl_array.h>
#include <ZToolLib/ztl_map.h>


struct zs_trading_calendar_s
{
    char        calendar_name[16];  // ������������
    char        market_name[16];    // ������������
    char        start_date[16];     // ��ʼʱ��
    char        end_date[16];       // ����ʱ��
    int64_t     start_session;      // 
    int64_t     end_session;        // 
    int         include_weekend;    // �Ƿ������ĩ
    ztl_set_t*  holidays;           // �ڼ��� valueΪ int64 ʱ��
    ztl_array_t all_days;           // ���н�����
    ztl_array_t all_minutes;        // ���н����յķ���
    ztl_array_t open_close;         // ���������յĿ�����ʱ��
};

typedef struct zs_trading_calendar_s zs_trading_calendar_t;



#endif
