/* This is auto-generated code. Edit at your own peril. */
#include <stdio.h>
#include <ZToolLib/ztl_unit_test.h>

#include "ZDataRepository.UnitTest.h"



void RunAllTests(void)
{
    ZuString *output = ZuStringNew();
    ZuSuite* suite = ZuSuiteNew();

    ZuSuiteRun(suite);
    ZuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main(int argc, char* argv[])
{
    //ZTL_NOTUSED(argc);
    //ZTL_NOTUSED(argv);

    //net_init();

    RunAllTests();
    return 0;
}
