#include "systrayicon.h"
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>
#include <QStateMachine>

#include <QDebug>


#define TEST

#ifdef TEST
#define DEFAULT_WORK_IN_SECONDS 10
#define DEFAULT_BREAK_IN_SECONDS 5
#define DEFAULT_BIG_BREAK_IN_SECONDS 15
#else
#define DEFAULT_WORK_IN_SECONDS 25 * 60
#define DEFAULT_BREAK_IN_SECONDS 5 * 60
#define DEFAULT_BIG_BREAK_IN_SECONDS 30 * 60
#endif

SysTrayIcon::SysTrayIcon()
    : workTimeInSeconds(DEFAULT_WORK_IN_SECONDS)
    , shortBreakInSeconds(DEFAULT_BREAK_IN_SECONDS)
    , longBreakInSeconds(DEFAULT_BIG_BREAK_IN_SECONDS)
    , currentTimeInSeconds(0)
    , pomodoros(0)
    , trayIcon(new QSystemTrayIcon(this))
    , timer(new QTimer(this))
    , machine(new QStateMachine(this))
    , idleState(new QState())
    , workState(new QState())
    , shortBreakState(new QState())
    , longBreakState(new QState())
{
    connect(timer, &QTimer::timeout, this, &SysTrayIcon::onTimerTimeout);

    trayIcon->setIcon(QIcon(":/icons/icon_tomato_black.ico"));

    QMenu *contextMenu = new QMenu();
    QAction *quit = new QAction("Quit", this);

    workAction = new QAction(QIcon(":/icons/icon_tomato_red.ico"), "Pomodoro", this);
    stopAction = new QAction(QIcon(":/icons/stop.svg"), "Stop", this);
    shortBreakAction = new QAction(QIcon(":/icons/icon_tomato_green.ico"), "Short break", this);
    longBreakAction = new QAction(QIcon(":/icons/icon_tomato_blue.ico"), "Long break", this);
    timerAction = new QAction(QIcon(":/icons/clock.svg"), "00:00", this);
    pomodoroCountAction = new QAction("No Pomodoros", this);
    resetPomodorosAction = new QAction("Reset count", this);
    aboutAction = new QAction("About qmodoro", this);
    preferencesAction = new QAction("Preferences...", this);

    connect(resetPomodorosAction, &QAction::triggered, this, &SysTrayIcon::onResetCount);
    connect(aboutAction, &QAction::triggered, this, &SysTrayIcon::onAbout);
    connect(preferencesAction, &QAction::triggered, this, &SysTrayIcon::onPreferences);

    timerAction->setEnabled(false);
    pomodoroCountAction->setEnabled(false);
    stopAction->setEnabled(false);
    resetPomodorosAction->setEnabled(false);

    connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    contextMenu->addAction(timerAction);
    contextMenu->addAction(stopAction);
    contextMenu->addSeparator();
    contextMenu->addAction(pomodoroCountAction);
    contextMenu->addAction(resetPomodorosAction);
    contextMenu->addSeparator();
    contextMenu->addAction(workAction);
    contextMenu->addAction(shortBreakAction);
    contextMenu->addAction(longBreakAction);
    contextMenu->addSeparator();
    contextMenu->addAction(aboutAction);
    contextMenu->addAction(preferencesAction);
    contextMenu->addSeparator();
    contextMenu->addAction(quit);

    trayIcon->setContextMenu(contextMenu);

    connect(idleState, &QState::entered, this, &SysTrayIcon::onIdleStateEntered);
    connect(workState, &QState::entered, this, &SysTrayIcon::onWorkStateEntered);
    connect(shortBreakState, &QState::entered, this, &SysTrayIcon::onShortBreakStateEntered);
    connect(longBreakState, &QState::entered, this, &SysTrayIcon::onLongBreakStateEntered);

    connect(workState, &QState::exited, this, &SysTrayIcon::onWorkStateExited);

    idleState->setObjectName("IdleState");
    idleState->addTransition(workAction, &QAction::triggered, workState);
    idleState->addTransition(shortBreakAction, &QAction::triggered, shortBreakState);
    idleState->addTransition(longBreakAction, &QAction::triggered, longBreakState);

    workState->setObjectName("WorkState");
    workState->addTransition(stopAction, &QAction::triggered, idleState);
    workState->addTransition(this, &SysTrayIcon::timeout, idleState);
    workState->addTransition(shortBreakAction, &QAction::triggered, shortBreakState);
    workState->addTransition(longBreakAction, &QAction::triggered, longBreakState);

    shortBreakState->setObjectName("ShortBreakState");
    shortBreakState->addTransition(stopAction, &QAction::triggered, idleState);
    shortBreakState->addTransition(this, &SysTrayIcon::timeout, idleState);
    shortBreakState->addTransition(workAction, &QAction::triggered, workState);
    shortBreakState->addTransition(longBreakAction, &QAction::triggered, longBreakState);

    longBreakState->setObjectName("LongBreakState");
    longBreakState->addTransition(stopAction, &QAction::triggered, idleState);
    longBreakState->addTransition(this, &SysTrayIcon::timeout, idleState);
    longBreakState->addTransition(workAction, &QAction::triggered, workState);
    longBreakState->addTransition(shortBreakAction, &QAction::triggered, shortBreakState);

    machine->addState(idleState);
    machine->addState(workState);
    machine->addState(shortBreakState);
    machine->addState(longBreakState);
    machine->setInitialState(idleState);
    machine->start();
}

void SysTrayIcon::show()
{
    trayIcon->show();
}

void SysTrayIcon::onIdleStateEntered()
{
    qDebug() << machine->configuration();
    trayIcon->setIcon(QIcon(":/icons/icon_tomato_black.ico"));
    currentTimeInSeconds = 0;
    timer->stop();
    timerAction->setText(QStringLiteral("00:00"));
    stopAction->setEnabled(false);
}

void SysTrayIcon::onWorkStateEntered()
{
    qDebug() << machine->configuration();
    trayIcon->setIcon(workAction->icon());
    currentTimeInSeconds = 0;
    timer->start(1000);
    stopAction->setEnabled(true);
}

void SysTrayIcon::onShortBreakStateEntered()
{
    qDebug() << machine->configuration();
    trayIcon->setIcon(shortBreakAction->icon());
    currentTimeInSeconds = 0;
    timer->start(1000);
    stopAction->setEnabled(true);
}

void SysTrayIcon::onLongBreakStateEntered()
{
    qDebug() << machine->configuration();
    trayIcon->setIcon(longBreakAction->icon());
    currentTimeInSeconds = 0;
    timer->start(1000);
    stopAction->setEnabled(true);
}

void SysTrayIcon::onWorkStateExited()
{
    if (currentTimeInSeconds >= workTimeInSeconds) {
        ++pomodoros;
        pomodoroCountAction->setText(QString("%1 pomodoros").arg(pomodoros));
        resetPomodorosAction->setEnabled(true);
    }
}

void SysTrayIcon::onResetCount()
{
    pomodoroCountAction->setText("No Pomodoros");
    pomodoros = 0;
    resetPomodorosAction->setEnabled(false);
}

void SysTrayIcon::onAbout()
{
    QMessageBox::about(nullptr, "About qmodoro", "qmodoro is a pomodoro-style timebox timer app, aimed for productivity.");
    qDebug() << "Did I reach here?";
}

void SysTrayIcon::onPreferences()
{
    QMessageBox::information(nullptr, "Not implemented", "Feature not implmemented yet, but it's planned");
}

void SysTrayIcon::onTimerTimeout()
{
    if (machine->configuration().size() == 0) {
        return;
    }

    ++currentTimeInSeconds;

    int timeInSeconds = 0;
    QAbstractState *state = machine->configuration().toList().first();
    if (state == workState)
        timeInSeconds = workTimeInSeconds;
    else if (state == shortBreakState)
        timeInSeconds = shortBreakInSeconds;
    else if (state == longBreakState)
        timeInSeconds = longBreakInSeconds;

    if (timeInSeconds > 0) {
        if (currentTimeInSeconds >= timeInSeconds) {
            emit timeout();
        } else {
            const int timeRemaining = timeInSeconds - currentTimeInSeconds;
            const int minutes = timeRemaining / 60;
            const int seconds = timeRemaining % 60;
            timerAction->setText(QStringLiteral("%1:%2").arg(minutes).arg(seconds));
        }
    }
}
