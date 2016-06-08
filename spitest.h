#ifndef SPITEST_H
#define SPITEST_H

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

#endif // SPITEST_H
