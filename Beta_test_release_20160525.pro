#-------------------------------------------------
#
# Project created by QtCreator 2016-05-25T14:43:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Beta_test_release_20160525
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    test_class1.cpp \
    guitool.cpp \
    sram_test.cpp \
    eeprom_test.cpp \
    gpi_gpo_test.cpp \
    pic_test.cpp \
    cctalk_test.cpp \
    timer_test.cpp \
    sas_test.cpp \
    rng_test.cpp \
    spi_test.cpp \
    task1.cpp \
    EEPROMTest.cpp \
    RandomGeneratorTest.cpp \
    SixteenBitTimerTest.cpp \
    PICIntrusionTest.cpp \
    GPITest.cpp \
    InterruptProcess.cpp \
    SASTest.cpp \
    OutputTextTool.cpp \
    SerialPort.cpp \
    SPITest.cpp \
    GPOTest.cpp \
    sramtest.cpp \
    cctalktest.cpp \
    testprogram.cpp


HEADERS  += mainwindow.h \
    test_class1.h \
    guitool.h \
    sram_test.h \
    eeprom_test.h \
    gpi_gpo_test.h \
    pic_test.h \
    cctalk_test.h \
    timer_test.h \
    sas_test.h \
    rng_test.h \
    spi_test.h \
    task1.h \
    OutputTextTool.h \
    RandomGeneratorTest.h \
    GPOTest.h \
    PICIntrusionTest.h \
    EEPROMTest.h \
    SPITest.h \
    GPITest.h \
    InterruptProcess.h \
    SixteenBitTimerTest.h \
    FunctionList.h \
    Platform.h \
    TestClass.h \
    SASTest.h \
    SerialPort.h \
    FpgaPicAPI.h \
    sramtest.h \
    cctalktest.h \
    testprogram.h \
    TestDefaultParam.h

FORMS    += mainwindow.ui

DISTFILES += \
    CFPGADrv.lib \
    CFPGADrv.dll

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lCFPGADrv
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lCFPGADrv

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

