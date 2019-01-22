#include "settings.h"

#include <QSettings>

static const QString shortBreakString = QStringLiteral("shortBreakTime");
static const QString longBreakString = QStringLiteral("longBreakTime");
static const QString workString = QStringLiteral("workTime");
static const QString autoStartString = QStringLiteral("autoStartTimer");
static const QString soundOnTimerStartString = QStringLiteral("soundOnTimerStart");
static const QString soundOnTimerEndString = QStringLiteral("soundOnTimerEnd");
static const QString notificationOnTimerEndString = QStringLiteral("notificationOnTimerEnd");
static const QString tickTockDuringWorkString = QStringLiteral("tickTockDuringWork");
static const QString tickTockDuringBreakString = QStringLiteral("tickTockDuringBreak");

#define DEFAULT_WORK 25
#define DEFAULT_BREAK 5
#define DEFAULT_BIG_BREAK 30

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    m_settings = new QSettings("com.qomodoro", "qomodoro", parent);
}

Settings::~Settings() { }

int Settings::shortBreakTime()
{
    return m_settings->value(shortBreakString, DEFAULT_BREAK).toInt();
}

void Settings::setShortBreakTime(int value)
{
    m_settings->setValue(shortBreakString, value);
}

int Settings::longBreakTime()
{
    return m_settings->value(longBreakString, DEFAULT_BIG_BREAK).toInt();
}

void Settings::setLongBreakTime(int value)
{
    m_settings->setValue(longBreakString, value);
}

int Settings::workTime()
{
    return m_settings->value(workString, DEFAULT_WORK).toInt();
}

void Settings::setWorkTime(int value)
{
    m_settings->setValue(workString, value);
}

bool Settings::autoStartNextTimer()
{
    return m_settings->value(autoStartString, false).toBool();
}

void Settings::setAutoStartNextTimer(bool autostart)
{
    m_settings->setValue(autoStartString, autostart);
}

bool Settings::soundOnTimerStart()
{
    return m_settings->value(soundOnTimerStartString, false).toBool();
}

void Settings::setSoundOnTimerStart(bool on)
{
    m_settings->setValue(soundOnTimerStartString, on);
}

bool Settings::soundOnTimerEnd()
{
    return m_settings->value(soundOnTimerEndString, false).toBool();
}

void Settings::setSoundOnTimerEnd(bool on)
{
    m_settings->setValue(soundOnTimerEndString, on);
}

bool Settings::notificationOnTimerEnd()
{
    return m_settings->value(notificationOnTimerEndString, false).toBool();
}

void Settings::setNotificationOnTimerEnd(bool on)
{
    m_settings->setValue(notificationOnTimerEndString, on);
}

bool Settings::tickTockDuringWork()
{
    return m_settings->value(tickTockDuringWorkString, false).toBool();
}

void Settings::setTickTockDuringWork(bool on)
{
    m_settings->setValue(tickTockDuringWorkString, on);
}

bool Settings::tickTockDuringBreak()
{
    return m_settings->value(tickTockDuringBreakString, false).toBool();
}

void Settings::setTickTockDuringBreak(bool on)
{
    m_settings->setValue(tickTockDuringBreakString, on);
}
