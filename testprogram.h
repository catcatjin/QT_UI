#ifndef TESTPROGRAM_H
#define TESTPROGRAM_H
#include "Platform.h"
#include <QString>
#include <QObject>

typedef enum
{
    TEST,
    BACK,
    STOP
} SELECT_ACTION;

class TestProgram : public QObject
{
    Q_OBJECT
public:
    TestProgram();
    ~TestProgram();
    void GetDeviceVerson();
    int TestManager(int FunctionSelect);

    void workFunctionDiagnostic();

    void ConnectSlot(QObject *pObj, const char * pSlot);
    MODEL_TYPE eModel;

    void SixteenBitTimerDia();
    void RandomGeneratorDia();
    void CCTalkDia();
    void SASDia();
    void SRAMDia();
    void EEPROMDia();
    void GPODia();
    void GPIDia();
    void PICIntrusionDia();
    void SPIDia();

signals:
    void updateText(QString str);

private:
    SELECT_ACTION TestSelectionMenu(string &info, TEST_FLOW &test_flow);
    SELECT_ACTION SingleFunctionMenu();
    void ShowMultiSelectionExample();
    SELECT_ACTION MultiFunctionMenu();


};

#endif // TESTPROGRAM_H
