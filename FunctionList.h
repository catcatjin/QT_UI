#pragma once

#ifndef FUNCTION_LIST_H
#define FUNCTION_LIST_H

#include "string"
using namespace::std;

extern bool g_bDetailInfo;

#define PrintDetailInfo(str, ...) \
	if(g_bDetailInfo) \
		printf(str, ##__VA_ARGS__)

#define Error(func, api_name, FPGA_API_STATUS)	\
	if(g_bDetailInfo) \
		printf("***%s : Line %d \"%s\" fail with error code 0x%x\n", \
							func, __LINE__, api_name, FPGA_API_STATUS); \
	else \
		printf("***\"%s\" fail with error code 0x%x\n", \
					api_name, FPGA_API_STATUS)


enum TEST_FLOW
{
	NONE = -1,
	ALL_FUNCTION,
	SINGLE_FUNCTION,
	MULITI_FUNCTION,
	MAX_FLOW_SELECTION_NUM
};

enum TEST_FUNCTION
{
	TIMER_16BIT_TEST,
	RANDOM_GEN_TEST,
	CCTALK_TEST,
	SAS_TEST,
	SRAM_TEST,
	EEPROM_TEST,
	GPO_TEST,
	GPI_TEST,
	RTC_INSTURSION_TEST,
	SPI_TEST,
	MAX_TEST_FUNCTION_NUM
};

enum NGPIO
{
	PIC,
	PLD,	// power low detect
	OVT,	// over temperature alarm
	SAS,
	TIMER_0,
	TIMER_1,
	TIMER_2,
	TIMER_3,
	GPO_PWM,
	RANDOM,
	AES,
	GPI_DEBOUNCE,
};

enum IRQ_TYPE
{
	GPI_IRQ_TYPE,
	CONFIG_IO_AS_INTPUT_IRQ_TYPE,
	PIC_IRQ_TYPE,
	OVT_IRQ_TYPE	= 4,
	SAS_IRQ_TYPE,
	TIMER_IRQ_TYPE,
	MAX_IRQ_TYPE
};

// user for INT_MSG.IntReg[12]
enum IRQ_REG_IDX
{
	IRQ_GPI,
	IRQ_SAS,
	IRQ_TIMER,
	IRQ_CONFIG_IO	=	8,
	IRQ_OTHER,
	MAX_IRQ			=	12
};

enum INTRUSION_LOG_INDEX
{
	DOOR_0,
	DOOR_1,
	DOOR_2,
	DOOR_3,
	DOOR_4,
	DOOR_5,
	DOOR_6,
	DOOR_7,
	POWER,
	BATTERY_1,
	BATTERY_2,
	BATTERY_3
};

enum DATETIME_INDEX
{
	YEAR_IDX,
	MONTH_IDX,
	DATE_IDX,
	HOUR_IDX,
	MINUTE_IDX,
	SECOND_IDX
};

enum SAS_IRQ_STATUS
{
	SAS_GET_CMD				=	0x01,
	SAS_GET_ACK				=	0x02,
	SAS_SYNC_FINISH			=	0x05,
	SAS_GET_BROCAST_CMD		=	0x11,
	SAS_LOOP_BREAK			=	0x20,
	SAS_HANDSHARKE_FAIL		=	0xE0,
	SAS_GET_CMD_TIMEOUT		=	0xE1,
	SAS_REPLY_DATA_TIMEOUT	=	0xF0
};

enum FPGA_API_STATUS
{
    FD_SUCCESS				= 0,
    FD_DEVICE_NOT_OPEN,
    FD_DEVICE_OPENED,
    FD_INVALID_PARAMETER,
    FD_READ_FAIL,
    FD_WRITE_FAIL,
    FD_PARAM_OUT_OF_RANGE,
    FD_INT_ENABLE_FAIL,
    FD_INT_IS_ENABLED,
    FD_INT_DISABLE_FAIL,
    FD_INT_IS_DISABLED,
    FD_INT_NO_HANDLER,
    FD_ALLOCATE_MEM_FAIL,
    FD_UNKNOW_CMD,
    FD_SMBUS_OPEN_FAIL,   //SM Bus device open failure
    FD_FPGA_WRONG_FW,
    FD_PIC_WRONG_FW,
    FD_PIC_COMMUTE_FAIL,  //handshaking failure with PIC
    FD_PIC_WRONG_DATA,
    FD_EEPROM_COMMUTE_FAIL,//handshaking failure with EEPROM
    FD_NO_USABLE_DATA,  //No error command data-Mirror write

    PCI_DRIVER_OPEN_FAIL	= 0x1800,
    PCI_RESOURCE_FAIL,
    PCI_DRIVER_CLOSE_FAIL,

    AES_KEY_LENGTH_FAIL,
    AES_WRONG_DATA,
    AES_ENCRYPT_FAIL,
    AES_DECRYPT_FAIL,
};


#define MAX_BIN_LEN MAX_TEST_FUNCTION_NUM + 1	// 1111111111b
#define MAX_HEX_LEN 5							// 0x3FF
#define MAX_DEC_LEN 4							// 1023
#define MAX_FUNCTION_VALUE ((1<<MAX_TEST_FUNCTION_NUM) - 1)

static const string sTestFlowSelectionName[MAX_FLOW_SELECTION_NUM] = {	"All Function Test",
																		"Single Function Test",
																		"Multiple Function Test"};


static const string sTestFunctionName[MAX_TEST_FUNCTION_NUM] = {	"16-bit timer",
																	"Random number generator",
																	"ccTalk",
																	"SAS",
																	"Memory - SRAM",
																	"Memory - EEPROM",
																	"GPO",
																	"GPI",
																	"PIC - RTC & Intrusions",
																	"SPI-FRAME(FM25CL64B)"};

#endif
