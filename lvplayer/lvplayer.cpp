/****************************************************************************
**
** lvplayer.cpp
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

/** @fle lvplayer.cpp
 */

#include "lvplayer.h"

#include <vlc/vlc.h>

#include <QtWidgets>

/**
 * @brief libvlc 미디어 플레이어 이벤트 콜백
 * @param e libvlc 이벤트 종류
 * @param p 사용자 데이터
 */
static void vlc_player_cb(const struct libvlc_event_t *e, void *p)
{
    // 끝까지 재생했으면 정지
    if (e->type == libvlc_MediaPlayerEndReached)
    {
        LVPlayer *lvplayer = reinterpret_cast<LVPlayer *>(p);

        QMetaObject::invokeMethod(lvplayer, "stop", Qt::QueuedConnection);
    }
}

/**
 * @brief Qt 스타일 경로를 libvlc 스타일 경로로 바꾼다
 * @param s Qt 경로
 * @return libvlc 스타일 경로
 */
static inline const char *qtv(const QString &s)
{
    return QDir::toNativeSeparators(s).toUtf8().constData();
}

/**
 * @brief LVPlayer 생성자
 * @param parent 부모 위젯
 */
LVPlayer::LVPlayer(QWidget *parent)
    : QMainWindow(parent)
{
    // libvlc 인스턴스 생성
    _vlc = libvlc_new(0, 0);
    // libvlc 미디어 플레이어 생성
    _vlc_player = libvlc_media_player_new(_vlc);
    // libvlc 이벤트 관리자 생성
    _vlc_player_event = libvlc_media_player_event_manager(_vlc_player);
    // libvlc 이벤트 콜백 등록. 끝까지 재생했을 때 호출
    libvlc_event_attach(_vlc_player_event, libvlc_MediaPlayerEndReached,
                        vlc_player_cb, this);

    QApplication::setApplicationDisplayName(tr("LVPlayer"));

    initMenus();
    initWidgets();
}

/**
 * @brief LVPlayer 소멸자
 */
LVPlayer::~LVPlayer()
{
    // 정지
    stop();

    // 이벤트 콜백 제거
    libvlc_event_detach(_vlc_player_event, libvlc_MediaPlayerEndReached,
                        vlc_player_cb, this);

    // libvlc 미디어 플레이어 해제
    libvlc_media_player_release(_vlc_player);
    // libvlc 인스턴스 해제
    libvlc_release(_vlc);
}

/**
 * @brief 메뉴를 초기화한다
 */
void LVPlayer::initMenus()
{
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));
    fileMenu->addAction(tr("열기(&O)"), this, SLOT(fileOpen()),
                        QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("끝내기(&x)"), this, SLOT(close()),
                        QKeySequence(tr("Ctrl+Q")));

    menuBar()->addMenu(fileMenu);
}

/**
 * @brief 위젯을 초기화한다
 */
void LVPlayer::initWidgets()
{
    // 재생 버튼
    _playPush = new QPushButton;
    setPlayText();

    // 정지 버튼
    _stopPush = new QPushButton(tr("정지(&S)"));

    // 가로로 배치
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(_playPush);
    hbox->addWidget(_stopPush);
    hbox->addStretch(1);

    // 동영상 위젯
    _movieWidget = new QWidget;
    _movieWidget->setAttribute(Qt::WA_OpaquePaintEvent);
    _movieWidget->installEventFilter(this);

    // 세로로 배치
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(_movieWidget);
    vbox->addLayout(hbox);

    // 시그널/슬롯 연결
    connect(_playPush, SIGNAL(clicked(bool)), this, SLOT(play()));
    connect(_stopPush, SIGNAL(clicked(bool)), this, SLOT(stop()));

    // 위젯 생성
    QWidget *w = new QWidget;
    w->setLayout(vbox);

    setCentralWidget(w);

    // 초기 크기
    resize(640, 480);

    // 입력 촛점 설정
    _movieWidget->setFocus();
}

/**
 * @brief 재생 버튼의 제목을 설정한다
 * @param play 재생 버튼이면 true, 일시정지 버튼이면 false
 */
void LVPlayer::setPlayText(bool play)
{
    if (play)
        _playPush->setText(tr("재생(&P)"));
    else
        _playPush->setText(tr("일시정지(&P)"));
}

bool LVPlayer::eventFilter(QObject *o, QEvent *e)
{
    if (o == _movieWidget)
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);

        // 재생 상태
        int playing = libvlc_media_player_is_playing(_vlc_player);
        // 일시 정지 상태
        int paused = libvlc_media_player_get_state(_vlc_player)
                        == libvlc_Paused;

        // 현재 재생 시간
        libvlc_time_t current = libvlc_media_player_get_time(_vlc_player);

        switch (ke->key())
        {
        case Qt::Key_Left:
            // 10 초 이전으로
            libvlc_media_player_set_time(_vlc_player, current - 10 * 1000);
            return true;

        case Qt::Key_Right:
            // 10 초 이후로
            libvlc_media_player_set_time(_vlc_player, current + 10 * 1000);
            return true;

         case Qt::Key_Space:
            // 일시정지/재생
            if (playing || paused)
                _playPush->click();
            return true;
        }
    }

    // 나머지는 부모 클래스에게
    return QMainWindow::eventFilter(o, e);
}

/**
 * @brief 동영상 파일을 연다
 */
void LVPlayer::fileOpen()
{
    // 필터 목록
    static QString filter(tr("비디오 (*.mkv *.mp4 *.avi *.mpg);;"
                             "모든 파일 (*)"));
    // 파일 이름을 얻음
    QString filePath(QFileDialog::getOpenFileName(this, QString(), QString(),
                                                  filter));

    // 파일을 선택했으면 재생
    if (!filePath.isEmpty())
    {
        _movieFilePath = filePath;
        play();
    }
}

/**
 * @brief 동영상을 재생한다
 */
void LVPlayer::play()
{
    // 입력 촛점은 언제나 동영상 위젯에
    _movieWidget->setFocus();

    // 재생 상태
    int playing = libvlc_media_player_is_playing(_vlc_player);
    // 일시정지 상태
    int paused = libvlc_media_player_get_state(_vlc_player) == libvlc_Paused;

    // 재생 버튼을 누른 경우에만 일시정지/재생 실행
    if (sender() == _playPush && (playing || paused))
    {
        libvlc_media_player_set_pause(_vlc_player, playing);
        setPlayText(playing);

        return;
    }

    // 동영상 파일을 아직 고르지 않았으면 파일을 열기부터
    if (_movieFilePath.isEmpty())
    {
        fileOpen();

        return;
    }

    // 로컬 파일에 대한 미디어를 생성
    libvlc_media_t *vlc_media = libvlc_media_new_path(_vlc,
                                                      qtv(_movieFilePath));
    // 미디어를 libvlc 미디어 플레이어에 등록
    libvlc_media_player_set_media(_vlc_player, vlc_media);
    // 등록한 미디어는 해제
    libvlc_media_release(vlc_media);

    // 동영상을 표시할 위젯 설정
    libvlc_media_player_set_hwnd(_vlc_player,
                                 reinterpret_cast<void*>(
                                     _movieWidget->winId()));

    // 동영상 재생
    libvlc_media_player_play(_vlc_player);

    // 재생 버튼 일시정지 상태로
    setPlayText(false);
}

/**
 * @brief 동영상 재생을 정지한다
 */
void LVPlayer::stop()
{
    // 입력 촛점은 언제나 동영상 위젯에
    _movieWidget->setFocus();

    // 재생을 정지
    libvlc_media_player_stop(_vlc_player);

    // 재생 버튼 재생 상태로
    setPlayText(true);
}
