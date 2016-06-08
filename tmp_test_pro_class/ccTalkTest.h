#pragma once

#ifndef CCTALK_TEST_H
#define CCTALK_TEST_H

#include "TestClass.h"
#include "SerialPort.h"
#include "vector"

typedef vector<SerialPort::COM_PORT_IDX> COMPORT_VECTOR;

class CCTalkTest : public TestClass, ComportRecvCallback
{
private :
    MODEL_TYPE m_eModelType;

    TEST_RESULT m_eResult;

    /*BYTE	m_led;
    WORD	m_wDuration;*/

    SerialPort m_Comport;
    COMPORT_VECTOR m_vComPortList;

    BYTE *m_pData;
    const BYTE *m_pCommandQueue;

    bool m_bKeepRecv;

    int m_nQueueIndex;
    int m_nQueueNum;
    int m_nRecvCnt;

    BYTE CheckSum8(const BYTE* input, int len);
    int generatorCommandCode(BYTE command);

    void getSupportComport();

    virtual int RecvCallBackFn(const BYTE* buffer, int bufferLen);

public:
    CCTalkTest(MODEL_TYPE modelType);
    ~CCTalkTest();

    virtual void diagnostic();

    virtual void showResult();
};

#endif
