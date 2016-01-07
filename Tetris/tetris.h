/****************************************************************************
**
** tetris.h
**
** Copyright (C) 2016 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Tetris.
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

/** @file
 */

#ifndef TETRIS_H
#define TETRIS_H

#include <QMainWindow>

#include <QtWidgets>

class Board;

/**
 * @brief 테트리스
 */
class Tetris : public QMainWindow
{
    Q_OBJECT

public:
    Tetris(QWidget *parent = 0);
    ~Tetris();

private:
    Board *_board;      ///< 테트리스 판

    void initMenus();
    void initWidgets();

private slots:
    void newGame();
};

#endif // TETRIS_H
