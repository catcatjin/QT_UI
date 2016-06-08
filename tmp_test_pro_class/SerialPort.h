#pragma once

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include "Platform.h"

#ifdef WINDOWS

#define DEVICE_HANDLE HANDLE

#else

#include "pthread.h"

#define DEVICE_HANDLE int
#define ONESTOPBIT 0
#define HANDLE	int
#define INVALID_HANDLE_VALUE -1

#endif

#ifndef BOOL
#define BOOL

#define TRUE	1
#define FALSE	0
#endif

#define RECV_BUFFER_COUNT	4

typedef struct _DATA_BUFFER
{
	int dataLen;
	int bufLen;
	BYTE *pData;
}DATA_BUFFER;

class ComportRecvCallback
{
public:
	virtual int RecvCallBackFn(const BYTE* buffer, int bufferLen) = 0;
};

class SerialPort
{
public:

	enum SERIALPORT_ERROR
	{
	    DEVICE_CTRL_FAIL = -1,
		NONE_ERROR = 0,
		CONNECT_FAIL,
		SET_PARAM_FAIL,
		RECV_THREAD_FAIL
	};

	enum COM_PORT_IDX
	{
		COM1,
		COM2,
		COM3,
		COM4,
		COM5,
		COM6,
		MAX_PORT
	};

	SerialPort();
	~SerialPort();

	std::string ErrorString(SERIALPORT_ERROR eErr);

	bool isOpen()	{ return m_hDevice != INVALID_HANDLE_VALUE;}

	int connect(COM_PORT_IDX ePort);

	// return value is SERIALPORT_ERROR
    SerialPort::SERIALPORT_ERROR connect(std::wstring sDevice, DWORD baudRate = 9600,
		BYTE stopBit = ONESTOPBIT, BYTE byteSize = 8);

	void disconnect();

	int sendData(unsigned char *buffer, int len);

	// this function will new data, and caller should do delete itself
	int receiveData(BYTE *buffer, int len);

	int getBytesToRead();

	void RecvFn();

	void clearPortBuffer();

#ifdef LINUX
	ComportRecvCallback	*m_pFn;
#else
	void setCallback(ComportRecvCallback* object);
#endif

private:

	bool m_bThreadWork;

	DEVICE_HANDLE	m_hDevice;
    std::wstring	m_sDeviceName;

	BYTE			m_CurbufferIdx;
	DATA_BUFFER		m_stRecvBuffer[RECV_BUFFER_COUNT];

#ifdef LINUX
    pthread_t       m_pRecvThread;
#else
	HANDLE			m_pRecvThread;
	OVERLAPPED		m_Overlapped;

	ComportRecvCallback	*m_pFn;
#endif
};

#endif
