#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QObject>

#include <QStateMachine>

class Application;

class StateMachine : public QObject
{
    Q_OBJECT
public:
    StateMachine(QObject *parent = nullptr);

    void start();
    void stop();

    void setupTransitions(Application *app);

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
    QStateMachine *machine;
    QState *idleState;
    QState *workState;
    QState *shortBreakState;
    QState *longBreakState;
    QState *currentState;
};

#endif // STATEMACHINE_H
