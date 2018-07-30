#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <ZToolLib/ztl_utils.h>
#include <ZToolLib/ztl_common.h>

#include <ZDataRepository/zdata_load.h>

void TestLoadCsv(const char* filepath);

int main(int argc, char* argv[])
{
    ZTL_NOTUSED(argc);
    ZTL_NOTUSED(argv);

    printf("ZDataRepository.Test ZToolLib version:%s\n", ZTL_Version);

    const char* filepath = "F:/bigdata/csv/bar1d/SHA_2018-Demo.csv";
    TestLoadCsv(filepath);

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
    bardata = dvec->Datas[0];
    printf("%s,%.2f,%.2f\n", bardata->Instrument, bardata->OpenPrice, bardata->ClosePrice);

    getchar();
}

