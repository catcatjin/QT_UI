#ifndef HELLOWORLDTASK_H
#define HELLOWORLDTASK_H
#include <QObject>

#include <QWidget>
#include <QThreadPool>


class HelloWorldTask : public QRunnable
{
    Q_OBJECT
public:
    HelloWorldTask();

signals:
    void run();
};

#endif // HELLOWORLDTASK_H
