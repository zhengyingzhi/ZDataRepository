#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <ZToolLib/ztl_times.h>
#include <ZToolLib/ztl_utils.h>

#include <ZToolLib/ztl_map.h>
#include <ZToolLib/ztl_common.h>

#include <ZDataRepository/zdata_load.h>

void TestLoadCsv(const char* filepath);

void TestCalendar();

int main(int argc, char* argv[])
{
    ZTL_NOTUSED(argc);
    ZTL_NOTUSED(argv);

    printf("ZDataRepository.Test ZToolLib version:%s\n", ZTL_Version);

    const char* filepath = "F:/bigdata/csv/bar1d/SHA_2018-Demo.csv";
    //TestLoadCsv(filepath);
    TestCalendar();

    return 0;
}


void TestLoadCsv(const char* filepath)
{
    int rv;
    zdatavec_t* dvec;

    zdatabar_index_t lIndexs;
    zdata_csv_index_init(&lIndexs);
    lIndexs.IndexAdjustFactor = 0;
    lIndexs.IndexTurnover = 1;
    lIndexs.IndexClosePrice = 2;
    lIndexs.IndexDateTime = 3;
    lIndexs.IndexHighPrice = 5;
    lIndexs.IndexInstrument = 6;
    lIndexs.IndexLowPrice = 7;
    lIndexs.IndexOpenPrice = 8;
    lIndexs.IndexVolume = 10;

    rv = zdata_load_from_csv_file(&dvec, filepath, &lIndexs);
    if (rv != 0)
    {
        printf("zdata_load_from_csv_file failed:%d\n", rv);
        return;
    }

    printf("zdata_load_from_csv_file records:%d\n", dvec->Count);

    zdatabar_t* bardata;
    bardata = (zdatabar_t*)ztl_array_at((&dvec->Datas), 0);
    printf("%s,%.2f,%.2f\n", bardata->Instrument, bardata->OpenPrice, bardata->ClosePrice);

    getchar();
}


//////////////////////////////////////////////////////////////////////////

static const char* holidays_raw[] = {
    "2018-01-01", "2018-02-15",
    "2018-02-16", "2018-01-19", "2018-01-20", "2018-01-21", "2018-01-22" };


static inline bool IsLeapYear(int year)
{
    return ((year & 3) == 0 && (year % 100) != 0) || (year % 400 == 0);
}

static int monthdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

struct zdate_desc
{
    int32_t     date;
    int32_t     index;
    uint16_t    year;
    uint16_t    month;
    uint16_t    mday;       // month day
    uint16_t    yday;       // year day
    uint8_t     wday;       // week day 0~6
    char        sdate[9];
};

// 表示各年份对应的index(如2005在0，2006在125, 2007在(125+366)， 2008在(125+266+365))
// 以便快索引
int start_year = 2005;
int year_table[64] = {0};
static inline int get_year_index(int start_year, int cur_year)
{
    // 返回对应年份的index
    return (cur_year - start_year);
}

static inline int get_mdays(int year, int month)
{
    int mdays = monthdays[month];
    if (month == 2 && IsLeapYear(year)) {
        mdays += 1;
    }
    return mdays;
}

static int GenDays(int DayArr[], int start_date, int end_date)
{
    int n = 0;
    int cur_year = start_date / 10000;
    int end_year = end_date / 10000;
    int start_month = (start_date % 10000) / 100;
    int start_day = (start_date % 10000) % 100;

    int days_per_year = 0;
    int cur_date = 0;
    while (cur_year <= end_year)
    {
        int end_month = 12;
        if (cur_year == end_year) {
            end_month = (end_date % 10000) / 100;
        }

        // each month 0~11
        for (int m = start_month - 1; m < end_month; ++m)
        {
            int mdays = get_mdays(cur_year, m + 1);
            int yearmonth = cur_year * 10000 + (m + 1) * 100;

            /* 这里可以使用一个静态的月份天数table更快 */
            for (int d = start_day; d <= mdays; d++)
            {
                days_per_year++;
                cur_date = yearmonth + d;
                DayArr[n++] = cur_date;
                printf("%d ", cur_date);

                if (cur_date >= end_date) {
                    break;
                }
            }
            printf("\n");

            start_day = 1;
        }
        printf("** cur_year=%d, days_per_year=%d\n", cur_year, days_per_year);
        days_per_year = 0;

        start_month = 1;

        // next year
        cur_year += 1;
    }
    return n;
}

static int64_t _conv_datetime(ztl_tm* ztm, const char datetime[]);
void TestCalendar()
{
    int64_t idt;
    ztl_tm ztm = { 0 };
    idt = _conv_datetime(&ztm, "2018-08-10 16:18:00");

    int DayArr[4096] = { 0 };
    int n = GenDays(DayArr, 20160110, 20180305);

    return;

    //////////////////////////////////////////////////////////////////////////

    char start_date[16] = "2018-01-01";
    char end_date[16] = "2018-03-01";
    printf("holiday %s\n", holidays_raw[0]);

    int seconds = 24 * 60 * 60;
    time_t lNow = time(0);

    struct tm ltm = { 0 };
    localtime_s(&ltm, &lNow);

    ltm.tm_year -= 10;
    ltm.tm_mon = 0;
    ltm.tm_mday = 1;
    ltm.tm_hour = 0;
    ltm.tm_min = 0;
    ltm.tm_sec = 0;

    time_t tt = mktime(&ltm);
    struct tm* lptm2 = localtime(&tt);

    int monthdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    int days = 0;
    int lastmonth = 0;
    for (int i = 0; i < 10000; ++i)
    {
        struct tm ltm = { 0 };
        localtime_s(&ltm, &tt);
        if (lastmonth != ltm.tm_mon)
        {
            if (lastmonth == 1) {
                assert(days >= 28);
                assert(days <= 29);
                if (days == 29) {
                    printf("** %04d-%02d-%02d wday:%d, yday:%d\n", ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_wday, ltm.tm_yday);
                }
            }
            else {
                assert(days == monthdays[lastmonth]);
            }
            printf("-- %d月%d天---------------------\n", lastmonth + 1, days);
            lastmonth = ltm.tm_mon;
            days = 0;
        }
        days++;

        printf("%04d-%02d-%02d wday:%d, yday:%d\n", ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_wday, ltm.tm_yday);

        tt += seconds;
    }

    printf("----\n");
}


/*  */
int zs_generate_days(ztl_array_t* arr)
{
    int DayArr[4096] = { 0 };
    int n = GenDays(DayArr, 20160110, 20180305);

    return n;

    //////////////////////////////////////////////////////////////////////////

    char start_date[16] = "2018-01-01";
    char end_date[16] = "2018-03-01";
    printf("holiday %s\n", holidays_raw[0]);

    int seconds = 24 * 60 * 60;
    time_t lNow = time(0);

    struct tm ltm = { 0 };
    localtime_s(&ltm, &lNow);

    ltm.tm_year -= 10;
    ltm.tm_mon = 0;
    ltm.tm_mday = 1;
    ltm.tm_hour = 0;
    ltm.tm_min = 0;
    ltm.tm_sec = 0;

    time_t tt = mktime(&ltm);
    struct tm* lptm2 = localtime(&tt);

    int monthdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    int days = 0;
    int lastmonth = 0;
    for (int i = 0; i < 10000; ++i)
    {
        struct tm ltm = { 0 };
        localtime_s(&ltm, &tt);
        if (lastmonth != ltm.tm_mon)
        {
            if (lastmonth == 1) {
                assert(days >= 28);
                assert(days <= 29);
                if (days == 29) {
                    printf("** %04d-%02d-%02d wday:%d, yday:%d\n", ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_wday, ltm.tm_yday);
                }
            }
            else {
                assert(days == monthdays[lastmonth]);
            }
            printf("-- %d月%d天---------------------\n", lastmonth + 1, days);
            lastmonth = ltm.tm_mon;
            days = 0;
        }
        days++;

        printf("%04d-%02d-%02d wday:%d, yday:%d\n", ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_wday, ltm.tm_yday);

        tt += seconds;
    }

    printf("----\n");
}

struct open_close_time 
{
    int hour : 8;
    int minute : 8;
    int flag : 16
};

struct zs_trading_calendar_s
{
    char        calendar_name[16];  // 交易日历名字
    char        market_name[16];    // 交易日历名字
    char        start_date[16];     // 开始时间
    char        end_date[16];       // 结束时间
    int64_t     start_session;      // 
    int64_t     end_session;        // 
    int         include_weekend;    // 是否包含周末
    ztl_array_t holidays;           // 节假日
    ztl_array_t all_days;           // 所有交易日
    ztl_array_t all_minutes;        // 所有交易日的分钟
    ztl_array_t open_close;         // 各个交易日的开收盘时间
};

typedef struct zs_trading_calendar_s zs_trading_calendar_t;

/* 根据交易日历生成各个clock事件 */
int zs_clocks(zs_trading_calendar_t* trading_calendar);


/* 根据假期和是否包含周末生成交易日历 */
static int64_t _conv_datetime(struct tm* ztm, const char datetime[])
{
    struct tm ltm = { 0 };
    if (!ztm) {
        ztm = &ltm;
    }

    int64_t ret = -1;
    int len = strlen(datetime);
    if (len == 8)
    {
        ztm->tm_year = atoi_n(datetime, 4);
        ztm->tm_mon = atoi_n(datetime + 4, 2);
        ztm->tm_mday = atoi_n(datetime + 6, 2);
    }
    else if (len == 10)
    {
        ztm->tm_year = atoi_n(datetime, 4);
        ztm->tm_mon = atoi_n(datetime + 5, 2);
        ztm->tm_mday = atoi_n(datetime + 8, 2);
    }
    else if (len == 16 || len == 19)
    {
        ztm->tm_year = atoi_n(datetime, 4);
        ztm->tm_mon = atoi_n(datetime + 5, 2);
        ztm->tm_mday = atoi_n(datetime + 8, 2);

        ztm->tm_hour = atoi_n(datetime + 11, 2);
        ztm->tm_min = atoi_n(datetime + 14, 2);
        if (len == 19)
            ztm->tm_sec = atoi_n(datetime + 17, 2);
    }
    else {
        return ret;
    }

    ret = ztm->tm_year * 10000000000ULL + ztm->tm_mon * 100000000 + ztm->tm_mday * 1000000 +
        ztm->tm_hour * 10000 + ztm->tm_min * 100 + ztm->tm_sec;

    return ret;
}

zs_trading_calendar_t* zs_tc_create(const char start_date[], const char end_date[], const ztl_array_t* holidays, int include_weekend)
{
    int64_t key;
    ztl_map_t* hmap;
    hmap = ztl_map_create(ztl_array_size(holidays) + 1);
    for (uint32_t i = 0; i < ztl_array_size(holidays); ++i)
    {
        const char* lpholiday = ztl_array_at(holidays, i);
        key = _conv_datetime(NULL, lpholiday);
        if (key <= 0) {
            continue;
        }
        
        ztl_map_add(hmap, key, NULL);
    }

    struct tm start_tm = { 0 };
    int64_t starti = _conv_datetime(&start_tm, start_date);
    int64_t endi = _conv_datetime(NULL, end_date);

    // generate days
    int seconds = 24 * 60 * 60;
    int monthdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    struct tm ltm = { 0 };
    time_t lNow = time(0);
    localtime_s(&ltm, &lNow);

    ltm.tm_year = start_tm.tm_year - 1900;
    ltm.tm_mon = start_tm.tm_mon - 1;
    ltm.tm_mday = start_tm.tm_mday;
    ltm.tm_hour = 0;
    ltm.tm_min = 0;
    ltm.tm_sec = 0;
    time_t tt = mktime(&ltm);

    ztl_pool_t* pool = ztl_create_pool(4096);
    ztl_array_t allDays;
    ztl_array_init(&allDays, pool, 4096, sizeof(int64_t));
    ztl_array_t* pAllDays = &allDays;

    int days = 0;
    int lastmonth = 0;
    for (int i = 0; i < 10000; ++i)
    {
        struct tm ltm2 = { 0 };
        localtime_s(&ltm2, &tt);
        if (lastmonth != ltm2.tm_mon)
        {
            if (lastmonth == 1) {
                assert(days >= 28);
                assert(days <= 29);
                if (days == 29) {
                    printf("** %04d-%02d-%02d wday:%d, yday:%d\n", ltm2.tm_year + 1900, ltm2.tm_mon + 1, ltm2.tm_mday, ltm2.tm_wday, ltm2.tm_yday);
                }
            }
            else {
                assert(days == monthdays[lastmonth]);
            }
            printf("-- %d月%d天---------------------\n", lastmonth + 1, days);
            lastmonth = ltm2.tm_mon;
            days = 0;
        }
        days++;

        if (!include_weekend && (ltm2.tm_wday == 5 || ltm2.tm_wday == 6)) {
            continue;
        }

        int64_t ldt = ltm2.tm_year * 10000000000ULL + ltm2.tm_mon * 100000000 + ltm2.tm_mday * 1000000 +
            ltm2.tm_hour * 10000 + ltm2.tm_min * 100 + ltm2.tm_sec;

        if (ldt > endi) {
            break;
        }

        if (ztl_map_find(hmap, ldt)) {
            continue;
        }

        int64_t* lpdt = ztl_array_push(pAllDays);
        *lpdt = ldt;

        printf("%04d-%02d-%02d wday:%d, yday:%d\n", ltm2.tm_year + 1900, ltm2.tm_mon + 1, ltm2.tm_mday, ltm2.tm_wday, ltm2.tm_yday);

        tt += seconds;
    }

    return NULL;
}

zs_trading_calendar_t* zs_tc_create_by_market(const char start_date[], const char end_date[], const char* market_name);
zs_trading_calendar_t* zs_tc_create_by_tradingdays(const char start_date[], const char end_date[], ztl_array_t* trading_days);
int64_t zs_tc_release(zs_trading_calendar_t* tc);

int64_t zs_tc_prev_session(zs_trading_calendar_t* tc, int64_t cur_session);
int64_t zs_tc_next_session(zs_trading_calendar_t* tc, int64_t cur_session);

int64_t zs_tc_prev_minute(zs_trading_calendar_t* tc, int64_t cur_minute);
int64_t zs_tc_next_minute(zs_trading_calendar_t* tc, int64_t cur_minute);

bool zs_tc_is_open(zs_trading_calendar_t* tc, int64_t the_minute);

