#pragma once

#ifndef TEST_DEFAULT_PARAM_H
#define TEST_DEFAULT_PARAM_H

extern bool g_bDetailInfo;
extern bool g_bStepPause;
extern MODEL_TYPE eModel;

// 16-bit Timer param
#define SLEEP_MS_TIME			50
#define TOLERANCE_FPGA_COUNT	3
#define TOLERANCE_SYSTEM_TIME	5
#define SLEEP_EXTRA_WAIT_TIMES	2

// random generator param
#define TOLERANCE_RATE			0.002	// testing threshold
#define RAND_TIMES				50000	// random number count for testing

// ccTalk Param
#define COMMAND_INTERVAL		10		// 10ms
#define COMPORT_POLLTIME		100		// 100ms
#define COMPORT_POLL_MAX_CNT	8		// 8

// SAS Param
#define GENERAL_POLL_BIT		0x80
#define DEFAULT_SAS_ADDRSS		1
#define SAS_RESPONSE_DATA_CNT	10
#define MAX_SAS_WAIT_COUNT		100
#define TIME_OUT_CNT			20

// SRAM param
#define RESERVE_BLOCK			2048	// 2K for SRAM normal mode reserve block
#define BLOCK_UNIT_32KB			32768	// 32 * 1024, functional test
#define BLOCK_UNIT_1KB			1024	// integrity test and param
#define QUARTER_OF_1KB			256		// 1024 / 4, should always be the quarter of previous defined param
#define DIV_OF_RAND_ACCESS		256		// count of random value is m_dwMemSize / DIV_OF_RAND_ACCESS for random access test


// EEPROM param
#define EEPROM_BUFFER_SIZE			256	// API max buffer size
#define FUNCTION_TEST_BLOCK_SIZE	128
#define TEST_BLOCK_COUNT			2	//

// GPO param
#define MAX_GPO_LENGTH			32
#define	TURN_ON_OFF_TIMES		4		// 2 times for turn-on 200ms and turn-off 200ms
#define DURATION_INTERVAL		200
#define PWM_FREQ				1000	// freq range is 100~200K Hz
#define PWM_INCREASE_LEVEL		5
#define PWM_MAX_TEST_LEVEL		140		// real LEVEL range is 0~255
#define PWM_DURATION			2000	// ms for total time of PWM Test
#define PWM_TEST_REPEAT			2		// repeat times

// GPI param
#define MAX_GPI_LENGTH			32
#define TRIGGER_DETECT_INTERVAL	200
#define GPI_COUNT_IN_EACH_ROW	8
#define MAX_WAIT_TIME			10000	// 10s = 10000ms

// PIC param
#define INTRUSION_COUNT_IN_EACH_ROW	GPI_COUNT_IN_EACH_ROW
#define MAX_SN_LEN				6
#define MAX_PIC_INTRUSIONS		8
#define DATE_TIME_LEN			6
#define RTC_THRESHOLD			3
#define SPECIAL_MODEL_DOOR_DB	4	// 40ms
#define EVENT_BUF_LEN			8

// SPI
#define MAX_SPI_FREQ			15000000	// 30MHz is max, but FRAME(FM25CL64B) only support 20MHz
#define MAX_SPI_DATA_LEN		16 // byte
#define MAX_SPI_MODE			3

#endif
