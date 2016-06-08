#include "InterruptProcess.h"
#include "TestClass.h"

InterruptProcess::InterruptProcess(IRQ_TYPE type, IRQ_REG_IDX idx)
{
	SET_BIT(m_checkBitValue, type);
	m_eIrqRegIdx = idx;

	memset(&m_stIRQparam, 0, sizeof(INT_ENABLE_ITEM));
}

InterruptProcess::~InterruptProcess(void)
{
}

void STD_CALL InterruptCallback(void* pParam, void* pClass)
{
	INT_MSG *stInterruptInfo = (INT_MSG*)pParam;
	InterruptProcess* pThis = (InterruptProcess*)pClass;

	BYTE type = stInterruptInfo->int_type;

	if(pThis->isTrigger(type))
	{
		pThis->DealInterrupt(stInterruptInfo->IntReg[pThis->getRegOrder()]);
	}
}

bool InterruptProcess::isTrigger(BYTE type)
{
	return (type & m_checkBitValue) > 0;
}

IRQ_REG_IDX InterruptProcess::getRegOrder()
{
	return m_eIrqRegIdx;
}

bool InterruptProcess::isParamValid()
{
	for(int i = 0; i < 8 ; i++)
	{
		if(i < 4 && m_stIRQparam.GPIO_I[i] > 0)
			return true;

		if(m_stIRQparam.GPI[i] > 0)
			return true;
	}

	if(m_stIRQparam.NIO > 0)
		return true;

	return false;
}

bool InterruptProcess::isInterruptEnabled()
{
	DWORD dwAvail = 0;
	BYTE  avail = 0;
	int nGPIO_offset = 8;

	for(int i = 0; i < 8 ; i++)
	{
		if(i < 4 && m_stIRQparam.GPIO_I[i] > 0)
		{
			FD_GetIntEnable(nGPIO_offset + i, &dwAvail);

			if(dwAvail > 0)
				return true;
		}

		if(m_stIRQparam.GPI[i] > 0)
		{
			FD_GetIntEnable(i, &dwAvail);

			if(dwAvail > 0)
				return true;
		}
	}

	if(m_stIRQparam.NIO > 0)
	{
		FD_GetNGPIOIntEnable(&avail);
		return avail > 0;
	}

	return false;
}

bool InterruptProcess::enableInterrupt()
{

	if(!isParamValid())
	{
		Error("Program Error", "Interrupt parameter wrong!", 0);
		return false;
	}

	disableInterrupt();

	FPGA_API_STATUS dwStatus = (FPGA_API_STATUS)FD_IntEnableClassType(InterruptCallback, this, m_stIRQparam);

	if(dwStatus != FD_SUCCESS)
	{
		Error("enableInterrupt", "enable Interrupt", dwStatus);
		return false;
	}

	return true;
}

bool InterruptProcess::disableInterrupt()
{
	if(!isInterruptEnabled())
		return false;

	FPGA_API_STATUS dwStatus = (FPGA_API_STATUS)FD_IntDisable();

	if(dwStatus != FD_SUCCESS)
	{
		Error("disableInterrupt", "disable Interrupt", dwStatus);
		return false;
	}
	return true;
}
