#include "eepromtest.h"

EEPROMTest::EEPROMTest(MODEL_TYPE modelType)
{
    m_eModelType = modelType;
    m_eResult = PASS;

    getEEPROMSize();
}

EEPROMTest::~EEPROMTest()
{
}

void EEPROMTest::getEEPROMSize()
{
    switch (m_eModelType)
    {
    case PGB_5120S:
    case PGB_5021:
    case PGB_4020:
    case PGB_6040:
    case PGB_5320:
    case PGB_5322:
    case GCB_203:
    default:
        m_nMemSize = 64 * 1024;
    }
}

void EEPROMTest::FunctionalTest()
{
    FPGA_API_STATUS status;
    int i = 0;

    BYTE *randomData = new BYTE[FUNCTION_TEST_BLOCK_SIZE];
    UINT uBlockCount = m_nMemSize / FUNCTION_TEST_BLOCK_SIZE;
    WORD offset[TEST_BLOCK_COUNT] = {0};

    for(; i < TEST_BLOCK_COUNT ; i++)
    {
        offset[i] = (WORD)(rand() % uBlockCount) * FUNCTION_TEST_BLOCK_SIZE;

        for(int j = 0; j < FUNCTION_TEST_BLOCK_SIZE ; j++)
        {
            randomData[j] = rand() % 0xFF;
        }

        status = (FPGA_API_STATUS)FD_Test_EEPROM16_Block(offset[i], FUNCTION_TEST_BLOCK_SIZE, randomData);

        if(status != FD_SUCCESS)
        {
            Error("\nFunctionalTest", "FD_Test_EEPROM16_Block", status);
            m_eResult = TEST_ERROR;
            return ;
        }

        //OutputTextTool::showTextWithVariableDots("Writing ", dwCount, m_dwMemSize);
        OutputTextTool::showProgress((int)((i + 1)* 100 / TEST_BLOCK_COUNT));
    }

    delete [] randomData;

    if(i == TEST_BLOCK_COUNT)
        m_eResult = PASS;
    else
        m_eResult = FAIL;

}

void EEPROMTest::RandomgAccessTest()
{
    BYTE value;
    std::map<WORD, BYTE> rand_map;
    FPGA_API_STATUS status;
    WORD idx = 0, offset;

    for(; idx < EEPROM_BUFFER_SIZE ; idx++)
    {
        do
        {
            offset = (WORD)(rand() % m_nMemSize);

        }while(rand_map.find(offset) != rand_map.end());

        rand_map[offset] = value = rand() & 0xFF;

        status = (FPGA_API_STATUS)FD_WriteEEPROM16(offset, 1, &value);

        if(status != FD_SUCCESS)
        {
            Error("\nRandomgAccessTest", "FD_WriteEEPROM16", status);
            m_eResult = TEST_ERROR;
            return ;
        }

        OutputTextTool::showProgress((int)(idx + 1) * 50 / EEPROM_BUFFER_SIZE);	// first 50 %
    }

    if(idx != EEPROM_BUFFER_SIZE)
    {
        m_eResult = FAIL;
        return;
    }

    RandomgAccessVerify(rand_map);
}

void EEPROMTest::RandomgAccessVerify(std::map<WORD, BYTE> &rand_map)
{
    BYTE memValue;
    FPGA_API_STATUS status;

    size_t totalcount = rand_map.size();

    std::map<WORD, BYTE>::iterator iter = rand_map.begin();

    for(size_t count = 0 ; iter != rand_map.end() ; iter++, count++)
    {
        status = (FPGA_API_STATUS)FD_ReadEEPROM16(iter->first, 1, &memValue);

        if(status != FD_SUCCESS)
        {
            Error("\nRandomgAccessVerify", "FD_ReadEEPROM16", status);
            m_eResult = TEST_ERROR;
            goto VERIFY_FAIL;
        }

        //OutputTextTool::showTextWithVariableDots("Write ", dwCount, m_dwMemSize);
        OutputTextTool::showProgress((int)((count + 1) * 50 / totalcount  + 50));	// last 50%

        if(memValue != iter->second)
        {
            printf("\nData mismatch!\n");
            m_eResult = FAIL;
            goto VERIFY_FAIL;
        }
    }

    m_eResult = PASS;

VERIFY_FAIL:

    rand_map.clear();
}


void EEPROMTest::diagnostic()
{
    ClearScreen();
    printf("Diagnostic %s : \n\n", sTestFunctionName[EEPROM_TEST].c_str());
    printf("==========================================================================\n");


    // 1. 128KB block write with random value
    printf("Part 1 : Functional Test (1/2)\n\n");

    FunctionalTest();

    if(m_eResult == TEST_ERROR)
    {
        goto End;
    }

    // 2. random address and value to write
    printf("\n---------------------------------------\n");
    printf("Part 2 : Random Access Test (2/2)\n\n");

    RandomgAccessTest();

End:
    showResult();
}

void EEPROMTest::showResult()
{
    printf("\nDiagnostic Result: \n");
    printf("==========================================================================\n");

    printf("\n--> \"%s\"\n\n", resultEnumToString(m_eResult).c_str());

    char result[256];
    sprintf(result, "[%d] %s\t\t...  %s!!\n\n",
        EEPROM_TEST, sTestFunctionName[EEPROM_TEST].c_str(), resultEnumToString(m_eResult).c_str());

    m_sResult.append(result);

    printf("\n");

    if(g_bStepPause)
        SystemPause();
}
