#include "statemachine.h"

#include "application.h"

#include <QAction>

StateMachine::StateMachine(QObject *parent)
    : QObject(parent),
      machine(new QStateMachine(this)),
      idleState(new QState()),
      workState(new QState()),
      shortBreakState(new QState()),
      longBreakState(new QState())
{
    connect(idleState, &QState::entered, this, &StateMachine::idleStateEntered);
    connect(workState, &QState::entered, this, &StateMachine::workStateEntered);
    connect(shortBreakState, &QState::entered, this, &StateMachine::shortBreakStateEntered);
    connect(longBreakState, &QState::entered, this, &StateMachine::longBreakStateEntered);

    connect(workState, &QState::exited, this, &StateMachine::workStateExited);
    connect(shortBreakState, &QState::exited, this, &StateMachine::shortBreakStateExited);
    connect(longBreakState, &QState::exited, this, &StateMachine::longBreakStateExited);

    machine->addState(idleState);
    machine->addState(workState);
    machine->addState(shortBreakState);
    machine->addState(longBreakState);
    machine->setInitialState(idleState);

    machine->configuration();
}

void StateMachine::start()
{
    machine->start();
}

void StateMachine::stop()
{
    machine->stop();
}

void StateMachine::setupTransitions()
{
    idleState->setObjectName("IdleState");
    idleState->addTransition(this, &StateMachine::workAction, workState);
    idleState->addTransition(this, &StateMachine::shortBreakAction, shortBreakState);
    idleState->addTransition(this, &StateMachine::longBreakAction, longBreakState);

    workState->setObjectName("WorkState");
    workState->addTransition(this, &StateMachine::stopAction, idleState);
    workState->addTransition(this, &StateMachine::timeoutAction, idleState);
    workState->addTransition(this, &StateMachine::shortBreakAction, shortBreakState);
    workState->addTransition(this, &StateMachine::longBreakAction, longBreakState);

    shortBreakState->setObjectName("ShortBreakState");
    shortBreakState->addTransition(this, &StateMachine::stopAction, idleState);
    shortBreakState->addTransition(this, &StateMachine::timeoutAction, idleState);
    shortBreakState->addTransition(this, &StateMachine::workAction, workState);
    shortBreakState->addTransition(this, &StateMachine::longBreakAction, longBreakState);

    longBreakState->setObjectName("LongBreakState");
    longBreakState->addTransition(this, &StateMachine::stopAction, idleState);
    longBreakState->addTransition(this, &StateMachine::timeoutAction, idleState);
    longBreakState->addTransition(this, &StateMachine::workAction, workState);
    longBreakState->addTransition(this, &StateMachine::shortBreakAction, shortBreakState);
}

bool StateMachine::isEmpty()
{
    return machine->configuration().size() == 0;
}

bool StateMachine::isWorkState()
{
    return machine->configuration().toList().first() == workState;
}

bool StateMachine::isShortBreakState()
{
    return machine->configuration().toList().first() == shortBreakState;
}

bool StateMachine::isLongBreakState()
{
    return machine->configuration().toList().first() == longBreakState;
}
