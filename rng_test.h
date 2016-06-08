#ifndef RNG_TEST_H
#define RNG_TEST_H

#include <QObject>
#include "windows.h"
#include <QWidget>

#include "guitool.h"

class RNG_test : public QObject
{
    Q_OBJECT
public:
    explicit RNG_test(QObject *parent = 0);

public slots:
    virtual void doTest();

signals:
    void updateText(QString str);
    void updateBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);

private:
    int btnName = RNG_BTN;
};

#endif // RNG_TEST_H
