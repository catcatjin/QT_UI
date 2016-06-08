#include "sram_test.h"
#include "SRAMTest.h"
#include "testprogram.h"
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
TestProgram *testProgramManager = new TestProgram;
SRAM_test::SRAM_test()
{

}


//void SRAM_test::run()

void SRAM_test::doTest()
{


    int selectedFunction = (int)0x10;
    emit enableBtn(btnName, false);
    emit updateBtn(btnName, YELLOW_XBTN);
    emit updateText(tr("SRAM testing:Start test now.."));
    QEventLoop eventloop;
    QTimer::singleShot(1, &eventloop, SLOT(quit()));
    eventloop.exec();
    //writeLog("[START COM1&COM3 TEST]");
/*
    if(openCOM1COM3())
    {
        emit updateBtn(UART1_BTN, RED_XBTN);
        emit updateText(tr("COM1 testing:port open fail.."));
        writeLog(tr("COM1 testing:port open fail.."));
        emit enableBtn(UART1_BTN, true);
        result = -1;
        return;
    }

    if(COMRxTxTest())
    {
        result = -1;
        ::close(fd1);
        ::close(fd2);
        emit updateBtn(UART1_BTN, RED_XBTN);
        emit enableBtn(UART1_BTN, true);
        return;
    }
*/
    testProgramManager->GetDeviceVerson();
    if (1)
    {
        //SRAMTest1 sramTest;
        //sramTest.diagnostic();
        //sramtest->diagnostic();
        //testProgramManager->TestManager(selectedFunction);
        testProgramManager->SRAMDia();
        //sFinalResult.append(sramTest.getResultString());
        emit updateBtn(btnName, GREEN_XBTN);


    }
    //emit updateText(tr("COM1 testing:RX/TX Test PASS!!"));
    //writeLog(tr("COM1 testing:RX/TX Test PASS!!"));

    //::close(fd1);
    //::close(fd2);

    emit enableBtn(btnName, true);
}

void SRAM_test::ConnectSignalSlot(QObject *pObj, const char * pSlot)
{
    if (NULL == pObj || NULL == pSlot)
    {
        qDebug() << "param error";
        return;
    }
    testProgramManager->ConnectSlot(pObj, pSlot);
}
