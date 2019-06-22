#include "statemachine.h"

#include "application.h"

#include <QAction>

StateMachine::StateMachine(Application *app)
    : appIcon(app),
      machine(new QStateMachine(this)),
      idleState(new QState()),
      workState(new QState()),
      shortBreakState(new QState()),
      longBreakState(new QState())
{
    connect(idleState, &QState::entered, this, &StateMachine::idleStateIn);
    connect(workState, &QState::entered, this, &StateMachine::workStateIn);
    connect(shortBreakState, &QState::entered, this, &StateMachine::shortBreakStateIn);
    connect(longBreakState, &QState::entered, this, &StateMachine::longBreakStateIn);

    connect(workState, &QState::exited, this, &StateMachine::workStateOut);
    connect(shortBreakState, &QState::exited, this, &StateMachine::shortBreakStateOut);
    connect(longBreakState, &QState::exited, this, &StateMachine::longBreakStateOut);

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

void StateMachine::setUpActions()
{
    idleState->setObjectName("IdleState");
    idleState->addTransition(appIcon->workAction(), &QAction::triggered, workState);
    idleState->addTransition(appIcon->shortBreakAction(), &QAction::triggered, shortBreakState);
    idleState->addTransition(appIcon->longBreakAction(), &QAction::triggered, longBreakState);

    workState->setObjectName("WorkState");
    workState->addTransition(appIcon->stopAction(), &QAction::triggered, idleState);
    workState->addTransition(appIcon, &Application::timeout, idleState);
    workState->addTransition(appIcon->shortBreakAction(), &QAction::triggered, shortBreakState);
    workState->addTransition(appIcon->longBreakAction(), &QAction::triggered, longBreakState);

    shortBreakState->setObjectName("ShortBreakState");
    shortBreakState->addTransition(appIcon->stopAction(), &QAction::triggered, idleState);
    shortBreakState->addTransition(appIcon, &Application::timeout, idleState);
    shortBreakState->addTransition(appIcon->workAction(), &QAction::triggered, workState);
    shortBreakState->addTransition(appIcon->longBreakAction(), &QAction::triggered, longBreakState);

    longBreakState->setObjectName("LongBreakState");
    longBreakState->addTransition(appIcon->stopAction(), &QAction::triggered, idleState);
    longBreakState->addTransition(appIcon, &Application::timeout, idleState);
    longBreakState->addTransition(appIcon->workAction(), &QAction::triggered, workState);
    longBreakState->addTransition(appIcon->shortBreakAction(), &QAction::triggered,
                                  shortBreakState);
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
