#ifndef SYSTRAYICON_H
#define SYSTRAYICON_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMediaPlayer>

class QStateMachine;
class QState;
class QTimer;
class QAction;
class Settings;
class PreferencesDialog;

class Application : public QObject
{
    Q_OBJECT
public:
    Application();

    void show();

    enum Period { IDLE = 0, WORK, SHORT_BREAK, LONG_BREAK };
signals:
    void timeout();

private slots:
    void onAbout();
    void onTimerTimeout();
    void onResetCount();
    void onPreferences();
    void onPreferencesSaved();

    void onIdleStateEntered();
    void onWorkStateEntered();
    void onShortBreakStateEntered();
    void onLongBreakStateEntered();
    void onWorkStateExited();
    void onShortBreakStateExited();
    void onLongBreakStateExited();

    void onMediaPlayerError(QMediaPlayer::Error error);

private:
    void loadSettings();
    void saveSettings();

    void stateChanged(QIcon icon);

    void playTimerStartSound();
    void playTimerEndSound();
    void playTickTockSound(bool work = false);

private:
    int workTimeInSeconds;
    int shortBreakInSeconds;
    int longBreakInSeconds;
    Period currentPeriod;
    int currentTimeInSeconds;

    int workPeriods;

    QSystemTrayIcon *trayIcon;
    QTimer *timer;

    QAction *workAction;
    QAction *stopAction;
    QAction *shortBreakAction;
    QAction *longBreakAction;
    QAction *timerAction;
    QAction *workPeriodsCountAction;
    QAction *resetWorkPeriodsAction;
    QAction *aboutAction;
    QAction *preferencesAction;

    QStateMachine *machine;
    QState *idleState;
    QState *workState;
    QState *shortBreakState;
    QState *longBreakState;
    QState *currentState;

    PreferencesDialog *prefDialog;

    Settings *settings;

    QIcon idleIcon;

    QMediaPlayer *playStart;
    QMediaPlayer *playEnd;
    QMediaPlayer *playTickTock;
};

#endif // SYSTRAYICON_H
