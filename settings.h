#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class QSettings;

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    int shortBreakTime();
    void setShortBreakTime(int value);
    int longBreakTime();
    void setLongBreakTime(int value);
    int workTime();
    void setWorkTime(int value);
    bool autoStartNextTimer();
    void setAutoStartNextTimer(bool autostart);
    bool soundOnTimerStart();
    void setSoundOnTimerStart(bool on);
    bool soundOnTimerEnd();
    void setSoundOnTimerEnd(bool on);
    bool notificationOnTimerEnd();
    void setNotificationOnTimerEnd(bool on);
    bool tickTockDuringWork();
    void setTickTockDuringWork(bool on);
    bool tickTockDuringBreak();
    void setTickTockDuringBreak(bool on);

private:
    QSettings *m_settings;
};

#endif // SETTINGS_H
