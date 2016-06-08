#ifndef SRAM_TEST_H
#define SRAM_TEST_H
#include "windows.h"
#include <QWidget>

#include "guitool.h"
#include "sramtest.h"

class SRAM_test : public QObject//, SRAMTest
{
    Q_OBJECT
public:
    SRAM_test();

public slots:
    void doTest();

    void ConnectSignalSlot(QObject *pObj, const char * pSlot);

signals:
    void updateText(QString str);
    void updateBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);

private:
    int btnName = SRAM_BTN;


};

#endif // SRAM_TEST_H
