#ifndef SYSTRAYICON_H
#define SYSTRAYICON_H

#include <QObject>
#include <QSystemTrayIcon>

class QStateMachine;
class QState;
class QTimer;
class QAction;

class SysTrayIcon : public QObject
{
    Q_OBJECT
public:
    SysTrayIcon();

    void show();

    enum Period{
        IDLE = 0,
        WORK,
        SHORT_BREAK,
        LONG_BREAK
    };
signals:
    void timeout();

private slots:
    /*
    void onWork();
    void onStop();
    void onShortBreak();
    void onLongBreak();
    */
    void onTimerTimeout();
    void onResetCount();
    void onAbout();

    void onIdleStateEntered();
    void onWorkStateEntered();
    void onShortBreakStateEntered();
    void onLongBreakStateEntered();
    void onWorkStateExited();

    void onNotImplementedYet();

private:
    int workTimeInSeconds;
    int shortBreakInSeconds;
    int longBreakInSeconds;
    Period currentPeriod;
    int currentTimeInSeconds;

    int pomodoros;

    QSystemTrayIcon *trayIcon;
    QTimer *timer;

    QAction *workAction;
    QAction *stopAction;
    QAction *shortBreakAction;
    QAction *longBreakAction;
    QAction *timerAction;
    QAction *pomodoroCountAction;
    QAction *resetPomodorosAction;
    QAction *aboutAction;
    QAction *preferencesAction;

    QStateMachine *machine;
    QState *idleState;
    QState *workState;
    QState *shortBreakState;
    QState *longBreakState;
    QState *currentState;
};

#endif // SYSTRAYICON_H
