#include "PICIntrusionTest.h"


typedef struct
{
    DWORD Year;
    DWORD Month;
    DWORD Day;
    DWORD Hour;
    DWORD Minute;
    DWORD Second;
} BufferTime;

void Tm2Buffer(BufferTime* buffer, tm * localTime)
{
    localTime->tm_sec	= buffer->Second;
    localTime->tm_min	= buffer->Minute;
    localTime->tm_hour	= buffer->Hour;
    localTime->tm_mday	= buffer->Day;
    localTime->tm_mon	= buffer->Month - 1;
    localTime->tm_year	= buffer->Year - 1900;
}

void Buffer2Tm(tm * localTime, BufferTime* buffer)
{
    // write current time
    buffer->Second = localTime->tm_sec;
    buffer->Minute = localTime->tm_min;
    buffer->Hour = localTime->tm_hour;
    buffer->Day = localTime->tm_mday;
    buffer->Month = localTime->tm_mon + 1;
    buffer->Year = localTime->tm_year + 1900;
}

PICIntrusionTest::PICIntrusionTest(MODEL_TYPE modelType) : InterruptProcess(PIC_IRQ_TYPE, IRQ_OTHER)
{
    DWORD dwAvail;
    FD_GetNGPIOAvail(&dwAvail);

    m_bAvail = TEST_BIT(dwAvail, PIC);

    if(m_bAvail)
        SET_BIT(m_stIRQparam.NIO, PIC);

    m_eModelType = modelType;

    m_nWatchDog = 0;
    getMaxValidDoor();
}

PICIntrusionTest::~PICIntrusionTest()
{
}

void PICIntrusionTest::serialNumberTest()
{
    if(eModel != PGB_5322 && eModel != GCB_203 && eModel != PGB_5321R1)
    {
        m_eSNResult = NOT_AVAIL;
        PrintDetailInfo(resultEnumToString(NOT_AVAIL).c_str());
        return;
    }

    BYTE sn[MAX_SN_LEN];

    memset(sn, 0, MAX_SN_LEN);
    FPGA_API_STATUS status = (FPGA_API_STATUS)PIC_GetSN(sn);

    if(status != FD_SUCCESS)
    {
        Error("serialNumberTest", "PIC_GetSN", status);
        m_eSNResult = TEST_ERROR;
        return;
    }

    printf("Serial number : ");

    for(int i = 0; i < MAX_SN_LEN ; i++)
        printf("%02x", sn[i]);

    printf("\n");

    m_eSNResult = PASS;
}

void PICIntrusionTest::RTCTest()
{
    tm *stCurrent, stPICtime;
    time_t tCurrent = time(NULL);

    BufferTime datetime;
    memset(&datetime, 0, sizeof(BufferTime));

    stCurrent = localtime(&tCurrent);

    // write current time
    Buffer2Tm(stCurrent, &datetime);

    FPGA_API_STATUS status = (FPGA_API_STATUS)PIC_WriteRTC((DWORD*)&datetime);
    if(status != FD_SUCCESS)
    {
        Error("RTCTest", "PIC_WriteRTC", status);
        m_eRTCResult = TEST_ERROR;
        return;
    }

    // read and check
    memset(&datetime, 0, sizeof(BufferTime));

    tCurrent = time(NULL);
    status = (FPGA_API_STATUS)PIC_ReadRTC((DWORD*)&datetime);
    if(status != FD_SUCCESS)
    {
        Error("RTCTest", "PIC_ReadRTC", status);
        m_eRTCResult = TEST_ERROR;
        return;
    }

    Tm2Buffer(&datetime, &stPICtime);

    time_t tPICtime = mktime(&stPICtime);

    double dTimeDif = difftime(tPICtime, tCurrent);
    if(dTimeDif > RTC_THRESHOLD || dTimeDif < -1 * RTC_THRESHOLD)
    {
        m_eRTCResult = FAIL;
        PrintDetailInfo("RTC time diff = %.1f larger than %d sec\n", dTimeDif, RTC_THRESHOLD);
        return;
    }

    PrintDetailInfo("RTC time diff = %.1f smaller than %d sec\n", dTimeDif, RTC_THRESHOLD);

    m_eRTCResult = PASS;
}

void PICIntrusionTest::getMaxValidDoor()
{
    switch (m_eModelType)
    {
    case PGB_5021:
    case PGB_6040:
        m_bsValidDoor = 0x3f;
        break;
    case PGB_4020:
        m_bsValidDoor = 0x6f;
        break;
    case PGB_5120S:
    case PGB_5320:
    case PGB_5322:
    case GCB_203:
    case PGB_5321R1:
    default:
        m_bsValidDoor = 0xff;
        break;
    }
}

void PICIntrusionTest::showTriggerTable()
{
    m_bDrawing = true;

    ClearScreen();
    printf("Diagnostic %s : \n\n", sTestFunctionName[RTC_INSTURSION_TEST].c_str());
    printf("==========================================================================\n");
    printf("\nPart 3. Instrusion diagnostic\n");
    printf("--------------------------------------------------------------------------\n");

    int count = 0;
    BYTE validDoor = m_bsValidDoor;

    while (validDoor >>= 1)
    {
        count++;
    }
    printf("Please press the all valid available intrusion from 0 to %d ...\n\n", count);


    for(int row = 0 ; row < INTRUSION_COUNT_IN_EACH_ROW ; row++)
    {
        if(row == 0)
				printf(" ========");
        else if(row == INTRUSION_COUNT_IN_EACH_ROW - 1)
            printf("|======== \n");
        else
            printf("|========");
    }

    printf("|");

    for(int row = 0, index = 0 ; row < INTRUSION_COUNT_IN_EACH_ROW ; row++, index++)
    {
        if( TEST_BIT(m_bsValidDoor, index))
            printf(" Door %d |", index);
        else
            printf("       |");
    }

    printf("\n|========");

    for(int row = 1 ; row < INTRUSION_COUNT_IN_EACH_ROW ; row++)
    {
        printf("|========");
    }
    printf("|\n|");

    for(int row = 0, index = 0 ; row < INTRUSION_COUNT_IN_EACH_ROW ; row++, index++)
    {
        if( TEST_BIT(m_bsValidDoor, index))
            printf("%s |", TEST_BIT(m_bsDown, index) ? "  Down " : TEST_BIT(m_bsTrigger, index) ? "   Ok  " : " Press ");
        else
            printf("       |");
    }
    printf("\n");

    for(int row = 0 ; row < INTRUSION_COUNT_IN_EACH_ROW ; row++)
    {
        if(row == 0)
            printf(" ========");
        else if(row == INTRUSION_COUNT_IN_EACH_ROW - 1)
            printf("|======== \n\n");
        else
            printf("|========");
    }

    m_bDrawing = false;
}

void PICIntrusionTest::DealInterrupt(DWORD reg)
{
    BYTE index;
    bool bTrigger = false;

    for(int i = 0; i < MAX_PIC_INTRUSIONS; i++)
    {
        if(TEST_BIT(m_bsValidDoor, i))
        {
            PIC_GetCurrentIndex(i, &index);

            if(index != m_bsEventIndex[i])
            {
                m_tEventTime[i] = time(NULL);
                PIC_GetEventByIndex(i, index, m_dwEventLog[i]);

                SET_BIT(m_bsTrigger, i);

                // check by odd and event because call the PIC_CleanAllEvent first
                if(index%2 == 0)
                    SET_BIT(m_bsDown, i);
                else
                    RESET_BIT(m_bsDown, i);

                m_bsEventIndex[i] = index;
                bTrigger = true;
            }
        }
    }

    m_nWatchDog = 0;

    if(bTrigger)
        showTriggerTable();
}

void PICIntrusionTest::intrusionTest()
{
    FPGA_API_STATUS status;

    m_bsDown = 0;
    m_bsTrigger = ~m_bsValidDoor;

    #pragma region Special situation process
    //set door debounce to 40ms
    if (m_eModelType == PGB_5322 ||
            m_eModelType == GCB_203)
    {
        for (int i = 0; i < MAX_PIC_INTRUSIONS; i++)
        {
            if( TEST_BIT(m_bsValidDoor, i))
            {
                status = (FPGA_API_STATUS)PIC_SetDoorDBByIndex((BYTE)i, SPECIAL_MODEL_DOOR_DB);
                if(status != FD_SUCCESS)
                {
                    Error("diagnostic", "PIC_SetDoorDBByIndex", status);
                    m_eIntrusionResult = TEST_ERROR;
                    return;
                }
            }
        }
    }
    #pragma endregion

    status = (FPGA_API_STATUS)PIC_CleanAllEvent();
    if(status != FD_SUCCESS)
    {
        Error("diagnostic", "PIC_CleanAllEvent", status);
        m_eIntrusionResult = TEST_ERROR;
        return;
    }

    // save log index for each intrusion
    for(int i = 0; i < MAX_PIC_INTRUSIONS; i++)
    {
        if(TEST_BIT(m_bsValidDoor, i))
        {
            status = (FPGA_API_STATUS)PIC_GetCurrentIndex(i, &m_bsEventIndex[i]);
            if(status != FD_SUCCESS)
            {
                Error("intrusionTest", "PIC_GetCurrentIndex", status);
                m_eIntrusionResult = TEST_ERROR;
                return;
            }
        }
    }

    showTriggerTable();

    status = (FPGA_API_STATUS)PIC_EnableInt(m_bsValidDoor);
    if(status != FD_SUCCESS)
    {
        Error("diagnostic", "PIC_SetDoorDBByIndex", status);
        m_eIntrusionResult = TEST_ERROR;
        return;
    }

    if(!enableInterrupt())
    {
        m_eIntrusionResult = TEST_ERROR;
        return;
    }

    #pragma region Wait until all intrusions are triggered or timeout
    BYTE bsAllTriggerValue = ~0x0;
    int nMaxCnt = MAX_WAIT_TIME / TRIGGER_DETECT_INTERVAL;
    m_nWatchDog = 0;

    do
    {
        if(!m_bDrawing)
            OutputTextTool::waitText(SLEEP_MS_TIME);

        Sleep(TRIGGER_DETECT_INTERVAL);

        m_nWatchDog++;

    }
    while(m_nWatchDog < nMaxCnt && m_bsTrigger != bsAllTriggerValue);

    #pragma endregion

    disableInterrupt();

    m_bsDown = 0;
    showTriggerTable();

    if(m_nWatchDog >=  nMaxCnt)
    {
        m_eIntrusionResult = TIME_OUT;
        return;
    }

    tm *stTriggerTime;
    time_t tTriggerTime;
    BufferTime *dwEventLog;
    for(int i = 0; i < MAX_PIC_INTRUSIONS; i++)
    {
        if(TEST_BIT(m_bsValidDoor, i))
        {
            dwEventLog = (BufferTime*)m_dwEventLog[i];
            stTriggerTime = localtime( &m_tEventTime[i]);

            Buffer2Tm(stTriggerTime, dwEventLog);

            tTriggerTime = mktime(stTriggerTime);

            double dTimeDif = difftime(m_tEventTime[i], tTriggerTime);
            if(dTimeDif > RTC_THRESHOLD || dTimeDif < -1 * RTC_THRESHOLD)
            {
                m_eIntrusionResult = FAIL;
                PrintDetailInfo("Door %d log time diff = %.1f larger than %d sec\n", i, dTimeDif, RTC_THRESHOLD);
                return ;
            }

            PrintDetailInfo("Door %d log time diff = %.1f smaller than %d sec\n", i, dTimeDif, RTC_THRESHOLD);
        }
    }

    m_eIntrusionResult = PASS;
}

void PICIntrusionTest::diagnostic()
{
    ClearScreen();
    printf("Diagnostic %s : \n\n", sTestFunctionName[RTC_INSTURSION_TEST].c_str());
    printf("==========================================================================\n");

    // 1. Serial nubmer
    printf("Part 1. Serial number diagnostic\n");
    printf("--------------------------------------------------------------------------\n");

    serialNumberTest();

    // 2. RTC
    printf("\nPart 2. RTC diagnostic\n");
    printf("--------------------------------------------------------------------------\n");

    RTCTest();

    if(g_bStepPause)
        SystemPause();

    // 3. Instrusion
    if(m_bsValidDoor == 0)
    {
        m_eIntrusionResult = NOT_AVAIL;
        goto End;
    }

    intrusionTest();

End:
    showResult();
}


void PICIntrusionTest::showResult()
{
    char result[256];
    std::string sResult;
    sprintf(result, "[%d] %s:\n", RTC_INSTURSION_TEST,
            sTestFunctionName[RTC_INSTURSION_TEST].c_str());

    m_sResult.assign(result);

    printf("\nDiagnostic Result: \n");
    printf("==========================================================================\n");

    // serial number
    sResult = resultEnumToString(m_eSNResult);

    printf("PIC --> \"%s\"\n\n", sResult.c_str());

    sprintf(result, "\tSerial number\t\t...  %s!!\n", sResult.c_str());
    m_sResult.append(result);

    // RTC
    sResult = resultEnumToString(m_eRTCResult);

    printf("RTC --> \"%s\"\n\n", sResult.c_str());

    sprintf(result, "\tRTC\t\t\t...  %s!!\n", sResult.c_str());
    m_sResult.append(result);

    // intrusion
    sResult = resultEnumToString(m_eIntrusionResult);

    printf("Intrusion --> \"%s\"\n\n", sResult.c_str());

    sprintf(result, "\tIntrusion\t\t...  %s!!\n", sResult.c_str());
    m_sResult.append(result);

    if(g_bStepPause)
        SystemPause();
}
