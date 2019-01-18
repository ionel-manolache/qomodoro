#include "systrayicon.h"
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>

#include <QDebug>

// TODO implement state machine and remove duplicated code
// TODO think about adding a "Break" action


#define TEST

#ifdef TEST
#define DEFAULT_WORK_IN_SECONDS 1 * 60
#define DEFAULT_BREAK_IN_SECONDS 1 * 60
#define DEFAULT_BIG_BREAK_IN_SECONDS 1 * 60
#else
#define DEFAULT_WORK_IN_SECONDS 25 * 60
#define DEFAULT_BREAK_IN_SECONDS 5 * 60
#define DEFAULT_BIG_BREAK_IN_SECONDS 30 * 60
#endif

SysTrayIcon::SysTrayIcon()
    : workTimeInSeconds(DEFAULT_WORK_IN_SECONDS)
    , shortBreakInSeconds(DEFAULT_BREAK_IN_SECONDS)
    , bigBreakInSeconds(DEFAULT_BIG_BREAK_IN_SECONDS)
    , current(0)
    , pomodoros(0)
    , trayIcon(new QSystemTrayIcon(this))
    , timer(new QTimer(this))
{
    connect(timer, &QTimer::timeout, this, &SysTrayIcon::onTimerTimeout);

    trayIcon->setIcon(QIcon(":/icons/icon_tomato_black.ico"));

    QMenu *contextMenu = new QMenu();
    QAction *quit = new QAction("Quit", this);
    workAction = new QAction(QIcon(":/icons/icon_tomato_red.ico"), "Pomodoro", this);
    stopAction = new QAction(QIcon(":/icons/stop.svg"), "Stop", this);
    shortBreakAction = new QAction(QIcon(":/icons/icon_tomato_green.ico"), "Short break", this);
    bigBreakAction = new QAction(QIcon(":/icons/icon_tomato_blue.ico"), "Big break", this);
    timerAction = new QAction("00:00", this);
    pomodoroCountAction = new QAction("No Pomodoros", this);
    resetPomodorosAction = new QAction("Reset count", this);
    aboutAction = new QAction("About qmodoro", this);
    preferencesAction = new QAction("Preferences...", this);

    connect(workAction, &QAction::triggered, this, &SysTrayIcon::onWork);
    connect(stopAction, &QAction::triggered, this, &SysTrayIcon::onStop);
    connect(shortBreakAction, &QAction::triggered, this, &SysTrayIcon::onShortBreak);
    connect(bigBreakAction, &QAction::triggered, this, &SysTrayIcon::onBigBreak);
    connect(resetPomodorosAction, &QAction::triggered, this, &SysTrayIcon::onResetCount);
    connect(aboutAction, &QAction::triggered, this, &SysTrayIcon::onAbout);
    connect(preferencesAction, &QAction::triggered, this, &SysTrayIcon::onNotImplementedYet);

    timerAction->setEnabled(false);
    pomodoroCountAction->setEnabled(false);
    stopAction->setEnabled(false);

    connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    contextMenu->addAction(timerAction);
    contextMenu->addAction(stopAction);
    contextMenu->addSeparator();
    contextMenu->addAction(pomodoroCountAction);
    contextMenu->addAction(resetPomodorosAction);
    contextMenu->addSeparator();
    contextMenu->addAction(workAction);
    contextMenu->addAction(shortBreakAction);
    contextMenu->addAction(bigBreakAction);
    contextMenu->addSeparator();
    contextMenu->addAction(aboutAction);
    contextMenu->addAction(preferencesAction);
    contextMenu->addSeparator();
    contextMenu->addAction(quit);

    trayIcon->setContextMenu(contextMenu);
}

void SysTrayIcon::show()
{
    trayIcon->show();
}

void SysTrayIcon::onWork()
{
    qDebug() << "workAction\n";
    trayIcon->setIcon(QIcon(":/icons/icon_tomato_red.ico"));
    // TODO start timer with 25 minutes
    currentPeriod = WORK;
    current = 0;
    timer->start(1000);
    stopAction->setEnabled(true);
}

void SysTrayIcon::onStop()
{
    qDebug() << "stopAction\n";
    trayIcon->setIcon(QIcon(":/icons/icon_tomato_black.ico"));
    currentPeriod = IDLE;
    current = 0;
    timer->stop();
    timerAction->setText(QStringLiteral("00:00"));
    stopAction->setEnabled(false);
}

void SysTrayIcon::onShortBreak()
{
    qDebug() << "shortBreakAction\n";
    trayIcon->setIcon(QIcon(":/icons/icon_tomato_green.ico"));
    // TODO start timer with 5 minutes
    currentPeriod = SHORT_BREAK;
    current = 0;
    timer->start(1000);
    stopAction->setEnabled(true);
}

void SysTrayIcon::onBigBreak()
{
    qDebug() << "bigBreakAction\n";
    trayIcon->setIcon(QIcon(":/icons/icon_tomato_blue.ico"));
    // TODO start timer with 30 minutes
    currentPeriod = BIG_BREAK;
    current = 0;
    timer->start(1000);
    stopAction->setEnabled(true);
}

void SysTrayIcon::onResetCount()
{
    qDebug() << "reset pomodoro count\n";
    pomodoroCountAction->setText("No Pomodoros");
    pomodoros = 0;
}

void SysTrayIcon::onAbout()
{
    QMessageBox::about(nullptr, "About qmodoro", "qmodoro is a pomodoro-style timebox timer app, aimed for productivity.");
}

void SysTrayIcon::onNotImplementedYet()
{
    QMessageBox::information(nullptr, "Not implemented", "Feature not implmemented yet, but it's planned");
}

void SysTrayIcon::onTimerTimeout()
{
    ++current;
    if (currentPeriod == WORK) {
        if (current >= workTimeInSeconds) {
            // Work period is done
            ++pomodoros;
            pomodoroCountAction->setText(QString("%1 pomodoros").arg(pomodoros));
            //Enter IDLE state
            timer->stop();
            trayIcon->setIcon(QIcon(":/icons/icon_tomato_black.ico"));
            currentPeriod = IDLE;
            current = 0;
            timerAction->setText(QStringLiteral("00:00"));
            stopAction->setEnabled(false);
        } else {
            const int timeRemaining = workTimeInSeconds - current;
            const QString time("%1:%2");
            const int minutes = timeRemaining / 60;
            const int seconds = timeRemaining % 60;
            timerAction->setText(time.arg(minutes).arg(seconds));
        }
    }
    if (currentPeriod == SHORT_BREAK) {
        if (current >= shortBreakInSeconds) {
            // Enter IDLE state
            timer->stop();
            trayIcon->setIcon(QIcon(":/icons/icon_tomato_black.ico"));
            currentPeriod = IDLE;
            current = 0;
            timerAction->setText(QStringLiteral("00:00"));
            stopAction->setEnabled(false);
        } else {
            const int timeRemaining = shortBreakInSeconds - current;
            const QString time("%1:%2");
            const int minutes = timeRemaining / 60;
            const int seconds = timeRemaining % 60;
            timerAction->setText(time.arg(minutes).arg(seconds));
        }
    }
    if (currentPeriod == BIG_BREAK) {
        if (current >= bigBreakInSeconds) {
            // Enter IDLE state
            timer->stop();
            trayIcon->setIcon(QIcon(":/icons/icon_tomato_black.ico"));
            currentPeriod = IDLE;
            current = 0;
            timerAction->setText(QStringLiteral("00:00"));
            stopAction->setEnabled(false);
        } else {
            const int timeRemaining = bigBreakInSeconds - current;
            const QString time("%1:%2");
            const int minutes = timeRemaining / 60;
            const int seconds = timeRemaining % 60;
            timerAction->setText(time.arg(minutes).arg(seconds));
        }
    }
}
