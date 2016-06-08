#ifndef SAS_TEST_H
#define SAS_TEST_H

#include <QObject>
#include "windows.h"
#include <QWidget>

#include "guitool.h"

class SAS_test : public QObject
{
    Q_OBJECT
public:
    explicit SAS_test(QObject *parent = 0);

public slots:
    virtual void doTest();

signals:
    void updateText(QString str);
    void updateBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);

private:
    int btnName = SAS_BTN;
};

#endif // SAS_TEST_H
