#ifndef EEPROMTEST_H
#define EEPROMTEST_H

#include "TestClass.h"
#include <map>

class EEPROMTest : public TestClass
{
private:

        MODEL_TYPE m_eModelType;

        TEST_RESULT m_eResult;
        int  m_nMemSize;

        void getEEPROMSize();

        void FunctionalTest();
        void RandomgAccessTest();
        void RandomgAccessVerify(std::map<WORD, BYTE> &rand_map);

public:

        EEPROMTest(MODEL_TYPE modelType);
        ~EEPROMTest();

        virtual void diagnostic();
        virtual void showResult();
};

#endif // EEPROMTEST_H
