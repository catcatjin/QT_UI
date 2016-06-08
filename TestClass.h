#ifndef TESTCLASS_H
#define TESTCLASS_H

#include "Platform.h"
#include "TestDefaultParam.h"
#include "OutputTextTool.h"

#define TEST_BIT(src, bit)	(src & (((DWORD)0x01) << (bit)))
#define SET_BIT(src, bit)	(src  |= ((DWORD)0x01) << (bit))
#define NONE_BIT(src)		(src == 0)
#define RESET_BIT(src, bit)	(src &= ~(((DWORD)0x01)<<(bit)))
#define INVERSE_BIT(src, bit)	(src  ^= ((DWORD)0x01) << (bit))

enum TEST_RESULT
{
	TEST_ERROR	= -2,
	FAIL	= -1,
	NOT_AVAIL,
	TIME_OUT,
	PASS
};

class TestClass
{
protected:
	std::string m_sResult;

public:

	virtual void diagnostic() = 0;

	virtual void showResult() = 0;

    virtual std::string getResultString()	{return m_sResult;}//;

	virtual std::string resultEnumToString(TEST_RESULT eResult)
	{
		switch(eResult)
		{
		case TEST_ERROR:
			return "Error";
		case FAIL:
			return "Fail";
		case PASS:
			return "Pass";
		case TIME_OUT:
			return "Wait timeout";
		default:
			return "Not available";
		}
    }//;
};

#endif
