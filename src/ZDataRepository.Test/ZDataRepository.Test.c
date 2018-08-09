#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <ZToolLib/ztl_times.h>

#include <ZToolLib/ztl_utils.h>
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

static int arr[] = { 1, 2, 3 };

void TestCalendar()
{
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
            printf("-- %dÔÂ%dÌì---------------------\n", lastmonth + 1, days);
            lastmonth = ltm.tm_mon;
            days = 0;
        }
        days++;

        printf("%04d-%02d-%02d wday:%d, yday:%d\n", ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_wday, ltm.tm_yday);

        tt += seconds;
    }

    printf("----\n");
}
