#ifndef TEST_CLASS1_H
#define TEST_CLASS1_H
#include "windows.h"
#include <QWidget>
#include <QThread>

#include "guitool.h"

class test_class1 : public QObject //: public QThread
{
    Q_OBJECT
public:
    test_class1();
    void doTest();
public slots:



//protected:
    //void run();


signals:
    void updateText(QString str);
    void updateBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);
    void test_manager();

private:


};

#endif // TEST_CLASS1_H
