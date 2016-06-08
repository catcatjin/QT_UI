#ifndef PICINTRUSIONTEST_H
#define PICINTRUSIONTEST_H

#include "TestClass.h"
#include "InterruptProcess.h"
#include "bitset"

class PICIntrusionTest : public TestClass , InterruptProcess
{
private:
        bool m_bAvail;

        MODEL_TYPE m_eModelType;

        BYTE m_bsValidDoor;
        BYTE m_bsTrigger;
        BYTE m_bsDown;
        BYTE m_bsEventIndex[MAX_PIC_INTRUSIONS];
        time_t m_tEventTime[MAX_PIC_INTRUSIONS];
        DWORD m_dwEventLog[MAX_PIC_INTRUSIONS][EVENT_BUF_LEN];

        TEST_RESULT m_eSNResult;
        TEST_RESULT m_eRTCResult;
        TEST_RESULT m_eIntrusionResult;

        bool m_bDrawing;
        int m_nWatchDog;

        void getMaxValidDoor();

        void serialNumberTest();
        void RTCTest();
        void intrusionTest();
        void showTriggerTable();

public :
        PICIntrusionTest(MODEL_TYPE modelType);
        ~PICIntrusionTest();

        bool isAvail()	{ return m_bAvail; }//;	// no use

        virtual void DealInterrupt(DWORD reg);

        virtual void diagnostic();
        virtual void showResult();
};

#endif // PICINTRUSIONTEST_H
