#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QObject>

#include <QStateMachine>

class Application;

class StateMachine : public QObject
{
    Q_OBJECT
public:
    StateMachine(Application *app);

    void start();
    void stop();

    void setUpActions();

    bool isEmpty();

    bool isWorkState();
    bool isShortBreakState();
    bool isLongBreakState();

signals:
    void idleStateIn();
    void workStateIn();
    void shortBreakStateIn();
    void longBreakStateIn();
    void workStateOut();
    void shortBreakStateOut();
    void longBreakStateOut();

private:
    Application *appIcon;

    QStateMachine *machine;
    QState *idleState;
    QState *workState;
    QState *shortBreakState;
    QState *longBreakState;
    QState *currentState;
};

#endif // STATEMACHINE_H
