/****************************************************************************
**
** lvplayer.h
**
** Copyright (C) 2017 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of LVPlayer.
**
** $BEGIN_LICENSE$
**
** This program is free software. It comes without any warranty, to
** the extent permitted by applicable law. You can redistribute it
** and/or modify it under the terms of the Do What The Fuck You Want
** To Public License, Version 2, as published by Sam Hocevar. See
** http://www.wtfpl.net/ for more details.
**
** $END_LICENSE$
**
****************************************************************************/

/** @file lvplayer.h
 */

#ifndef LVPLAYER_H
#define LVPLAYER_H

#include <QMainWindow>

#include <vlc/vlc.h>

#include <QtWidgets>

class LVPlayer : public QMainWindow
{
    Q_OBJECT

public:
    LVPlayer(QWidget *parent = 0);
    ~LVPlayer();

protected:
    bool eventFilter(QObject *o, QEvent *e);

private:
    /// libvlc 인스턴스
    libvlc_instance_t *_vlc;
    /// libvlc 미디어 플레이어
    libvlc_media_player_t *_vlc_player;
    /// libvlc 이벤트 관리자
    libvlc_event_manager_t *_vlc_player_event;

    QWidget *_movieWidget;  ///< 동영상 위젯
    QPushButton *_playPush; ///< 재생 버튼
    QPushButton *_stopPush; ///< 정지 버튼

    QString _movieFilePath; ///< 동영상 파일 경로

    void initMenus();
    void initWidgets();

    void setPlayText(bool play = true);

private slots:
    void fileOpen();
    void play();
    void stop();
};

#endif // LVPLAYER_H
