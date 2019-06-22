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
    void idleStateEntered();
    void workStateEntered();
    void shortBreakStateEntered();
    void longBreakStateEntered();
    void workStateExited();
    void shortBreakStateExited();
    void longBreakStateExited();

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
