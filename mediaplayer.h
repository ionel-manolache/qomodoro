#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QObject>

#include <QMediaPlayer>
#include <QMediaPlaylist>

class MediaPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MediaPlayer(QObject *parent = nullptr);

public:
    void stop();
    void playStart();
    void playEnd();
    void playTickTock();

private slots:
    void onMediaPlayerError(QMediaPlayer::Error error);

private:
    QMediaPlayer *m_playStart;
    QMediaPlayer *m_playEnd;
    QMediaPlayer *m_playTickTock;

    QMediaPlaylist *m_tickTockPlaylist;
};

#endif // MEDIAPLAYER_H
