#include "application.h"
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>

#include <QMediaPlayer>

#include <QFile>

#include "mediaplayer.h"
#include "preferencesdialog.h"
#include "settings.h"
#include "statemachine.h"

#include <QDebug>

static const QString timeString("%1:%2");

static QString getTimeString(int minutes, int seconds)
{
    return timeString.arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

Application::Application(QObject *parent)
    : QObject(parent),
      m_workTimeInSeconds(0),
      m_shortBreakInSeconds(0),
      m_longBreakInSeconds(0),
      m_currentTimeInSeconds(0),
      m_workPeriods(0),
      m_trayIcon(new QSystemTrayIcon(this)),
      m_timer(new QTimer(this)),
      m_prefDialog(new PreferencesDialog),
      m_stateMachine(new StateMachine(this)),
      m_mediaPlayer(new MediaPlayer(this))
{
    m_settings = new Settings(this);

    m_workTimeInSeconds = m_settings->workTime() * 60;
    m_shortBreakInSeconds = m_settings->shortBreakTime() * 60;
    m_longBreakInSeconds = m_settings->longBreakTime() * 60;

    connect(m_timer, &QTimer::timeout, this, &Application::onTimerTimeout);

    m_prefDialog->setSettings(m_settings);
    connect(m_prefDialog, &PreferencesDialog::accepted, this, &Application::onPreferencesSaved);

    m_idleIcon = QIcon(":/icons/icon_tomato_red.png");
    m_trayIcon->setIcon(m_idleIcon);

    setupActions();

    setupContextMenu();

    setupStateMachine();
    startStateMachine();
}

QString formatTime(int duration)
{
    return QString("%1m").arg(duration);
}

void Application::setupActions()
{
    m_workAction = new QAction(QIcon(":/icons/icon_tomato_blue.png"), tr("Work (%1)").arg(formatTime(m_settings->workTime())), this);
    m_stopAction = new QAction(QIcon(":/icons/stop.svg"), tr("Stop"), this);
    m_shortBreakAction = new QAction(QIcon(":/icons/icon_tomato_green.png"), tr("Short break (%1)").arg(formatTime(m_settings->shortBreakTime())), this);
    m_longBreakAction = new QAction(QIcon(":/icons/icon_tomato_yellow.png"), tr("Long break (%1)").arg(formatTime(m_settings->longBreakTime())), this);
    m_timerAction = new QAction(QIcon(":/icons/clock.svg"), getTimeString(0, 0), this);
    m_workPeriodsCountAction = new QAction(tr("No work periods"), this);
    m_resetWorkPeriodsAction = new QAction(tr("Reset count"), this);
    m_aboutAction = new QAction(tr("About qmodoro"), this);
    m_preferencesAction = new QAction(tr("Preferences..."), this);

    connect(m_resetWorkPeriodsAction, &QAction::triggered, this, &Application::onResetCount);
    connect(m_aboutAction, &QAction::triggered, this, &Application::onAbout);
    connect(m_preferencesAction, &QAction::triggered, this, &Application::onPreferences);

    m_timerAction->setEnabled(false);
    m_workPeriodsCountAction->setEnabled(false);
    m_stopAction->setEnabled(false);
    m_resetWorkPeriodsAction->setEnabled(false);
}

void Application::setupContextMenu()
{
    QAction *quit = new QAction(tr("Quit"), this);
    quit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);

    QMenu *contextMenu = new QMenu();
    contextMenu->addAction(m_timerAction);
    contextMenu->addAction(m_stopAction);
    contextMenu->addSeparator();
    contextMenu->addAction(m_workPeriodsCountAction);
    contextMenu->addAction(m_resetWorkPeriodsAction);
    contextMenu->addSeparator();
    contextMenu->addAction(m_workAction);
    contextMenu->addAction(m_shortBreakAction);
    contextMenu->addAction(m_longBreakAction);
    contextMenu->addSeparator();
    contextMenu->addAction(m_aboutAction);
    contextMenu->addAction(m_preferencesAction);
    contextMenu->addSeparator();
    contextMenu->addAction(quit);

    m_trayIcon->setContextMenu(contextMenu);
}

void Application::setupStateMachine()
{
    connect(m_stateMachine, &StateMachine::idleStateEntered, this, &Application::onIdleStateEntered);
    connect(m_stateMachine, &StateMachine::workStateEntered, this, &Application::onWorkStateEntered);
    connect(m_stateMachine, &StateMachine::shortBreakStateEntered, this,
            &Application::onShortBreakStateEntered);
    connect(m_stateMachine, &StateMachine::longBreakStateEntered, this,
            &Application::onLongBreakStateEntered);

    connect(m_stateMachine, &StateMachine::workStateExited, this, &Application::onWorkStateExited);
    connect(m_stateMachine, &StateMachine::shortBreakStateExited, this,
            &Application::onShortBreakStateExited);
    connect(m_stateMachine, &StateMachine::longBreakStateExited, this,
            &Application::onLongBreakStateExited);
}

void Application::startStateMachine()
{
    m_stateMachine->setupTransitions();

    connect(m_workAction, &QAction::triggered, m_stateMachine, &StateMachine::workAction);
    connect(m_stopAction, &QAction::triggered, m_stateMachine, &StateMachine::stopAction);
    connect(m_shortBreakAction, &QAction::triggered, m_stateMachine, &StateMachine::shortBreakAction);
    connect(m_longBreakAction, &QAction::triggered, m_stateMachine, &StateMachine::longBreakAction);
    connect(this, &Application::timeout, m_stateMachine, &StateMachine::timeoutAction);

    m_stateMachine->start();
}

void Application::show()
{
    m_trayIcon->show();
}

void Application::onIdleStateEntered()
{
    m_mediaPlayer->stop();

    m_trayIcon->setIcon(m_idleIcon);
    m_currentTimeInSeconds = 0;
    m_timer->stop();
    m_stopAction->setEnabled(false);

    m_timerAction->setText(getTimeString(0, 0));
}

void Application::playTimerStartSound()
{
    if (m_settings->soundOnTimerStart())
        m_mediaPlayer->playStart();
}

void Application::playTimerEndSound()
{
    if (m_settings->soundOnTimerEnd())
        m_mediaPlayer->playEnd();
}

void Application::playWorkTickTockSound()
{
    if (m_settings->tickTockDuringWork())
        m_mediaPlayer->playTickTock();
}

void Application::playBreakTickTockSound()
{
    if (m_settings->tickTockDuringBreak())
        m_mediaPlayer->playTickTock();
}

void Application::onWorkStateEntered()
{
    m_trayIcon->setIcon(m_workAction->icon());
    stateChanged();

    playTimerStartSound();

    playWorkTickTockSound();
}

void Application::onShortBreakStateEntered()
{
    m_trayIcon->setIcon(m_shortBreakAction->icon());
    stateChanged();

    playTimerStartSound();

    playBreakTickTockSound();
}

void Application::onLongBreakStateEntered()
{
    m_trayIcon->setIcon(m_longBreakAction->icon());
    stateChanged();

    playTimerStartSound();

    playBreakTickTockSound();
}

void Application::onWorkStateExited()
{
    if (m_currentTimeInSeconds >= m_workTimeInSeconds) {
        ++m_workPeriods;
        m_workPeriodsCountAction->setText(tr("%1 work periods").arg(m_workPeriods));
        m_resetWorkPeriodsAction->setEnabled(true);
        playTimerEndSound();
    }
}

void Application::onShortBreakStateExited()
{
    if (m_currentTimeInSeconds >= m_shortBreakInSeconds)
        playTimerEndSound();
}

void Application::onLongBreakStateExited()
{
    if (m_currentTimeInSeconds >= m_longBreakInSeconds)
        playTimerEndSound();
}

void Application::stateChanged()
{
    m_currentTimeInSeconds = 0;
    m_timer->start(1000);
    m_stopAction->setEnabled(true);
}

void Application::onResetCount()
{
    m_workPeriodsCountAction->setText(tr("No work periods"));
    m_workPeriods = 0;
    m_resetWorkPeriodsAction->setEnabled(false);
}

void Application::onAbout()
{
    QMessageBox::about(
            nullptr, tr("About %1").arg(qApp->applicationName()),
            tr("%1 is a timebox timer app, aimed for productivity.").arg(qApp->applicationName()));
}

void Application::onPreferences()
{
    m_prefDialog->loadSettings();
    m_prefDialog->show();
}

void Application::onPreferencesSaved()
{
    m_workTimeInSeconds = m_settings->workTime() * 60;
    m_shortBreakInSeconds = m_settings->shortBreakTime() * 60;
    m_longBreakInSeconds = m_settings->longBreakTime() * 60;

    m_workAction->setText(tr("Work (%1)").arg(formatTime(m_settings->workTime())));
    m_shortBreakAction->setText(tr("Short break (%1)").arg(formatTime(m_settings->shortBreakTime())));
    m_longBreakAction->setText(tr("Long break (%1)").arg(formatTime(m_settings->longBreakTime())));
}

void Application::onTimerTimeout()
{
    if (m_stateMachine->isEmpty())
        return;

    ++m_currentTimeInSeconds;

    int timeInSeconds = 0;
    if (m_stateMachine->isWorkState())
    {
        timeInSeconds = m_workTimeInSeconds;
    }
    else if (m_stateMachine->isShortBreakState())
    {
        timeInSeconds = m_shortBreakInSeconds;
    }
    else if (m_stateMachine->isLongBreakState())
    {
        timeInSeconds = m_longBreakInSeconds;
    }

    if (timeInSeconds > 0) {
        if (m_currentTimeInSeconds >= timeInSeconds) {
            emit timeout();
        } else {
            const int timeRemaining = timeInSeconds - m_currentTimeInSeconds;
            const int minutes = timeRemaining / 60;
            const int seconds = timeRemaining % 60;
            m_timerAction->setText(getTimeString(minutes, seconds));
        }
    }
}
