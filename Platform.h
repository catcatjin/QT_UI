#pragma once

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>
#include <stdlib.h>
#include "FpgaPicAPI.h"
#include "FunctionList.h"

#if (defined _WIN32 || defined _WIN64 || defined WINNT)
// ¤U¦C¥¨¶°·|©w¸q³Ì§C¥­¥x»Ý¨D¡C³Ì§C¥­¥x»Ý¨D¬O«ü¦UºØª©¥»ªº
// Windows¡BInternet Explorer ¤¤¡A¨ã³Æ°õ¦æÀ³¥Îµ{¦¡©Ò»Ý¥\¯àªº
// ³Ì¦­ª©¥»¡C¥¨¶°ªº§@¥Î¡A¬O¦b«ü©w©Î§ó·sª©¥»ªº¥­¥x¤W±Ò¥Î
// ©Ò¦³¥i¥Îªº¥\¯à¡C

// ¦pªG±z¦³¥²¶·Àu¥ý¿ï¨úªº¥­¥x¡A½Ð­×§ï¤U¦C©w¸q¡C
// °Ñ¦Ò MSDN ¨ú±o¤£¦P¥­¥x¹ïÀ³­Èªº³Ì·s¸ê°T¡C
#ifndef _WIN32_WINNT            // «ü©w³Ì§C¥­¥x»Ý¨D¬° Windows Vista¡C
#define _WIN32_WINNT 0x0600     // ±N¥¦ÅÜ§ó¬°°w¹ï Windows ¨ä¥Lª©¥»ªº¾A·í­È¡C
#endif

#define WINDOWS

#include <windows.h>
#include <time.h>

#define SystemPause() system("pause")
#define ClearScreen() system("cls")

#else

#ifndef LINUX
#define LINUX
#endif

#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#ifndef BOOL

typedef int BOOL;
#define TRUE    1
#define FALSE   0

#endif // BOOL

#define SystemPause() {printf("Press Enter key to continue...");  fgetc(stdin); printf("\n");}
#define ClearScreen() system("clear")
#define Sleep(t)	usleep(t*1000)

#endif

#ifndef UINT
#define UINT	unsigned int
#endif


#define TEST_NAME				"Gaming function testing v0.0.1"
#define SUPPORT_SRAM_VERSION	0x010300

enum MODEL_TYPE
{
	UNKNOWN_MODEL = -1,
    PGB_5120S = 0,
    PGB_5021,
    PGB_4020,	// no maintain
    PGB_6040,	// no maintain
    PGB_5320,
    PGB_5322,
    GCB_203,
	PGB_5321R1,
	RESERVE,
	MAX_MODEL
};

static const string sModelName[MAX_MODEL] = {	"Unknown",
												"PGB 5120S",
												"PGB 5021",
												"PGB 4020",
												"PGB 6040",
												"PGB 5320",
												"PGB 5322",
												"GCB 203",
												"PGB_5321R1"};
#endif
