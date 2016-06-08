#ifndef SRAMTEST1_H
#define SRAMTEST1_H
#include "TestClass.h"
#include "sram_test.h"
#include <map>

typedef int(*MEM32_FUNC_CALLBACK)(DWORD*, DWORD, DWORD);
typedef int(*MEM8_FUNC_CALLBACK)(BYTE*, DWORD, DWORD);

enum SRAM_MODE
{
    NORMAL_MODE = 0,
    MIRROR_MODE
};

class SRAMTest1 : public TestClass, public SRAM_test
{
private:

    bool	m_bMirrorTestResult;
    bool	m_bNormalTestResult;
    DWORD	m_dwMemSize;

    SRAM_MODE	m_eSramMode;

    MEM32_FUNC_CALLBACK	m_pWriteMem32Func;
    MEM32_FUNC_CALLBACK	m_pReadMem32Func;
    MEM8_FUNC_CALLBACK	m_pWriteMem8Func;
    MEM8_FUNC_CALLBACK	m_pReadMem8Func;

    DWORD getMaxOffsetSRAMAddr(int OffsetBlock);
    bool memoryTest();

    bool FunctionalTest();
    bool IntegrityTest();
    bool RandomgAccessTest();

    bool IntegrityDataVerify();
    bool RandomgAccessVerify(std::map<DWORD, BYTE> &rand_map);

    static int m_ReadMirror32(DWORD*, DWORD, DWORD);
    static int m_ReadMirror8(BYTE*, DWORD, DWORD);

public slots:
    void doTest() override;

public:
    SRAMTest1();
    ~SRAMTest1();

    virtual void diagnostic();
    virtual void showResult();
};

#endif // SRAMTEST1_H
