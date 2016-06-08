#ifndef SPI_TEST_H
#define SPI_TEST_H

#include <QObject>
#include "windows.h"
#include <QWidget>

#include "guitool.h"

class SPI_test : public QObject
{
    Q_OBJECT
public:
    explicit SPI_test(QObject *parent = 0);

public slots:
    virtual void doTest();

signals:
    void updateText(QString str);
    void updateBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);

private:
    int btnName = SPI_BTN;
};

#endif // SPI_TEST_H
