#ifndef INTERRUPTPROCESS_H
#define INTERRUPTPROCESS_H

#include "FpgaPicAPI.h"
#include "FunctionList.h"

class InterruptProcess
{
public:
    InterruptProcess(IRQ_TYPE type, IRQ_REG_IDX idx);
    ~InterruptProcess(void);

    bool isInterruptEnabled();
    bool enableInterrupt();
    bool disableInterrupt();

    IRQ_REG_IDX getRegOrder();

    bool isTrigger(BYTE type);
    virtual void DealInterrupt(DWORD reg) = 0;

private:
    bool isParamValid();

protected:

    BYTE	m_checkBitValue;

    INT_ENABLE_ITEM	m_stIRQparam;
    IRQ_REG_IDX m_eIrqRegIdx;
};

#endif // INTERRUPTPROCESS_H
