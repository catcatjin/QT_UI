#include "SerialPort.h"

#ifdef WINDOWS

#define TIME_OUT_TIME	5000	// 5s

static const std::wstring DEVICE_NAME[SerialPort::MAX_PORT] = { L"COM1", L"COM2", L"COM3", L"COM4", L"COM5", L"COM6"};

SerialPort::SerialPort()
{
	m_pRecvThread = INVALID_HANDLE_VALUE;
	m_hDevice = INVALID_HANDLE_VALUE;
	m_sDeviceName.clear();

	m_bThreadWork = false;
	m_CurbufferIdx = -1;

	memset(&m_Overlapped, 0, sizeof(OVERLAPPED));

	for(int i = 0; i < RECV_BUFFER_COUNT; i++)
	{
		m_stRecvBuffer[i].dataLen = 0;
		m_stRecvBuffer[i].bufLen = 0;
		m_stRecvBuffer[i].pData = NULL;
	}

	m_pFn = NULL;
}

SerialPort::~SerialPort()
{
	disconnect();

	if(m_pRecvThread != INVALID_HANDLE_VALUE)
	{
		m_bThreadWork = false;
		SetCommMask(m_hDevice, 0);
		WaitForSingleObject(m_pRecvThread, TIME_OUT_TIME);

		CloseHandle(m_pRecvThread);
		m_pRecvThread = INVALID_HANDLE_VALUE;
	}

	for(int i = 0; i < RECV_BUFFER_COUNT; i++)
	{
		if(m_stRecvBuffer[i].bufLen == 0)
		{
			delete [] m_stRecvBuffer[i].pData;
			m_stRecvBuffer[i].pData = NULL;
			m_stRecvBuffer[i].bufLen = 0;
		}
	}
}

unsigned int __stdcall RecevieThread(PVOID pParam)
{
	SerialPort *pThis = (SerialPort*)pParam;

	pThis->RecvFn();

	return 0;
}

std::string SerialPort::ErrorString(SERIALPORT_ERROR eErr)
{
	switch(eErr)
	{
    case DEVICE_CTRL_FAIL:
        return "can't use the device";
        default:
	case NONE_ERROR:
		return "connect success";

	case CONNECT_FAIL:
		return "connect fail";

	case SET_PARAM_FAIL:
		return "can't set wrong parameter";

	case RECV_THREAD_FAIL:
		return "create receive thread fail";
	}
}

int SerialPort::connect(COM_PORT_IDX ePort)
{
	return connect(DEVICE_NAME[ePort]);
}

SerialPort::SERIALPORT_ERROR SerialPort::connect(std::wstring sDevice, DWORD baudRate, BYTE stopBit, BYTE byteSize)
{
	SERIALPORT_ERROR eErrorNo = NONE_ERROR;

	DCB stDCB = {0};
	COMMTIMEOUTS timeouts = {MAXDWORD, 0, 0, 0, 0};

	m_sDeviceName = sDevice;

    m_hDevice = CreateFile(m_sDeviceName.c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	if(m_hDevice == INVALID_HANDLE_VALUE)
	{
		eErrorNo = CONNECT_FAIL;
		goto FINAL;

	}

	if(!GetCommState(m_hDevice, &stDCB))
	{
		eErrorNo = SET_PARAM_FAIL;
		goto FINAL;
	}

	stDCB.BaudRate	= baudRate;
	stDCB.Parity	= NOPARITY;
	stDCB.fParity	= NOPARITY;
	stDCB.StopBits	= stopBit;
	stDCB.ByteSize	= byteSize;

	stDCB.fDsrSensitivity = 0;
	stDCB.fDtrControl	  = DTR_CONTROL_ENABLE;
	stDCB.fOutxDsrFlow    = 0;


	if(!SetCommState(m_hDevice, &stDCB))
	{
		eErrorNo = SET_PARAM_FAIL;
		goto FINAL;
	}

	if(!SetCommTimeouts(m_hDevice, &timeouts))
	{
		eErrorNo = SET_PARAM_FAIL;
		goto FINAL;
	}

	clearPortBuffer();

	if(m_pRecvThread == INVALID_HANDLE_VALUE)
	{
		m_Overlapped.hEvent = m_hDevice;

		SetCommMask(m_hDevice, EV_TXEMPTY | EV_RXCHAR);

		m_bThreadWork = true;
		m_pRecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecevieThread, this, 0, NULL);

		if(m_pRecvThread == INVALID_HANDLE_VALUE || m_pRecvThread == NULL)
		{
			printf("**Error : create receive thread fail!\n");

			m_pRecvThread = INVALID_HANDLE_VALUE;
			eErrorNo = RECV_THREAD_FAIL;
		}
		else	// all success, create overlap event
		{
            m_Overlapped.hEvent = CreateEvent(NULL, false, false, L"GL_SerialPortEvent");

		}
	}


FINAL:

	if(errno != NONE_ERROR)
	{
		disconnect();
	}

	PrintDetailInfo("Open serial port (%s) %s!\n", m_sDeviceName.c_str(),
		ErrorString(eErrorNo).c_str());

	return eErrorNo;
}

void SerialPort::disconnect()
{
	if(m_pRecvThread != INVALID_HANDLE_VALUE)
	{
		m_bThreadWork = false;
		SetCommMask(m_hDevice, 0);
		SetEvent(m_Overlapped.hEvent);

		WaitForSingleObject(m_pRecvThread, TIME_OUT_TIME);

		CloseHandle(m_pRecvThread);
		m_pRecvThread = INVALID_HANDLE_VALUE;
	}

	if(m_hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(m_hDevice);

	m_hDevice = INVALID_HANDLE_VALUE;

	m_Overlapped.hEvent = INVALID_HANDLE_VALUE;


	for(int i = 0; i < RECV_BUFFER_COUNT; i++)
	{
		m_stRecvBuffer[i].dataLen = 0;		
	}

	PrintDetailInfo("Close serial port (%s) success!\n", m_sDeviceName.c_str());

}

int SerialPort::sendData(unsigned char *buffer, int len)
{
	unsigned long result;

	if(m_hDevice == INVALID_HANDLE_VALUE)
		return FALSE;

	if(g_bDetailInfo)
	{
		printf("Send com port : ");

		for(int i = 0 ; i < len ; i++)
		{
			printf(" %02x%s", buffer[i], i == len - 1? "" : "," );
		}
		printf("\n");
	}

	if(!WriteFile(m_hDevice, buffer, len, &result, &m_Overlapped) && GetLastError() != ERROR_IO_PENDING)
	{
		if(g_bDetailInfo)
			printf("***%s : Line %d \"%s\" fail with error code %d\n", "SerialPort::sendData", __LINE__, "WriteFile", GetLastError());
		else
			printf("***\"%s\" fail with error code %d\n", "WriteFile",  GetLastError());
	}

	return result;
}

// this function will new data, and caller should do delete itself
int SerialPort::receiveData(BYTE *buffer, int len)
{
	if(m_CurbufferIdx == -1)
		return 0;

	DATA_BUFFER	*recvBuffer = &m_stRecvBuffer[m_CurbufferIdx];

	if(recvBuffer->dataLen == 0)
		return 0;

	if(buffer != NULL && len != 0)
		delete [] buffer;

	if(buffer == NULL)
		buffer = new BYTE[recvBuffer->dataLen];

	memcpy_s(buffer, recvBuffer->bufLen, recvBuffer->pData, recvBuffer->dataLen);

	return recvBuffer->dataLen;
}

int SerialPort::getBytesToRead()
{
	COMSTAT comstat;
	DWORD  dwErr;

	if(m_hDevice == INVALID_HANDLE_VALUE)
		return 0;

	ClearCommError(m_hDevice, &dwErr, &comstat);

	return comstat.cbInQue;
}

void SerialPort::setCallback(ComportRecvCallback* object)
{
	m_pFn = object;
}

void SerialPort::RecvFn()
{
	DWORD dwEvtMask;
	unsigned long result = 0;
	int recvLen;
	BYTE tempIdx;

	while(m_bThreadWork)
	{
		if(WaitCommEvent(m_hDevice, &dwEvtMask, NULL) == 0)
		{
			printf("Error: WaitCommEvent fail with code %d \n", GetLastError());

			Sleep(1);
			continue;
		}

		// no data
		if(!(dwEvtMask & EV_RXCHAR) || (recvLen = getBytesToRead()) == 0)
		{
			continue;
		}

		tempIdx = m_CurbufferIdx+1;

		if(m_stRecvBuffer[tempIdx].bufLen > 0)
		{
			if(m_stRecvBuffer[tempIdx].bufLen < recvLen)
			{
				delete [] m_stRecvBuffer[tempIdx].pData;
				m_stRecvBuffer[tempIdx].bufLen = 0;
			}
			else
			{
				m_stRecvBuffer[tempIdx].dataLen = recvLen;
			}

		}

		if(m_stRecvBuffer[tempIdx].bufLen == 0)
		{
			m_stRecvBuffer[tempIdx].pData = new BYTE[recvLen];
			m_stRecvBuffer[tempIdx].dataLen = recvLen;
			m_stRecvBuffer[tempIdx].bufLen = recvLen;
		}

		if(!ReadFile(m_hDevice, m_stRecvBuffer[tempIdx].pData, recvLen, &result, &m_Overlapped))
		{
			if(g_bDetailInfo)
				printf("***%s : Line %d \"%s\" fail with error code %d\n", 
					"SerialPort::RecvFn", __LINE__, "ReadFile", GetLastError());
			else
				printf("***\"%s\" fail with error code %d\n", "ReadFile",  GetLastError());
		}

		if(g_bDetailInfo)
		{
			printf("Recv com port : ");

			for(int i = 0 ; i < recvLen ; i++)
			{
				printf(" %02x%s", m_stRecvBuffer[tempIdx].pData[i], i == recvLen - 1? "" : "," );
			}
			printf("\n");
		}

		if(m_pFn != NULL)
		{
			m_pFn->RecvCallBackFn(m_stRecvBuffer[tempIdx].pData, recvLen);
		}
	}

}

void SerialPort::clearPortBuffer()
{
	DWORD dwErr;
	COMSTAT comStat;
	if(m_hDevice == INVALID_HANDLE_VALUE)
		return;

	ClearCommError(m_hDevice, &dwErr, &comStat);
	PurgeComm(m_hDevice, PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT);
}

#else

#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl_defined.h>
#include <fcntl.h>
#include <errno.h>

static const std::string DEVICE_NAME[SerialPort::MAX_PORT] =
{ "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS4", "/dev/ttyS5"};

SerialPort::SerialPort()
{
	m_hDevice = INVALID_HANDLE_VALUE;
	m_pRecvThread = INVALID_HANDLE_VALUE;
	m_sDeviceName.clear();

	m_bThreadWork = false;
	m_CurbufferIdx = -1;

	for(int i = 0; i < RECV_BUFFER_COUNT; i++)
	{
		m_stRecvBuffer[i].dataLen = 0;
		m_stRecvBuffer[i].bufLen = 0;
		m_stRecvBuffer[i].pData = NULL;
	}

	m_pFn = NULL;
}

SerialPort::~SerialPort()
{
	disconnect();

	for(int i = 0; i < RECV_BUFFER_COUNT; i++)
	{
		if(m_stRecvBuffer[i].bufLen == 0)
		{
			delete [] m_stRecvBuffer[i].pData;
			m_stRecvBuffer[i].pData = NULL;
			m_stRecvBuffer[i].bufLen = 0;
		}
	}
}


void *RecevieThread(void *pParam)
{
	SerialPort *pThis = (SerialPort*)pParam;

	pThis->RecvFn();

	pthread_exit(0);
}

void SerialPort::RecvFn()
{
	int recvLen;
	BYTE tempIdx;

	int retval;
	fd_set fdset;
	timeval tv;
	int nMaxFd = m_hDevice + 1;
	int nWatchDog = 50, nCount = 0;

	tv.tv_sec = 1;
	tv.tv_usec = 0;

	FD_ZERO(&fdset);
	FD_SET(m_hDevice, &fdset);

	while(m_bThreadWork && nCount < nWatchDog)
	{
		recvLen = getBytesToRead();
		if(recvLen <= 0)	// if no data, then wait to get data
		{
			retval = select( nMaxFd, &fdset, NULL, NULL, &tv);
	
			if(retval < 0)
			{
		            Error("void SerialPort::RecvFn()", "select", retval);
        		    return;
			}
        		else if(retval == 0)
			{
				// time out because no data
				//Sleep(1);printf(".");
				//nCount++;
				continue;
			}
			else
        		{
			    nCount = 0;
			}	

			recvLen = getBytesToRead();
		}
		
		tempIdx = m_CurbufferIdx+1;

		if(m_stRecvBuffer[tempIdx].bufLen > 0)
		{
			if(m_stRecvBuffer[tempIdx].bufLen < recvLen)
			{

				delete [] m_stRecvBuffer[tempIdx].pData;
				m_stRecvBuffer[tempIdx].bufLen = 0;
			}
			else
			{
				m_stRecvBuffer[tempIdx].dataLen = recvLen;
			}
		}

		if(m_stRecvBuffer[tempIdx].bufLen == 0)
		{
			m_stRecvBuffer[tempIdx].pData = new BYTE[recvLen];
			m_stRecvBuffer[tempIdx].dataLen = recvLen;
			m_stRecvBuffer[tempIdx].bufLen = recvLen;
		}

		if(read(m_hDevice, m_stRecvBuffer[tempIdx].pData, recvLen) == -1)
		{
			if(g_bDetailInfo)
				printf("***%s : Line %d \"%s\" fail with error code %d\n", 
					"SerialPort::RecvFn", __LINE__, "read", errno);
			else
				printf("***\"%s\" fail with error code %d\n", "read",  errno);
		}

		if(g_bDetailInfo)
		{
            		printf("recv data : ");

			for(int i = 0 ; i < recvLen ; i++)
			{
				printf(" %02x%s", m_stRecvBuffer[tempIdx].pData[i], i == recvLen - 1? "" : "," );
			}
			printf("\n");
		}

		if(m_pFn != NULL)
		{
			m_pFn->RecvCallBackFn(m_stRecvBuffer[tempIdx].pData, recvLen);
		}
		recvLen = 0;
	}
}

int SerialPort::connect(COM_PORT_IDX ePort)
{
	return connect(DEVICE_NAME[ePort]);
}

int SerialPort::connect(std::string sDevice, DWORD baudRate, BYTE stopBit, BYTE byteSize)
{
	SERIALPORT_ERROR eErrorNo = NONE_ERROR;

	int nRet = 0;
	speed_t	rate;
	termios options;

	m_sDeviceName = sDevice;

	// O_RDWR - read and write access
	// O_NOCTTY - prevent other input(like keyboard) from affecting what we read
	// O_NDELAY - don't care if the other side is connected( same devices don't explicitly connect)
    	m_hDevice = open( m_sDeviceName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

	if(m_hDevice == INVALID_HANDLE_VALUE)
	{
		Error("connect", "open", m_hDevice);
		eErrorNo = CONNECT_FAIL;
		goto FINAL;
	}

	// get the current setting of the serial port ---------------------------------------------
	//tcgetattr(m_hDevice, &options);
	memset(&options, 0, sizeof(termios));

	// set the read/write speed ---------------------------------------------------------------
	// all speeds can be prefixed with B as a setting

	switch(baudRate)
	{
		case 2400:
			break;

			rate = B2400;
		case 4800:
			rate = B4800;
			break;

		case 19200:
			rate = B19200;
			break;

		case 38400:
			rate = B38400;
			break;

		case 57600:
			rate = B57600;
			break;

		case 115200:
			rate = B115200;
			break;

		case 230400:
			rate = B230400;
			break;

		default:
			rate = B9600;
			break;
	}

	cfsetispeed(&options, rate);
	cfsetospeed(&options, rate);


	// now default set no parity ---------------------------------------------------------------
	options.c_cflag &= ~PARENB;	// PARENB is enable parity
	options.c_cflag &= ~CSTOPB;	// CSTOPB is 2 stop bits
	options.c_cflag &= ~CSIZE;	// clear out the current word size, before set CS8
	options.c_cflag |= CS8;		// CS8 is 8-bits per work

	// Set timeout ------------------------------------------------------------------------------
	// VMIN is minimum amount of characters to read
	options.c_cc[VMIN] = 0;

	// the amount of time to wait for the amount of data specified by VMIN in tenths of a second
	options.c_cc[VTIME] = 1;

	// CLOCAL is don't allow control of the part to be changed
	// CREAD says to enable the receiver
	options.c_cflag |= (CLOCAL | CREAD);


	// apply the settings ----------------------------------------------------------------------
	if(tcsetattr(m_hDevice, TCSANOW, &options) != 0)
	{
		Error("connect", "tcsetattr", -1);
		eErrorNo = DEVICE_CTRL_FAIL;
		goto FINAL;
	}

	clearPortBuffer();

    	if(m_pRecvThread == INVALID_HANDLE_VALUE)
	{
		m_bThreadWork = true;
		nRet = pthread_create(&m_pRecvThread, NULL, &RecevieThread, this);

		if(nRet != 0)
		{
			printf("**Error : create receive thread fail!\n");

			m_pRecvThread = 0;
			disconnect();
			eErrorNo = RECV_THREAD_FAIL;
		}
	}

FINAL:

	if(errno != NONE_ERROR)
	{
		disconnect();
	}

    	PrintDetailInfo("Open serial port (%s) %s!\n", m_sDeviceName.c_str(),
        ErrorString(eErrorNo).c_str());

	return eErrorNo;
}

void SerialPort::disconnect()
{
	if(m_hDevice == INVALID_HANDLE_VALUE)
		return;

    	if(m_pRecvThread != INVALID_HANDLE_VALUE)
	{
		m_bThreadWork = false;

		pthread_join(m_pRecvThread, NULL);

		m_pRecvThread = INVALID_HANDLE_VALUE;
	}

	if(close(m_hDevice) == -1)
	{
		Error("disconnect", "close", -1);
	}
	else
		m_hDevice = INVALID_HANDLE_VALUE;

	for(int i = 0; i < RECV_BUFFER_COUNT; i++)
	{
		m_stRecvBuffer[i].dataLen = 0;	
	}


	PrintDetailInfo("\nClose serial port (%s) success!\n", m_sDeviceName.c_str());
}

std::string SerialPort::ErrorString(SERIALPORT_ERROR eErr)
{
	switch(eErr)
	{
    	case DEVICE_CTRL_FAIL:
        	return "can't use the device";
	default:
	case NONE_ERROR:
		return "connect success";

	case CONNECT_FAIL:
		return "connect fail";

	case SET_PARAM_FAIL:
		return "can't set wrong parameter";

	case RECV_THREAD_FAIL:
		return "create receive thread fail";
	}
}

int SerialPort::sendData(unsigned char *buffer, int len)
{
	if(m_hDevice == INVALID_HANDLE_VALUE)
		return 0;

    if(g_bDetailInfo)
    {
        printf("\nSend data : ");
        for(int i = 0 ; i < len ; i++)
        {
            printf(" %02x%s", buffer[i], i == len - 1? "" : "," );
        }
        printf("\n\n");
    }

	return write(m_hDevice, buffer, len);
}

int SerialPort::receiveData(unsigned char *buffer, int len)
{
	if(m_CurbufferIdx == -1)
		return 0;

	DATA_BUFFER	*recvBuffer = &m_stRecvBuffer[m_CurbufferIdx];

	if(recvBuffer->dataLen == 0)
		return 0;

	if(buffer != NULL && len != 0)
		delete [] buffer;

	if(buffer == NULL)
		buffer = new BYTE[recvBuffer->dataLen];

	memcpy(buffer, recvBuffer->pData, recvBuffer->dataLen);

	return recvBuffer->dataLen;
}

int SerialPort::getBytesToRead()
{
	int bytes = 0;

	ioctl(m_hDevice, FIONREAD, &bytes);

	return bytes;
}

void SerialPort::clearPortBuffer()
{
	if(m_hDevice == INVALID_HANDLE_VALUE)
		return;

	// flushes data without read/write
	tcflush(m_hDevice, TCOFLUSH);
	tcflush(m_hDevice, TCIFLUSH);
}


#endif
