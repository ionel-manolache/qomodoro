#include "systrayicon.h"
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>
#include <QStateMachine>

#include <QSettings>

#include "preferencesdialog.h"

#include <QDebug>


static const QString shortBreakString = QStringLiteral("shortBreakTime");
static const QString longBreakString = QStringLiteral("longBreakTime");
static const QString workString = QStringLiteral("workTime");
static const QString autoStartString = QStringLiteral("autoStartTimer");
static const QString soundOnTimerStartString = QStringLiteral("soundOnTimerStart");
static const QString soundOnTimerEndString = QStringLiteral("soundOnTimerEnd");
static const QString notificationOnTimerEndString = QStringLiteral("notificationOnTimerEnd");
static const QString tickTockDuringWorkString = QStringLiteral("tickTockDuringWork");
static const QString tickTockDuringBreakString = QStringLiteral("tickTockDuringBreak");

static const QString timeString = QStringLiteral("%1:%2");


#define DEFAULT_WORK 25
#define DEFAULT_BREAK 5
#define DEFAULT_BIG_BREAK 30

static QString getTimeString(int minutes, int seconds)
{
    return timeString.arg(minutes, 2, 10, QChar('0'))
                     .arg(seconds, 2, 10, QChar('0'));
}

SysTrayIcon::SysTrayIcon()
    : workTimeInSeconds(0)
    , shortBreakInSeconds(0)
    , longBreakInSeconds(0)
    , currentTimeInSeconds(0)
    , pomodoros(0)
    , trayIcon(new QSystemTrayIcon(this))
    , timer(new QTimer(this))
    , machine(new QStateMachine(this))
    , idleState(new QState())
    , workState(new QState())
    , shortBreakState(new QState())
    , longBreakState(new QState())
    , prefDialog(new PreferencesDialog)
{
    settings = new QSettings("com.qomodoro", "qomodoro");

    workTimeInSeconds = settings->value(workString, DEFAULT_WORK).toInt() * 60;
    shortBreakInSeconds = settings->value(shortBreakString, DEFAULT_BREAK).toInt() * 60;
    longBreakInSeconds = settings->value(longBreakString, DEFAULT_BIG_BREAK).toInt() * 60;

    connect(timer, &QTimer::timeout, this, &SysTrayIcon::onTimerTimeout);

    idleIcon = QIcon(":/icons/icon_tomato_black.ico");
    trayIcon->setIcon(idleIcon);

    QMenu *contextMenu = new QMenu();
    QAction *quit = new QAction(tr("Quit"), this);
    quit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

    workAction = new QAction(QIcon(":/icons/icon_tomato_red.ico"), tr("Pomodoro"), this);
    stopAction = new QAction(QIcon(":/icons/stop.svg"), tr("Stop"), this);
    shortBreakAction = new QAction(QIcon(":/icons/icon_tomato_green.ico"), tr("Short break"), this);
    longBreakAction = new QAction(QIcon(":/icons/icon_tomato_blue.ico"), tr("Long break"), this);
    timerAction = new QAction(QIcon(":/icons/clock.svg"), getTimeString(0, 0), this);
    pomodoroCountAction = new QAction(tr("No Pomodoros"), this);
    resetPomodorosAction = new QAction(tr("Reset count"), this);
    aboutAction = new QAction(tr("About qmodoro"), this);
    preferencesAction = new QAction(tr("Preferences..."), this);

    connect(resetPomodorosAction, &QAction::triggered, this, &SysTrayIcon::onResetCount);
    connect(aboutAction, &QAction::triggered, this, &SysTrayIcon::onAbout);
    connect(preferencesAction, &QAction::triggered, this, &SysTrayIcon::onPreferences);

    prefDialog->setSettings(settings);
    connect(prefDialog, &PreferencesDialog::accepted, this, &SysTrayIcon::onPreferencesSaved);

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
    trayIcon->setIcon(idleIcon);
    currentTimeInSeconds = 0;
    timer->stop();
    stopAction->setEnabled(false);

    timerAction->setText(getTimeString(0, 0));
}

void SysTrayIcon::onWorkStateEntered()
{
    stateChanged(workAction->icon());
}

void SysTrayIcon::onShortBreakStateEntered()
{
    stateChanged(shortBreakAction->icon());
}

void SysTrayIcon::onLongBreakStateEntered()
{
    stateChanged(longBreakAction->icon());
}

void SysTrayIcon::onWorkStateExited()
{
    if (currentTimeInSeconds >= workTimeInSeconds) {
        ++pomodoros;
        pomodoroCountAction->setText(tr("%1 pomodoros").arg(pomodoros));
        resetPomodorosAction->setEnabled(true);
    }
}

void SysTrayIcon::stateChanged(QIcon icon)
{
    trayIcon->setIcon(icon);
    currentTimeInSeconds = 0;
    timer->start(1000);
    stopAction->setEnabled(true);
}

void SysTrayIcon::onResetCount()
{
    pomodoroCountAction->setText(tr("No Pomodoros"));
    pomodoros = 0;
    resetPomodorosAction->setEnabled(false);
}

void SysTrayIcon::onAbout()
{
    QMessageBox::about(nullptr, tr("About %1").arg(qApp->applicationName())
                       , tr("%1 is a pomodoro-style timebox timer app, aimed for productivity.")
                       .arg(qApp->applicationName()));
}

void SysTrayIcon::onPreferences()
{
    prefDialog->loadSettings();
    prefDialog->show();
}

void SysTrayIcon::onPreferencesSaved()
{
    workTimeInSeconds = settings->value(workString, DEFAULT_WORK).toInt() * 60;
    shortBreakInSeconds = settings->value(shortBreakString, DEFAULT_BREAK).toInt() * 60;
    longBreakInSeconds = settings->value(longBreakString, DEFAULT_BIG_BREAK).toInt() * 60;
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
            timerAction->setText(getTimeString(minutes, seconds));
        }
    }
}
