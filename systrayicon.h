#ifndef SYSTRAYICON_H
#define SYSTRAYICON_H

#include <QObject>
#include <QSystemTrayIcon>

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
        BIG_BREAK
    };

private slots:
    void onWork();
    void onStop();
    void onShortBreak();
    void onBigBreak();
    void onTimerTimeout();
    void onResetCount();
    void onAbout();

    void onNotImplementedYet();

private:
    int workTimeInSeconds;
    int shortBreakInSeconds;
    int bigBreakInSeconds;
    Period currentPeriod;
    int current;

    int pomodoros;

    QSystemTrayIcon *trayIcon;
    QTimer *timer;

    QAction *workAction;
    QAction *stopAction;
    QAction *shortBreakAction;
    QAction *bigBreakAction;
    QAction *timerAction;
    QAction *pomodoroCountAction;
    QAction *resetPomodorosAction;
    QAction *aboutAction;
    QAction *preferencesAction;
};

#endif // SYSTRAYICON_H
