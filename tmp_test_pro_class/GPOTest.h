#pragma once

#ifndef GPOTEST_H
#define GPOTEST_H

#include "TestClass.h"
#include "bitset"

class GPOTest : public TestClass
{
private:
	DWORD m_dwAvail;
	int m_nMaxBit;
	
	TEST_RESULT m_eResult;

	void normalTest();
	void pwmTest(DWORD dwNGPIO);

public :
	GPOTest();
	~GPOTest();

	bool isAvail()	{ return m_dwAvail > 0; };	// no use

	virtual void diagnostic();
	virtual void showResult();
};

#endif