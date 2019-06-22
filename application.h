#ifndef SYSTRAYICON_H
#define SYSTRAYICON_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMediaPlayer>

class QTimer;
class QAction;

class Settings;
class PreferencesDialog;
class StateMachine;
class MediaPlayer;

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject *parent = nullptr);

    void show();

    enum Period { IDLE = 0, WORK, SHORT_BREAK, LONG_BREAK };

    QAction *workAction() { return _workAction; }
    QAction *stopAction() { return _stopAction; }
    QAction *shortBreakAction() { return _shortBreakAction; }
    QAction *longBreakAction() { return _longBreakAction; }

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

    QAction *_workAction;
    QAction *_stopAction;
    QAction *_shortBreakAction;
    QAction *_longBreakAction;
    QAction *timerAction;
    QAction *workPeriodsCountAction;
    QAction *resetWorkPeriodsAction;
    QAction *aboutAction;
    QAction *preferencesAction;

    PreferencesDialog *prefDialog;

    Settings *settings;

    QIcon idleIcon;

    StateMachine *stateMachine;

    MediaPlayer *mediaPlayer;
};

#endif // SYSTRAYICON_H
