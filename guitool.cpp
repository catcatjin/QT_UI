#include "guitool.h"

GUITool::GUITool(){
    //Red and RedX
    btnColorStr[0] = "QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(255,50,50), stop:1 white)}";
    btnColorStr[6] = "QPushButton:hover{background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(255,130,130), stop:1 white)} \
                      QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(255,50,50), stop:1 white)} \
                      QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 1,stop:0 white stop:0.5 yellow stop:1 white)} \
                      QPushButton {border: 2px solid #8f8f91; border-radius: 6px;}";
    //Green and GreenX
    btnColorStr[1] = "QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(50,255,50), stop:1 white)}";
    btnColorStr[7] = "QPushButton:hover{background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(130,255,130), stop:1 white)} \
                      QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(50,255,50), stop:1 white)} \
                      QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 1,stop:0 white stop:0.5 yellow stop:1 white)} \
                      QPushButton {border: 2px solid #8f8f91; border-radius: 6px;}";
    //Blue and BlueX
    btnColorStr[2] = "QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(50,50,255), stop:1 white)}";
    btnColorStr[8] = "QPushButton:hover{background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(100,100,255), stop:1 white)} \
                      QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(50,50,255), stop:1 white)} \
                      QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 1,stop:0 white stop:0.5 yellow stop:1 white)} \
                      QPushButton {border: 2px solid #8f8f91; border-radius: 6px;}";
    //Yellow and YellowX
    btnColorStr[3] = "QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(255,255,50), stop:1 white)}";
    btnColorStr[9] = "QPushButton:hover{background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(255,255,100), stop:1 white)} \
                      QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(255,255,50), stop:1 white)} \
                      QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 1,stop:0 white stop:0.5 yellow stop:1 white)} \
                      QPushButton {border: 2px solid #8f8f91; border-radius: 6px;}";

    //Gray and GrayX
    btnColorStr[5] = "QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(78,55,22), stop:1 white)}";
    btnColorStr[11] = "QPushButton:hover{background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(78,55,22), stop:1 white)} \
                      QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(78,55,22), stop:1 white)} \
                      QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 1,stop:0 white stop:0.5 yellow stop:1 white)} \
                      QPushButton {border: 2px solid #8f8f91; border-radius: 6px;}";

    //Default
    btnColorStr[19] = "QPushButton:hover{background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 rgb(170,170,170), stop:1 white)} \
                      QPushButton {background: qlineargradient(x1:0, y1:1, x2:1, y2:1,stop:0 white, stop: 0.5 white, stop:1 white)} \
                      QPushButton:pressed {background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 1,stop:0 white stop:0.5 yellow stop:1 white)} \
                      QPushButton {border: 2px solid #8f8f91; border-radius: 6px;}";

    pBarColorStr[0] = "QProgressBar{border: 2px solid grey;border-radius: 5px;text-align: center;} \
                      QProgressBar::chunk{background-color: #05B8CC;width: 20px;margin: 0.5px;}";
}

GUITool::~GUITool(){

}

void GUITool::SetButtonColor(QPushButton *btn, int color){
    btn->setStyleSheet(btnColorStr[color]);
}

void GUITool::SetProgress(QProgressBar *pbar, int type){
    pbar->setStyleSheet(pBarColorStr[type]);
}

void GUITool::GetButtonColor(QPushButton *btn, QColor *color)
{
    *color = btn->palette().color(QPalette::Button);
}
