#ifndef SIXTEENBITTIMERTEST_H
#define SIXTEENBITTIMERTEST_H


#include "TestClass.h"
#include "InterruptProcess.h"

enum INTERVAL_MODE
{
        ALL_TIMER_SAME,
        ALL_TIMER_DIFF,
        MAX_INTERVAL_MODE
};

enum TIMER_ID
{
        TIMER_ID_0,
        TIMER_ID_1,
        TIMER_ID_2,
        TIMER_ID_3,
        MAX_TIMER
};

#ifndef WINDOWS
typedef struct _SYSTEMTIME
{
    DWORD wSecond;
    DWORD wMilliseconds;
}SYSTEMTIME;
#endif

class SixteenBitTimerTest : public TestClass, InterruptProcess
{
private:

        bool m_bAllTimerReachAmount;
        INTERVAL_MODE m_eIntervalMode;

        bool m_bAvail[MAX_TIMER];
        TEST_RESULT m_eTimerResult;

        // record diag param
        int m_nAmount[MAX_INTERVAL_MODE];
        int m_nTriggerCount[MAX_TIMER][MAX_INTERVAL_MODE];
        int m_nSameInterval;
        int m_nInterval[MAX_TIMER];

        // caculate param
        unsigned long m_ulFPGAStartCounting[MAX_TIMER][MAX_INTERVAL_MODE];
        unsigned long *m_pulFPGATriggerCounting[MAX_TIMER];

        SYSTEMTIME m_stSystemStartTime[MAX_TIMER][MAX_INTERVAL_MODE];
        SYSTEMTIME *m_pstSystemTriggerTime[MAX_TIMER];

        UINT *m_puSystemTriggerInterval[MAX_TIMER];

        int subSystemTime(SYSTEMTIME& time1, SYSTEMTIME& time2);

        bool isNeedReallocBuffer(int totalAmount);
        void createDynamicBuffer(int totalAmount);
        void releaseDynamicBuffer();

        void generateRandomInterval(int *interval, int baseTime, int randRange);

        void sameIntervaldiagnostic(int amount, int interval);
        void diffIntervaldiagnostic(int amount, int interval[MAX_TIMER]);

protected:

        void resetAll(INTERVAL_MODE mode);
        void reset(TIMER_ID id, INTERVAL_MODE mode);

        void setIntervalAll(unsigned short time);
        void setInterval(TIMER_ID id, unsigned short time, INTERVAL_MODE mode);

        unsigned long getTick(TIMER_ID id);

        void DealInterrupt(DWORD reg);

public:
        SixteenBitTimerTest();
        ~SixteenBitTimerTest(void);

        bool isAvail(TIMER_ID id);	// no use

        void diagnostic(int interval, int amount_same, int amount_diff,
                                                                  int baseTime, int randRange) ;

        virtual void diagnostic() {	diagnostic(500, 6, 6, 200, 1800);	}
        virtual void showResult();
};

#endif // SIXTEENBITTIMERTEST_H
