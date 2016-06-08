// TestProgram.cpp : ©w¸q¥D±±¥xÀ³¥Îµ{¦¡ªº¶i¤JÂI¡C
//
#include <QLibrary>
#include <signal.h>

#include "Platform.h"

#include "SixteenBitTimerTest.h"
#include "RandomGeneratorTest.h"
#include "cctalktest.h"
#include "SASTest.h"
#include "SRAMTest.h"
#include "EEPROMTest.h"
#include "GPOTest.h"
#include "GPITest.h"
#include "PICIntrusionTest.h"
#include "SPITest.h"

#include <iostream>

#ifdef WINDOWS
#include <iostream>
#include <tchar.h>
#include <time.h>
#else
#endif



typedef enum
{
    TEST,
    BACK,
    STOP
} SELECT_ACTION;


int nFunctionSelect = 0;
TEST_FLOW eTest_flow = NONE;

SELECT_ACTION TestSelectionMenu(string &info, TEST_FLOW &test_flow);
SELECT_ACTION SingleFunctionMenu();
SELECT_ACTION MultiFunctionMenu();
void workFunctionDiagnostic(MODEL_TYPE);

bool g_bDetailInfo  = false;
bool g_bStepPause	= false;
MODEL_TYPE eModel = UNKNOWN_MODEL;


int test(int argc, char* argv[])
{
    string sInfo;	// show total version info
    char sAPIVer[128], sFPGADriverVer[128], sSMBusDriverVer[128], temp[64];
    int type;
    BYTE PICVer;
    FPGA_INFO fpgaInfo;
    SELECT_ACTION select_action;

#ifdef WINDOWS
    // resize console windows
    HWND console = GetConsoleWindow();

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    MoveWindow(console, (screenW - 680)>>1, (screenH - 900)>>1, 800, 900, TRUE);

    // ignore Ctrl-C and Ctrl-Break signal
    signal(SIGINT, SIG_IGN);
    signal(SIGBREAK, SIG_IGN);
#endif

    // initial part

    // 1. open fpga
    FPGA_API_STATUS dwStatus = (FPGA_API_STATUS) FpgaPic_Init();

    if(dwStatus != FD_SUCCESS)
    {
        Error("main", "FpgaPic_Init", dwStatus);
        goto End;
    }

    // 2. get version
    #pragma region get version

    sInfo.assign(TEST_NAME);
    sInfo.append("\n=========================================================\n");
    sInfo.append("Detail info\n");
    sInfo.append("------------------------------\n");

    FD_GetFPGAVer(&fpgaInfo);

    type = fpgaInfo.hw_id & 0x0F;

    sprintf(temp, "Model : %s\n------------------------------\n",
            type < MAX_MODEL ? sModelName[type + 1].c_str() : sModelName[0].c_str());

    eModel = type < MAX_MODEL ? (MODEL_TYPE)type : UNKNOWN_MODEL;

    sInfo.append(temp);

    FD_GetAPIVersion(sAPIVer);
    sInfo += sAPIVer;
    sInfo += '\n';

    sprintf(temp, "FPGA version %d.%d.%d", (int)(fpgaInfo.version >>16) & 0xFF,
            (int)(fpgaInfo.version >>8) & 0xFF, (int)fpgaInfo.version & 0xFF);

    sInfo.append(temp);

    if(fpgaInfo.type == 0)
        sInfo.append(" standard\n");
    else
    {
        sprintf(temp, " customize %d\n", fpgaInfo.type);
        sInfo.append(temp);
    }

    FD_GetDriverVersion(sFPGADriverVer);
    sInfo += sFPGADriverVer;
    sInfo += '\n';

    FD_PIC_KernelVersion(sSMBusDriverVer);
    sInfo += sSMBusDriverVer;
    sInfo += '\n';

    PIC_GetVersion(&PICVer);

    sprintf(temp, "PIC version %d\n", PICVer);
    sInfo += temp;

    sInfo.append("------------------------------");

    #pragma endregion

    do
    {
        // 3. choice test operation selection
        #pragma region show menu of function selection

        select_action = TEST;
        select_action = TestSelectionMenu(sInfo, eTest_flow);

        if(eTest_flow == NONE)
        {
            printf("\nTo many times of input errors. Program will exit..\n");
            goto End;
        }
        else if(select_action == STOP)
        {
            break;
        }

        switch(eTest_flow)
        {
        case ALL_FUNCTION:
            nFunctionSelect = MAX_FUNCTION_VALUE;
            break;
        case SINGLE_FUNCTION:
            select_action = SingleFunctionMenu();
            break;
        case MULITI_FUNCTION:
            select_action = MultiFunctionMenu();
            break;
        }

        if(select_action == BACK)
        {
            fflush(stdin);
            continue;
        }
        else if(select_action == STOP)
        {
            break;
        }

        ClearScreen();
        printf("Test following function:\n\n");
        for(int i = TIMER_16BIT_TEST; i < MAX_TEST_FUNCTION_NUM ; i++)
        {
            if(eTest_flow == ALL_FUNCTION || (nFunctionSelect & 1<<i))
                printf("%s\n", sTestFunctionName[i].c_str());
        }

        #pragma endregion

        // 4. do test
#ifdef WINDOWS
        // disable close button, all testing finish will close window
        RemoveMenu(GetSystemMenu(console,FALSE), SC_CLOSE, MF_GRAYED);
        RemoveMenu(GetSystemMenu(console,FALSE), SC_MAXIMIZE, MF_GRAYED);
        DrawMenuBar(console);
#endif

        workFunctionDiagnostic(eModel);

    }
    while(1);

End:

    // 5. end and close
    dwStatus = (FPGA_API_STATUS) FpgaPic_Close();

    if(dwStatus != FD_SUCCESS)
    {
        printf("\nClose FPGA fail\n");
    }
    else
    {
        printf("\n");
    }

    SystemPause();

    return 0;
}

SELECT_ACTION TestSelectionMenu(string &info, TEST_FLOW &test_flow)
{
    int number = 0;
    int count = 10;
    char input[64];
    bool bShowTip = false;

    do
    {
        // clear screen
        ClearScreen();

        printf("%s\n", info.c_str());

        printf("Test Operation Selection Menu:\n\n");

        for(int i = ALL_FUNCTION; i < MAX_FLOW_SELECTION_NUM ; i++)
        {
            printf("   [%d] %s\n", i + 1, sTestFlowSelectionName[i].c_str());
        }


        if(bShowTip)
        {
            printf("\n---------------------------------------------------------\n");
            printf("<Tip>\n");
            printf(" -----\n\n");
            printf("  selection[flag1][flag2]]\n\n");
            printf("  flags:\n");
            printf("   'D' or 'd' - more information for debugging\n");
            printf("   'P' or 'p' - pause for every test result\n\n");
            printf("  Enter \"1dp\", then running all testing with more detail \n");
            printf("  information and pause between each function testing\n");
            printf("\n---------------------------------------------------------\n\n");
        }

        printf("\nPlease key in:\n\n");
        printf("  n[d][p] => n : 1 ~ 3, select Test function and continue\n");
        printf("          => d : detail log info\n");
        printf("          => p : pause for each testing function\n");
        printf("  ? => help\n");
        printf("  q => stop\n\n");
        printf("Input>");

        cin.clear();
        std::cin.getline(input, 64);

        if(input[0] == '?')
        {
            bShowTip = !bShowTip;
            continue;
        }
        else if(input[0] == 'q' || input[0] == 'Q')
        {
            return STOP;
        }

        if(isdigit(input[0]))
        {
            std::string key(input);

            g_bStepPause = (key.find('p') != std::string::npos || key.find('P') != std::string::npos);
            g_bDetailInfo = (key.find('d') != std::string::npos || key.find('D') != std::string::npos);

            input[1] = '\0';	// only trans first character
            number = strtol(input, NULL, 10);
        }

        count--;

        if(count == 0)
        {
            test_flow = NONE;
            return STOP;
        }

    }
    while(number < 1 || number > 3);

    test_flow = (TEST_FLOW)(number - 1);
    return TEST;
}

SELECT_ACTION SingleFunctionMenu()
{
    int number = 0;
    int count = 10;
    char input[64];

    do
    {
        // clear screen
        ClearScreen();

        printf("[%s]\n", sTestFlowSelectionName[SINGLE_FUNCTION].c_str());
        printf("Choose the desired test function:\n\n");

        for(int i = TIMER_16BIT_TEST; i < MAX_TEST_FUNCTION_NUM ; i++)
        {
            printf(" [%d] %s\n", i + 1, sTestFunctionName[i].c_str());
        }

        printf("\n");
        printf("Please key in:\n\n");
        printf("  %d ~ %d => select test and continue\n", TIMER_16BIT_TEST + 1, MAX_TEST_FUNCTION_NUM);
        printf("  back => go to back\n");
        printf("  q => stop\n\n");
        printf("Input>");

        cin.clear();
        std::cin.getline(input, 64);

        if(strncmp(input, "back", 4) >= 0)
        {
            return BACK;
        }
        else if(input[0] == 'q' || input[0] == 'Q')
        {
            return STOP;
        }
        else if(isdigit(input[0]))
        {
            if(input[0] == '1' && input[1] == '0')
                input[2] = '\0';	// trans two character
            else
                input[1] = '\0';	// only trans first character

            number = strtol(input, NULL, 10);
        }

        count--;

        if(count == 0)
        {
            printf("\nTo many times of input errors. Program will exit..\n");
            return STOP;
        }
    }
    while(number < 1 || number > 10);

    nFunctionSelect = (1 << (number - 1));

    return TEST;
}

void ShowMultiSelectionExample()
{
    // set seed
    srand((UINT)time(NULL));
    int select1 = rand()%MAX_TEST_FUNCTION_NUM, select2 = rand()%MAX_TEST_FUNCTION_NUM;
    int index = 0;

    while(select1 == select2)
    {
        select2 = rand() % MAX_TEST_FUNCTION_NUM;
    }

    if(select1 > select2)
    {
        int temp = select1;
        select1 = select2;
        select2 = temp;
    }

    printf("Example: \n\n");
    printf("  Choice :\n");
    printf("    Bit %d => %s\n", select1, sTestFunctionName[select1].c_str());
    printf("    Bit %d => %s\n\n", select2, sTestFunctionName[select2].c_str());

    int nSelectValue = 0;
    char sSelectValue[MAX_BIN_LEN + 1]; // include null terminal

    nSelectValue |= 1<<select1;
    nSelectValue |= 1<<select2;

#ifdef WINDOWS
    itoa(nSelectValue, sSelectValue, 2);
#else
    for(index = MAX_TEST_FUNCTION_NUM - 1; index >= 0 ; index--)
        sSelectValue[index] = (select1 == index || select2 == index) ? '1' : '0';
#endif

    int tail = (int)strlen(sSelectValue);
    sSelectValue[tail++] = 'b';
    sSelectValue[tail] = '\0';

    printf("  Enter binary value with ending with \"b\"\t=> %s ", sSelectValue);

    if(tail != MAX_BIN_LEN)
    {
        printf("or ");

        while(tail++ != MAX_BIN_LEN)
        {
            printf("0");
        }

        printf("%s", sSelectValue);
    }


    printf("\n    or changed to Decimal value \t\t=> %d", nSelectValue);
    printf("\n    or changed to Hexadecimal value with start \"0x\"\t=> %#x\n\n", nSelectValue);
}

SELECT_ACTION MultiFunctionMenu()
{
    int number = 0;
    char input[MAX_BIN_LEN + 1];
    int count = 10;

    bool bTip = false;

    do
    {
        // clear screen
        ClearScreen();

        printf("[%s]\n", sTestFlowSelectionName[MULITI_FUNCTION].c_str());
        printf("Choose the desired test function(s):\n\n");

        for(int i = TIMER_16BIT_TEST; i < MAX_TEST_FUNCTION_NUM ; i++)
        {
            printf("  [Bit %d] %s\n", i, sTestFunctionName[i].c_str());
        }

        printf("\n");

        if(bTip)
            ShowMultiSelectionExample();

        printf("Please key in:\n\n");
        printf("  \"Binary, Decimal or Hexadecimal Value\" => select functions and continue\n");
        printf("  ? => tip\n");
        printf("  back => go to back\n");
        printf("  q => stop\n\n");
        printf("Input>");

        cin.clear();
        std::cin.getline(input, MAX_BIN_LEN + 1);

        count--;

        if(input[0] == '?')
        {
            bTip = !bTip;
            continue;
        }
        else if(strncmp(input, "back", 4) == 0)
        {
            return BACK;
        }
        else if(input[0] == 'q' || input[0] == 'Q')
        {
            return STOP;
        }
        else if(isdigit(input[0]))
        {
            std::string key(input);

            if(key.find("0x") == 0)
            {
                input[MAX_HEX_LEN + 1] = '\0';
                number = strtol(input, NULL, 16);
            }
            else if(key.find('b') == key.length() - 1)
            {
                number = strtol(input, NULL, 2);
            }
            else
            {
                number = strtol(input, NULL, 10); printf("Dec\n");
            }
        }

        count--;

        if(count == 0 && number == 0)
        {
            printf("\nTo many times of input errors. Program will exit..\n");
            return STOP;
        }

    }while(number == 0);

    if(number >= MAX_FUNCTION_VALUE)
    {
        eTest_flow = ALL_FUNCTION;
    }

    nFunctionSelect = number;

    return TEST;
}


void workFunctionDiagnostic(MODEL_TYPE modelType)
{
    std::string sFinalResult;

    if( TEST_BIT( nFunctionSelect, TIMER_16BIT_TEST))
    {
        SixteenBitTimerTest timerTest;
        timerTest.diagnostic();

        sFinalResult.append(timerTest.getResultString());
    }

    if( TEST_BIT(nFunctionSelect,  RANDOM_GEN_TEST))
    {
        RandomGeneratorTest randomTest;
        randomTest.diagnostic();

        sFinalResult.append(randomTest.getResultString());
    }


    if( TEST_BIT(nFunctionSelect,  CCTALK_TEST))
    {
        CCTalkTest ccTalkTest(modelType);
        ccTalkTest.diagnostic();

        sFinalResult.append(ccTalkTest.getResultString());
    }


    if( TEST_BIT(nFunctionSelect,  SAS_TEST))
    {
        SASTest sasTest;
        sasTest.diagnostic();


        sFinalResult.append(sasTest.getResultString());
    }

    if( TEST_BIT(nFunctionSelect,  SRAM_TEST))
    {
        //SRAMTest sramTest;
        //sramTest.diagnostic();

        //sFinalResult.append(sramTest.getResultString());
    }

    if( TEST_BIT(nFunctionSelect,  EEPROM_TEST))
    {
        EEPROMTest eepromTest(modelType);
        eepromTest.diagnostic();

        sFinalResult.append(eepromTest.getResultString());
    }


    if( TEST_BIT(nFunctionSelect,  GPO_TEST))
    {
        GPOTest gpoTest;
        gpoTest.diagnostic();

        sFinalResult.append(gpoTest.getResultString());
    }


    if( TEST_BIT(nFunctionSelect,  GPI_TEST))
    {
        GPITest gpiTest;
        gpiTest.diagnostic();

        sFinalResult.append(gpiTest.getResultString());
    }

    if( TEST_BIT(nFunctionSelect,  RTC_INSTURSION_TEST))
    {
        PICIntrusionTest picTest(modelType);
        picTest.diagnostic();

        sFinalResult.append(picTest.getResultString());
    }

    if( TEST_BIT(nFunctionSelect,  SPI_TEST))
    {
        SPITest spiTest;
        spiTest.diagnostic();

        sFinalResult.append(spiTest.getResultString());
    }

    ClearScreen();
    printf("Diagnostic Complete!\n\nResult:\n\n");
    printf("%s", sFinalResult.c_str());
    SystemPause();
}
