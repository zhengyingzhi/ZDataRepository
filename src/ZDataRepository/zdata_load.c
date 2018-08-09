#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <ZToolLib/ztl_palloc.h>
#include <ZToolLib/ztl_shm.h>
#include <ZToolLib/ztl_times.h>
#include <ZToolLib/ztl_utils.h>

#include "zdata_load.h"



/* default csv index: 
 * instrument, open, high, low, close, settle, open_interest, adjust_factor, volume, turnover, time
 */
zdatabar_index_t field_index = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

void zdata_csv_index_init(zdatabar_index_t* indexs)
{
    memset(indexs, 0, sizeof(zdatabar_index_t));
    // only need set some fields
    indexs->IndexAdjustFactor = -1;
    indexs->IndexSettlePrice = -1;
    indexs->IndexOpenInterest = -1;
    indexs->IndexTurnover = -1;
}

/* convert time string to std time
 * 2018-01-02, 2018-01-02 09:02:00
 */
static time_t zdata_std_time(const char* timestr, int len)
{
    struct tm ltm = { 0 };
    
    // convert date
    ltm.tm_year = atoi(timestr) - 1900;
    ltm.tm_mon = atoi(timestr + 5) - 1;
    ltm.tm_mday = atoi(timestr + 8);

    if (len >= 19)
    {
        ltm.tm_hour = atoi(timestr + 11);
        ltm.tm_min = atoi(timestr + 14);
    }

    time_t t = mktime(&ltm);
    return t;
}

/* convert time string to std time
 * 2018-01-02, 2018-01-02 09:02:00
 */
static uint32_t _zdata_int_time(const char* timestr, int len)
{
    uint32_t it = 0;
    return it;
}


/* 返回下一行的起始地址
 */
static char* _zdata_csv_next_line(char* pcur, int* plen)
{
    char* pend;
    pend = strstr(pcur, "\n");
    if (!pend) {
        return pend;
    }

    int len = pend - pcur;
    if (pcur[len] == '\r') {
        --len;
    }

    *plen = len;
    ++pend;
    return pend;
}

static int _zdata_csv_line_parse(char* pcur, zdatabar_t* bardata)
{
    int lCount = 0;
    char* lArray[32] = { 0 };
    int lLenArray[32] = { 0 };
    lCount = str_delimiter_ex(pcur, lArray, 32, lLenArray, 32, ',');
    if (lCount < ZDATA_BAR_FIELDS_N)
    {
        if (lCount == 0)
            return 1;
        return -1;
    }

    strncpy(bardata->Instrument, lArray[field_index.IndexInstrument], lLenArray[field_index.IndexInstrument]);
    bardata->OpenPrice  = atof(lArray[field_index.IndexOpenPrice]);
    bardata->ClosePrice = atof(lArray[field_index.IndexClosePrice]);
    bardata->HighPrice  = atof(lArray[field_index.IndexHighPrice]);
    bardata->LowPrice   = atof(lArray[field_index.IndexLowPrice]);
    bardata->Volume     = atoll(lArray[field_index.IndexVolume]);

    if (field_index.IndexTurnover >= 0)
        bardata->Turnover   = atoll(lArray[field_index.IndexTurnover]);

    if (field_index.IndexSettlePrice >= 0)
        bardata->SettlePrice = atof(lArray[field_index.IndexSettlePrice]);

    if (field_index.IndexOpenInterest >= 0)
        bardata->OpenInterest = atof(lArray[field_index.IndexOpenInterest]);

    if (field_index.IndexAdjustFactor >= 0) {
        bardata->AdjustFactor = atof(lArray[field_index.IndexAdjustFactor]);
    }
    else {
        bardata->AdjustFactor = 1.0;
    }

    bardata->DateTime = zdata_std_time(lArray[field_index.IndexDateTime],
        lLenArray[field_index.IndexDateTime]);

    return 0;
}

static int _zdata_file_size(const char* filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fclose(fp);

    return size;
}

static int _zdata_file_to_shm(ztl_shm_t** ppshm, const char* filepath, bool readonly)
{
    int openOrCreate;
    int accessMode;
    if (readonly) {
        openOrCreate = ztl_open_or_create;
        //accessMode = ztl_read_only;
        accessMode = ztl_read_write;
    }
    else {
        openOrCreate = ztl_open_or_create;
        accessMode = ztl_read_write;
    }

    int rv;
    ztl_shm_t*  zshm;
    zshm = ztl_shm_create(filepath, openOrCreate, accessMode, false);
    if (!zshm) {
        return -1;
    }

    // FIX the size!!
    int64_t size = _zdata_file_size(filepath);
    rv = ztl_shm_truncate(zshm, size);
    if (rv != 0) {
        return -2;
    }

    rv = ztl_shm_map_region(zshm, accessMode);
    if (rv != 0) {
        return -3;
    }

    *ppshm = zshm;

    return 0;
}

int zdata_load_from_csv_file(zdatavec_t** ppdvec, const char* filepath, zdatabar_index_t* indexs)
{
    if (indexs)
    {
        field_index = *indexs;
    }

    int         rv;
    ztl_shm_t*  zshm;
    void*       paddr;
    rv = _zdata_file_to_shm(&zshm, filepath, true);
    if (rv != 0) {
        return -1;
    }

    paddr = ztl_shm_get_address(zshm);

    /* parse the data */
    ztl_pool_t* pool;
    zdatavec_t* dvec;

    uint32_t poolSize = ZTL_DEFAULT_POOL_SIZE * 256;
    pool = ztl_create_pool(poolSize);

    dvec = (zdatavec_t*)ztl_pcalloc(pool, sizeof(zdatavec_t));
    dvec->Pool = pool;
    dvec->Capacity = 65536;
    ztl_array_init(&dvec->Datas, pool, dvec->Capacity, dvec->Capacity);

    // todo: parallel parse the csv lines to databar objects
    zdatabar_t* bardata;
    int len = 0;
    char* pcur = (char*)paddr;
    char* line = pcur;

    // except the first line
    pcur = _zdata_csv_next_line(line, &len);

    do 
    {
        line = pcur;
        bardata = (zdatabar_t*)ztl_pcalloc(pool, sizeof(zdatabar_t));
        if ((rv != _zdata_csv_line_parse(line, bardata)) != 0)
        {
            if (rv < 0)
                printf("csv_line_parse failed at %d\n", dvec->Count);
            break;
        }

        ztl_array_push_back(&dvec->Datas, bardata);

        pcur = _zdata_csv_next_line(line, &len);
    } while (pcur);

    printf("zdata_load_csv %d lines\n", dvec->Count);

    *ppdvec = dvec;
    return 0;
}

int zdata_load_from_binary_file(zdatavec_t** ppdvec, const char* filepath)
{
    int         rv;
    ztl_shm_t*  zshm;
    void*       paddr;

    rv = _zdata_file_to_shm(&zshm, filepath, false);
    if (rv != 0) {
        return -1;
    }

    paddr = ztl_shm_get_address(zshm);

    // parse into process container
    ztl_pool_t* pool;
    zdatavec_t* dvec;

    uint32_t poolSize = ZTL_DEFAULT_POOL_SIZE * 256;
    pool = ztl_create_pool(poolSize);

    dvec = (zdatavec_t*)ztl_pcalloc(pool, sizeof(zdatavec_t));
    dvec->Pool = pool;
    dvec->Capacity = 65536;
    ztl_array_init(&dvec->Datas, pool, dvec->Capacity, dvec->Capacity);

    // directly parse into dvec
    zdatabar_t* bardata;
    bardata = (zdatabar_t*)paddr;
    while (bardata->Instrument[0])
    {
        void** lpdst = (void**)ztl_array_push(&dvec->Datas);
        *lpdst = bardata;

        // next bar data
        bardata += 1;
    }

    *ppdvec = dvec;

    return 0;
}

int zdata_save_to_binary_file(const char* datapath, const zdatavec_t* dvec)
{
    return 0;
}




void zdata_dvec_reset(zdatavec_t* dvec)
{
}
