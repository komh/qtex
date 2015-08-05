/****************************************************************************
**
** calc.cpp
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Calculator.
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

/*! \file calc.cpp
 */

#include "calc.h"

#include <QtWidgets>

/*!
 * \brief 생성자
 * \param parent 부모 위젯
 */
Calc::Calc(QWidget *parent)
    : QMainWindow(parent)
    , _ans(0)
    , _operand(0)
    , _opMode(None)
    , _freezed(false)
{
    initMenus();        // 메뉴 초기화
    initWidgets();      // 위젯 초기화
    initConnections();  // 시그널과 슬롯 연결
}

/*!
 * \brief 소멸자
 */
Calc::~Calc()
{
}

static const QString Backspace("←");    //!< 마지막 문자 지우기
static const QString Negation("±");     //!< 부호 바꾸기
static const QString SquareRoot("√");   //!< 제곱근

/*!
 * \brief 키보드 입력 이벤트를 처리한다
 * \param e 키보드 입력 이벤트
 */
void Calc::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
    case Qt::Key_Period:
    case Qt::Key_Backspace:
    case Qt::Key_Plus:
    case Qt::Key_Minus:
    case Qt::Key_Asterisk:
    case Qt::Key_Slash:
    case Qt::Key_Percent:
    case Qt::Key_Equal:
    case Qt::Key_Enter:
    case Qt::Key_Return:
    {
        QString text;

        // 입력된 키를 문자열로 전환
        if (e->key() == Qt::Key_Backspace)
            text = Backspace;
        else if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
            text = "=";
        else
            text = e->text();

        calculate(text);
        break;
    }

    default:
        // 처리되지 않은 것은 부모 클래스에 전달
        QMainWindow::keyPressEvent(e);

        return;
    }

    // 처리 완료
}

/*!
 * \brief 메뉴를 초기화한다
 */
void Calc::initMenus()
{
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));
    fileMenu->addAction(tr("끝내기(&x)"), qApp, SLOT(quit()),
                        QKeySequence(tr("Ctrl+Q")));

    menuBar()->addMenu(fileMenu);
}

/*!
 * \brief 위젯 초기화
 */
void Calc::initWidgets()
{
    // 수식 생성
    _exprLine = new QLineEdit;
    // 읽기 전용
    _exprLine->setReadOnly(true);
    // 입력 촛점 받지 않음
    _exprLine->setFocusPolicy(Qt::NoFocus);
    // 수평 오른쪽 정렬, 수직 가운데 정렬
    _exprLine->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    _exprLine->setText(QString::number(_ans));

    // 버튼 생성
    _buttons.append(new QPushButton(Backspace));
    _buttons.append(new QPushButton("CE"));
    _buttons.append(new QPushButton("C"));
    _buttons.append(new QPushButton(Negation));
    _buttons.append(new QPushButton(SquareRoot));

    _buttons.append(new QPushButton("7"));
    _buttons.append(new QPushButton("8"));
    _buttons.append(new QPushButton("9"));
    _buttons.append(new QPushButton("/"));
    _buttons.append(new QPushButton("%"));

    _buttons.append(new QPushButton("4"));
    _buttons.append(new QPushButton("5"));
    _buttons.append(new QPushButton("6"));
    _buttons.append(new QPushButton("*"));
    _buttons.append(new QPushButton("1/x"));

    _buttons.append(new QPushButton("1"));
    _buttons.append(new QPushButton("2"));
    _buttons.append(new QPushButton("3"));
    _buttons.append(new QPushButton("-"));
    _buttons.append(new QPushButton("="));

    _buttons.append(new QPushButton("0"));
    _buttons.append(new QPushButton("."));
    _buttons.append(new QPushButton("+"));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(_exprLine, 0, 0, 1, CalcCols);

    int row = 1, col = 0;

    for (int i = 0; i < _buttons.count(); ++i)
    {
        QPushButton *button = _buttons.at(i);
        // 크기 정책 설정, 최소 크기는 보존하되, 확장은 자유
        button->setSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::MinimumExpanding);
        // 입력 촛점 받지 않음
        button->setFocusPolicy(Qt::NoFocus);

        // "=" 는 세로 두 칸 차지
        int rowSpan = 1 + (button->text() == "=");
        // "0" 은 가로 두 칸 차지
        int colSpan = 1 + (button->text() == "0");

        gridLayout->addWidget(button, row, col, rowSpan, colSpan);

        col = (col + colSpan) % CalcCols;
        row += col == 0;
    }

    // 기본 위젯 생성
    QWidget *w = new QWidget;
    w->setLayout(gridLayout);

    // 센트럴 위젯 설정
    setCentralWidget(w);
}

/*!
 * \brief 시그널과 슬롯 연결
 */
void Calc::initConnections()
{
    // 모든 버튼의 clicked() 시그널을 buttonClicked() 슬롯에 연결
    Q_FOREACH (QPushButton *button, _buttons)
        connect(button, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
}

/*!
 * \brief \a s 에 따라 계산한다
 * \param s 숫자 또는 명령어
 */
void Calc::calculate(const QString &s)
{
    if (s.length() == 1 && (s.at(0).isNumber() || s == "." || s == Backspace))
    {
        // 수식 편집

        // 수식의 형태는 숫자[ 연산자 [숫자]]
        QString expr(_exprLine->text());

        if (s == Backspace)
        {
            if (!_freezed && !expr.endsWith(' '))
            {                       // 산출된 상태가 아니고 숫자 입력 중이면
                expr.chop(1);       // 마지막 문자 지움
                if (expr.isEmpty()) // 모두 지워졌으면
                    expr = "0";     // "0" 으로
            }
        }
        else
        {
            if (_freezed)           // 산출된 상태면
            {
                expr.clear();       // 기존의 수식 지움
                _freezed = false;   // 산출되지 않음
            }

            if (s == ".")
            {
                // 두 개 이상의 "." 은 허용 안됨
                if (!expr.contains('.'))
                {
                    if (expr.isEmpty()) // "." 부터 입력되었으면
                        expr = "0";     // "0." 으로

                    expr.append('.');
                }
            }

            // 맨 앞자리의 "0" 은 하나만 허용
            if (expr != "0" || s != "0")
            {
                if (expr == "0")    // 수식이 "0" 이면
                    expr.clear();   // 수식 지움

                // 문자열 추가
                expr.append(s);
            }
        }

        // 수식 설정
        _exprLine->setText(expr);
    }
    else
    {
        // 연산 처리

        // 수식의 형태는 숫자[ 연산자 [숫자]]
        QString expr(_exprLine->text());
        // 수식을 공백을 기준으로 토큰 분리
        QStringList tokens(expr.split(' '));

        bool ok;
        // 현재 입력 숫자
        float num = tokens.last().toFloat(&ok);

        if (s == "C")
        {
            // 계산 새로 시작
            _ans = 0;
            _operand = 0;
            _opMode = None;
            _freezed = false;
            _exprLine->setText("0");

            return;
        }

        if (!ok)    // 마지막 토큰이 숫자가 아니면
            return;

        if (s == "CE")
        {
            tokens.last().clear();      // 현재 수식 지우기
            expr = tokens.join(' ');    // 토큰사이를 공백으로 연결
            if (expr.isEmpty())         // 모든 수식이 지워졌으면
                expr = "0";             // "0" 으로

            _exprLine->setText(expr);

            return;
        }
        else if (s == Negation || s == SquareRoot || s == "%" || s == "1/x")
        {
            if (s == Negation)
            {
                if (num != 0)   // 0 은 부호가 없으니까
                    num *= -1;
            }
            else if (s == SquareRoot)
            {
                if (num > 0)            // 양수일 때만
                    num = sqrt(num);    // 제곱근 계산
            }
            else if (s == "%")
                num = _ans * num / 100;
            else /* if (s == "1/x") */
                num = 1 / num;

            // 현재 입력 숫자 설정
            tokens.last().setNum(num);

            // 수식 설정
            _exprLine->setText(tokens.join(' '));

            return;
        }

        // 연산자 처리

        if (tokens.count() == 1)    // 숫자만 있으면
        {
            if (s == "=")           // "=" 이면
                _ans = _operand;    // 기존의 계산 반복
            else                    // 아니면
            {                       // 계산 새로 시작
                _ans = 0;
                _operand = 0;
                _opMode = None;
                _freezed = false;
            }
        }

        // 연산 수행
        switch (_opMode)
        {
        case None:          // 초기화 상태면
            _ans = num;     // 수식 저장
            break;

        case Plus:
            _ans += num;
            break;

        case Minus:
            _ans -= num;
            break;

        case Multiply:
            _ans *= num;
            break;

        case Divide:
            _ans /= num;
            break;
        }

        // 연산자 토큰으로 바꾸기
        QString opStr;
        opStr.append(" ");
        opStr.append(s);
        opStr.append(" ");

        // 산출되지 않음
        _freezed = false;

        if (s == "+")
            _opMode = Plus;
        else if (s == "-")
            _opMode = Minus;
        else if (s == "*")
            _opMode = Multiply;
        else if (s == "/")
            _opMode = Divide;
        else /*if (s == "=")*/
        {
            // 연산자 토큰 지우기
            opStr.clear();

            if (tokens.count() > 1) // 숫자 연산자 숫자 형태이면
                _operand = num;     // 마지막 숫자 저장

            // 산출된 상태
            _freezed = true;
        }

        // 토큰을 수직으로 바꾸기
        expr.setNum(_ans);
        expr.append(opStr);

        // 수식 설정
        _exprLine->setText(expr);
    }
}

void Calc::buttonClicked()
{
    // 시그널을 보낸 버튼의 텍스트를 calculate() 에 전달
    calculate(qobject_cast<QPushButton *>(sender())->text());
}
