#include "cctalktest.h"
#define TEST_QUEUE_CNT		9
#define TEST_QUEUE_2_CNT	10


static const BYTE TEST_QUEUE[TEST_QUEUE_CNT] =
				{ 254, 245, 244, 192, 246, 242, 241, 4, 197};

static const BYTE TEST_QUEUE_2[TEST_QUEUE_2_CNT] =
				{ 144, 145, 144, 145, 144, 145, 144, 145, 144, 145 };

typedef struct _RECV_DATA
{
	int dataLen;
	int bufSize;
	BYTE *pData;
}RECV_DATA;

RECV_DATA recvData = {0, 0, NULL};

CCTalkTest::CCTalkTest(MODEL_TYPE modelType)
{
	m_eModelType = modelType;

	//m_wDuration = m_led = 0;
	m_pData = NULL;

	m_bKeepRecv = false;

	m_nQueueIndex = 0;
	m_nQueueIndex = 0;

	getSupportComport();
}

CCTalkTest::~CCTalkTest()
{
	if(m_pData != NULL)
	{
		delete [] m_pData;
		m_pData = NULL;
	}
}

void CCTalkTest::getSupportComport()
{
	m_vComPortList.clear();

	switch (m_eModelType)
	{
	case PGB_5021:
	case PGB_6040:
		m_vComPortList.push_back(SerialPort::COM5);
		m_vComPortList.push_back(SerialPort::COM6);
		break;

	case PGB_5120S:
		m_vComPortList.push_back(SerialPort::COM6);
		break;

	case PGB_4020:
		m_vComPortList.push_back(SerialPort::COM2);
		m_vComPortList.push_back(SerialPort::COM3);
		m_vComPortList.push_back(SerialPort::COM4);
		break;

	case PGB_5322:
	case GCB_203:
		m_vComPortList.push_back(SerialPort::COM3);
		m_vComPortList.push_back(SerialPort::COM4);
		break;

	case PGB_5320:
	case PGB_5321R1:
	default:
		m_vComPortList.push_back(SerialPort::COM5);
		break;
	}
}

 BYTE CCTalkTest::CheckSum8(const BYTE* input, int len)
{
    UINT total = 0;
    BYTE checksum = 0;

    for (int i = 0; i < len; i++)
    {
        total += input[i];
    }

    checksum = (BYTE)(total % 256);
    checksum = (BYTE)(256 - checksum);

    return checksum;
}

int CCTalkTest::generatorCommandCode(BYTE command)
{
    BYTE Dest = 40, Src = 1, len = 0, crc=0;

    switch (command)
    {
        case 4:
        case 192:
        case 197:
        case 241:
        case 242:
        case 244:
        case 245:
        case 246:
        case 254:
        case 145:   //self defined:send this command and PIC will response 128BYTEs
            len=0;
            break;
        case 144:   //self defined:send this command with 128BYTEs
            len = 128;
            break;
        //case 146:   //self defined:control PIC Port C b7~b1 + duration
        //    len = 3;
            break;
    }

	if(m_pData != NULL && sizeof(m_pData) < len + 5)
	{
		delete [] m_pData;
		m_pData = NULL;
	}

	if(m_pData == NULL)
	{
		m_pData = new BYTE[len + 5];
	}

	m_pData[0] = Dest;
    m_pData[1] = len;		// data length
    m_pData[2] = Src;
    m_pData[3] = command;	// header
	len += 5; // prefix info and the last check sum bit

    if (command == 144)
    {
        for (int i = 4; i < len; i++)
        {
            m_pData[i] = (BYTE) rand() % 0xFE + 1;
        }
    }
	//else if (command == 146)
    //{
    //    m_pData[4] = m_led;
    //    m_pData[5] = (BYTE)(m_wDuration>>8);
    //    m_pData[6] = (BYTE)(m_wDuration & 0xff);
    //}

    crc = CheckSum8(m_pData, len - 1);

	m_pData[len - 1] = crc;

	return len;
}

int CCTalkTest::RecvCallBackFn(const BYTE* buffer, int bufferLen)
{
	int usedLen = 0, dataLen = 0;
	BYTE* data;

	if(bufferLen <= 0)
		return 0;

	// re-alloc size and copy data
	if(recvData.bufSize < bufferLen + recvData.dataLen)
	{
		data = new BYTE[bufferLen + recvData.dataLen];
		recvData.bufSize = bufferLen + recvData.dataLen;

		memcpy(data, recvData.pData, recvData.dataLen);

		delete [] recvData.pData;

		recvData.pData = data;
	}

	// copy recv data
	memcpy(recvData.pData + recvData.dataLen, buffer, bufferLen);
	recvData.dataLen += bufferLen;

	data = recvData.pData;
	dataLen = recvData.dataLen;

	while(usedLen < recvData.dataLen)
	{
		// offset to destination address
		while(data[0] != 40 && data[0] != 1)
		{
			data++;
			usedLen++;
		}

		dataLen = (BYTE)(data[1] + 5);

		if (recvData.dataLen >= usedLen + dataLen)
		{
			// check without last crc bit
			if (CheckSum8(data, dataLen - 1) == data[dataLen - 1])
			{
				if (data[0] == 1)
				{
					if (data[3] == 0)    //Ack
					{
						m_nRecvCnt++;
						PrintDetailInfo("\nReceive OK!(ACK)\n");

						// all recv
						if(m_nRecvCnt == m_nQueueNum)
						{
							m_bKeepRecv = false;
						}
					}
					else if (data[3] == 5)    //NAK
						PrintDetailInfo("Receive Fail!(NAK)\n");

					if (m_nQueueIndex != m_nQueueNum)
					{
						/*if (test_type == 3)
							led = CmdQueueData[m_nQueueIndex];*/

						/*if (TestInterval == 0)
						{   */
							// send next command
							int len = generatorCommandCode(m_pCommandQueue[m_nQueueIndex++]);
							m_Comport.sendData(m_pData, len);
						/*}
						else
						{
							fGetResponse = false;
							fSendNext = true;
						}    */
					}
				}
			}
			else
			{
				printf("**Error: receive with incorrect data\n");
				m_eResult = TEST_ERROR;
				return 0;
			}

			usedLen += dataLen;
			data += dataLen;
		}
		else
		{
			// no enough size, wait next recv callback
			break;
		}
	}

	// move buf

	recvData.dataLen -= usedLen;

	if(dataLen != 0)
		memcpy(recvData.pData, data, recvData.dataLen);

	return 0;
}

void CCTalkTest::diagnostic()
{
	int nPollcnt, nMaxPollCnt, len;

	ClearScreen();
	printf("Diagnostic %s : \n\n", sTestFunctionName[CCTALK_TEST].c_str());
	printf("==========================================================================\n");
	if(m_vComPortList.size() == 0)
	{
		printf("%s function not available!", sTestFunctionName[CCTALK_TEST].c_str());
		m_eResult = NOT_AVAIL;
		goto End;
	}

#ifdef WINDOWS
	m_Comport.setCallback(this);
#else
    m_Comport.m_pFn = this;
#endif

	for(COMPORT_VECTOR::iterator iter =  m_vComPortList.begin(); iter != m_vComPortList.end(); iter++)
	{
		if(m_Comport.isOpen())
			m_Comport.disconnect();

		m_Comport.connect(*iter);

		// Test_Initialization --------------------------------------------
#pragma region Test Initialization
		m_nRecvCnt = m_nQueueIndex = 0;
		m_nQueueNum = TEST_QUEUE_CNT;

		nPollcnt = 0;
		m_bKeepRecv = true;
		m_pCommandQueue = TEST_QUEUE;
		nMaxPollCnt = COMPORT_POLL_MAX_CNT*2;

		len = generatorCommandCode(m_pCommandQueue[m_nQueueIndex++]);

		m_Comport.sendData(m_pData, len);

		do
		{
			if(!g_bDetailInfo)
				OutputTextTool::waitText(COMPORT_POLLTIME);

			Sleep(COMPORT_POLLTIME);
		}while(m_bKeepRecv && nPollcnt++ < nMaxPollCnt);

		if(m_nRecvCnt != m_nQueueIndex)
		{		   
			if(m_nRecvCnt == 0)
				PrintDetailInfo("No request from serial port\n");
			else
				PrintDetailInfo("Receive loss\n");

			m_eResult = FAIL;
			printf("goto end\n");
			goto End;
		}

		if(m_nQueueIndex != m_nQueueNum)
		{
			PrintDetailInfo("Receive data time out\n");
			m_eResult = TIME_OUT;
			goto End;
		}

#pragma endregion
		// ------------------------------------------------------------------
		//Sleep(COMMAND_INTERVAL);

		// Test_Block -------------------------------------------------------
#pragma region Test Block
		m_nRecvCnt = m_nQueueIndex = 0;
		m_nQueueNum = TEST_QUEUE_2_CNT;

		nPollcnt = 0;
		m_bKeepRecv = true;
		m_pCommandQueue = TEST_QUEUE_2;
		nMaxPollCnt = 5 * TEST_QUEUE_2_CNT;

		PrintDetailInfo("2. Test Block\n");

		len = generatorCommandCode(m_pCommandQueue[m_nQueueIndex++]);
		m_Comport.sendData(m_pData, len);

		do
		{
			if(!g_bDetailInfo)
				OutputTextTool::waitText(COMPORT_POLLTIME);

			Sleep(COMPORT_POLLTIME);
		}while(m_bKeepRecv && nPollcnt++ < nMaxPollCnt);


		if(m_nRecvCnt != m_nQueueIndex)
		{
			if(m_nRecvCnt == 0)
				PrintDetailInfo("No request from serial port\n");
			else
				PrintDetailInfo("Receive loss\n");

			m_eResult = FAIL;
			goto End;
		}

		if(m_nQueueIndex != m_nQueueIndex)
		{
			PrintDetailInfo("Receive data time out\n");
			m_eResult = TIME_OUT;
			goto End;
		}

		Sleep(COMMAND_INTERVAL);
#pragma endregion
	}

	m_eResult = PASS;

End:

	if(m_Comport.isOpen())
		m_Comport.disconnect();

	showResult();
}

void CCTalkTest::showResult()
{
	printf("\nDiagnostic Result: \n");
	printf("==========================================================================\n");

	m_sResult = resultEnumToString(m_eResult);

	printf("\n--> \"%s\"\n\n", m_sResult.c_str());

	char result[256];
	sprintf(result, "[%d] %s\t\t\t...  %s!!\n\n", CCTALK_TEST,
		sTestFunctionName[CCTALK_TEST].c_str(), m_sResult.c_str());

	m_sResult.assign(result);

	printf("\n");

	if(g_bStepPause)
		SystemPause();
}
