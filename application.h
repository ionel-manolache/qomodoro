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
    void setupActions();
    void setupContextMenu();

    void startStateMachine();

    void loadSettings();
    void saveSettings();

    void stateChanged();

    void playTimerStartSound();
    void playTimerEndSound();
    void playWorkTickTockSound();
    void playBreakTickTockSound();

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

    PreferencesDialog *prefDialog;

    Settings *settings;

    QIcon idleIcon;

    StateMachine *stateMachine;

    MediaPlayer *mediaPlayer;
};

#endif // SYSTRAYICON_H
