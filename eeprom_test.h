#ifndef EEPROM_TEST_H
#define EEPROM_TEST_H
#include "windows.h"
#include <QWidget>

#include "guitool.h"

class EEPROM_test : public QObject
{
    Q_OBJECT
public:
    EEPROM_test();

public slots:
    virtual void doTest();

signals:
    void updateText(QString str);
    void updateBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);

private:
    int btnName = EEPROM_BTN;
};

#endif // EEPROM_TEST_H
