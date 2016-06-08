#include "gpotest.h"

GPOTest::GPOTest()
{
    m_dwAvail = 0;
    FD_GetGPOAvail(0, &m_dwAvail);

    int count = sizeof(DWORD) * 8;
    for(int i = 0; i < count ; i++)
    {
        if(TEST_BIT(m_dwAvail, i))
        {
            m_nMaxBit = i;
        }
    }
}


GPOTest::~GPOTest()
{
}

void GPOTest::normalTest()
{
    FPGA_API_STATUS status;

    printf("GPO general output diagnostic (1/2)\n");
    printf("--------------------------------------------------------------------------\n");

    int index = -1, count;
    BOOL bLight;
    char str[64] = {0};

    // two times for turn-on 200ms and turn-off 200ms
    while(++index <= m_nMaxBit)
    {

        if(((m_dwAvail >> index) & 0x1) == 0)
        {
            continue;
        }

        sprintf(str, "trigger \"GPO %d\"", index);

        if(index == m_nMaxBit)
            OutputTextTool::showProgress(99, str);
        else
            OutputTextTool::showProgress(index * 100 / m_nMaxBit, str);

        count = TURN_ON_OFF_TIMES;

        bLight = FALSE;

        while(count--)
        {
            bLight = !bLight;
            status = (FPGA_API_STATUS)FD_SetGPOByIndex(0, index, bLight);
            if(status != FD_SUCCESS)
            {
                Error("diagnostic", "FD_SetGPOByIndex", status);
                m_eResult = TEST_ERROR;
                return;
            }
            Sleep(DURATION_INTERVAL);
        }
    }

    OutputTextTool::showProgress(100, "");
    m_eResult = PASS;
}

void GPOTest::pwmTest(DWORD dwNGPIO)
{
    int index = -1;
    FPGA_API_STATUS status;
    int level,  increase_unit;
    int step(0), repeat = PWM_TEST_REPEAT;
    int total_step = PWM_MAX_TEST_LEVEL * 2 / PWM_INCREASE_LEVEL;
    DWORD sleep_time = PWM_DURATION / total_step;

    printf("GPO PWM output diagnostic (2/2)\n");
    printf("--------------------------------------------------------------------------\n");

    if( TEST_BIT(dwNGPIO, GPO_PWM))
    {
        total_step *= PWM_TEST_REPEAT;

        while(repeat--)
        {
            level = 5;
            index = 0;
            increase_unit = PWM_INCREASE_LEVEL;

            do
            {
                OutputTextTool::showProgress((++step) * 100 / total_step);

                index = -1;
                while(++index <= m_nMaxBit)
                {
                    if(((m_dwAvail >> index) & 0x01) == 0)
                    {
                        continue;
                    }

                    status = (FPGA_API_STATUS)FD_SetGPOCfgByIndex(index, 1, PWM_FREQ, (WORD)level);
                    if(status != FD_SUCCESS)
                    {
                        Error("diagnostic", "FD_SetGPOCfgByIndex", status);
                        m_eResult = TEST_ERROR;
                        return;
                    }
                }

                level += increase_unit;

                if(level >= PWM_MAX_TEST_LEVEL)
                {
                    level = PWM_MAX_TEST_LEVEL;
                    increase_unit *= -1;
                }

                Sleep(sleep_time);
            }while(level >= 0);

            Sleep(500);
        }
    }
    else
    {
        printf("PWM function is not available!\n");
        m_eResult = NOT_AVAIL;
    }
}

void GPOTest::diagnostic()
{
    FPGA_API_STATUS status;
    DWORD dwGPOStatus(0), dwNGPIO(0);

    if(m_dwAvail == 0)
    {
        printf("%s function not available!", sTestFunctionName[GPO_TEST].c_str());
        m_eResult = NOT_AVAIL;
        goto End;
    }

    // get status

#pragma region pre-config
    status = (FPGA_API_STATUS)FD_GetNGPIOAvail(&dwNGPIO);
    if(status != FD_SUCCESS)
    {
        Error("diagnostic", "FD_GetNGPIOAvail", status);
        m_eResult = TEST_ERROR;
        goto End;
    }

    if(TEST_BIT(dwNGPIO, GPO_PWM))
    {
        // set general output
        for(int i = 0; i <= m_nMaxBit ; i++)
        {
            if( TEST_BIT(m_dwAvail, i))
            {
                status = (FPGA_API_STATUS)FD_SetGPOCfgByIndex(i, 0, 0, 0);
                if(status != FD_SUCCESS)
                {
                    Error("diagnostic", "FD_SetGPOCfgByIndex", status);
                    m_eResult = TEST_ERROR;
                    goto End;
                }
            }
        }
    }

    // set GPO turn off
    status = (FPGA_API_STATUS)FD_GetGPOAll(0, &dwGPOStatus);
    if(status != FD_SUCCESS)
    {
        Error("diagnostic", "FD_GetGPOAll", status);
        m_eResult = TEST_ERROR;
        goto End;
    }


    if(dwGPOStatus > 0)
    {
        // reset all
        status = (FPGA_API_STATUS)FD_SetGPOAll(0, 0);
        if(status != FD_SUCCESS)
        {
            Error("diagnostic", "FD_SetGPOAll", status);
            m_eResult = TEST_ERROR;
            goto End;
        }
    }
#pragma endregion

    ClearScreen();
    printf("Diagnostic %s : \n\n", sTestFunctionName[GPO_TEST].c_str());
    printf("==========================================================================\n");

    m_eResult = PASS;

    // 1. Normal mode
    normalTest();

    printf("--------------------------------------------------------------------------\n");

    if(m_eResult == PASS)
    {
        // 2. PWM mode
        pwmTest(dwNGPIO);
    }

End:
    showResult();
}

void GPOTest::showResult()
{
    printf("\nDiagnostic Result: \n");
    printf("==========================================================================\n");

    PrintDetailInfo("\n\n");

    m_sResult = resultEnumToString(m_eResult);

    printf("\n--> \"%s\"\n\n", m_sResult.c_str());

    char result[256];
    sprintf(result, "[%d] %s\t\t\t\t...  %s!!\n\n", GPO_TEST,
        sTestFunctionName[GPO_TEST].c_str(), m_sResult.c_str());

    m_sResult.assign(result);

    printf("\n");

    if(g_bStepPause)
        SystemPause();
}

