#ifndef _FPGAAPI_H_
#define _FPGAAPI_H_

#ifdef    __cplusplus
extern "C" {
#endif    /*    __cplusplus    */

typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;
#if defined(WINNT)
typedef int BOOL;
#endif

#if defined(WINNT)

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif
#ifndef STD_CALL
#define STD_CALL __stdcall
#endif

#else

#define DLLEXPORT
#define STD_CALL 

#endif

typedef struct FPGA_Resource{	
	DWORD bar0BaseAddr;
	DWORD bar1BaseAddr;
	DWORD SRAMBaseAddr;
	DWORD IOBaseAddr;
}FPGARes;

typedef struct {    
    BYTE int_type;
	DWORD IntReg[12];
}INT_MSG;

typedef struct {    
	DWORD GPI[8];
	DWORD GPIO_I[4];
	BYTE NIO;
}INT_ENABLE_ITEM;

typedef struct {
	DWORD version;
	BYTE type;
	BYTE hw_id;
}FPGA_INFO;

enum GPO_ID{
	GPO_0 = 0, GPO_1, GPO_2, GPO_3, GPO_4, GPO_5, GPO_6, GPO_7, GPO_8, GPO_9,
	GPO_10, GPO_11, GPO_12, GPO_13, GPO_14, GPO_15, GPO_16, GPO_17, GPO_18, GPO_19,
	GPO_20, GPO_21, GPO_22, GPO_23, GPO_24
};


enum GPI_ID{
	GPI_0 = 0, GPI_1, GPI_2, GPI_3, GPI_4, GPI_5, GPI_6, GPI_7, GPI_8, GPI_9,
	GPI_10, GPI_11, GPI_12, GPI_13, GPI_14, GPI_15, GPI_16, GPI_17, GPI_18, GPI_19,
	GPI_20, GPI_21, GPI_22, GPI_23, GPI_24, GPI_25, GPI_26, GPI_27, GPI_28, GPI_29,
	GPI_30, GPI_31, GPI_32, GPI_33, GPI_34, GPI_35, GPI_36, GPI_37
};

int DLLEXPORT FpgaPic_Init();

int DLLEXPORT FpgaPic_Close();

int DLLEXPORT FD_GetAPIVersion(char *ver);//FD_Version(char *ver);

int DLLEXPORT FD_GetDriverVersion(char *ver);//FD_KernelVersion(char *ver);

int DLLEXPORT FD_GetVersion(WORD *ver);

int DLLEXPORT FD_GetFPGAVer(FPGA_INFO *info);

int DLLEXPORT FD_GetHWModel(DWORD *info);

int DLLEXPORT FD_GetGPIAvail(BYTE set_num, DWORD *avail);

int DLLEXPORT FD_GetGPOAvail(BYTE set_num, DWORD *avail);

int DLLEXPORT FD_GetGPIOAvail(BYTE set_num, DWORD *avail);

int DLLEXPORT FD_GetNGPIOAvail(DWORD *avail);

int DLLEXPORT FD_GetNGPIOIntEnable(BYTE *enable);

int DLLEXPORT FD_GetNGPIOStatus(BYTE *status);

int DLLEXPORT FD_GetIntEnable(BYTE set_num, DWORD *value);

int DLLEXPORT FD_GetI2CAdds(BYTE num, BYTE *addr);

int DLLEXPORT FD_SetI2CAdds(BYTE num, BYTE addr);

int DLLEXPORT FD_SetGPOAll(BYTE set_num, DWORD value); // bit0 to bit24 = GPO0 to GPO24

int DLLEXPORT FD_SetGPOByIndex(BYTE set_num, BYTE index, BYTE value); // Set GPO by index

int DLLEXPORT FD_SetCGPOByIndex(BYTE set_num, BYTE index, BYTE value); // Set GPO by index

int DLLEXPORT FD_SetGPIOCfgByIndex(BYTE set_num, BYTE index, BYTE mode);

int DLLEXPORT FD_SetCGPOAll(BYTE set_num, DWORD value);

int DLLEXPORT FD_GetGPOAll(BYTE set_num, DWORD *value);

int DLLEXPORT FD_GetCGPOAll(BYTE set_num, DWORD *value);

int DLLEXPORT FD_GetCGPIByIndex(BYTE set_num, BYTE index, BYTE *value);

int DLLEXPORT FD_GetGPIAll(BYTE set_num, DWORD *value); // value1 bit0 to bit31 = GPI0 to GPI31 ; value2 bit0 to bit5 = GPI32 to GPI37

int DLLEXPORT FD_GetCGPIAll(BYTE set_num, DWORD *value);

int DLLEXPORT FD_GetDipSwAll(BYTE set_num, DWORD *value);

int DLLEXPORT FD_GetGPIByIndex(BYTE set_num, BYTE index, BYTE *value);

int DLLEXPORT FD_GetGPIByIndex(BYTE set_num, BYTE index, BYTE *value);

int DLLEXPORT FD_GetDipSwByIndex(BYTE set_num, BYTE index, BYTE *value);

int DLLEXPORT FD_GetGPIOConfig(BYTE set_num, DWORD *value);

int DLLEXPORT FD_SetGPIOConfig(BYTE set_num, DWORD value);

int DLLEXPORT FD_IntEnable(void (STD_CALL *int_handler)(void*), INT_ENABLE_ITEM items);

int DLLEXPORT FD_IntEnableClassType(void (STD_CALL *int_handler)(void*, void*), void *pData, INT_ENABLE_ITEM items);

int DLLEXPORT FD_IntDisable();

int DLLEXPORT FD_WriteSram8(BYTE *wbuf, DWORD offset, DWORD len); //Write 'len' bytes data(sbuf) to FRAM start from address 'offset'

int DLLEXPORT FD_ReadSram8(BYTE *rbuf, DWORD offset, DWORD len); //read 'len' bytes data(sbuf) from FRAM start from address 'offset'

int DLLEXPORT FD_WriteSram16(WORD *wbuf, DWORD offset, DWORD len); //Write 'len' words(2 bytes) data(sbuf) to FRAM start from address 'offset'

int DLLEXPORT FD_ReadSram16(WORD *rbuf, DWORD offset, DWORD len); //read 'len' words(2 bytes) data(sbuf) from FRAM start from address 'offset'

int DLLEXPORT FD_WriteSram32(DWORD *wbuf, DWORD offset, DWORD len); //Write 'len' dwords(4 bytes) data(sbuf) to FRAM start from address 'offset'

int DLLEXPORT FD_ReadSram32(DWORD *rbuf, DWORD offset, DWORD len); //read 'len' dwords(4 bytes) data(sbuf) from FRAM start from address 'offset'

int DLLEXPORT FD_TestSramBlock(BYTE *wbuf, DWORD offset, DWORD len);

//Mirror functions
int DLLEXPORT FD_GetSramAvail(DWORD *size);

int DLLEXPORT FD_GetSramMode(BYTE *mode);

int DLLEXPORT FD_SetSramMode(BYTE mode, BYTE datafill);

int DLLEXPORT FD_ReadMirror8(BYTE blk_num, BYTE *rbuf, DWORD offset, DWORD len);

int DLLEXPORT FD_ReadMirror16(BYTE blk_num, WORD *rbuf, DWORD offset, DWORD len);

int DLLEXPORT FD_ReadMirror32(BYTE blk_num, DWORD *rbuf, DWORD offset, DWORD len);

int DLLEXPORT FD_WriteMirror8(BYTE *wbuf, DWORD offset, DWORD len);

int DLLEXPORT FD_WriteMirror16(WORD *wbuf, DWORD offset, DWORD len);

int DLLEXPORT FD_WriteMirror32(DWORD *wbuf, DWORD offset, DWORD len);

int DLLEXPORT FD_CompareMirror8(BYTE *rbuf, DWORD offset, DWORD len);

int DLLEXPORT FD_CompareMirror16(WORD *rbuf, DWORD offset, DWORD len);

int DLLEXPORT FD_CompareMirror32(DWORD *rbuf, DWORD offset, DWORD len);

int DLLEXPORT FD_GetMirrorLastCmd(char *cmd_str, DWORD *offset, DWORD *len);

int DLLEXPORT FD_GetMirrorLastData8(BYTE *rbuf, DWORD len);

int DLLEXPORT FD_GetMirrorLastData16(WORD *rbuf, DWORD len);

int DLLEXPORT FD_GetMirrorLastData32(DWORD *rbuf, DWORD len);

int DLLEXPORT FD_EraseSram();

//GPIO functions
int DLLEXPORT ReadFPGA32(DWORD offset, DWORD *value);

int DLLEXPORT WriteFPGA32(DWORD offset, DWORD data);

int DLLEXPORT ReadBAR132(DWORD offset, DWORD *value);

int DLLEXPORT WriteBAR132(DWORD offset, DWORD data);

int DLLEXPORT FD_GetSRAMSegment(BYTE *seg);

int DLLEXPORT FD_GetIOSegment(BYTE *seg);

int DLLEXPORT FD_GetIOOffset(BYTE *seg);

int DLLEXPORT FD_SetSRAMSegment(BYTE seg);

int DLLEXPORT FD_SetIOSegment(BYTE seg);

int DLLEXPORT FD_SetIOOffset(BYTE seg);

int DLLEXPORT FD_GetGPIIntTriByIndex(BYTE set_num, BYTE index, BYTE *mode);

int DLLEXPORT FD_SetGPIIntTriAll(BYTE set_num, BYTE mode);

int DLLEXPORT FD_SetGPIIntTriByIndex(BYTE set_num, BYTE index, BYTE mode);

int DLLEXPORT FD_GetCGPIIntTriByIndex(BYTE set_num, BYTE index, BYTE *mode);

int DLLEXPORT FD_SetCGPIIntTriAll(BYTE set_num, BYTE mode);

int DLLEXPORT FD_SetCGPIIntTriByIndex(BYTE set_num, BYTE index, BYTE mode);
//SAS
int DLLEXPORT FD_GetSASAddress(BYTE *addr);

int DLLEXPORT FD_SetSASAddress(BYTE addr);

int DLLEXPORT FD_SetSASResp(BYTE *resp, DWORD len);

int DLLEXPORT FD_GetSASCmd(BYTE *cmdbuf, DWORD *length);

int DLLEXPORT FD_CalcSASCRC(BYTE *data, int length, WORD seed, WORD *result);

//GPO PWM function
int DLLEXPORT FD_GetGPOConfig(DWORD *config);

int DLLEXPORT FD_SetGPOCfgByIndex(BYTE index, BYTE type, DWORD freq, WORD level);

int DLLEXPORT FD_GetPWMParam(BYTE index, DWORD *freq, WORD *level);

//16-bit Timer
int DLLEXPORT FD_SetTMRInterval(BYTE num, WORD time);

int DLLEXPORT FD_GetTMRCount(BYTE num, DWORD *time);

//Get random number
int DLLEXPORT FD_GetRandom(DWORD *random);

//API for AES functions
int DLLEXPORT FD_AES_DataEncrypt(BYTE *key, BYTE *plaintext, BYTE *ciphertext);

int DLLEXPORT FD_AES_DataDecrypt(BYTE *key, BYTE *ciphertext, BYTE *plaintext);

//Add for GPI debounce time
int DLLEXPORT FD_GetGPIDBByIndex(BYTE index, DWORD *dbtime);

int DLLEXPORT FD_SetGPIDBByIndex(BYTE index, DWORD dbtime);

//Add for GPIO-GPI debounce time
int DLLEXPORT FD_GetCGPIDBByIndex(BYTE index, DWORD *dbtime);

int DLLEXPORT FD_SetCGPIDBByIndex(BYTE index, DWORD dbtime);

//Add SPI fucntion here
int DLLEXPORT FD_SPI_SetClock(DWORD freq);

int DLLEXPORT FD_SPI_ReadClock(DWORD *freq);

int DLLEXPORT FD_SPI_BlockWrite(BYTE mode, BYTE *sbuf, BYTE slen);

int DLLEXPORT FD_SPI_BlockRead(BYTE mode, BYTE *sbuf, BYTE slen, DWORD gap, BYTE *rbuf, BYTE rlen);

int DLLEXPORT FD_PIC_KernelVersion(char *ver);

int DLLEXPORT PIC_GetFWInfo(BYTE *buf);

int DLLEXPORT PIC_ReadEEPROM(BYTE offset, BYTE length, BYTE *data);

//int DLLEXPORT PIC_WriteEEPROM(BYTE offset, BYTE length, BYTE *data);

int DLLEXPORT PIC_ReadRTC(DWORD *datetime);

int DLLEXPORT PIC_WriteRTC(DWORD *datetime);

int DLLEXPORT PIC_GetCurrentIndex(BYTE e_type, BYTE *index);

int DLLEXPORT PIC_GetEventByIndex(BYTE e_type, BYTE index, DWORD *buf);

int DLLEXPORT PIC_EnableInt(BYTE door_stauts);

int DLLEXPORT PIC_CleanAllEvent();

int DLLEXPORT PIC_GetDoorStatus(DWORD *status);

int DLLEXPORT PIC_GetVersion(BYTE *ver);

int DLLEXPORT FD_ReadEEPROM8(BYTE offset, WORD length, BYTE *data);

int DLLEXPORT FD_WriteEEPROM8(BYTE offset, WORD length, BYTE *data);

int DLLEXPORT FD_ReadEEPROM16(WORD offset, WORD length, BYTE *data);

int DLLEXPORT FD_WriteEEPROM16(WORD offset, WORD length, BYTE *data);

int DLLEXPORT FD_Test_EEPROM8_Block(BYTE offset, WORD length, BYTE *data);

int DLLEXPORT FD_Test_EEPROM16_Block(WORD offset, WORD length, BYTE *data);

int DLLEXPORT PIC_GetDoorDBByIndex(BYTE index, DWORD *dbtime);

int DLLEXPORT PIC_SetDoorDBByIndex(BYTE index, DWORD dbtime);

int DLLEXPORT PIC_GetSN(BYTE *sn);


#ifdef __cplusplus
}
#endif    /*    __cplusplus    */

#endif
