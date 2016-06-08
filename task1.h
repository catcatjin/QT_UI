#ifndef TASK1_H
#define TASK1_H

#include <QObject>
#include <QWidget>
#include <QThread>

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

class task1 : public QThread
{
    Q_OBJECT
public:
    task1();

    void run();

signals:
    void start_task();

private:
    test_class1 *test1 = new test_class1;
    SRAM_test *sram_test = new SRAM_test;
    EEPROM_test *eeprom_test = new EEPROM_test;
    GPI_GPO_test *gpi_gpo_test = new GPI_GPO_test;
    PIC_test *pic_test = new PIC_test;
    CCTALK_test *ccTalk_test = new CCTALK_test;
    TIMER_test *timer_test = new TIMER_test;
    SAS_test *sas_test = new SAS_test;
    RNG_test *rng_test = new RNG_test;
    SPI_test *spi_test = new SPI_test;

};

#endif // TASK1_H
