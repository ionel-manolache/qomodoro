#include "mediaplayer.h"

#include <QMediaPlayer>

MediaPlayer::MediaPlayer(QObject *parent) : QObject(parent)
{
    m_playStart = new QMediaPlayer(this);
    m_playStart->setVolume(90);
    m_playStart->setMedia(QUrl("qrc:/sounds/timer_start.mp3"));
    connect(m_playStart,
            static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this,
            &MediaPlayer::onMediaPlayerError);

    m_playEnd = new QMediaPlayer(this);
    m_playEnd->setVolume(90);
    m_playEnd->setMedia(QUrl("qrc:/sounds/timer_end.mp3"));
    connect(m_playEnd,
            static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this,
            &MediaPlayer::onMediaPlayerError);

    m_playTickTock = new QMediaPlayer(this);
    m_playTickTock->setVolume(90);
    m_playTickTock->setMedia(QUrl("qrc:/sounds/timer_tick.mp3"));
    connect(m_playTickTock,
            static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error), this,
            &MediaPlayer::onMediaPlayerError);
}

void MediaPlayer::onMediaPlayerError(QMediaPlayer::Error error)
{
    QMediaPlayer *player = qobject_cast<QMediaPlayer *>(QObject::sender());
    qDebug() << player->currentMedia().canonicalUrl().toString();

    qDebug() << "MediaPlayer::onMediaPlayerError(...) : ";
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

void MediaPlayer::stop()
{
    m_playStart->stop();
    m_playEnd->stop();
    m_playTickTock->stop();
}

void MediaPlayer::playStart()
{
    if (m_playStart->state() == QMediaPlayer::PlayingState)
        m_playStart->stop();
    m_playStart->play();
}

void MediaPlayer::playEnd()
{
    if (m_playEnd->state() == QMediaPlayer::PlayingState)
        m_playEnd->stop();
    m_playEnd->play();
}

void MediaPlayer::playTickTock()
{
    if (m_playTickTock->state() == QMediaPlayer::PlayingState)
        m_playTickTock->stop();
    m_playTickTock->play();
}
