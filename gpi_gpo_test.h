#ifndef GPI_GPO_TEST_H
#define GPI_GPO_TEST_H

#include "windows.h"
#include <QWidget>

#include "guitool.h"

class GPI_GPO_test : public QObject
{
    Q_OBJECT
public:
    GPI_GPO_test();

public slots:
    virtual void doTest();

signals:
    void updateText(QString str);
    void updateBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);

private:
    int btnName = GPI_GPO_BTN;
};

#endif // GPI_GPO_TEST_H
