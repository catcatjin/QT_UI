#pragma once

#ifndef RANDOMGENERATORTEST_H
#define RANDOMGENERATORTEST_H

#include "TestClass.h"
#include "vector"

class RandomGeneratorTest: public TestClass
{
private:
	bool m_bAvail;

	int m_nAmount;

	TEST_RESULT m_eResult;

	vector<DWORD> m_vRepeat;
	DWORD *m_dwRandomNumber;

public:
	RandomGeneratorTest();
	~RandomGeneratorTest();
	
	bool isAvail()	{ return m_bAvail; };	// no use

	virtual void diagnostic();
	virtual void showResult();
};

#endif