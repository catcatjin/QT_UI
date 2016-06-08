#include "spitest.h"


SPITest::SPITest()
{
}

SPITest::~SPITest()
{
}


void SPITest::diagnostic()
{
    int i, j;
    FPGA_API_STATUS status;
    DWORD freq, data;
    BYTE offset[2], rand_data[MAX_SPI_DATA_LEN], recv_data[MAX_SPI_DATA_LEN], spi_data[MAX_SPI_DATA_LEN];
    BYTE mode = 0;

    memset(rand_data, 0, MAX_SPI_DATA_LEN);
    memset(recv_data, 0, MAX_SPI_DATA_LEN);
    memset(spi_data, 0, MAX_SPI_DATA_LEN);

    ClearScreen();
    printf("Diagnostic %s : \n\n", sTestFunctionName[SPI_TEST].c_str());
    printf("==========================================================================\n");

    srand((unsigned int)time(NULL));
    m_eResult = PASS;

    // SPI testing
    printf("SPI diagnostic\n");
    printf("--------------------------------------------------------------------------\n");

    // test SPI read write clock
    PrintDetailInfo("Test read/write SPI frequency\n");

    freq = rand() % MAX_SPI_FREQ + 1;	// max freq is 20M for Frame(FM25CL64B)
    status = (FPGA_API_STATUS)FD_SPI_SetClock(freq);  // 10K
    if (status != FD_SUCCESS)
    {
        Error("\ndiagnostic", "FD_SPI_SetClock", status);
        m_eResult = TEST_ERROR;
        goto End;
    }

    status = (FPGA_API_STATUS)FD_SPI_ReadClock(&data);

    if (status != FD_SUCCESS)
    {
        Error("\ndiagnostic", "FD_SPI_ReadClock", status);
        m_eResult = TEST_ERROR;
        goto End;
    }

    // fix freq
    freq = 62500000/((62500000/(freq*2))*2) ;
    if (data != freq)
    {
        PrintDetailInfo("Set SPI clock wrong!\n");
        m_eResult = FAIL;
        goto End;
    }
    else
        PrintDetailInfo("Clock is %u Hz... PASS\n", (UINT)freq);


    // SPI R/W data by SPI FRAM( FM25CL64B)
    PrintDetailInfo("Test read/write with FRAM( FM25CL64B)\n");

    do
    {
        PrintDetailInfo("\nCase %d:\n", mode / MAX_SPI_MODE + 1);

        offset[0] = 0;//rand() % 0xFF;// minus 1 to avoid data offset overflow
        offset[1] = 0;//rand() % 0x100;

        // test by SPI FRAM( FM25CL64B)
        spi_data[0] = 0x06;  // enable write

        status = (FPGA_API_STATUS)FD_SPI_BlockWrite(mode, spi_data, 1);

        if (status != FD_SUCCESS)
        {
            Error("\ndiagnostic", "FD_SPI_BlockWrite", status);
            m_eResult = TEST_ERROR;
            goto End;
        }
        else
        {
            rand_data[0] = 0x02;             // write command
            rand_data[1] = offset[0];        // offset hight bits
            rand_data[2] = offset[1];        // offset low bits

            PrintDetailInfo("Write Frame => Send Data: 0x%02X 0x%02X 0x%02X", rand_data[0], rand_data[1], rand_data[2]);

            for(i = 3 ; i < MAX_SPI_DATA_LEN; i++)
            {
                rand_data[i] = rand() % 0x100;
                PrintDetailInfo(" 0x%02X", rand_data[i]);
            }
            PrintDetailInfo("\n");

            status = (FPGA_API_STATUS)FD_SPI_BlockWrite(mode, rand_data, MAX_SPI_DATA_LEN);
            if (status != FD_SUCCESS)
            {
                Error("\ndiagnostic", "FD_SPI_BlockWrite", status);
                m_eResult = TEST_ERROR;
                goto End;
            }

            spi_data[0] = 0x03;             // read command
            spi_data[1] = offset[0];        // offset hight bits
            spi_data[2] = offset[1];        // offset low bits

            status = (FPGA_API_STATUS)FD_SPI_BlockRead(mode, spi_data, 3, 100, recv_data, MAX_SPI_DATA_LEN - 3);	// subtract command and offset
            if (status == FD_SUCCESS)
            {
                PrintDetailInfo("\nRead Frame  => Send Data:  --- 0x%02X 0x%02X", spi_data[1], spi_data[2]);
                for(i = 0, j = 3 ; i < MAX_SPI_DATA_LEN - 3; i++, j++)
                {
                    PrintDetailInfo(" 0x%02X", recv_data[i]);

                    if(recv_data[i] != rand_data[j])
                        m_eResult = FAIL;
                }
                PrintDetailInfo("\n");
            }
            else
            {
               Error("\ndiagnostic", "FD_SPI_BlockRead", status);
               m_eResult = TEST_ERROR;
            }

            if(m_eResult != PASS)
            {
                goto End;
            }
        }

        mode += MAX_SPI_MODE;

        // use to change mode
        if(mode <= MAX_SPI_MODE)
        {
            spi_data[0] = 0x06;  // enable write
            status = (FPGA_API_STATUS)FD_SPI_BlockWrite(mode, spi_data, 1);
        }

    }while(m_eResult == PASS && mode <= MAX_SPI_MODE);

End:

    showResult();
}

void SPITest::showResult()
{
    printf("\nDiagnostic Result: \n");
    printf("==========================================================================\n");

    printf("\n--> \"%s\"\n\n", resultEnumToString(m_eResult).c_str());

    char result[256];
    sprintf(result, "[%d] %s\t...  %s!!\n\n",
        EEPROM_TEST, sTestFunctionName[SPI_TEST].c_str(), resultEnumToString(m_eResult).c_str());

    m_sResult.append(result);

    printf("\n");

    if(g_bStepPause)
        SystemPause();
}

