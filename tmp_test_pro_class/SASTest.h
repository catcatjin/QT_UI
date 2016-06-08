#pragma once

#ifndef SAS_TEST_H
#define SAS_TEST_H

#include "TestClass.h"
#include "InterruptProcess.h"
#include "vector"

class SASTest : public TestClass, InterruptProcess
{
private :
	MODEL_TYPE m_eModelType;

	bool m_bAvail;

	TEST_RESULT m_eResult;

	BYTE *m_pRespCmd;

	bool m_bKeepRecv;
	bool m_bGeneralPoll;
	bool m_bDealWithCmd;

	int m_nRecvCnt;
	int m_nSendCnt;
	int m_nTimeOutCnt;

	TEST_RESULT dealWithCmd();
	bool isSASCmdValid(BYTE *buf, DWORD len);
	void generatorResponseMsg(BYTE *cmdBuf,DWORD cmdLen, BYTE *respBuf, DWORD &respLen);

public:
	SASTest();
	~SASTest();

	bool isAvail()	{ return m_bAvail;};

	virtual void DealInterrupt(DWORD reg);

	virtual void diagnostic();

	virtual void showResult();
};

#endif
