#ifndef PIC_TEST_H
#define PIC_TEST_H

#include <QObject>
#include "windows.h"
#include <QWidget>

#include "guitool.h"

class PIC_test : public QObject
{
    Q_OBJECT
public:
    PIC_test();

public slots:
    virtual void doTest();

signals:
    void updateText(QString str);
    void updateBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);

private:
    int btnName = PIC_BTN;
};

#endif // PIC_TEST_H
