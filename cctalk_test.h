#ifndef CCTALK_TEST_H
#define CCTALK_TEST_H

#include <QObject>
#include "windows.h"
#include <QWidget>

#include "guitool.h"
//#include "cctalktest.h"
class CCTalkTest;

class CCTALK_test : public QObject//, CCTalkTest
{
    Q_OBJECT
public:
    explicit CCTALK_test(QObject *parent = 0);

public slots:
    void doTest();

signals:
    void updateText(QString str);
    void updateBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);

private:
    int m_nBtnName = CCTALK_BTN;
    CCTalkTest *m_pCCTalktest;
};

#endif // CCTALK_TEST_H
