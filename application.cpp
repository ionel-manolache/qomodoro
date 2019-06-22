#include "application.h"
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>

#include <QMediaPlayer>

#include <QFile>

#include "settings.h"
#include "preferencesdialog.h"
#include "statemachine.h"

#include <QDebug>

static const QString timeString("%1:%2");

static QString getTimeString(int minutes, int seconds)
{
    return timeString.arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

Application::Application()
    : workTimeInSeconds(0),
      shortBreakInSeconds(0),
      longBreakInSeconds(0),
      currentTimeInSeconds(0),
      workPeriods(0),
      trayIcon(new QSystemTrayIcon(this)),
      timer(new QTimer(this)),
      prefDialog(new PreferencesDialog),
      stateMachine(new StateMachine(this))
{
    settings = new Settings(this);

    playStart = new QMediaPlayer(this);
    playStart->setVolume(90);
    playStart->setMedia(QUrl("qrc:/sounds/timer_start.mp3"));
    connect(playStart,
            static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this,
            &Application::onMediaPlayerError);

    playEnd = new QMediaPlayer(this);
    playEnd->setVolume(90);
    playEnd->setMedia(QUrl("qrc:/sounds/timer_end.mp3"));
    connect(playEnd, static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
            this, &Application::onMediaPlayerError);

    playTickTock = new QMediaPlayer(this);
    playTickTock->setVolume(90);
    playTickTock->setMedia(QUrl("qrc:/sounds/timer_tick.mp3"));
    connect(playTickTock,
            static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this,
            &Application::onMediaPlayerError);

    workTimeInSeconds = settings->workTime() * 60;
    shortBreakInSeconds = settings->shortBreakTime() * 60;
    longBreakInSeconds = settings->longBreakTime() * 60;

    connect(timer, &QTimer::timeout, this, &Application::onTimerTimeout);

    idleIcon = QIcon(":/icons/icon_tomato_red.png");
    trayIcon->setIcon(idleIcon);

    QMenu *contextMenu = new QMenu();
    QAction *quit = new QAction(tr("Quit"), this);
    quit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

    _workAction = new QAction(QIcon(":/icons/icon_tomato_blue.png"), tr("Work"), this);
    _stopAction = new QAction(QIcon(":/icons/stop.svg"), tr("Stop"), this);
    _shortBreakAction =
            new QAction(QIcon(":/icons/icon_tomato_green.png"), tr("Short break"), this);
    _longBreakAction = new QAction(QIcon(":/icons/icon_tomato_yellow.png"), tr("Long break"), this);
    timerAction = new QAction(QIcon(":/icons/clock.svg"), getTimeString(0, 0), this);
    workPeriodsCountAction = new QAction(tr("No work periods"), this);
    resetWorkPeriodsAction = new QAction(tr("Reset count"), this);
    aboutAction = new QAction(tr("About qmodoro"), this);
    preferencesAction = new QAction(tr("Preferences..."), this);

    connect(resetWorkPeriodsAction, &QAction::triggered, this, &Application::onResetCount);
    connect(aboutAction, &QAction::triggered, this, &Application::onAbout);
    connect(preferencesAction, &QAction::triggered, this, &Application::onPreferences);

    prefDialog->setSettings(settings);
    connect(prefDialog, &PreferencesDialog::accepted, this, &Application::onPreferencesSaved);

    timerAction->setEnabled(false);
    workPeriodsCountAction->setEnabled(false);
    _stopAction->setEnabled(false);
    resetWorkPeriodsAction->setEnabled(false);

    connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    contextMenu->addAction(timerAction);
    contextMenu->addAction(_stopAction);
    contextMenu->addSeparator();
    contextMenu->addAction(workPeriodsCountAction);
    contextMenu->addAction(resetWorkPeriodsAction);
    contextMenu->addSeparator();
    contextMenu->addAction(_workAction);
    contextMenu->addAction(_shortBreakAction);
    contextMenu->addAction(_longBreakAction);
    contextMenu->addSeparator();
    contextMenu->addAction(aboutAction);
    contextMenu->addAction(preferencesAction);
    contextMenu->addSeparator();
    contextMenu->addAction(quit);

    trayIcon->setContextMenu(contextMenu);

    connect(stateMachine, &StateMachine::idleStateIn, this, &Application::onIdleStateEntered);
    connect(stateMachine, &StateMachine::workStateIn, this, &Application::onWorkStateEntered);
    connect(stateMachine, &StateMachine::shortBreakStateIn, this,
            &Application::onShortBreakStateEntered);
    connect(stateMachine, &StateMachine::longBreakStateIn, this,
            &Application::onLongBreakStateEntered);

    connect(stateMachine, &StateMachine::workStateOut, this, &Application::onWorkStateExited);
    connect(stateMachine, &StateMachine::shortBreakStateOut, this,
            &Application::onShortBreakStateExited);
    connect(stateMachine, &StateMachine::longBreakStateOut, this,
            &Application::onLongBreakStateExited);

    stateMachine->setUpActions();
    stateMachine->start();
}

void Application::show()
{
    trayIcon->show();
}

void Application::onIdleStateEntered()
{
    playStart->stop();
    playTickTock->stop();

    trayIcon->setIcon(idleIcon);
    currentTimeInSeconds = 0;
    timer->stop();
    _stopAction->setEnabled(false);

    timerAction->setText(getTimeString(0, 0));
}

void Application::playTimerStartSound()
{
    if (playStart->state() == QMediaPlayer::PlayingState)
        playStart->stop();

    if (settings->soundOnTimerStart())
        playStart->play();
}

void Application::playTimerEndSound()
{
    if (playEnd->state() == QMediaPlayer::PlayingState)
        playEnd->stop();

    if (settings->soundOnTimerEnd())
        playEnd->play();
}

void Application::playTickTockSound(bool work)
{
    if (playTickTock->state() == QMediaPlayer::PlayingState)
        playTickTock->stop();

    if (work && settings->tickTockDuringWork())
        playTickTock->play();

    if (!work && settings->tickTockDuringBreak())
        playTickTock->play();
}

void Application::onWorkStateEntered()
{
    playTimerStartSound();

    stateChanged(_workAction->icon());

    playTickTockSound(true);
}

void Application::onShortBreakStateEntered()
{
    playTimerStartSound();

    stateChanged(_shortBreakAction->icon());

    playTickTockSound(false);
}

void Application::onLongBreakStateEntered()
{
    playTimerStartSound();

    stateChanged(_longBreakAction->icon());

    playTickTockSound(false);
}

void Application::onWorkStateExited()
{
    if (currentTimeInSeconds >= workTimeInSeconds) {
        ++workPeriods;
        workPeriodsCountAction->setText(tr("%1 work periods").arg(workPeriods));
        resetWorkPeriodsAction->setEnabled(true);
        playTimerEndSound();
    }
}

void Application::onShortBreakStateExited()
{
    if (currentTimeInSeconds >= shortBreakInSeconds)
        playTimerEndSound();
}

void Application::onLongBreakStateExited()
{
    if (currentTimeInSeconds >= longBreakInSeconds)
        playTimerEndSound();
}

void Application::stateChanged(QIcon icon)
{
    trayIcon->setIcon(icon);
    currentTimeInSeconds = 0;
    timer->start(1000);
    _stopAction->setEnabled(true);
}

void Application::onMediaPlayerError(QMediaPlayer::Error error)
{
    QMediaPlayer *player = qobject_cast<QMediaPlayer *>(QObject::sender());
    qDebug() << player->currentMedia().canonicalUrl().toString();

    qDebug() << "Application::onMediaPlayerError(...) : ";
    switch (error) {
    case QMediaPlayer::NoError:
        qDebug() << "No error";
        break;
    case QMediaPlayer::ResourceError:
        qDebug() << "Resource Error";
        break;
    case QMediaPlayer::FormatError:
        qDebug() << "Format Error";
        break;
    case QMediaPlayer::NetworkError:
        qDebug() << "Network Error";
        break;
    case QMediaPlayer::AccessDeniedError:
        qDebug() << "AccessDenied Error";
        break;
    case QMediaPlayer::ServiceMissingError:
        qDebug() << "ServiceMissing Error";
        break;
    case QMediaPlayer::MediaIsPlaylist:
        qDebug() << "MediaIsPlaylist Error";
        break;
    }
}

void Application::onResetCount()
{
    workPeriodsCountAction->setText(tr("No work periods"));
    workPeriods = 0;
    resetWorkPeriodsAction->setEnabled(false);
}

void Application::onAbout()
{
    QMessageBox::about(
            nullptr, tr("About %1").arg(qApp->applicationName()),
            tr("%1 is a timebox timer app, aimed for productivity.").arg(qApp->applicationName()));
}

void Application::onPreferences()
{
    prefDialog->loadSettings();
    prefDialog->show();
}

void Application::onPreferencesSaved()
{
    workTimeInSeconds = settings->workTime() * 60;
    shortBreakInSeconds = settings->shortBreakTime() * 60;
    longBreakInSeconds = settings->longBreakTime() * 60;
}

void Application::onTimerTimeout()
{
    if (stateMachine->isEmpty())
        return;

    ++currentTimeInSeconds;

    int timeInSeconds = 0;
    if (stateMachine->isWorkState())
        timeInSeconds = workTimeInSeconds;
    else if (stateMachine->isShortBreakState())
        timeInSeconds = shortBreakInSeconds;
    else if (stateMachine->isLongBreakState())
        timeInSeconds = longBreakInSeconds;

    if (timeInSeconds > 0) {
        if (currentTimeInSeconds >= timeInSeconds) {
            emit timeout();
        } else {
            const int timeRemaining = timeInSeconds - currentTimeInSeconds;
            const int minutes = timeRemaining / 60;
            const int seconds = timeRemaining % 60;
            timerAction->setText(getTimeString(minutes, seconds));
        }
    }
}
