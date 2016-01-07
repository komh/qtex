/****************************************************************************
**
** tetris.cpp
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

#include "tetris.h"

/**
 * @brief 테트리스 블럭
 */
class Block
{
public:
    /**
     * @brief 블럭 조각의 폭을 돌려준다
     * @return 블럭 조각의 폭
     */
    static int squareWidth()
    {
        return 30;
    }

    /**
     * @brief 블럭 조각의 높이를 돌려준다
     * @return 블럭 조각의 높이
     */
    static int squareHeight()
    {
        return 30;
    }

    /**
     * @brief 생성자
     * @param cols 블럭의 열 수
     * @param rows 블럭의 행 수
     * @param color 블럭의 내부 색깔
     */
    Block(int cols, int rows, QColor color)
        : _cols(cols)
        , _rows(rows)
        , _color(color)
    {
        // 블럭 초기화
        for (int row = 0; row < _rows; ++row)
        {
            QVector<bool> v;

            for (int col = 0; col < _cols; ++col)
            {
                v.append(false);
            }

            _map.append(v);
        }

        // 블럭 조각 생성
        _square = new QPixmap(squareWidth(), squareHeight());

        QPainter painter;
        painter.begin(_square);
        painter.fillRect(_square->rect(), color);
        painter.setPen(Qt::darkGray);
        painter.drawRect(0, 0,
                         _square->width() - 1, _square->height() - 1);
        painter.end();

    }

    /**
     * @brief 소멸자
     */
    virtual ~Block()
    {
        delete _square;
    }

    /**
     * @brief 블럭의 열 수를 돌려준다
     * @return 블럭의 열 수
     */
    int cols() const
    {
        return _cols;
    }

    /**
     * @brief 블럭의 행 수를 돌려준다
     * @return 블럭의 행 수
     */
    int rows() const
    {
        return _rows;
    }

    /**
     * @brief 블럭의 특정 위치에 블럭 조각이 있는지 확인한다
     * @param col 블럭 조각의 열 위치
     * @param row 블럭 조각의 행 위치
     * @return 블럭 조각이 있으면 true, 없으면 false
     */
    bool marked(int col, int row) const
    {
        return _map.at(row).at(col);
    }

    /**
     * @brief 블럭의 특정 위치에 블럭 조각이 있는지 표시한다
     * @param col 블럭 조각의 열 위치
     * @param row 블럭 조각의 행 위치
     */
    void mark(int col, int row )
    {
        _map[row][col] = true;
    }

    /**
     * @brief 블럭을 회전한다
     * @param clockWise 참이면 시계 방향으로, 거짓이면 반시계 방향으로 회전
     */
    void rotate(bool clockWise = true)
    {
        // 새로운 블럭 모양
        QVector<QVector<bool> > newMap;

        // 행/열 바꿈
        int newRows = _cols;
        int newCols = _rows;

        int row;
        int col;

        // 새 블럭 초기화
        for (row = 0; row < newRows; ++row)
        {
            newMap.append(QVector<bool>(newCols));
        }

        if (clockWise)
        {
            // 시계 방향 회전
            for (row = 0; row < _rows; ++row)
            {
                for (col = 0; col < _cols; ++ col)
                {
                    newMap[newRows - 1 - col][row] = marked(col, row);
                }
            }
        }
        else
        {
            // 반시계 방향 회전
            for (row = 0; row < _rows; ++row)
            {
                for (col = 0; col < _cols; ++ col)
                {
                    newMap[col][newCols - 1 - row] = marked(col, row);
                }
            }
        }

        // 회전 시킨 블럭으로 교체
        _rows = newRows;
        _cols = newCols;
        _map = newMap;
    }

    /**
     * @brief 블럭 조각을 주어진 위치에 그린다
     * @param col 테트리스 판 내부 열 위치
     * @param row 테트리스 판 내부 행 위치
     * @param painter 테트리스 판의 페인터
     */
    void drawSquare(int col, int row, QPainter *painter)
    {
        painter->drawPixmap(col * squareWidth(), row * squareHeight(),
                            *_square);
    }

private:
    int _cols;  ///< 블럭의 열 수
    int _rows;  ///< 블럭의 행 수
    QColor _color;  ///< 블럭 내부의 색깔
    QVector<QVector<bool> > _map;   ///< 블럭 조각 배치도

    QPixmap *_square;   ///< 블럭 조각 모양
};

/**
 * @brief 테트리스 판
 */
class Board : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 생성자
     * @param cols 판의 열 수
     * @param rows 판의 행 수
     * @param parent 부모 위젯
     */
    Board(int cols = 10, int rows = 20, QWidget *parent = 0)
        : QWidget(parent)
        , _cols(cols)
        , _rows(rows)
    {
        // I 블럭 생성
        _blockI = new Block(1, 4, Qt::red);         // 빨강
        _blockI->mark(0, 0);
        _blockI->mark(0, 1);
        _blockI->mark(0, 2);
        _blockI->mark(0, 3);

        // J 블럭 생성
        _blockJ = new Block(3, 2, Qt::white);       // 하양
        _blockJ->mark(0, 0);
        _blockJ->mark(0, 1); _blockJ->mark(1, 1); _blockJ->mark(2, 1);

        // L 블럭 생성
        _blockL = new Block(3, 2, Qt::magenta);     // 자홍
        _blockL->mark(0, 0); _blockL->mark(1, 0); _blockL->mark(2, 0);
        _blockL->mark(0, 1);

        // O 블럭 생성
        _blockO = new Block(2, 2, Qt::blue);        // 파랑
        _blockO->mark(0, 0); _blockO->mark(1, 0);
        _blockO->mark(0, 1); _blockO->mark(1, 1);

        // S 블럭 생성
        _blockS = new Block(3, 2, Qt::green);       // 녹색
                             _blockS->mark(1, 0); _blockS->mark(2, 0);
        _blockS->mark(0, 1); _blockS->mark(1, 1);

        // T 블럭 생성
        _blockT = new Block(3, 2, "#A52A2A");       // 갈색
        _blockT->mark(0, 0); _blockT->mark(1,0); _blockT->mark(2,0);
                             _blockT->mark(1, 1);

         // Z 블럭 생성
        _blockZ = new Block(3, 2, Qt::cyan);        // 하늘색
        _blockZ->mark(0, 0); _blockZ->mark(1, 0);
                             _blockZ->mark(1,1); _blockZ->mark(2,1);

         // 빈 블럭 생성
        _blockEmpty = new Block(1, 1, Qt::black);   // 검정
        _blockEmpty->mark(0, 0);

        // 위젯 크기 고정
        setFixedSize(_cols * Block::squareWidth(),
                     _rows * Block::squareHeight());

        // 난수 씨앗 초기화
        qsrand(QTime::currentTime().msecsSinceStartOfDay());

        // 판 초기화
        for (int row = 0; row < _rows; ++row)
        {
            _map.append(QVector<Block *>(_cols));
        }

        // 새 게임 시작
        newGame();
    }

    /**
     * @brief 소멸자
     */
    ~Board()
    {
        delete _blockI;
        delete _blockJ;
        delete _blockL;
        delete _blockO;
        delete _blockS;
        delete _blockT;
        delete _blockZ;
        delete _blockEmpty;
    }

    /**
     * @brief 새 게임을 시작한다
     */
    void newGame()
    {
        // 게임중...
        _gameOver = false;

        // 판 모두 비움
        for (int row = 0; row < _rows; ++row)
        {
            for (int col = 0; col < _cols; ++col)
            {
                _map[row][col] = _blockEmpty;
            }
        }

        // 새 블럭 생성
        makeNewBlock();

        // 타이머 연결, 1초에 한 번씩 내려감
        connect(&_timer, &_timer.timeout, this, &this->moveDown);
        _timer.start(1000);

        // 판 갱신
        update();
    }

protected:
    /**
     * @brief 키보드가 눌리면 발생하는 이벤트를 처리한다
     * @param e 키보드 이벤트
     */
    void keyPressEvent(QKeyEvent *e)
    {
        // 게임이 끝났으면 아무것도 하지 않음
        if (_gameOver)
            return;

        switch(e->key())
        {
        case Qt::Key_Up:    // 위
            rotate();       // 회전
            break;

        case Qt::Key_Down:  // 아래
            moveDown();     // 한 칸 아래로
            break;

        case Qt::Key_Left:  // 왼쪽
            moveLeft();     // 한 칸 왼쪽으로
            break;

        case Qt::Key_Right: // 오른쪽
            moveRight();    // 한 칸 오른쪽으로
            break;

        case Qt::Key_Space:     // 스페이스 바
            moveDownToBottom(); // 바닥으로
            break;

        default:                        // 나머지
            QWidget::keyPressEvent(e);  // 부모 위젯에 전달
            return;
        }
    }

    /**
     * @brief 그리기 요청이 있으면 발생하는 이벤트를 처리한다
     * @todo 필요한 부분만 그리기
     */
    void paintEvent(QPaintEvent *)
    {
        QPainter painter;

        painter.begin(this);
        // 판 내부의 블럭 조각을 그림
        for (int row = 0; row < _rows; ++row)
        {
            for (int col = 0; col < _cols; ++col)
            {
                _map.at(row).at(col)->drawSquare(col, row, &painter);
            }
        }
        painter.end();
    }

private:
    int _cols;  ///< 판의 열 수
    int _rows;  ///< 판의 행 수

    int _col;   ///< 현재 블럭의 열 위치
    int _row;   ///< 현재 블럭의 행 위치

    bool _gameOver; ///< 게임 종료 여부

    Block *_block;      ///< 현재 블럭
    Block *_blockI;     ///< I 블럭
    Block *_blockJ;     ///< J 블럭
    Block *_blockL;     ///< L 블럭
    Block *_blockO;     ///< O 블럭
    Block *_blockS;     ///< S 블럭
    Block *_blockT;     ///< T 블럭
    Block *_blockZ;     ///< Z 블럭
    Block *_blockEmpty; ///< 빈 블럭

    QVector<QVector<Block *> > _map;    ///< 판 내부 블럭 조각 배치도

    QTimer _timer;  ///< 한 칸 아래로 내려가는 시간을 조절하는 타이머

    /**
     * @brief 새로운 생성을 만든다
     */
    void makeNewBlock()
    {
        // 블럭 종류
        static Block *blocks[] =
            {_blockI, _blockJ, _blockL, _blockO, _blockS, _blockT, _blockZ};

        // 새로운 블럭 생성
        int newBlock = qFloor(qrand() / (RAND_MAX + 1.0f) * 7);

        _block = blocks[newBlock];

        // 가로 위치는 화면 중앙에
        _col = (_cols - _block->cols()) / 2;
        // 세로 위치는 판 바로 위에
        _row = -_block->rows();
    }

    /**
     * @brief 행 위치가 판 내부인지 확인한다
     * @param row 행 위치
     * @return 행 위치가 판 내부이면 참, 아니면 거짓
     */
    bool checkRow(int row)
    {
        return row >= 0 && row < _rows;
    }

    /**
     * @brief 열 위치가 판 내부인지 확인한다
     * @param col 열 위치
     * @return 열 위치가 판 내부이면 참, 아니면 거짓
     */
    bool checkCol(int col)
    {
        return col >= 0 && col < _cols;
    }

    /**
     * @brief 현재 블럭을 지운다
     */
    inline void eraseBlock()
    {
        putBlock(true);
    }

    /**
     * @brief 현재 블럭을 표시한다
     * @param clear 참이면 블럭을 지우고, 거짓이면 블럭을 표시함
     */
    void putBlock(bool clear = false)
    {
        Block *block = clear ? _blockEmpty : _block;

        for (int r = 0, row = _row; r < _block->rows(); ++r, ++row)
        {
            for (int c = 0, col = _col; c < _block->cols(); ++c, ++col)
            {
                if (checkRow(row) && checkCol(col) && _block->marked(c, r))
                {
                    _map[row][col] = block;
                }
            }
        }
    }

    /**
     * @brief 주어진 행의 줄을 지운다
     * @param row 행 위치
     */
    void removeLine(int row)
    {
        // 행 윗쪽의 내용을 아래로 내림
        for (; row > 0; --row)
        {
            for (int col = 0; col < _cols; ++col)
            {
                _map[row][col] = _map.at(row - 1).at(col);
            }
        }

        // 가장 윗 줄은 빈 줄
        for (int col = 0; col < _cols; ++col)
            _map[0][col] = _blockEmpty;

        // 위젯 다시 그림
        update();
    }

    /**
     * @brief 블럭이 도달한 위치의 줄이 모두 채워져 있으면 줄을 지운다
     */
    void checkLine()
    {
        for (int row = _row; row < _row + _block->rows(); ++row)
        {
            int col;

            for (col = 0; col < _cols; ++col)
            {
                 if (_map.at(row).at(col) == _blockEmpty)
                     break;
            }

            if (col == _cols)
                removeLine(row);
        }
    }

    /**
     * @brief 블럭을 회전시킨다
     */
    void rotate()
    {
        // 블럭을 지움
        eraseBlock();
        // 블럭 회전
        _block->rotate();

        // 회전한 블럭이 판 내부 조각들과 충돌하는지 확인
        for (int r = 0, row = _row; r < _block->rows(); ++r, ++row)
        {
            for (int c = 0, col = _col; c < _block->cols(); ++c, ++col)
            {
                if (!checkRow(row) || !checkCol(col)
                        || (_block->marked(c, r)
                                && _map.at(row).at(col) != _blockEmpty))
                {
                    // 충돌했음

                    // 블럭 원래로
                    _block->rotate(false);

                    // 블럭 표시
                    putBlock();

                    return;
                }
            }
        }

        // 충돌하지 않았음

        // 블럭 표시
        putBlock();

        // 다시 그림
        update();
    }

    /**
     * @brief 블럭을 한 칸 아래로
     * @return 아래로 내려갔으면 참, 아니면 거짓
     */
    bool moveDown()
    {
        for (int c = 0; c < _block->cols(); ++c)
        {
            int r;

            // 블럭 가장 아래 조각 찾기
            for (r = _block->rows() - 1; r >= 0; --r)
            {
                if (_block->marked(c, r))
                    break;
            }

            int col = c + _col;
            int row = r + _row;

            if (row + 1 == _rows        // 가장 아랫줄 ?
                    || (checkRow(row)   // 밑에 조각이 있나 ?
                            && _map.at(row + 1).at(col) != _blockEmpty))
            {
                if (_row < 0)   // 꽉 찼으면
                {
                    // 게임 끝 처리
                    _timer.stop();
                    _gameOver = true;

                    QMessageBox::information(this,
                                             qApp->applicationDisplayName(),
                                             tr("게임이 끝났습니다."));
                }
                else    // 블럭 더 이상 못 내려감
                {
                    // 줄 확인
                    checkLine();
                    // 새 블럭 생성
                    makeNewBlock();
                }

                // 다시 그림
                update();

                // 못 내려갔음
                return false;
            }
        }

        // 블럭 지움
        eraseBlock();
        // 한 칸 아래로
        ++_row;
        // 블럭 표시
        putBlock();

        // 다시 그림
        update();

        // 내려갔음
        return true;
    }

    /**
     * @brief 블럭을 바닥으로
     */
    void moveDownToBottom()
    {
        while (moveDown())
            /* nothing */;
    }

    /**
     * @brief 블럭을 한 칸 왼쪽으로
     */
    void moveLeft()
    {
        // 왼쪽 벽이면
        if (_col <= 0)
            return;

        for (int r = 0; r < _block->rows(); ++r)
        {
            int c;

            // 가장 왼쪽 블럭 조각 찾기
            for (c = 0; c < _block->cols(); ++c)
            {
                if (_block->marked(c, r))
                    break;
            }

            int row = r + _row;
            int col = c + _col;

            // 왼쪽이 막혀 있으면
            if (checkRow(row) && _map.at(row).at(col - 1) != _blockEmpty)
                return;
        }

        // 블럭 지움
        eraseBlock();
        // 한 칸 왼쪽으로
        --_col;
        // 블럭 표시
        putBlock();

        // 다시 그림
        update();
    }

    /**
     * @brief 블럭을 한 칸 오른쪽으로
     */
    void moveRight()
    {
        // 오른쪽 벽이면
        if (_col + _block->cols() >= _cols)
            return;

        for (int r = 0; r < _block->rows(); ++r)
        {
            int c;

            // 가장 오른쪽 조각 찾기
            for (c = _block->cols() - 1; c >= 0; --c)
            {
                if (_block->marked(c, r))
                    break;
            }

            int row = r + _row;
            int col = c + _col;

            // 오른쪽이 막혀 있으면
            if (checkRow(row) && _map.at(row).at(col + 1) != _blockEmpty)
                return;
        }

        // 블럭 지움
        eraseBlock();
        // 한 칸 오른쪽으로
        ++_col;
        // 블럭 표시
        putBlock();

        // 다시 그림
        update();
    }
};

/**
 * @brief 생성자
 * @param parent
 */
Tetris::Tetris(QWidget *parent)
    : QMainWindow(parent)
{
    // 메뉴 초기화
    initMenus();
    // 위젯 초기화
    initWidgets();
}

/**
 * @brief 소멸자
 */
Tetris::~Tetris()
{
}

/**
 * @brief 메뉴를 초기화한다
 */
void Tetris::initMenus()
{
    // '파일' 메뉴 생성
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));
    // '새 게임' 항목 추가
    fileMenu->addAction(tr("새 게임(&N)"), this, SLOT(newGame()),
                        QKeySequence(QKeySequence::New));
    // '끝내기' 항목 추가
    fileMenu->addAction(tr("끝내기(&x)"), this, SLOT(close()),
                        QKeySequence(tr("Ctrl+Q")));

    // '파일' 메뉴 추가
    menuBar()->addMenu(fileMenu);
}

/**
 * @brief 위젯을 초기화한다
 */
void Tetris::initWidgets()
{
    // 테트리스 판을 센트럴 위젯으로
    _board = new Board;
    setCentralWidget(_board);

    // 메인 창 크기 고정
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    // 테트리스 판에 입력 포커스 설정
    _board->setFocus();
}

/**
 * @brief 새 게임을 시작한다
 */
void Tetris::newGame()
{
    _board->newGame();
}

// .cpp 소스 내부의 클래스에서 시그널/슬롯을 쓰기 위해
#include "tetris.moc"
