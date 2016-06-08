#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QString>
#include "test_class1.h"
#include "sram_test.h"
#include "eeprom_test.h"
#include "gpi_gpo_test.h"
#include "pic_test.h"
#include "cctalk_test.h"
#include "timer_test.h"
#include "sas_test.h"
#include "rng_test.h"
#include "spi_test.h"

#include "Platform.h"

#include "SixteenBitTimerTest.h"
#include "RandomGeneratorTest.h"
#include "ccTalkTest.h"
#include "SASTest.h"
#include "SRAMTest.h"
#include "EEPROMTest.h"
#include "GPOTest.h"
#include "GPITest.h"
#include "PICIntrusionTest.h"
#include "SPITest.h"



#include "guitool.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void connectUI(QPushButton* btn, QObject *obj1, QObject *UI_obj);


private slots:
    void slotClickBtn();// after btn clicked, update UI
    void runTask();

    void setTextEdit(QString str);
    void setBtn(int btnID, int color);
    void enableBtn(int btnID, bool enable);


private:
    Ui::MainWindow *ui;
    GUITool guiTool;

    QThread *thread;// = new QThread;
    test_class1 *test1;// = new test_class1;
    SRAM_test *sram_test;// = new SRAM_test;
    //SRAMTest *sramtest;// = new SRAMTest;
    EEPROM_test *eeprom_test;// = new EEPROM_test;
    GPI_GPO_test *gpi_gpo_test;// = new GPI_GPO_test;
    PIC_test *pic_test;// = new PIC_test;
    CCTALK_test *ccTalk_test;// = new CCTALK_test;
    TIMER_test *timer_test;// = new TIMER_test;
    SAS_test *sas_test;// = new SAS_test;
    RNG_test *rng_test;// = new RNG_test;
    SPI_test *spi_test;// = new SPI_test;



};

#endif // MAINWINDOW_H
