/****************************************************************************
**
** puzzle.h
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

#ifndef PUZZLE_H
#define PUZZLE_H

#include <QMainWindow>
#include <QtWidgets>

/**
 * @brief 숫자 퍼즐 클래스
 */
class Puzzle : public QMainWindow
{
    Q_OBJECT

public:
    Puzzle(QWidget *parent = 0);
    ~Puzzle();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    /// 퍼즐 세로 줄 갯수
    static const int PuzzleRows = 3;
    /// 패줄 가로 줄 갯수
    static const int PuzzleCols = 3;
    /// 퍼줄 조각 갯수
    static const int PuzzlePieces = PuzzleRows * PuzzleCols;

    QList<QPushButton *> _pieces;   ///< 퍼즐 조각 리스트
    QMap<int, int> _puzzleMap;      ///< 퍼즐 조각 위치 맵
    int _emptyPiece;                ///< 빈 칸 위치
    bool _shuffle;                  ///< 섞는 중
    int _tryCount;                  ///< 이동 횟수

    void initMenus();
    void initWidgets();
    void initConnections();

    int upPiece(int from);
    int downPiece(int from);
    int leftPiece(int from);
    int rightPiece(int from);
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void movePiece(int from);
    void checkResult();

private slots:
    void newPuzzle();
    void pieceClicked();
};

#endif // PUZZLE_H
