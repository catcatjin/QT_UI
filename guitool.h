#ifndef GUITOOL_H
#define GUITOOL_H

#include <QPushButton>
#include <QProgressBar>
#include <QString>

enum ButtonColor{
    RED_BTN = 0,
    GREEN_BTN,
    BULE_BTN,
    YELLOW_BTN,
    WHITE_BTN,
    GRAY_BTN,
    RED_XBTN,
    GREEN_XBTN,
    BULE_XBTN,
    YELLOW_XBTN,
    WHITE_XBTN,
    GRAY_XBTN,
    DEFAULT_BTN = 19,
};

enum ButtonID{
    TEST_BTN = 0,
    SRAM_BTN ,
    EEPROM_BTN ,
    GPI_GPO_BTN ,
    PIC_BTN ,
    CCTALK_BTN ,
    TIMER_BTN , //16 bit timer
    SAS_BTN ,
    RNG_BTN , //Random number
    SPI_BTN ,

    // [Start] Add new button here

    // [End] Add
};

enum PBarType{
    PBAR_TYPE0 = 0,
    PBAR_TYPE1,
};

class GUITool{
public:
    GUITool();
    ~GUITool();
    void SetButtonColor(QPushButton *btn, int color);
    void SetProgress(QProgressBar *pbar, int type);
    void GetButtonColor(QPushButton *btn, QColor *color);

private:
    QString btnColorStr[20];
    QString pBarColorStr[20];
};


#endif // GUITOOL_H
