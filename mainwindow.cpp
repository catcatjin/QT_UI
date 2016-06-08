#include <QString>
#include <cstdlib>

#include <QWaitCondition>
#include <QColorDialog>
#include <QThreadPool>
#include "windows.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "test_class1.h"


#include <QLibrary>
#include <signal.h>

#include "Platform.h"

#include "SixteenBitTimerTest.h"
#include "RandomGeneratorTest.h"
#include "cctalktest.h"
#include "SASTest.h"
#include "SRAMTest.h"
#include "EEPROMTest.h"
#include "GPOTest.h"
#include "GPITest.h"
#include "PICIntrusionTest.h"
#include "SPITest.h"

#include <iostream>

#ifdef WINDOWS
#include <iostream>
#include <tchar.h>
#include <time.h>
#else
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    thread = new QThread;
    test1 = new test_class1;
    sram_test = new SRAM_test;

    //sramtest = new SRAMTest;

    eeprom_test = new EEPROM_test;
    gpi_gpo_test = new GPI_GPO_test;
    pic_test = new PIC_test;
    ccTalk_test = new CCTALK_test;
    timer_test = new TIMER_test;
    sas_test = new SAS_test;
    rng_test = new RNG_test;
    spi_test = new SPI_test;

    ui->setupUi(this);

    setFixedSize(this->width(),this->height());
//connect(hello, start(), ccTalk_test, doTest());
    ui->sram_checkBox->setChecked(true);
    ui->eeprom_checkBox->setChecked(true);
    ui->gpi_gpo_checkBox->setChecked(true);
    ui->pic_checkBox->setChecked(true);
    ui->ccTalk_checkBox->setChecked(true);
    ui->timer_checkBox->setChecked(true);
    ui->sas_checkBox->setChecked(true);
    ui->rng_checkBox->setChecked(true);
    ui->spi_checkBox->setChecked(true);

    test1->moveToThread(thread);
    sram_test->moveToThread(thread);
    eeprom_test->moveToThread(thread);
    gpi_gpo_test->moveToThread(thread);
    pic_test->moveToThread(thread);
    ccTalk_test->moveToThread(thread);
    timer_test->moveToThread(thread);
    sas_test->moveToThread(thread);
    rng_test->moveToThread(thread);
    spi_test->moveToThread(thread);

     //connect(thread, SIGNAL(started()), sram_test, SLOT(doTest()));
     //connect(thread, SIGNAL(started()), eeprom_test, SLOT(doTest()));
     //connect(thread, SIGNAL(started()), gpi_gpo_test, SLOT(doTest()));
     //connect(thread, SIGNAL(started()), pic_test, SLOT(doTest()));
     //connect(thread, SIGNAL(started()), ccTalk_test, SLOT(doTest()));
     //connect(thread, SIGNAL(started()), spi_test, SLOT(doTest()));
     //connect(thread, SIGNAL(started()), timer_test, SLOT(doTest()));
     //connect(thread, SIGNAL(started()), sas_test, SLOT(doTest()));
     //connect(thread, SIGNAL(started()), rng_test, SLOT(doTest()));

    //thread->start();
    connectUI(ui->test_btn, test1, this);
    connectUI(ui->testSRAM, sram_test, this);

    //connectUI(ui->testSRAM, sramtest, this);

    connectUI(ui->testEEPROM, eeprom_test, this);
    connectUI(ui->testGPI_GPO, gpi_gpo_test, this);
    connectUI(ui->testPIC, pic_test, this);
    connectUI(ui->testCCTalk, ccTalk_test, this);
    connectUI(ui->testTimer, timer_test, this);
    connectUI(ui->testSAS, sas_test, this);
    connectUI(ui->testRNG, rng_test, this);
    connectUI(ui->testSPI, spi_test, this);
    sram_test->ConnectSignalSlot(this, SLOT(setTextEdit(QString)));
//thread->exit();

    //ui->testSRAM->click();
    //connect(ui->testSRAM, SIGNAL(clicked()), this, SLOT(slotClickBtn()));
    //connect(sram_test, SIGNAL(updateBtn(int, int)), this, SLOT(setBtn(int, int)));
    //connect(test1, SIGNAL(updateText(QString)), this, SLOT(setTextEdit(QString)));
    //connect(test1, SIGNAL(updateBtn(int, int)), this, SLOT(setBtn(int, int)));
    //connect(test1, SIGNAL(enableBtn(int, bool)), this, SLOT(enableBtn(int, bool)));
    //QThreadPool::globalInstance()->start(hello);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setTextEdit(QString str)
{
    ui->test_status->append(str);
}

void MainWindow::slotClickBtn()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    disconnect(thread, SIGNAL(started()), sram_test, SLOT(doTest()));

    //disconnect(thread, SIGNAL(started()), sramtest, SLOT(doTest()));

    disconnect(thread, SIGNAL(started()), eeprom_test, SLOT(doTest()));
    disconnect(thread, SIGNAL(started()), gpi_gpo_test, SLOT(doTest()));
    disconnect(thread, SIGNAL(started()), pic_test, SLOT(doTest()));
    disconnect(thread, SIGNAL(started()), ccTalk_test, SLOT(doTest()));
    disconnect(thread, SIGNAL(started()), spi_test, SLOT(doTest()));
    disconnect(thread, SIGNAL(started()), timer_test, SLOT(doTest()));
    disconnect(thread, SIGNAL(started()), sas_test, SLOT(doTest()));
    disconnect(thread, SIGNAL(started()), rng_test, SLOT(doTest()));

    //test1->start();
    if(btn == ui->test_btn)
    {
        if (ui->sram_checkBox->isChecked())
        {
            //ui->testSRAM->click();
            connect(thread, SIGNAL(started()), sram_test, SLOT(doTest()));
            //connect(thread, SIGNAL(started()), sramtest, SLOT(doTest()));
        }
        if (ui->eeprom_checkBox->isChecked())
        {
            connect(thread, SIGNAL(started()), eeprom_test, SLOT(doTest()));
        }
        if (ui->gpi_gpo_checkBox->isChecked())
        {
            connect(thread, SIGNAL(started()), gpi_gpo_test, SLOT(doTest()));
        }
        if (ui->pic_checkBox->isChecked())
        {
            connect(thread, SIGNAL(started()), pic_test, SLOT(doTest()));
        }
        if (ui->ccTalk_checkBox->isChecked())
        {
            connect(thread, SIGNAL(started()), ccTalk_test, SLOT(doTest()));
        }
        if (ui->spi_checkBox->isChecked())
        {
            connect(thread, SIGNAL(started()), spi_test, SLOT(doTest()));
        }
        if (ui->timer_checkBox->isChecked())
        {
            connect(thread, SIGNAL(started()), timer_test, SLOT(doTest()));
        }
        if (ui->sas_checkBox->isChecked())
        {
            connect(thread, SIGNAL(started()), sas_test, SLOT(doTest()));
        }
        if (ui->rng_checkBox->isChecked())
        {
            connect(thread, SIGNAL(started()), rng_test, SLOT(doTest()));
        }

        thread->start();
        thread->exit();

    }
    else if (btn == ui->testSRAM)
    {
        //sram_test->doTest();
        connect(thread, SIGNAL(started()), sram_test, SLOT(doTest()));
        //connect(thread, SIGNAL(started()), sramtest, SLOT(doTest()));
        thread->start();
        thread->exit();
    }
    else if (btn == ui->testEEPROM)
    {
        //eeprom_test->doTest();
        connect(thread, SIGNAL(started()), eeprom_test, SLOT(doTest()));
        thread->start();
        thread->exit();
    }
    else if (btn == ui->testGPI_GPO)
    {
        //gpi_gpo_test->doTest();
        connect(thread, SIGNAL(started()), gpi_gpo_test, SLOT(doTest()));
        thread->start();
        thread->exit();
    }
    else if (btn == ui->testPIC)
    {
        //pic_test->doTest();
        connect(thread, SIGNAL(started()), pic_test, SLOT(doTest()));
        thread->start();
        thread->exit();
    }
    else if (btn == ui->testCCTalk)
    {
        //ccTalk_test->doTest();
        connect(thread, SIGNAL(started()), ccTalk_test, SLOT(doTest()));
        thread->start();
        thread->exit();
    }
    else if (btn == ui->testTimer)
    {
        //timer_test->doTest();
        connect(thread, SIGNAL(started()), timer_test, SLOT(doTest()));
        thread->start();
        thread->exit();
    }
    else if (btn == ui->testSAS)
    {
        //sas_test->doTest();
        connect(thread, SIGNAL(started()), sas_test, SLOT(doTest()));
        thread->start();
        thread->exit();
    }
    else if (btn == ui->testRNG)
    {
        //rng_test->doTest();
        connect(thread, SIGNAL(started()), rng_test, SLOT(doTest()));
        thread->start();
        thread->exit();
    }
    else if (btn == ui->testSPI)
    {
        //spi_test->doTest();
        connect(thread, SIGNAL(started()), spi_test, SLOT(doTest()));
        thread->start();
        thread->exit();
    }


}

void MainWindow::setBtn(int btnID, int color)
{

    switch(btnID)
    {
    case TEST_BTN:
        guiTool.SetButtonColor(ui->test_btn, color);
        //ui->test_status->append("Hello world");
        break;

    case SRAM_BTN:
        guiTool.SetButtonColor(ui->testSRAM, color);
        break;

    case EEPROM_BTN:
        guiTool.SetButtonColor(ui->testEEPROM, color);
        break;
    case GPI_GPO_BTN:
        guiTool.SetButtonColor(ui->testGPI_GPO, color);
        break;

    case PIC_BTN:
        guiTool.SetButtonColor(ui->testPIC, color);
        break;

    case CCTALK_BTN:
        guiTool.SetButtonColor(ui->testCCTalk, color);
        break;

    case TIMER_BTN:
        guiTool.SetButtonColor(ui->testTimer, color);
        break;

    case SAS_BTN:
        guiTool.SetButtonColor(ui->testSAS, color);
        break;

    case RNG_BTN:
        guiTool.SetButtonColor(ui->testRNG, color);
        break;

    case SPI_BTN:
        guiTool.SetButtonColor(ui->testSPI, color);
        break;

    default:
        break;
    }
}

void MainWindow::enableBtn(int btnID, bool enable)
{
    switch(btnID)
    {
    case TEST_BTN:
        ui->test_btn->setEnabled(enable);
        //ui->test_status->append("disable test btn");
        break;

    case SRAM_BTN:
        ui->testSRAM->setEnabled(enable);
        break;

    case EEPROM_BTN:
        ui->testEEPROM->setEnabled(enable);
        break;

    case GPI_GPO_BTN:
        ui->testGPI_GPO->setEnabled(enable);
        break;

    case PIC_BTN:
        ui->testPIC->setEnabled(enable);
        break;

    case CCTALK_BTN:
        ui->testCCTalk->setEnabled(enable);
        break;

    case TIMER_BTN:
        ui->testTimer->setEnabled(enable);
        break;

    case SAS_BTN:
        ui->testSAS->setEnabled(enable);
        break;

    case RNG_BTN:
        ui->testRNG->setEnabled(enable);
        break;

    case SPI_BTN:
        ui->testSPI->setEnabled(enable);
        break;


    default:
        break;
    }

}

void MainWindow::connectUI(QPushButton* btn, QObject *obj1, QObject *UI_obj)
{
    connect(btn, SIGNAL(clicked()), UI_obj, SLOT(slotClickBtn()));
    connect(obj1, SIGNAL(updateText(QString)), UI_obj, SLOT(setTextEdit(QString)));
    connect(obj1, SIGNAL(updateBtn(int, int)), UI_obj, SLOT(setBtn(int, int)));
    connect(obj1, SIGNAL(enableBtn(int, bool)), UI_obj, SLOT(enableBtn(int, bool)));
}

void MainWindow::runTask()
{

}


