#ifndef GPITEST_H
#define GPITEST_H

#include "TestClass.h"
#include "InterruptProcess.h"
#include "bitset"

enum GPI_TRIGGER_MODE
{
    RISING_EDGE = 1,
    FALLING_EDGE,
    LEVEL_EDGE
};

class GPITest : public TestClass, InterruptProcess
{
private:
    DWORD m_dwAvail;
    DWORD m_dwTrigger;
    DWORD m_dwDown;

    TEST_RESULT m_eResult;

    bool m_bDrawing;
    int m_nMaxBit;
    int m_mColumn;
    int m_nWatchDog;

    void showTriggerTable();

public :
    GPITest();
    ~GPITest();

    bool isAvail()
    {
        return m_dwAvail > 0;
    }	// no use

    virtual void DealInterrupt(DWORD reg);

    virtual void diagnostic();
    virtual void showResult();
};

#endif // GPITEST_H
