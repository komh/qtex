/****************************************************************************
**
** dirlister.cpp
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Directory Lister.
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

/** @file dirlister.cpp
 */

#include "dirlister.h"

/**
 * @brief DirLister 생성자
 * @param parent 부모 위젯
 */
DirLister::DirLister(QWidget *parent)
    : QMainWindow(parent)
{
    initMenus();    // 메뉴 초기화
    initWidgets();  // 위젯 초기화
}

/**
 * @brief DirLister 소멸자
 */
DirLister::~DirLister()
{

}

/**
 * @brief 메뉴를 초기화한다
 */
void DirLister::initMenus()
{
    // '파일' 메뉴 생성
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));

    // '끝내기' 항목 추가
    fileMenu->addAction(tr("끝내기(&x)"), this, SLOT(close()),
                        QKeySequence(tr("Ctrl+Q")));

    // '파일' 메뉴 추가
    menuBar()->addMenu(fileMenu);
}

/**
 * @brief 위젯을 초기화한다
 */
void DirLister::initWidgets()
{
    // '위치' 한줄편집기 생성
    _locationLine = new QLineEdit;
    // 읽기 전용으로
    _locationLine->setReadOnly(true);

    // '위치' 레이블 생성
    QLabel *locationText = new QLabel(tr("위치(&L)"));
    // 단축키를 '위치' 한줄편집기와 연결
    locationText->setBuddy(_locationLine);

    // 디렉토리 구조용 파일 시스템 모델 생성
    _dirModel = new QFileSystemModel;
    // 디렉토리, 모든 드라이브. '.' 과 '..' 는 제외
    _dirModel->setFilter(QDir::AllDirs | QDir::Drives | QDir::NoDotAndDotDot);

    // 위치 위젯 가로로 배치
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(locationText);
    hboxLayout->addWidget(_locationLine);

    // 드라이브 또는 '/' 디렉토리 내용을 읽음
    foreach (QFileInfo info, QDir::drives())
    {
        QEventLoop loop;
        connect(_dirModel, &_dirModel->directoryLoaded, &loop, &loop.quit);

        // 읽을 디렉토리 설정
        _dirModel->setRootPath(info.filePath());
        loop.exec();
    }

    QString currentDir = QDir::currentPath();
    // 현재 디렉토리 구성 요소를 따라 디렉토리 내용을 읽음
    for (int i = 0; i != -1;)
    {
        i = currentDir.indexOf("/", i + 1);
        QString dir = currentDir.mid(0, i);

        if (_dirModel->rootPath() != dir)
        {
            QEventLoop loop;
            connect(_dirModel, &_dirModel->directoryLoaded, &loop, &loop.quit);

            _dirModel->setRootPath(dir);
            loop.exec();
        }
    }

    // 디렉토리 구조용 트리뷰 생성
    _dirView = new QTreeView;
    // 모델 설정
    _dirView->setModel(_dirModel);
    // 헤더 숨김
    _dirView->setHeaderHidden(true);
    // 두번째 이후 모든 컬럼 숨김
    for (int i = _dirModel->columnCount() - 1; i > 0; --i)
        _dirView->setColumnHidden(i, true);
    connect(_dirView, &_dirView->activated, this, &this->dirActivated);
    connect(_dirView, &_dirView->clicked, this, &this->dirActivated);

    // 디렉토리 내용용 파일 시스템 모델 생성
    _entryModel = new QFileSystemModel;
    // 모든 디렉토리, 파일. '.' 과 '..' 은 제외
    _entryModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    _entryModel->setRootPath("");

    // 디렉토리 내용용 트리뷰 생성
    _entryView = new QTreeView;
    // 모델 설정
    _entryView->setModel(_entryModel);
    // 펼치기/감추기 아이콘 숨김
    _entryView->setRootIsDecorated(false);
    // 더블 클릭 무시
    _entryView->setExpandsOnDoubleClick(false);

    // 스플리터 생성
    QSplitter *splitter = new QSplitter;
    splitter->addWidget(_dirView);
    splitter->setStretchFactor(0, 0);
    splitter->addWidget(_entryView);
    splitter->setStretchFactor(1, 1);

    // 위치와 스플리터 세로 배치
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addLayout(hboxLayout);
    vboxLayout->addWidget(splitter);

    // 중앙 위젯 생성
    QWidget *w = new QWidget;
    w->setLayout(vboxLayout);

    // 중앙 위젯 설정
    setCentralWidget(w);

    // 디렉토리 구조용 트리뷰에 촛점 설정
    _dirView->setFocus();
    // 현재 디렉토리를 현재 인덱스로 설정
    _dirView->setCurrentIndex(_dirModel->index(QDir::currentPath()));
    // 현재 인덱스 활성화
    dirActivated(_dirView->currentIndex());
}

/**
 * @brief 디렉토리가 활성화되었을 때 호출됨
 * @param index 활성화된 인덱스
 */
void DirLister::dirActivated(const QModelIndex &index)
{
    // 인덱스로부터 파일 경로를 얻음
    QString path(_dirModel->filePath(index));

    // 위치 텍스트를 경로로 설정
    _locationLine->setText(QDir::toNativeSeparators(path));
    // 디렉토리 내용용 트리뷰에 디렉토리 내용 표시
    _entryView->setRootIndex(_entryModel->index(path));
}
