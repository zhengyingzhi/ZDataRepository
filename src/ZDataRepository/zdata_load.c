#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ZToolLib/ztl_palloc.h>
#include <ZToolLib/ztl_shm.h>
#include <ZToolLib/ztl_utils.h>

#include "zdata_struct.h"

/* default csv index: 
 * instrument, open, close, high, close, volume, turnover, adjust_factor, time
 */
zdatabar_index_t default_index = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

void zdata_set_csv_index(zdatabar_index_t* indexs)
{
    default_index = *indexs;
}

static char* _zdata_csv_next_line(char* pcur, int* plen)
{
    char* pend;
    pend = strstr(pcur, "\n");
    if (!pend) {
        return pend;
    }

    // TODO
    *plen = pend - pcur;
    ++pend;
    return pend;
}

static int _zdata_csv_line_parse(char* pcur, zdatabar_t* bardata)
{
    int lCount = 0;
    char* lArray[32] = { 0 };
    lCount = str_delimiter(pcur, lArray, 32, ',');
    if (lCount < ZDATA_BAR_FIELDS_N)
    {
        return -1;
    }

    strncpy(bardata->Instrument, lArray[default_index.IndexInstrument], 6);
    bardata->OpenPrice  = atof(lArray[default_index.IndexOpenPrice]);
    bardata->ClosePrice = atof(lArray[default_index.IndexClosePrice]);
    bardata->HighPrice  = atof(lArray[default_index.IndexHighPrice]);
    bardata->LowPrice   = atof(lArray[default_index.IndexLowPrice]);
    bardata->Volume     = atoll(lArray[default_index.IndexVolume]);
    bardata->Turnover   = atoll(lArray[default_index.IndexTurnover]);
    bardata->AdjustFactor = atof(lArray[default_index.IndexAdjustFactor]);
    //bardata->Time = 

    return 0;
}

static int _zdata_file_to_shm(const char* datapath, ztl_shm_t** ppshm)
{
    ztl_shm_t*  zshm;
    zshm = ztl_shm_create(datapath, ztl_open_only, ztl_read_only, false);
    if (!zshm) {
        return -1;
    }

    // todo:
    *ppshm = zshm;

    return 0;
}

int zdata_load_csv(const char* datapath, zdatavec_t** ppdvec)
{
    int         rv;
    uint64_t    size;
    ztl_shm_t*  zshm;
    void*       paddr;
    zshm = ztl_shm_create(datapath, ztl_open_only, ztl_read_only, false);
    if (!zshm) {
        return -1;
    }

    // FIX the size!!
    size = 0;
    rv = ztl_shm_truncate(zshm, size);
    if (rv != 0) {
        return -2;
    }

    rv = ztl_shm_map_region(zshm, ztl_read_only);
    if (rv != 0) {
        return -3;
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
    dvec->Datas = (zdatabar_t**)malloc(sizeof(void*) * dvec->Capacity);

    zdatabar_t* bardata;
    int len = 0;
    char* line = (char*)paddr;
    do 
    {
        bardata = (zdatabar_t*)ztl_palloc(pool, sizeof(zdatabar_t));
        if (0 != _zdata_csv_line_parse(line, bardata))
        {
            printf("csv_line_parse failed at %d\n", dvec->Count);
            break;
        }
        dvec->Datas[dvec->Count++] = bardata;

        if (dvec->Count >= dvec->Capacity)
        {
            dvec->Capacity *= 2;
            dvec->Datas = (zdatabar_t**)realloc(dvec->Datas, sizeof(void*) * dvec->Capacity);
        }

        line = _zdata_csv_next_line(line, &len);
    } while (line);

    printf("zdata_load_csv %d lines\n", dvec->Count);

    *ppdvec = dvec;
    return 0;
}

int load_binary_from_file(const char* filepath, zdatavec_t** ppdvec)
{
    int         rv;
    ztl_shm_t*  zshm;
    void*       paddr;
    rv = _zdata_file_to_shm(filepath, &zshm);
    if (rv != 0) {
        return rv;
    }

    paddr = ztl_shm_get_address(zshm);

    ztl_pool_t* pool;
    zdatavec_t* dvec;

    uint32_t poolSize = ZTL_DEFAULT_POOL_SIZE * 256;
    pool = ztl_create_pool(poolSize);

    dvec = (zdatavec_t*)ztl_pcalloc(pool, sizeof(zdatavec_t));
    dvec->Pool = pool;
    dvec->Capacity = 65536;
    dvec->Datas = (zdatabar_t**)malloc(sizeof(void*) * dvec->Capacity);

    // todo: directly parse into dvec

    *ppdvec = dvec;

    return 0;
}

int save_binary_to_file(zdatavec_t* dvec)
{
    return 0;
}

