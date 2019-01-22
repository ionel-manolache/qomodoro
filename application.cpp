#include "application.h"
#include <QAction>
#include <QApplication>
#include <QMenu>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>
#include <QStateMachine>

#include <QMediaPlayer>

#include <QFile>

#include "settings.h"
#include "preferencesdialog.h"

#include <QDebug>


static const QString autoStartString = QStringLiteral("autoStartTimer");
static const QString soundOnTimerStartString = QStringLiteral("soundOnTimerStart");
static const QString soundOnTimerEndString = QStringLiteral("soundOnTimerEnd");
static const QString notificationOnTimerEndString = QStringLiteral("notificationOnTimerEnd");
static const QString tickTockDuringWorkString = QStringLiteral("tickTockDuringWork");
static const QString tickTockDuringBreakString = QStringLiteral("tickTockDuringBreak");

static const QString timeString = QStringLiteral("%1:%2");


#define DEFAULT_WORK 25
#define DEFAULT_BREAK 5
#define DEFAULT_BIG_BREAK 30

static QString getTimeString(int minutes, int seconds)
{
    return timeString.arg(minutes, 2, 10, QChar('0'))
                     .arg(seconds, 2, 10, QChar('0'));
}


Application::Application()
    : workTimeInSeconds(0)
    , shortBreakInSeconds(0)
    , longBreakInSeconds(0)
    , currentTimeInSeconds(0)
    , pomodoros(0)
    , trayIcon(new QSystemTrayIcon(this))
    , timer(new QTimer(this))
    , machine(new QStateMachine(this))
    , idleState(new QState())
    , workState(new QState())
    , shortBreakState(new QState())
    , longBreakState(new QState())
    , prefDialog(new PreferencesDialog)
{
    settings = new Settings(this);

    playStart = new QMediaPlayer(this);
    playStart->setVolume(100);
    playStart->setMedia(QUrl::fromLocalFile("/home/ionel/git/qomodoro/sounds/timer_start.mp3"));
    connect(playStart, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Application::onMediaPlayerError);

    playEnd = new QMediaPlayer(this);
    playEnd->setVolume(100);
    playEnd->setMedia(QUrl::fromLocalFile("/home/ionel/git/qomodoro/sounds/timer_end.mp3"));
    connect(playEnd, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Application::onMediaPlayerError);

    playTickTock = new QMediaPlayer(this);
    playTickTock->setVolume(100);
    playTickTock->setMedia(QUrl::fromLocalFile("/home/ionel/git/qomodoro/sounds/timer_tick.mp3"));
    connect(playTickTock, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Application::onMediaPlayerError);

    // workTimeInSeconds = settings->workTime() * 60;
    // shortBreakInSeconds = settings->shortBreakTime() * 60;
    // longBreakInSeconds = settings->longBreakTime() * 60;

    //TEST VALUES:
    workTimeInSeconds = 10;
    shortBreakInSeconds = 5;
    longBreakInSeconds = 10;
    //---------------

    connect(timer, &QTimer::timeout, this, &Application::onTimerTimeout);

    idleIcon = QIcon(":/icons/icon_tomato_black.ico");
    trayIcon->setIcon(idleIcon);

    QMenu *contextMenu = new QMenu();
    QAction *quit = new QAction(tr("Quit"), this);
    quit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

    workAction = new QAction(QIcon(":/icons/icon_tomato_red.ico"), tr("Pomodoro"), this);
    stopAction = new QAction(QIcon(":/icons/stop.svg"), tr("Stop"), this);
    shortBreakAction = new QAction(QIcon(":/icons/icon_tomato_green.ico"), tr("Short break"), this);
    longBreakAction = new QAction(QIcon(":/icons/icon_tomato_blue.ico"), tr("Long break"), this);
    timerAction = new QAction(QIcon(":/icons/clock.svg"), getTimeString(0, 0), this);
    pomodoroCountAction = new QAction(tr("No Pomodoros"), this);
    resetPomodorosAction = new QAction(tr("Reset count"), this);
    aboutAction = new QAction(tr("About qmodoro"), this);
    preferencesAction = new QAction(tr("Preferences..."), this);

    connect(resetPomodorosAction, &QAction::triggered, this, &Application::onResetCount);
    connect(aboutAction, &QAction::triggered, this, &Application::onAbout);
    connect(preferencesAction, &QAction::triggered, this, &Application::onPreferences);

    prefDialog->setSettings(settings);
    connect(prefDialog, &PreferencesDialog::accepted, this, &Application::onPreferencesSaved);

    timerAction->setEnabled(false);
    pomodoroCountAction->setEnabled(false);
    stopAction->setEnabled(false);
    resetPomodorosAction->setEnabled(false);

    connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    contextMenu->addAction(timerAction);
    contextMenu->addAction(stopAction);
    contextMenu->addSeparator();
    contextMenu->addAction(pomodoroCountAction);
    contextMenu->addAction(resetPomodorosAction);
    contextMenu->addSeparator();
    contextMenu->addAction(workAction);
    contextMenu->addAction(shortBreakAction);
    contextMenu->addAction(longBreakAction);
    contextMenu->addSeparator();
    contextMenu->addAction(aboutAction);
    contextMenu->addAction(preferencesAction);
    contextMenu->addSeparator();
    contextMenu->addAction(quit);

    trayIcon->setContextMenu(contextMenu);

    connect(idleState, &QState::entered, this, &Application::onIdleStateEntered);
    connect(workState, &QState::entered, this, &Application::onWorkStateEntered);
    connect(shortBreakState, &QState::entered, this, &Application::onShortBreakStateEntered);
    connect(longBreakState, &QState::entered, this, &Application::onLongBreakStateEntered);

    connect(workState, &QState::exited, this, &Application::onWorkStateExited);
    connect(shortBreakState, &QState::exited, this, &Application::onShortBreakStateExited);
    connect(longBreakState, &QState::exited, this, &Application::onLongBreakStateExited);

    idleState->setObjectName("IdleState");
    idleState->addTransition(workAction, &QAction::triggered, workState);
    idleState->addTransition(shortBreakAction, &QAction::triggered, shortBreakState);
    idleState->addTransition(longBreakAction, &QAction::triggered, longBreakState);

    workState->setObjectName("WorkState");
    workState->addTransition(stopAction, &QAction::triggered, idleState);
    workState->addTransition(this, &Application::timeout, idleState);
    workState->addTransition(shortBreakAction, &QAction::triggered, shortBreakState);
    workState->addTransition(longBreakAction, &QAction::triggered, longBreakState);

    shortBreakState->setObjectName("ShortBreakState");
    shortBreakState->addTransition(stopAction, &QAction::triggered, idleState);
    shortBreakState->addTransition(this, &Application::timeout, idleState);
    shortBreakState->addTransition(workAction, &QAction::triggered, workState);
    shortBreakState->addTransition(longBreakAction, &QAction::triggered, longBreakState);

    longBreakState->setObjectName("LongBreakState");
    longBreakState->addTransition(stopAction, &QAction::triggered, idleState);
    longBreakState->addTransition(this, &Application::timeout, idleState);
    longBreakState->addTransition(workAction, &QAction::triggered, workState);
    longBreakState->addTransition(shortBreakAction, &QAction::triggered, shortBreakState);

    machine->addState(idleState);
    machine->addState(workState);
    machine->addState(shortBreakState);
    machine->addState(longBreakState);
    machine->setInitialState(idleState);
    machine->start();
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
    stopAction->setEnabled(false);

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

    stateChanged(workAction->icon());

    playTickTockSound(true);
}

void Application::onShortBreakStateEntered()
{
    playTimerStartSound();

    stateChanged(shortBreakAction->icon());

    playTickTockSound(false);
}

void Application::onLongBreakStateEntered()
{
    playTimerStartSound();

    stateChanged(longBreakAction->icon());

    playTickTockSound(false);
}

void Application::onWorkStateExited()
{
    if (currentTimeInSeconds >= workTimeInSeconds) {
        ++pomodoros;
        pomodoroCountAction->setText(tr("%1 pomodoros").arg(pomodoros));
        resetPomodorosAction->setEnabled(true);
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
    stopAction->setEnabled(true);
}

void Application::onMediaPlayerError(QMediaPlayer::Error error)
{
    switch(error) {
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
    pomodoroCountAction->setText(tr("No Pomodoros"));
    pomodoros = 0;
    resetPomodorosAction->setEnabled(false);
}

void Application::onAbout()
{
    QMessageBox::about(nullptr, tr("About %1").arg(qApp->applicationName())
                       , tr("%1 is a pomodoro-style timebox timer app, aimed for productivity.")
                       .arg(qApp->applicationName()));
}

void Application::onPreferences()
{
    prefDialog->loadSettings();
    prefDialog->show();
}

void Application::onPreferencesSaved()
{
    //workTimeInSeconds = settings->workTime() * 60;
    //shortBreakInSeconds = settings->shortBreakTime() * 60;
    //longBreakInSeconds = settings->longBreakTime() * 60;

    //TEST VALUES:
    workTimeInSeconds = 10;
    shortBreakInSeconds = 5;
    longBreakInSeconds = 10;
    //---------------
}

void Application::onTimerTimeout()
{
    if (machine->configuration().size() == 0) {
        return;
    }

    ++currentTimeInSeconds;

    int timeInSeconds = 0;
    QAbstractState *state = machine->configuration().toList().first();
    if (state == workState)
        timeInSeconds = workTimeInSeconds;
    else if (state == shortBreakState)
        timeInSeconds = shortBreakInSeconds;
    else if (state == longBreakState)
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
