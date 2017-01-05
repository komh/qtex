/****************************************************************************
**
** mpgui.cpp
**
** Copyright (C) 2017 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of MPGui.
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

/** @file mpgui.cpp
 */

#include "mpgui.h"

#include <QtWidgets>

/**
 * @brief MPGui 생성자
 * @param parent 부모 위젯
 */
MPGui::MPGui(QWidget *parent)
    : QMainWindow(parent)
    , _mplayerPath("mplayer.exe")
{
    // 애플리케이션 표시 이름 설정
    QApplication::setApplicationDisplayName(tr("MPGui"));

    initMenus();
    initWidgets();
}

/**
 * @brief MPGui 소멸자
 */
MPGui::~MPGui()
{
    // MPlayer 가 끝날 때까지 기다림
    _mplayer.write("quit\n");
    _mplayer.waitForFinished();
}

/**
 * @brief 이벤트를 필터한다
 * @param o 필터할 객체
 * @param e 발생한 이벤트
 * @return 필터링했으면 true, 그렇지 않으면 false
 */
bool MPGui::eventFilter(QObject *o, QEvent *e)
{
    // MPlayer 실행 중일 때에만 필터함
    if (_mplayer.state() == QProcess::Running && o == centralWidget())
    {
        // 키가 눌렸다면
        if (e->type() == QEvent::KeyPress)
        {
            // 키보드 이벤트로 캐스트
            QKeyEvent *ke = static_cast<QKeyEvent *>(e);

            // MPlayer 에 전달할 명령 문자열
            QString cmd;

            switch (ke->key())
            {
            case Qt::Key_Left:  // 왼쪽 화살표키: MPlayer 왼쪽키
                cmd = QString("key_down_event %1")
                        .arg(0x1000000 + 17/* Key_Left */);
                break;

            case Qt::Key_Right: // 오른쪽 화살표 키: MPlayer 오른쪽 키
                cmd = QString("key_down_event %1")
                        .arg(0x1000000 + 16/* Key_Right */);
                break;

            case Qt::Key_P: // P 키: 처음부터 다시 재생
                cmd = QString("loadfile %1").arg(_movieFilePath);
                break;

            case Qt::Key_S: // S 키: 재생 중지
                cmd = "stop";
                break;

            case Qt::Key_Space: // 스페이스 바: 일시정지
                cmd = "pause";
                break;
            }

            // 전달할 명령이 있을 때만
            if (!cmd.isEmpty())
            {
                // MPlayer 는 줄단위로 입력받음
                cmd += "\n";

                // MPlayer 에 전달
                _mplayer.write(cmd.toLocal8Bit());

                // 처리했음
                return true;
            }
        }
    }

    // 필터하지 않은 이벤트는 부모 클래스에 전달
    return QMainWindow::eventFilter(o, e);
}

/**
 * @brief 메뉴를 초기화한다
 */
void MPGui::initMenus()
{
    // '파일' 메뉴 생성
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));
    // '열기' 항목 추가
    fileMenu->addAction(tr("열기(&O)"), this, SLOT(fileOpen()),
                        QKeySequence::Open);
    // '구분자' 추가
    fileMenu->addSeparator();
    // '끝내기' 항목 추가
    fileMenu->addAction(tr("끝내기(&x)"), this, SLOT(close()),
                        QKeySequence(tr("Ctrl+Q")));

    // 메뉴바에 추가
    menuBar()->addMenu(fileMenu);
}

/**
 * @brief 위젯을 초기화한다
 */
void MPGui::initWidgets()
{
    QWidget *w = new QWidget;
    // 위젯 속성 설정: 위젯의 모든 영역을 직접 그림
    w->setAttribute(Qt::WA_OpaquePaintEvent);
    // 이벤트 필터 설치
    w->installEventFilter(this);

    // 중앙 위젯으로 설정
    setCentralWidget(w);

    // 기본 크기 설정
    resize(640, 480);

    // 입력 촛점 설정
    w->setFocus();
}

/**
 * @brief MPlayer 로 재생
 * @param filePath 동영상 파일 경로
 */
void MPGui::play(const QString &filePath)
{
    QStringList mplayerOptions;

    // MPlayer 가 이미 실행중이면, 기존 MPlayer 종료
    if (_mplayer.state() != QProcess::NotRunning)
    {
        _mplayer.write(QString("loadfile %1\n").arg(filePath).toLocal8Bit());
        return;
    }

    // MPlayer 옵션 설정
    // -slave: 슬레이브 모드
    // -wid: 동영상을 보여줄 창 ID
    // -colorkey: 오버레이에 쓰일 색상. 기본은 녹색
    // -quiet: 조용히
    mplayerOptions << "-slave"
                   << "-wid" << QString::number(centralWidget()->winId())
                   << "-colorkey" << "0x010101"
                   << "-quiet"
                   << filePath;

    // MPlayer 실행
    _mplayer.start(_mplayerPath, mplayerOptions, QIODevice::WriteOnly);

    // MPlayer 를 실행하지 못했으면, 오류 메세지 보여줌
    if (_mplayer.state() == QProcess::NotRunning)
        QMessageBox::warning(this, QApplication::applicationDisplayName(),
                             tr("MPlayer 를 실행할 수 없습니다."));

    // 동영상 파일 경로 저장. 이벤트 필터에서 쓰임
    _movieFilePath = filePath;
}

/**
 * @brief 동영상 파일을 열고 재생한다
 */
void MPGui::fileOpen()
{
    // 필터 목록
    static QString filter(tr("비디오 (*.mkv *.mp4 *.avi *.mpg);;"
                             "모든 파일 (*)"));
    // 파일 이름을 얻음
    QString filePath(QFileDialog::getOpenFileName(this, QString(), QString(),
                                                  filter));

    // 파일을 선택했으면 재생
    if (!filePath.isEmpty())
        play(filePath);
}
