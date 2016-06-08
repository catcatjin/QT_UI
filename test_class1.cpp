#include "test_class1.h"
#include <QEventLoop>
#include <QTimer>

test_class1::test_class1()
{

}

//void test_class1::run()
void test_class1::doTest()
{
    emit enableBtn(TEST_BTN, false);
    emit updateBtn(TEST_BTN, YELLOW_XBTN);
    emit updateText(tr("ALL testing:Start test now.."));
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
    if (1)
    {
        emit updateBtn(TEST_BTN, GREEN_XBTN);


    }
    //emit updateText(tr("COM1 testing:RX/TX Test PASS!!"));
    //writeLog(tr("COM1 testing:RX/TX Test PASS!!"));

    //::close(fd1);
    //::close(fd2);

    emit enableBtn(TEST_BTN, true);
}

