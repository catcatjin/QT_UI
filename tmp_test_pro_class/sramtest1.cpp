#include "sramtest1.h"
#include <QEventLoop>
#include <QTimer>

#define COMPARE_ERROR_CODE	0xAA	// see FPGA register table, 0x55 means equal, but 0xAA means unequal

SRAMTest1::SRAMTest1()
{
    // set seed
    srand((UINT)time(NULL));
}

SRAMTest1::~SRAMTest1()
{
}

int SRAMTest1::m_ReadMirror32(DWORD* rbuf, DWORD offset, DWORD len)
{
    // parameter 1 range is 1 ~ 3
    return FD_ReadMirror32(rand()%3 + 1, rbuf, offset, len);
}

int SRAMTest1::m_ReadMirror8(BYTE* rbuf, DWORD offset, DWORD len)
{
    // parameter 1 range is 1 ~ 3
    return FD_ReadMirror8(rand()%3 + 1, rbuf, offset, len);
}

DWORD SRAMTest1::getMaxOffsetSRAMAddr(int OffsetBlock)
{
    if(m_eSramMode == NORMAL_MODE)
    {
        // last block is reserve in normal mode
        return (m_dwMemSize - RESERVE_BLOCK) / BLOCK_UNIT_32KB * BLOCK_UNIT_32KB;
    }

    return m_dwMemSize / OffsetBlock * OffsetBlock;
}

bool SRAMTest1::FunctionalTest()
{
    FPGA_API_STATUS status;
    DWORD dwCount = 0;

    BYTE *randomData = new BYTE[BLOCK_UNIT_32KB];

    DWORD dwMaxOffset = getMaxOffsetSRAMAddr(BLOCK_UNIT_32KB);

    for( ; dwCount < dwMaxOffset ; dwCount += BLOCK_UNIT_32KB)
    {
        for(int i = 0; i < BLOCK_UNIT_32KB ; i++)
        {
            randomData[i] = rand() % 0xFF;
        }

        status = (FPGA_API_STATUS)FD_TestSramBlock(randomData, dwCount, BLOCK_UNIT_32KB);
        if(status != FD_SUCCESS)
        {
            Error("\nFunctionalTest", "FD_TestSramBlock", status);
            break;
        }

        //OutputTextTool::showTextWithVariableDots("Writing ", dwCount, m_dwMemSize);
        OutputTextTool::showProgress((int)((dwCount + BLOCK_UNIT_32KB)* 100 / dwMaxOffset));
    }

    delete [] randomData;

    status = (FPGA_API_STATUS)FD_EraseSram(); //erase all data to "0"
    if(status != FD_SUCCESS)
    {
        Error("\nFunctionalTest", "FD_EraseSram", status);
        return false;
    }

    return dwCount == dwMaxOffset;
}

bool SRAMTest1::IntegrityTest()
{
    DWORD dwOffset = 0;
    DWORD dwSpecialValue;
    FPGA_API_STATUS status;

    DWORD dwMaxOffset = getMaxOffsetSRAMAddr(BLOCK_UNIT_1KB);

    for(int i = 0 ; dwOffset < dwMaxOffset ; dwOffset += BLOCK_UNIT_1KB, i++)
    {
        dwSpecialValue = (DWORD)((i + 1) | ((i + 1) << 16));

        // write first DWORD
        status = (FPGA_API_STATUS)(*m_pWriteMem32Func)(&dwSpecialValue, dwOffset, 1);

        if(status != FD_SUCCESS)
        {
            Error("\nIntegrityTest", "WriteMem32", status);
            break;
        }

        //OutputTextTool::showTextWithVariableDots("Write ", dwOffset, m_dwMemSize);
        OutputTextTool::showProgress((int)(dwOffset + BLOCK_UNIT_1KB) * 50 / dwMaxOffset);	// first 50 %
    }

    return dwOffset == dwMaxOffset && IntegrityDataVerify() ;
}


bool SRAMTest1::IntegrityDataVerify()
{
    int idx;
    bool result = false;
    DWORD dwCount = 0;
    DWORD dwTargetValue;
    FPGA_API_STATUS status;

    DWORD *dwCmpCode = new DWORD[BLOCK_UNIT_1KB];
    DWORD *dwSpecialValue = new DWORD[QUARTER_OF_1KB];

    int div = (m_eSramMode == MIRROR_MODE ? 2 : 1);
    DWORD dwMaxOffset = getMaxOffsetSRAMAddr(BLOCK_UNIT_1KB);

    for(int i = 0 ; dwCount < dwMaxOffset ; dwCount += BLOCK_UNIT_1KB, i++)
    {
        dwTargetValue = (DWORD)((i + 1) | ((i + 1) << 16));

        memset(dwSpecialValue, 0, QUARTER_OF_1KB * sizeof(DWORD));

        status = (FPGA_API_STATUS)(*m_pReadMem32Func)(dwSpecialValue, dwCount, QUARTER_OF_1KB);

        if(status != FD_SUCCESS)
        {
            Error("\nIntegrityDataVerify", "ReadMem32", status);
            goto VERIFY_FAIL;
        }

        for(int idx = 0 ; idx < QUARTER_OF_1KB; idx++)
        {
            if( idx != 0)
                dwTargetValue = 0;

            if(dwSpecialValue[idx] != dwTargetValue)
            {
                goto VERIFY_FAIL;
            }
        }

        //OutputTextTool::showTextWithVariableDots("Write ", dwCount, m_dwMemSize);
        OutputTextTool::showProgress((int)(dwCount + BLOCK_UNIT_1KB) * 50 / dwMaxOffset / div + 50);	// last 50%
    }

    // mirror use compare to verify
    if(m_eSramMode == MIRROR_MODE)
    {
        for(DWORD offset = 0 ; offset < dwMaxOffset ; offset += BLOCK_UNIT_1KB)
        {
            status = (FPGA_API_STATUS)FD_CompareMirror32(dwCmpCode, offset, QUARTER_OF_1KB);
            if(status != FD_SUCCESS)
            {
                Error("\nIntegrityDataVerify", "FD_CompareMirror32", status);
                goto VERIFY_FAIL;
            }

            idx = BLOCK_UNIT_1KB;
            while(idx--)
            {
                // 0x55 means equal, but 0xAA means unequal
                if(dwCmpCode[idx] == COMPARE_ERROR_CODE)
                {
                    printf("\nMirror data not equal in each block!!\n");
                    goto VERIFY_FAIL;
                }
            }

            OutputTextTool::showProgress((int)(offset + BLOCK_UNIT_1KB) * 25 / dwMaxOffset + 75);	// last 25%
        }
    }

    status = (FPGA_API_STATUS)FD_EraseSram(); //erase all data to "0"
    if(status != FD_SUCCESS)
    {
        Error("\nIntegrityDataVerify", "FD_EraseSram", status);
        goto VERIFY_FAIL;
    }

    result =  true;

VERIFY_FAIL:

    delete [] dwCmpCode;
    delete [] dwSpecialValue;

    return result;
}

bool SRAMTest1::RandomgAccessTest()
{
    BYTE value;
    std::map<DWORD, BYTE> rand_map;
    FPGA_API_STATUS status;

    DWORD idx = 0, offset;
    DWORD dwCount = m_dwMemSize / 256;

    DWORD dwMaxOffset = getMaxOffsetSRAMAddr(1);

    for(; idx < dwCount ; idx++)
    {
        offset = rand() % dwMaxOffset;

        rand_map[offset] = value = rand() & 0xFF;

        status = (FPGA_API_STATUS)(*m_pWriteMem8Func)(&value, offset, 1);

        if(status != FD_SUCCESS)
        {
            Error("\nIntegrityTest", "WriteMem8", status);
            break;
        }

        //OutputTextTool::showTextWithVariableDots("Write ", dwCount, m_dwMemSize);
        OutputTextTool::showProgress((int)(idx + 1) * 50 / dwCount);	// first 50 %
    }

    return idx == dwCount && RandomgAccessVerify(rand_map);
}

bool SRAMTest1::RandomgAccessVerify(std::map<DWORD, BYTE> &rand_map)
{
    bool result;
    BYTE memValue;
    DWORD dwCmpCode;
    FPGA_API_STATUS status;

    size_t count = 0;
    size_t totalcount = rand_map.size();
    int div = (m_eSramMode == MIRROR_MODE ? 2 : 1);
    std::map<DWORD, BYTE>::iterator iter = rand_map.begin();

    for(size_t count = 0 ; iter != rand_map.end() ; iter++, count++)
    {
        status = (FPGA_API_STATUS)(*m_pReadMem8Func)(&memValue, iter->first, 1);

        if(status != FD_SUCCESS)
        {
            Error("\nIntegrityDataVerify", "ReadMem32", status);
            goto VERIFY_FAIL;
        }

        //OutputTextTool::showTextWithVariableDots("Write ", dwCount, m_dwMemSize);
        OutputTextTool::showProgress((int)((count + 1) * 50 / totalcount / div + 50));	// last 50%

        if(memValue != iter->second)
        {
            printf("\nData mismatch!\n");
            goto VERIFY_FAIL;
        }
    }

    // mirror use compare to verify
    if(m_eSramMode == MIRROR_MODE)
    {
        for(iter = rand_map.begin() ; iter != rand_map.end() ; iter++, count++)
        {
            status = (FPGA_API_STATUS)FD_CompareMirror32(&dwCmpCode, iter->first, 1);
            if(status != FD_SUCCESS)
            {
                Error("\nIntegrityDataVerify", "FD_EraseSram", status);
                goto VERIFY_FAIL;
            }

            // 0x55 means equal, but 0xAA means unequal
            if(dwCmpCode == COMPARE_ERROR_CODE)
            {
                printf("\nMirror data not equal in each block!!\n");
                goto VERIFY_FAIL;
            }

            OutputTextTool::showProgress((int)((count + 1) * 25 / totalcount + 75));	// last 25%
        }
    }

    status = (FPGA_API_STATUS)FD_EraseSram(); //erase all data to "0"
    if(status != FD_SUCCESS)
    {
        Error("\nIntegrityDataVerify", "FD_EraseSram", status);
        goto VERIFY_FAIL;
    }

    result =  true;

VERIFY_FAIL:

    rand_map.clear();

    return result;
}

bool SRAMTest1::memoryTest()
{
    // 1. 32KB block write with random value
    printf("Part 1 : Functional Test (1/3)\n\n");
    if(!FunctionalTest())
    {
        return false;
    }

    // 2. write first 4Bytes every 1KB block
    printf("\n---------------------------------------\n");
    printf("Part 2 : Data Integrity Test (2/3)\n\n");
    if(!IntegrityTest())
    {
        return false;
    }

    // 3. random address and value to write
    printf("\n---------------------------------------\n");
    printf("Part 3 : Random Access Test (3/3)\n\n");
    if(!RandomgAccessTest())
    {
        return false;
    }

    return true;
}

void SRAMTest1::diagnostic()
{
    FPGA_API_STATUS status;
    QString str;

    ClearScreen();
    //printf("Diagnostic %s : \n\n", sTestFunctionName[SRAM_TEST].c_str());
    str.sprintf("Diagnostic %s : \n", sTestFunctionName[SRAM_TEST].c_str());
    emit updateText(str);

    //printf("==========================================================================\n");
    str.sprintf("==========================================================================");
    emit updateText(str);

    // normal testing
    //printf("SRAM normal mode diagnostic\n");
    str.sprintf("SRAM normal mode diagnostic");
    emit updateText(str);
    //printf("--------------------------------------------------------------------------\n");
    str.sprintf("--------------------------------------------------------------------------");
    emit updateText(str);
    QEventLoop eventloop;
    QTimer::singleShot(1, &eventloop, SLOT(quit()));
    eventloop.exec();
    // set normal mode
    FD_SetSramMode(0, 1);

    m_eSramMode = NORMAL_MODE;

    FD_GetSramAvail(&m_dwMemSize);

    m_pReadMem8Func  = &FD_ReadSram8;
    m_pWriteMem8Func = &FD_WriteSram8;
    m_pReadMem32Func  = &FD_ReadSram32;
    m_pWriteMem32Func = &FD_WriteSram32;

    m_bNormalTestResult = memoryTest();

    status = (FPGA_API_STATUS)FD_EraseSram(); //erase all data to "0"

    if(status != FD_SUCCESS)
    {
        m_bNormalTestResult = false;
        Error("diagnostic", "FD_EraseSram", status);
    }

    // mirror testing
    //printf("\n\n==========================================================================\n");
    //printf("SRAM mirror mode diagnostic\n");
    //printf("--------------------------------------------------------------------------\n");

    str.sprintf("\n==========================================================================");
    emit updateText(str);
    str.sprintf("SRAM mirror mode diagnostic");
    emit updateText(str);
    str.sprintf("--------------------------------------------------------------------------");
    emit updateText(str);

    // set to mirror
    FD_SetSramMode(1, 1);

    m_eSramMode = MIRROR_MODE;

    FD_GetSramAvail(&m_dwMemSize);

    m_pReadMem8Func  = &SRAMTest1::m_ReadMirror8;
    m_pWriteMem8Func = &FD_WriteMirror8;
    m_pReadMem32Func  = &SRAMTest1::m_ReadMirror32;
    m_pWriteMem32Func = &FD_WriteMirror32;

    m_bMirrorTestResult = memoryTest();

    status = (FPGA_API_STATUS)FD_EraseSram(); //erase all data to "0"

    if(status != FD_SUCCESS)
    {
        m_bMirrorTestResult = false;
        Error("diagnostic", "FD_EraseSram", status);
    }

    showResult();
    //system("pause");
}

void SRAMTest1::showResult()
{
    char result[256];

    printf("\n==========================================================================\n");
    printf("Diagnostic Result: \n");
    printf("==========================================================================\n");

    sprintf(result, "[%d] %s:\n", SRAM_TEST, sTestFunctionName[SRAM_TEST].c_str());
    //m_sResult.append(result);

    // printf normal
    printf("%s : Normal mode ... %s!!\n\n", sTestFunctionName[SRAM_TEST].c_str(),
        m_bNormalTestResult ? "Pass" : "Fail");

    sprintf(result, "\tNormal mode\t\t...  %s!!\n", m_bNormalTestResult ? "Pass" : "Fail");

    //m_sResult.append(result);

    // printf mirror
    printf("%s : Mirror mode ... %s!!\n", sTestFunctionName[SRAM_TEST].c_str(),
        m_bMirrorTestResult ? "Pass" : "Fail");

    sprintf(result, "\tMirror mode\t\t...  %s!!\n\n", m_bMirrorTestResult ? "Pass" : "Fail");

    //m_sResult.append(result);


    if(g_bStepPause)
        SystemPause();
}

void SRAMTest1::doTest()
{
    emit enableBtn(btnName, false);
    emit updateBtn(btnName, YELLOW_XBTN);
    //emit updateText(tr("SRAM testing:Start test now.."));
    QEventLoop eventloop;
    QTimer::singleShot(1, &eventloop, SLOT(quit()));
    eventloop.exec();
    //writeLog("[START COM1&COM3 TEST]");
/*
    if(openCOM1COM3())
    {
        emit updateBtn(UART1_BTN, RED_XBTN);
        emit updateText(tr("COM1 testing:port open fail.."));
        writeLog(tr("COM1 testing:port open fail.."));
        emit enableBtn(UART1_BTN, true);
        result = -1;
        return;
    }

    if(COMRxTxTest())
    {
        result = -1;
        ::close(fd1);
        ::close(fd2);
        emit updateBtn(UART1_BTN, RED_XBTN);
        emit enableBtn(UART1_BTN, true);
        return;
    }
*/
    if (1)
    {
        //SRAMTest1 sramTest;
        //sramTest.diagnostic();
        diagnostic();
        //sFinalResult.append(sramTest.getResultString());
        emit updateBtn(btnName, GREEN_XBTN);


    }
    //emit updateText(tr("COM1 testing:RX/TX Test PASS!!"));
    //writeLog(tr("COM1 testing:RX/TX Test PASS!!"));

    //::close(fd1);
    //::close(fd2);

    emit enableBtn(btnName, true);
}
