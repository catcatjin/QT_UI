#include "SASTest.h"


SASTest::SASTest() : InterruptProcess(SAS_IRQ_TYPE, IRQ_SAS)
{
	m_pRespCmd = NULL;

	m_bKeepRecv = false;

	DWORD dwAvail;

	FD_GetNGPIOAvail(&dwAvail);

	m_bAvail = TEST_BIT(dwAvail, SAS) > 0;

	SET_BIT(m_stIRQparam.NIO , SAS);

	// generator response data
	m_pRespCmd = new BYTE[SAS_RESPONSE_DATA_CNT];

	for(BYTE i = 0 ; i < SAS_RESPONSE_DATA_CNT ; i++)
	{
		m_pRespCmd[i] =i + 0x11;
	}
}

SASTest::~SASTest()
{
	if(m_pRespCmd != NULL)
	{
		delete [] m_pRespCmd;
		m_pRespCmd = NULL;
	}
}

bool SASTest::isSASCmdValid(BYTE *buf, DWORD len)
{
	if(buf == NULL || len == 0)
		return false;

	// general poll
	if(buf[0] & GENERAL_POLL_BIT)
	{
		return true;
	}
	else if(len == 2)
	{
		return true;
	}

	if(len - 2 > 1)	// check crc
	{
		WORD wCRC = 0;
        BYTE crc1, crc2;

		FD_CalcSASCRC(buf, (int)(len - 2), 0, &wCRC);

        crc1 = (BYTE)((wCRC >> 8) & 0xFF);
        crc2 = (BYTE)(wCRC & 0xFF);
        if (buf[len - 2] == crc2 && buf[len - 1] == crc1)
        {
            return true;
        }
        else
        {
            PrintDetailInfo("SAS command CRC error!\n");
		}
	}
	else
	{
		PrintDetailInfo("SAS command is wrong format\n");
	}

	return false;
}

void SASTest::generatorResponseMsg(BYTE *cmdBuf,DWORD cmdLen, BYTE *respBuf, DWORD &respLen)
{
	 BYTE cmd = 0;
     WORD crc = 0;

	 if(cmdBuf == NULL || cmdLen == 0)
	 {
		 respLen = 0;
		 m_bKeepRecv = false;
		 return;
	 }

	 if(cmdBuf[0] & GENERAL_POLL_BIT)
	 {
		 //general poll
		 if(m_nSendCnt != SAS_RESPONSE_DATA_CNT)
		 {
			 respBuf[0] = m_pRespCmd[m_nSendCnt];
			 m_nSendCnt++;
		 }
		 else
		 {	 // no event
			 respBuf[0] = 0;
		 }

		 respLen = 1;
		 m_bGeneralPoll = true;
	 }
	 else
	 {
		 // long poll

		 // no implentation long poll ui
		/* if(cmdBuf[1] == LONG_USER_CMD)
		 {
			respBuf[0] = DEFAULT_SAS_ADDRSS;
			respBuf[1] = LONG_USER_CMD;
		 }*/

		respLen = 0;
		m_bKeepRecv = false;
		return;
	 }
}

TEST_RESULT SASTest::dealWithCmd()
{
	BYTE cmdData[512];
	BYTE respData[512];
	DWORD dataLen = 0, respLen = 0;

	FPGA_API_STATUS status = (FPGA_API_STATUS)FD_GetSASCmd(cmdData, &dataLen);

    if (status != FD_SUCCESS)
    {
		Error("dealWithCmd", "FD_GetSASCmd", status);
		return TEST_ERROR;
	}

	if(g_bDetailInfo)
	{
		printf("[RX] ");

		for(UINT i = 0; i < dataLen ; i++)
			printf("%2x ", cmdData[i]);

		printf("\n");
	}

	if (isSASCmdValid(cmdData, dataLen))
    {
        generatorResponseMsg(cmdData, dataLen, respData, respLen);

        if (respLen != 0)
        {
            status = (FPGA_API_STATUS)FD_SetSASResp(cmdData, dataLen);

			if(g_bDetailInfo)
			{
				printf("[TX] ");

				for(UINT i = 0 ;i < dataLen; i++)
					printf("%2x ", cmdData[i]);

				printf("\n");
			}

			if(status != FD_SUCCESS)
			{
				Error("dealWithCmd", "FD_SetSASResp", status);
				return TEST_ERROR;
			}
        }
	}
	return PASS;
}

void SASTest::DealInterrupt(DWORD reg)
{
	bool bTrigger = false;

	if(reg == SAS_GET_CMD)
	{
	    m_bDealWithCmd = true;
		dealWithCmd();

		if(m_eResult == TEST_ERROR)
		{
			m_bKeepRecv = false;
		}
	}
	else
	{
		PrintDetailInfo("[S] %2x\n", reg);
		if (m_bGeneralPoll)
		{
			if(reg == SAS_GET_ACK)
			{
				m_bGeneralPoll = false;

				if(SAS_RESPONSE_DATA_CNT == m_nRecvCnt)
				{
					m_bKeepRecv = false;
					return;
				}
				m_nRecvCnt++;
			}
			else if (reg == SAS_REPLY_DATA_TIMEOUT) //timeout
            {
                m_nTimeOutCnt++;
                if(m_nTimeOutCnt >= TIME_OUT_CNT)
                {
                    m_bKeepRecv = false;
                }
            }
		}
	}
}

void SASTest::diagnostic()
{
	FPGA_API_STATUS status;
	int nPollcnt = 0, temp_RecvCnt = 0;

	ClearScreen();
	printf("Diagnostic %s : \n\n", sTestFunctionName[SAS_TEST].c_str());
	printf("==========================================================================\n");


	m_eResult = FAIL;

	status = (FPGA_API_STATUS)FD_SetSASAddress(DEFAULT_SAS_ADDRSS);

	if(status != FD_SUCCESS)
	{
		Error("diagnostic", "FD_SetSASAddress", status);
		m_eResult = TEST_ERROR;
		goto End;
	}

	m_bKeepRecv = true;
	temp_RecvCnt = m_nRecvCnt = m_nSendCnt = m_nTimeOutCnt = 0;

	if(!enableInterrupt())
	{
		m_eResult = TEST_ERROR;
		goto End;
	}

	// every recv only wait 800 ms
	do
	{
		if(!g_bDetailInfo)
			OutputTextTool::waitText(COMPORT_POLLTIME);

		Sleep(COMPORT_POLLTIME);

		if(m_nRecvCnt == temp_RecvCnt)
		{
			nPollcnt++;
		}
		else
		{
			temp_RecvCnt = m_nRecvCnt;
			nPollcnt = 0;
		}

	}while(m_bKeepRecv && nPollcnt < COMPORT_POLL_MAX_CNT);

	disableInterrupt();

	if(m_eResult == TEST_ERROR)
	{
		goto End;
	}
	else if(m_nTimeOutCnt >= TIME_OUT_CNT)
	{
		m_eResult = TIME_OUT;
	}
	else if(SAS_RESPONSE_DATA_CNT > m_nSendCnt ||
		SAS_RESPONSE_DATA_CNT > m_nRecvCnt)
	{
		m_eResult = FAIL;
	}
	else
	{
		m_eResult = PASS;
	}

End:

	showResult();
}

void SASTest::showResult()
{
	printf("\nDiagnostic Result: \n");
	printf("==========================================================================\n");

	m_sResult = resultEnumToString(m_eResult);

	printf("\n--> \"%s\"\n\n", m_sResult.c_str());

	char result[256];
	sprintf(result, "[%d] %s\t\t\t\t...  %s!!\n\n", SAS_TEST,
		sTestFunctionName[SAS_TEST].c_str(), m_sResult.c_str());

	m_sResult.assign(result);

	printf("\n");

	if(g_bStepPause)
		SystemPause();
}
