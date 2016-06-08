#include "GPITest.h"


GPITest::GPITest() : InterruptProcess(GPI_IRQ_TYPE, IRQ_GPI), m_dwAvail(0L)
{
	FD_GetGPIAvail(0, &m_dwAvail);
printf("avail = %d\n", m_dwAvail);
	int count = sizeof(DWORD) * 8;
	for(DWORD i = 0; i < count ; i++)
	{
		if(TEST_BIT(m_dwAvail, i))
		{
		    printf("%d >> %d =>%d,\t",i, 1<<i, m_dwAvail&(1<<i));
			m_nMaxBit = i;
		}
	}
printf("m_nMaxBit = %d\n", m_nMaxBit);
	m_stIRQparam.GPI[0] = m_dwAvail;
	m_nWatchDog = 0;
}

GPITest::~GPITest()
{
}

void GPITest::showTriggerTable()
{
	m_bDrawing = true;

	ClearScreen();
	printf("Diagnostic %s : \n\n", sTestFunctionName[GPI_TEST].c_str());
	printf("==========================================================================\n");

	printf("Please press the available GPI from 0 to %d ...\n\n", m_nMaxBit);

	for(int idx = 0, col = 0; col < m_mColumn; col++, idx += GPI_COUNT_IN_EACH_ROW)
	{
		for(int row = 0 ; row < GPI_COUNT_IN_EACH_ROW ; row++)
		{
			if(row == 0)
				printf(" ========");
			else if(row == GPI_COUNT_IN_EACH_ROW - 1)
				printf("|======== \n");
			else
				printf("|========");
		}

		printf("|");

		for(int row = 0, index = idx ; row < GPI_COUNT_IN_EACH_ROW ; row++, index++)
		{
			if(index <= m_nMaxBit)
				printf(" GPI %2d |", index);
            else
                printf("        |");
		}

		printf("\n|========");

		for(int row = 1 ; row < GPI_COUNT_IN_EACH_ROW ; row++)
		{
			printf("|========");
		}
		printf("|\n|");

		for(int row = 0, index = idx ; row < GPI_COUNT_IN_EACH_ROW ; row++, index++)
		{
			if(index <= m_nMaxBit && TEST_BIT(m_dwAvail, index) )
				printf(" %s |", TEST_BIT(m_dwDown, index) ? "Down  " : TEST_BIT(m_dwTrigger, index) ? "  Ok  " : "Press ");
			else
				printf("        |");
		}
		printf("\n");

		for(int row = 0 ; row < GPI_COUNT_IN_EACH_ROW ; row++)
		{
			if(row == 0)
				printf(" ========");
			else if(row == GPI_COUNT_IN_EACH_ROW - 1)
				printf("|======== \n\n");
			else
				printf("|========");
		}
	}

	m_bDrawing = false;
}

void GPITest::DealInterrupt(DWORD reg)
{
	bool bRefresh = false;

	for(int i = m_nMaxBit; i >= 0 ; i--)
	{
		if(TEST_BIT(reg, i) && TEST_BIT(m_dwAvail, i))
		{
			SET_BIT(m_dwTrigger, i);

			INVERSE_BIT(m_dwDown, i);

			bRefresh = true;
		}
	}

	if(bRefresh)
	{
		m_nWatchDog = 0;
		showTriggerTable();
	}
}

void GPITest::diagnostic()
{
	DWORD dwAllTriggerValue = ~0L;
	int nMaxCnt = MAX_WAIT_TIME / TRIGGER_DETECT_INTERVAL;

	m_mColumn = m_nMaxBit / GPI_COUNT_IN_EACH_ROW + (m_nMaxBit % GPI_COUNT_IN_EACH_ROW > 0 ? 1 : 0);

	m_dwDown = 0;
	m_dwTrigger = ~m_dwAvail;

	FPGA_API_STATUS status = (FPGA_API_STATUS)FD_SetGPIIntTriAll(0, LEVEL_EDGE);

	if(status != FD_SUCCESS)
	{
		Error("diagnostic", "FD_SetGPIIntTriAll", status);
		m_eResult = TEST_ERROR;
		goto End;
	}

	showTriggerTable();

	if(!enableInterrupt())
	{
		m_eResult = TEST_ERROR;
		goto End;
	}

	m_nWatchDog = 0;

	do
	{
		if(!m_bDrawing)
			OutputTextTool::waitText(SLEEP_MS_TIME);

		Sleep(TRIGGER_DETECT_INTERVAL);

		m_nWatchDog++;

	}while(m_nWatchDog < nMaxCnt && m_dwTrigger != dwAllTriggerValue);

	printf("\r");

	disableInterrupt();

	if(m_nWatchDog >=  nMaxCnt)
		m_eResult = TIME_OUT;
	else
		m_eResult = PASS;

	m_dwDown = 0;
	showTriggerTable();

End:

	showResult();
}

void GPITest::showResult()
{
	printf("\nDiagnostic Result: \n");
	printf("==========================================================================\n");

	m_sResult = resultEnumToString(m_eResult);

	PrintDetailInfo("\n\n");
	printf("\n--> \"%s\"\n\n", m_sResult.c_str());

	char result[256];
	sprintf(result, "[%d] %s\t\t\t\t...  %s!!\n\n", GPI_TEST,
		sTestFunctionName[GPI_TEST].c_str(), m_sResult.c_str());

	m_sResult.assign(result);

	printf("\n");

	if(g_bStepPause)
		SystemPause();
}
