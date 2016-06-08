#pragma once

#ifndef SPI_TEST_H
#define SPI_TEST_H

#include "TestClass.h"

class SPITest : public TestClass
{
private:

	TEST_RESULT m_eResult;

public:
	SPITest();
	~SPITest();
		
	virtual void diagnostic();
	virtual void showResult();

};
#endif