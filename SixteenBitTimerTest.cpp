#include "sixteenbittimertest.h"

#include <time.h>

#define GET_MILLISECONDS(time) (time.wSecond * 1000 + time.wMilliseconds)
#define MILLISECONDS_OF_ONE_MIN	60000	// 1 min = 60 s = 60000 ms

#ifndef WINDOWS
void GetSystemTime(SYSTEMTIME *systime)
{
    timeval time;
    gettimeofday(&time, 0);

    systime->wSecond = time.tv_sec;
    systime->wMilliseconds = time.tv_usec / 1000;
}
#endif

#define RAND(base, range)	(rand()%range + base)

SixteenBitTimerTest::SixteenBitTimerTest() : InterruptProcess(TIMER_IRQ_TYPE, IRQ_TIMER)
{
    DWORD lNGPIOavail;

    FD_GetNGPIOAvail(&lNGPIOavail);

    for(int i = TIMER_ID_0; i < MAX_TIMER ; i++)
    {
        m_bAvail[i] = TEST_BIT(lNGPIOavail, TIMER_0 + 1) > 0;

        if(m_bAvail[i])
            SET_BIT(m_stIRQparam.NIO, i);

        m_pstSystemTriggerTime[i] = NULL;
        m_puSystemTriggerInterval[i] = NULL;
        m_pulFPGATriggerCounting[i] = NULL;
    }

    // offset to NGPIO timer
    m_stIRQparam.NIO <<= TIMER_0;
}

SixteenBitTimerTest::~SixteenBitTimerTest(void)
{
    FD_IntDisable();

    releaseDynamicBuffer();
}

bool SixteenBitTimerTest::isNeedReallocBuffer(int totalAmount)
{
    return sizeof(m_pulFPGATriggerCounting[0]) <= totalAmount * sizeof(unsigned long);
}

void SixteenBitTimerTest::createDynamicBuffer(int totalAmount)
{
    for(int i = TIMER_ID_0 ; i < MAX_TIMER ; i++)
    {
        if(m_pstSystemTriggerTime[i] == NULL)
        {
            m_pstSystemTriggerTime[i] = (SYSTEMTIME*)calloc(totalAmount, sizeof(SYSTEMTIME));
        }

        if(m_puSystemTriggerInterval[i] == NULL)
        {
            m_puSystemTriggerInterval[i] = (UINT*)calloc(totalAmount, sizeof(UINT));
        }

        if(m_pulFPGATriggerCounting[i] == NULL)
        {
            m_pulFPGATriggerCounting[i] = (unsigned long*)calloc(totalAmount, sizeof(unsigned long));
        }
    }
}

void SixteenBitTimerTest::releaseDynamicBuffer()
{
    for(int i = TIMER_ID_0 ; i < MAX_TIMER ; i++)
    {
        if(m_pstSystemTriggerTime[i] != NULL)
        {
            free(m_pstSystemTriggerTime[i]);
            m_pstSystemTriggerTime[i] = NULL;
        }

        if(m_puSystemTriggerInterval[i] != NULL)
        {
            free(m_puSystemTriggerInterval[i]);
            m_puSystemTriggerInterval[i] = NULL;
        }

        if(m_pulFPGATriggerCounting[i] != NULL)
        {
            free(m_pulFPGATriggerCounting[i]);
            m_pulFPGATriggerCounting[i] = NULL;
        }
    }
}

bool SixteenBitTimerTest::isAvail(TIMER_ID id)
{
    return m_bAvail[id];
}

unsigned long SixteenBitTimerTest::getTick(TIMER_ID id)
{
    if(!m_bAvail[id])
        return 0;

    unsigned long tick = 0;

    FD_GetTMRCount(id, &tick);

    return tick;
}

void SixteenBitTimerTest::DealInterrupt(DWORD reg)
{
    BYTE bsTimerFinish(0);

    for(int i = TIMER_ID_0; i < MAX_TIMER; i++)
    {
        if( TEST_BIT(reg, i) && (m_nTriggerCount[i][m_eIntervalMode] < m_nAmount[m_eIntervalMode]))
        {
            int offset = (m_eIntervalMode == ALL_TIMER_SAME ? 0 : m_nAmount[ALL_TIMER_SAME]) + m_nTriggerCount[i][m_eIntervalMode];

            // record time stamp
            m_pulFPGATriggerCounting[i][offset] = getTick((TIMER_ID)i);

            GetSystemTime(&m_pstSystemTriggerTime[i][offset]);

            PrintDetailInfo("Time%d trigger interrupt in  %2d.%03ds & %7d.%03dms\n", i,
                m_pstSystemTriggerTime[i][offset].wSecond, m_pstSystemTriggerTime[i][offset].wMilliseconds,
                m_pulFPGATriggerCounting[i][offset]/1000, m_pulFPGATriggerCounting[i][offset]%1000);

            m_nTriggerCount[i][m_eIntervalMode]++;
        }

        // counter is equal amount or not available
        if(m_nTriggerCount[i][m_eIntervalMode] >= m_nAmount[m_eIntervalMode] || !m_bAvail[i])
            SET_BIT(bsTimerFinish, i);
    }

    int nAllTimerFinishValue = (1 << (MAX_TIMER + 1)) - 1;

    if(bsTimerFinish == nAllTimerFinishValue)
    {
        m_bAllTimerReachAmount = true;
    }
}

void SixteenBitTimerTest::reset(TIMER_ID id, INTERVAL_MODE mode)
{
    m_ulFPGAStartCounting[id][mode] = 0;

    m_nTriggerCount[id][mode] = 0;

    memset(&m_stSystemStartTime[id][mode], 0, sizeof(SYSTEMTIME));
}

void SixteenBitTimerTest::resetAll(INTERVAL_MODE mode)
{
    for(int i = TIMER_ID_0; i < MAX_TIMER; i++)
    {
        reset((TIMER_ID)i, mode);
    }
}

void SixteenBitTimerTest::setInterval(TIMER_ID id, unsigned short interval, INTERVAL_MODE mode)
{
    m_nInterval[id] = interval;

    if(!m_bAvail[id])
        return ;

    FPGA_API_STATUS dwStatus = (FPGA_API_STATUS)FD_SetTMRInterval((BYTE)id, interval);

    if(dwStatus != FD_SUCCESS)
    {
        char sID[64];
        sprintf(sID, "Timer%d", id);
        Error("setInterval", sID, dwStatus);
        m_eTimerResult = TEST_ERROR;
        return;
    }

    GetSystemTime(&m_stSystemStartTime[id][mode]);

    m_ulFPGAStartCounting[id][mode] = getTick(id);

}

void SixteenBitTimerTest::setIntervalAll(unsigned short time)
{
    for(int i = TIMER_ID_0; i < MAX_TIMER; i++)
    {
        if(m_eTimerResult != TEST_ERROR)
        {
            setInterval((TIMER_ID)i, time, ALL_TIMER_SAME);
        }
    }
}

void SixteenBitTimerTest::sameIntervaldiagnostic(int amount, int interval)
{
    m_eIntervalMode = ALL_TIMER_SAME;
    m_bAllTimerReachAmount = false;
    m_nAmount[ALL_TIMER_SAME] = amount;
    m_nSameInterval = interval;	// ms

    resetAll(ALL_TIMER_SAME);

    setIntervalAll(interval);

    if(m_eTimerResult == TEST_ERROR ||
        !enableInterrupt())
    {
        m_eTimerResult = TEST_ERROR;
        return;
    }

    // assert step * sleep time can finish the trigger count
    int step = (amount * interval / SLEEP_MS_TIME) + SLEEP_EXTRA_WAIT_TIMES;

    do
    {
        if(!g_bDetailInfo)
            OutputTextTool::waitText(SLEEP_MS_TIME);

        Sleep(SLEEP_MS_TIME);
        step--;
    }
    while(!m_bAllTimerReachAmount && step != 0);

    if(!g_bDetailInfo)
        printf("\r");

    disableInterrupt();

    m_eTimerResult = PASS;
}

void SixteenBitTimerTest::diffIntervaldiagnostic(int amount, int interval[MAX_TIMER])
{
    int maxTime = 0;

    m_eIntervalMode = ALL_TIMER_DIFF;
    m_bAllTimerReachAmount = false;
    m_nAmount[ALL_TIMER_DIFF] = amount;

    resetAll(ALL_TIMER_DIFF);

    for(int i = TIMER_ID_0; i < MAX_TIMER; i++)
    {
        setInterval((TIMER_ID)i, interval[i], ALL_TIMER_DIFF);

        if(m_eTimerResult == TEST_ERROR)
            return;

        if(maxTime < interval[i])
            maxTime = interval[i];
    }

    if(!enableInterrupt())
    {
        m_eTimerResult = TEST_ERROR;
        return;
    }

    // assert step * sleep time can finish the trigger count
    int step = (amount * maxTime / SLEEP_MS_TIME) + SLEEP_EXTRA_WAIT_TIMES;

    do
    {
        if(!g_bDetailInfo)
            OutputTextTool::waitText(SLEEP_MS_TIME);

        Sleep(SLEEP_MS_TIME);
        step--;
    }
    while(!m_bAllTimerReachAmount && step != 0);

    PrintDetailInfo("\r");

    disableInterrupt();

    m_eTimerResult = PASS;
}

void SixteenBitTimerTest::generateRandomInterval(int *interval, int baseTime, int randRange)
{
    srand((UINT)time(NULL));

    for(int i = TIMER_ID_0; i < MAX_TIMER; i++)
    {
        interval[i] = RAND(baseTime, randRange);
    }
}

void SixteenBitTimerTest::diagnostic(int interval, int amount_same , int amount_diff ,
                                  int baseTime, int randRange ) // 200ms~2s
{
    if(!m_bAvail[TIMER_ID_0] && !m_bAvail[TIMER_ID_1] &&
        !m_bAvail[TIMER_ID_2] && !m_bAvail[TIMER_ID_2])
    {
        printf("%s function not available!", sTestFunctionName[TIMER_16BIT_TEST].c_str());
        return;
    }

    int totalAmount = amount_same + amount_diff;

    if(isNeedReallocBuffer(totalAmount))
        releaseDynamicBuffer();

    createDynamicBuffer(totalAmount);

    ClearScreen();
    printf("Diagnostic %s : \n\n", sTestFunctionName[TIMER_16BIT_TEST].c_str());
    printf("==========================================================================\n");

    printf("Set all %s interval %u \n", sTestFunctionName[TIMER_16BIT_TEST].c_str(), interval);
    printf("--------------------------------------------------------------------------\n");
    printf("Start %s diagnostic, part 1 (1/2) \n", sTestFunctionName[TIMER_16BIT_TEST].c_str());
    printf("--------------------------------------------------------------------------\n");


    sameIntervaldiagnostic(amount_same, interval);

    PrintDetailInfo("\n\t\t\t\t\t\t\t");
    printf("Part 1 finish !\n");

    printf("==========================================================================\n");

    int randInterval[MAX_TIMER];

    generateRandomInterval(randInterval, baseTime, randRange);


    printf("\nSet Timer0 -> %dms, Timer1 -> %dms, Timer2 -> %dms, Timer3 -> %dms\n",
        randInterval[TIMER_ID_0], randInterval[TIMER_ID_1], randInterval[TIMER_ID_2], randInterval[TIMER_ID_3]);
    printf("--------------------------------------------------------------------------\n");
    printf("Start %s diagnostic, part 2 (2/2) \n", sTestFunctionName[TIMER_16BIT_TEST].c_str());
    printf("--------------------------------------------------------------------------\n");

    if(m_eTimerResult != TEST_ERROR)
        diffIntervaldiagnostic(amount_diff, randInterval);

    PrintDetailInfo("\n\t\t\t\t\t\t\t");
    printf("Part 2 finish !\n");

    printf("==========================================================================\n");

    if(g_bDetailInfo && g_bStepPause)
        SystemPause();

    showResult();
}

int SixteenBitTimerTest::subSystemTime(SYSTEMTIME& time1, SYSTEMTIME& time2)
{
    int secDiff= GET_MILLISECONDS(time1) - GET_MILLISECONDS(time2);

    return secDiff >= 0 ? secDiff : secDiff + MILLISECONDS_OF_ONE_MIN;
}

void SixteenBitTimerTest::showResult()
{
    char result[256];
    BYTE bsFail(0);

    if(g_bDetailInfo)
        ClearScreen();
    PrintDetailInfo("%s diagnostic Result: \n", sTestFunctionName[TIMER_16BIT_TEST].c_str());

    UINT maxFPGACount = 0, minFPGACount = 0xFFFFFFFF;
    UINT maxSystemTime = 0, minSystemTime = 0xFFFFFFFF;

    for(int j = ALL_TIMER_SAME; j < MAX_INTERVAL_MODE && m_eTimerResult != TEST_ERROR ; j++)
    {
        printf("==========================================================================\n");

        if(j == ALL_TIMER_SAME)
        {
            printf("\nAll timers with the same interval %d ms :\n", m_nSameInterval);
            printf("--------------------------------------------------------------------------\n");
        }
        else
        {
            printf("\nEach Timer with difference intervals => \n");
            for(int idx = TIMER_ID_0 ; idx < MAX_TIMER ; idx++)
                printf("Timer%d -> %4dms%s", idx,  m_nInterval[idx], idx < TIMER_ID_3 ? ", " : "");

            printf(" :\n--------------------------------------------------------------------------\n");
        }

        int start = j * m_nAmount[ALL_TIMER_SAME], end = start + m_nAmount[j];
        for(int idx = TIMER_ID_0 ; idx < MAX_TIMER ; idx++)
        {
            if(!m_bAvail[idx])
            {
                PrintDetailInfo("-----------------------------------");
                printf("---------> \"not available!\"\n");
                continue;
            }

            maxFPGACount = 0;
            minFPGACount = 0xFFFFFFFF;
            maxSystemTime = 0;
            minSystemTime = 0xFFFFFFFF;

            for(int i = end - 1; i >= start ; i--)
            {
                m_pulFPGATriggerCounting[idx][i] -= (i > start ? m_pulFPGATriggerCounting[idx][i - 1] : m_ulFPGAStartCounting[idx][j]);

                if(maxFPGACount < m_pulFPGATriggerCounting[idx][i])
                    maxFPGACount = m_pulFPGATriggerCounting[idx][i];

                if(minFPGACount > m_pulFPGATriggerCounting[idx][i])
                    minFPGACount = m_pulFPGATriggerCounting[idx][i];

                m_puSystemTriggerInterval[idx][i] = subSystemTime( m_pstSystemTriggerTime[idx][i],
                                                    i > start ? m_pstSystemTriggerTime[idx][i - 1] : m_stSystemStartTime[idx][j]);

                if(maxSystemTime < m_puSystemTriggerInterval[idx][i])
                    maxSystemTime = m_puSystemTriggerInterval[idx][i];

                if(minSystemTime > m_puSystemTriggerInterval[idx][i])
                    minSystemTime = m_puSystemTriggerInterval[idx][i];
            }

            printf("Timer%d |\n",idx);
            PrintDetailInfo("--------\n",idx);

            if(m_nTriggerCount[idx][j] != m_nAmount[j])
            {
                printf("=> Wait timeout! amount count wrong... \n");
                printf("--------------------------------------------> \"Fail\"\n");

                SET_BIT(bsFail, idx);
                continue;
            }


            bool bPass = true;
            double dDeviationMax, dDeviationMin;
            int interval = (j == ALL_TIMER_SAME ? m_nSameInterval : m_nInterval[idx]);



            PrintDetailInfo("RTC  trigger  time range: %.03u~%.03ums\n", minSystemTime, maxSystemTime);

            // verify by SYSTEM tolerance value
            dDeviationMax = ((double)maxSystemTime - interval) * 100 / interval;
            dDeviationMin = ((double)minSystemTime - interval) * 100 / interval;

            if(abs((int)dDeviationMax > TOLERANCE_SYSTEM_TIME) || abs((int)dDeviationMin) > TOLERANCE_SYSTEM_TIME)
            {
                printf("=> RTC deviation too large!\n");
                printf("------------------------------------------> \"Fail\"\n");

                SET_BIT(bsFail, idx);
                bPass = false;
            }

            PrintDetailInfo("FPGA trigger count range: %.3f~%.3fms\n", (double)minFPGACount/1000, (double)maxFPGACount/1000);

            // verify by FPGA tolerance value
            // FPGA count unit is micro-second, so need to multiply 1000
            interval *= 1000;
            dDeviationMax = ((double)maxFPGACount - interval) * 100 / (interval);
            dDeviationMin = ((double)minFPGACount - interval) * 100 / (interval);

            if(abs((int)dDeviationMax) > TOLERANCE_FPGA_COUNT || abs((int)dDeviationMin) > TOLERANCE_FPGA_COUNT)
            {
                printf("=> FPGA count deviation too large!\n");
                printf("--------------------------------------------> \"Fail\"\n");

                SET_BIT(bsFail, idx);
                bPass = false;
            }

            if(bPass)
            {
                PrintDetailInfo("-----------------------------------");
                printf("---------> \"pass\"\n");
            }
        }
    }

    sprintf(result, "[%d] %s:\n", TIMER_16BIT_TEST, sTestFunctionName[TIMER_16BIT_TEST].c_str());
    m_sResult.assign(result);

    if(m_eTimerResult == TEST_ERROR)
    {
        sprintf(result, "\t All Timers \t\t...  Error!!\n");

        m_sResult.append(result);
        printf(result);
    }
    else
    {
        for(int i = TIMER_ID_0; i < MAX_TIMER; i++)
        {
            sprintf(result, "\tTimer %d\t\t\t...  %s!!\n", i,
                m_bAvail[i] ? ( TEST_BIT(bsFail,  i) ? "Fail" : "Pass") : "Not available");

            m_sResult.append(result);
        }

        m_sResult += "\n";
    }

    printf("\n");

    if(g_bStepPause)
        SystemPause();
}

