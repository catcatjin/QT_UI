#include "rng_test.h"
#include <QEventLoop>
#include <QTimer>

RNG_test::RNG_test(QObject *parent) : QObject(parent)
{

}

void RNG_test::doTest()
{
    emit enableBtn(btnName, false);
    emit updateBtn(btnName, YELLOW_XBTN);
    emit updateText(tr("RNG testing:Start test now.."));
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
    if (1)
    {
        Sleep(1000);
        emit updateBtn(btnName, GREEN_XBTN);


    }
    //emit updateText(tr("COM1 testing:RX/TX Test PASS!!"));
    //writeLog(tr("COM1 testing:RX/TX Test PASS!!"));

    //::close(fd1);
    //::close(fd2);

    emit enableBtn(btnName, true);
}
