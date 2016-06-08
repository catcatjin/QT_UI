#ifndef TIMER_TEST_H
#define TIMER_TEST_H

#include <QObject>
#include "windows.h"
#include <QWidget>

#include "guitool.h"

class TIMER_test : public QObject
{
    Q_OBJECT
public:
    explicit TIMER_test(QObject *parent = 0);

public slots:
    virtual void doTest();

signals:
    void updateText(QString str);
    void updateBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);

private:
    int btnName = TIMER_BTN;
};

#endif // TIMER_TEST_H
