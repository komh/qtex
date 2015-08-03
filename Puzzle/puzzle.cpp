/****************************************************************************
**
** puzzle.cpp
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Puzzle.
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

#include "puzzle.h"

/**
 * @brief 생성자
 * @param parent 부모 위젯
 */
Puzzle::Puzzle(QWidget *parent)
    : QMainWindow(parent)
    , _emptyPiece(0)
    , _shuffle(false)
    , _tryCount(0)
{
    initMenus();        // 메뉴 초기화
    initWidgets();      // 위젯 초기화
    initConnections();  // 시그널과 슬롯 연결

    newPuzzle();    // 새 퍼즐

}

/**
 * @brief 소멸자
 */
Puzzle::~Puzzle()
{

}

/**
 * @brief 키보드 이벤트를 처리한다
 * @param e 키보드 이벤트
 */
void Puzzle::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Up:    // 윗쪽 방향 키면
        moveUp();       // 위로 옮김
        break;

    case Qt::Key_Down:  // 아랫쪽 방향 키면
        moveDown();     // 아래로 옮김
        break;

    case Qt::Key_Left:  // 윗쪽 방향 키면
        moveLeft();     // 왼쪽으로 옮김
        break;

    case Qt::Key_Right: // 오른쪽 방향 키면
        moveRight();    // 오른쪽으로 옮김
        break;

    default:            // 나머지 이벤트는
        e->ignore();    // 부모 위젯에 전달
        return;
    }

    e->accept();    // 처리했음
}

/**
 * @brief 메뉴를 초기화한다
 */
void Puzzle::initMenus()
{
      // "파일" 메뉴 새성
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));
    // "새 퍼즐" 항목 추가
    fileMenu->addAction(tr("새 퍼즐(&N)"), this, SLOT(newPuzzle()),
                        QKeySequence(QKeySequence::New));
    // 구분줄 추가
    fileMenu->addSeparator();
    // "끝내기" 항목 추가
    fileMenu->addAction(tr("끝내기(&x)"), qApp, SLOT(quit()),
                        QKeySequence(tr("Ctrl+Q")));

    // 메뉴바에 "파일" 메뉴 추가
    menuBar()->addMenu(fileMenu);
}

/**
 * @brief 위젯들을 초기화한다
 */
void Puzzle::initWidgets()
{
    // 퍼즐 조각 생성
    while (_pieces.count() < PuzzlePieces)
    {
        QPushButton *piece = new QPushButton;
        piece->setFocusPolicy(Qt::NoFocus); // 입력 촛점을 받지 않음

        _pieces.append(piece);  // 생성된 푸시 버튼을 리스트에 추가
    }

    int row = 0, col = 0;

    QGridLayout *gridLayout = new QGridLayout;  // 그리드 레이아웃 생성

    Q_FOREACH (QPushButton *piece, _pieces)
    {
        // 그리드 레이아웃으로 퍼즐 조각 배치
        gridLayout->addWidget(piece, row, col, Qt::AlignCenter);

        // PuzzleCols 단위로 줄바꿈
        col = (col + 1) % PuzzleCols;
        row += col == 0;
    }

    setCentralWidget(new QWidget);          // 센트럴 위젯 생성
    centralWidget()->setLayout(gridLayout); // 센트럴 위젯 레이아웃 설정
}

/**
 * @brief 시그널과 슬롯을 연결한다
 */
void Puzzle::initConnections()
{
    // 모든 퍼즐 조각의 시그널을 하나의 슬롯에 연결
    Q_FOREACH (QPushButton *piece, _pieces)
        connect(piece, SIGNAL(clicked(bool)), this, SLOT(pieceClicked()));
}

/**
 * @brief @a from 윗쪽에 있는 퍼즐 조각의 위치를 돌려준다
 * @param from 기준이 되는 퍼즐 조각의 위치
 * @return @a from 윗쪽 퍼즐 조각의 위치
 */
int Puzzle::upPiece(int from)
{
    return qMax(from - PuzzleCols, from % PuzzleCols);
}

/**
 * @brief @a from 아랫쪽에 있는 퍼즐 조각의 위치를 돌려준다
 * @param from 기준이 되는 퍼즐 조각의 위치
 * @return @a from 아랫쪽 퍼즐 조각의 위치
 */
int Puzzle::downPiece(int from)
{
    return qMin(from + PuzzleCols,
                PuzzlePieces - PuzzleCols + (from % PuzzleCols));
}

/**
 * @brief @a from 왼쪽에 있는 퍼즐 조각의 위치를 돌려준다
 * @param from 기준이 되는 퍼즐 조각의 위치
 * @return @a from 왼쪽 퍼즐 조각의 위치
 */
int Puzzle::leftPiece(int from)
{
    return (from % PuzzleCols) == 0 ? from : from - 1;
}

/**
 * @brief @a from 오른쪽에 있는 퍼즐 조각의 위치를 돌려준다
 * @param from 기준이 되는 퍼즐 조각의 위치
 * @return @a from 오른쪽 퍼즐 조각의 위치
 */
int Puzzle::rightPiece(int from)
{
    return (from % PuzzleCols) == PuzzleCols - 1 ? from : from + 1;
}

/**
 * @brief 퍼즐 조각을 윗쪽 빈 칸으로 이동시킨다
 */
void Puzzle::moveUp()
{
    movePiece(downPiece(_emptyPiece));
}

/**
 * @brief 퍼즐 조각을 아랫쪽 빈 칸으로 이동시킨다
 */
void Puzzle::moveDown()
{
    movePiece(upPiece(_emptyPiece));
}

/**
 * @brief 퍼즐 조각을 왼쪽 빈 칸으로 이동시킨다
 */
void Puzzle::moveLeft()
{
    movePiece(rightPiece(_emptyPiece));
}

/**
 * @brief 퍼즐 조각을 오른쪽 빈 칸으로 이동시킨다
 */
void Puzzle::moveRight()
{
    movePiece(leftPiece(_emptyPiece));
}

/**
 * @brief 퍼즐 조각을 @a from 에서 빈 칸으로 이동시킨다
 * @param from 이동시킬 퍼즐 조각의 위치
 */
void Puzzle::movePiece(int from)
{
    // 움직이려는 조각이 빈 칸이 아니면
    if (from != _emptyPiece)
    {
        int to = _emptyPiece;

        // 움직이려는 조각과 빈칸을 바꿈
        _puzzleMap.insert(to, _puzzleMap.value(from));
        _puzzleMap.insert(from, 0);

        QPushButton *toPiece = _pieces.at(to);
        QPushButton *fromPiece = _pieces.at(from);

        toPiece->setText(fromPiece->text());
        fromPiece->setText(" ");

        // 빈 칸을 옮김
        _emptyPiece = from;

        // 섞고 있지 않으면
        if (!_shuffle)
        {
            // 움직인 횟수 증가
            ++_tryCount;

            // 결과 확인
            checkResult();
        }
    }
}

/**
 * @brief 퍼즐 조각이 모두 맞추어졌는지 확인한다
 */
void Puzzle::checkResult()
{
    bool result = true;

    QMapIterator<int, int> it(_puzzleMap);

    while (it.hasNext())
    {
        it.next();

        // 마지막 조각은 빈 칸(0)
        if (it.key() + 1 != it.value()
                && ((it.key() + 1 != PuzzlePieces) || it.value() != 0))
        {
            result = false;
        }
    }

    if (result)
        QMessageBox::information(this, qApp->applicationName(),
                                 tr("%1 번의 이동만에 성공하였습니다!!!")
                                    .arg(_tryCount));
}

/**
 * @brief 새로운 퍼즐을 시작한다
 */
void Puzzle::newPuzzle()
{
    // 밀리초로 나타낸 현재 시간을 난수 씨앗으로 설정
    qsrand(QTime::currentTime().msecsSinceStartOfDay());

    // 퍼즐 조각 위치 맵을 초기화
    _puzzleMap.clear();
    while (_puzzleMap.count() < PuzzlePieces)
        _puzzleMap.insert(_puzzleMap.count(), _puzzleMap.count());

    // 빈칸은 첫번째 칸
    _emptyPiece = 0;

    _shuffle = true;
    // 퍼즐 조각의 5 배를 섞는다
    for (int i = 0; i < PuzzlePieces * 5; ++i)
    {
        // 네 방향으로 임의로 움직인다
        switch (qrand() * 4 / RAND_MAX)
        {
        case 0:
            moveUp();
            break;

        case 1:
            moveDown();
            break;

        case 2:
            moveLeft();
            break;

        case 3:
        default:
            moveRight();
            break;
        }
    }
    _shuffle = false;

    QMapIterator<int, int> it(_puzzleMap);
    while (it.hasNext())
    {
        it.next();

        QString text(it.value() == 0 ? QString(" ") :
                                       QString::number(it.value()));

        // 위치를 문자열로 바꾸어 표현
        _pieces.at(it.key())->setText(text);
    }
}

/**
 * @brief 클릭된 퍼즐 조각을 이동시킨다
 */
void Puzzle::pieceClicked()
{
    // 시그널을 보낸 위젯의 위치를 얻는다
    int piece = _pieces.indexOf(qobject_cast<QPushButton *>(sender()));

    // 빈 칸이면 할 일 없다
    if (piece == _emptyPiece)
        return;

    // 윗쪽이 비었으면, 위로 옮김
    if (upPiece(piece) == _emptyPiece)
        moveUp();
    // 아랫쪽이 비었으면 아래로 옮김
    else if (downPiece(piece) == _emptyPiece)
        moveDown();
    // 왼쪽이 비었으면 왼쪽으로 옮김
    else if (leftPiece(piece) == _emptyPiece)
        moveLeft();
    // 오른쪽이 비었으면 오른쪽으로 옮
    else if (rightPiece(piece) == _emptyPiece)
        moveRight();
}
