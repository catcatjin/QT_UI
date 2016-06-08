#include "randomgeneratortest.h"
#include <time.h>

RandomGeneratorTest::RandomGeneratorTest() : m_dwRandomNumber(NULL)
{
    DWORD lNGPIOavail;

    FD_GetNGPIOAvail(&lNGPIOavail);

    m_bAvail = TEST_BIT(lNGPIOavail, RANDOM) > 0;
}

RandomGeneratorTest::~RandomGeneratorTest()
{
    if(m_dwRandomNumber != NULL)
    {
        delete [] m_dwRandomNumber;
    }
}

void RandomGeneratorTest::diagnostic()
{
    std::string sTail;
    time_t tLastTime(0) , tNow(0);

    m_vRepeat.clear();

    if(!m_bAvail)
    {
        printf("%s function not available!", sTestFunctionName[RANDOM_GEN_TEST].c_str());
        m_eResult = NOT_AVAIL;
        goto End;
    }

    m_nAmount = RAND_TIMES;

    if(m_dwRandomNumber != NULL &&
        sizeof(m_dwRandomNumber) < m_nAmount*sizeof(DWORD))
    {
        delete [] m_dwRandomNumber;
        m_dwRandomNumber = NULL;
    }

    if(m_dwRandomNumber == NULL)
        m_dwRandomNumber = new DWORD[RAND_TIMES];

    ClearScreen();
    printf("Diagnostic %s : \n\n", sTestFunctionName[RANDOM_GEN_TEST].c_str());
    printf("==========================================================================\n");

    for(int i = 0 ; i < m_nAmount ; i ++)
    {
        tNow = time(NULL);
        if(tNow != tLastTime)
        {
            tLastTime = tNow;

            OutputTextTool::showTextWithVariableDots(sTestFunctionName[RANDOM_GEN_TEST].c_str(), i, m_nAmount);
        }

        FPGA_API_STATUS dwStatus = (FPGA_API_STATUS)FD_GetRandom(&m_dwRandomNumber[i]);

        if(dwStatus != FD_SUCCESS)
        {
            Error("diagnostic", "get random value", dwStatus);
            m_eResult = TEST_ERROR;
            goto End;
        }

        for(int j = 0 ; j < i; j++)
        {
            if(m_dwRandomNumber[i] == m_dwRandomNumber[j])
            {
                m_vRepeat.push_back(m_dwRandomNumber[i]);
            }
        }
    }

    m_eResult = PASS;

End:

    printf("\r%s (%d/%d)\n", sTestFunctionName[RANDOM_GEN_TEST].c_str(), m_nAmount, m_nAmount);
    printf("==========================================================================\n");

    showResult();
}

void RandomGeneratorTest::showResult()
{
    printf("\nDiagnostic Result: \n");
    printf("==========================================================================\n");

    int repeat = (int)m_vRepeat.size();
    int threadhold = (int)(TOLERANCE_RATE * m_nAmount);


    if(m_eResult == PASS && repeat > threadhold)
    {
        m_eResult = FAIL;
    }

    m_sResult = resultEnumToString(m_eResult);

    if(g_bDetailInfo)
    {
        printf("Repeat %u times (%.3f%%):\nNumber = \n", repeat, (double)repeat*100/m_nAmount);
        for(int i = 0 ; i < repeat; i++)
        {
            if(i % 5 == 0)
                printf("\n");

            printf("%u%s ", m_vRepeat[i], i < repeat - 1 ? "," : "");
        }

        PrintDetailInfo("\n\n");
    }

    printf("\n--> \"%s\"\n\n", m_sResult.c_str());

    char result[256];
    sprintf(result, "[%d] %s\t...  %s!!\n\n", RANDOM_GEN_TEST,
        sTestFunctionName[RANDOM_GEN_TEST].c_str(), m_sResult.c_str());

    m_sResult.assign(result);

    printf("\n");

    if(g_bStepPause)
        SystemPause();
}

