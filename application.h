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

    void setupStateMachine();
    void startStateMachine();

    void loadSettings();
    void saveSettings();

    void stateChanged();

    void playTimerStartSound();
    void playTimerEndSound();
    void playWorkTickTockSound();
    void playBreakTickTockSound();

private:
    int m_workTimeInSeconds;
    int m_shortBreakInSeconds;
    int m_longBreakInSeconds;
    int m_currentTimeInSeconds;

    int m_workPeriods;

    QSystemTrayIcon *m_trayIcon;
    QTimer *m_timer;

    QAction *m_workAction;
    QAction *m_stopAction;
    QAction *m_shortBreakAction;
    QAction *m_longBreakAction;
    QAction *m_timerAction;
    QAction *m_workPeriodsCountAction;
    QAction *m_resetWorkPeriodsAction;
    QAction *m_aboutAction;
    QAction *m_preferencesAction;

    PreferencesDialog *m_prefDialog;

    Settings *m_settings;

    QIcon m_idleIcon;

    StateMachine *m_stateMachine;

    MediaPlayer *m_mediaPlayer;
};

#endif // SYSTRAYICON_H
