/****************************************************************************
**
** calc.h
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

/*! \file calc.h
 */

#ifndef CALC_H
#define CALC_H

#include <QMainWindow>
#include <QtWidgets>

/*!
 * \brief 계산기 클래스
 */

class Calc : public QMainWindow
{
    Q_OBJECT

public:
    Calc(QWidget *parent = 0);
    ~Calc();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    /*!
     * \brief 연산 모드
     */
    enum OperationMode {None = 0,   //!< 초기 상태
                        Plus,       //!< 더하기
                        Minus,      //!< 빼기
                        Multiply,   //!< 곱하기
                        Divide,     //!< 나누기
                       };

    static const int CalcRows = 5;  //!< 계산기 레이아웃 세로줄
    static const int CalcCols = 5;  //!< 계산기 레이아웃 가로줄

    float _ans;             //!< 계산 결과
    float _operand;         //!< 마지막 피연산자
    OperationMode _opMode;  //!< 연산 모드
    bool _freezed;          //!< 계산 결과가 산출된 상태

    QLineEdit *_exprLine;           //!< 수식
    QList<QPushButton *> _buttons;  //!< 버튼들

    void initMenus();
    void initWidgets();
    void initConnections();
    void calculate(const QString &s);

private slots:
    void buttonClicked();
};

#endif // CALC_H
