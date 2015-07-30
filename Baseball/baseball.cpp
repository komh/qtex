/****************************************************************************
**
** baseball.cpp
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Baseball.
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

#include "baseball.h"

#include <QtWidgets>

Baseball::Baseball(QWidget *parent)
    : QMainWindow(parent),
      _tryCount(0)
{
    initWidgets();
    initActions();

    newBaseball();
}

Baseball::~Baseball()
{
}

void Baseball::initWidgets()
{
    // 0..9 만 옳다
    QIntValidator *numValidator = new QIntValidator(0, 9, this);

    while (_numLineList.count() < MAX_ANSWER_COUNT)
    {
        QLineEdit *lineEdit = new QLineEdit;

        lineEdit->setMaxLength(1);              // 최대 1 문자
        lineEdit->setValidator(numValidator);   // 0..9 만 허용

        _numLineList.append(lineEdit);
    }

    _enterPush = new QPushButton(tr("입력(&E)"));
    _newPush = new QPushButton(tr("새로(&N)"));

    // 수평으로 나열
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(_numLineList.at(0));
    hboxLayout->addStretch();
    hboxLayout->addWidget(_numLineList.at(1));
    hboxLayout->addStretch();
    hboxLayout->addWidget(_numLineList.at(2));
    hboxLayout->addStretch();
    hboxLayout->addWidget(_enterPush);
    hboxLayout->addWidget(_newPush);

    _historyList = new QListWidget;

    // 수직으로 나열
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addLayout(hboxLayout);
    vboxLayout->addWidget(_historyList);

    // 센트럴 위젯 설정
    setCentralWidget(new QWidget);

    // 레이아웃 설정
    centralWidget()->setLayout(vboxLayout);
}

void Baseball::initActions()
{
    // 사용자가 "입력" 버튼을 클릭하면 checkAnswer() 호출
    connect(_enterPush, SIGNAL(clicked(bool)), this, SLOT(checkAnswer()));
    // 사용자가 "새로" 버튼을 클릭하면 newBaseball() 호출
    connect(_newPush, SIGNAL(clicked(bool)), this, SLOT(newBaseball()));
}

void Baseball::initAnswer()
{
    // _answerList 초기화
    _answerList.clear();

    // random seed 초기화
    qsrand(QTime::currentTime().msecsSinceStartOfDay());

    // 숫자 생성
    while (_answerList.count() < MAX_ANSWER_COUNT)
    {
        int num;

        do
        {
            num = qrand() * 9 / RAND_MAX;           // 0..9 숫자 생성
        } while (_answerList.indexOf(num) != -1);   // 같은 숫자가 있으면

        // 숫자 저장
        _answerList.append(num);
    }
}

void Baseball::checkAnswer()
{
    int emptyIndex = -1;

    // 비어 있는 것이 있는지 확인
    Q_FOREACH (QLineEdit *lineEdit, _numLineList)
    {
        if (lineEdit->text().isEmpty())
        {
            emptyIndex = _numLineList.indexOf(lineEdit);
            break;
        }
    }

    // 비어 있는 것이 있으면
    if (emptyIndex != -1)
    {
        QMessageBox::warning(this, qApp->applicationName(),
                             tr("%1번째 숫자가 입력되지 않았습니다.")
                                .arg(emptyIndex + 1));

        // 입력 촛점 설정
        _numLineList.at(emptyIndex)->setFocus();
        // 텍스트 전체 선택
        _numLineList.at(emptyIndex)->selectAll();

        return;
    }

    QList<int> inputNumList;

    // 입력된 숫자를 저장
    Q_FOREACH (QLineEdit *lineEdit, _numLineList)
        inputNumList.append(lineEdit->text().toInt());

    // 겹친 숫자가 있는지 확인
    Q_FOREACH (int num, inputNumList)
    {
        if (inputNumList.count(num) > 1)
        {
            QMessageBox::warning(this, qApp->applicationName(),
                                 tr("겹친 숫자(%1)가 있습니다.")
                                 .arg(num));

            int index = inputNumList.indexOf(num);
            // 입력 촛점 설정
            _numLineList.at(index)->setFocus();
            // 텍스트 전체 선택
            _numLineList.at(index)->selectAll();

            return;
        }
    }

    int strike = 0;
    int ball = 0;

    // 스트라이크, 볼 판단
    Q_FOREACH (int num, inputNumList)
    {
        int answerIndex = _answerList.indexOf(num);

        // 답이 있나 ?
        if (answerIndex != -1)
            answerIndex == inputNumList.indexOf(num) ?
                        strike++ :  // 같은 위치면 스트라이크
                        ball++;     // 아니면 볼
    }

    QString result;

    // 시도 횟수
    result.append(tr("%1 번째(").arg(++_tryCount));

    // 입력한 숫자들
    Q_FOREACH (QLineEdit *lineEdit, _numLineList)
        result.append(lineEdit->text()).append(" ");
    result.chop(1);     // 마지막 한 문자(빈칸) 삭제

    // 판정 결과
    result.append(tr(") : %1s %2b").arg(strike).arg(ball));

    // 입력 목록 첫번째 줄에 삽입
    _historyList->insertItem(0, result);

    if (strike == MAX_ANSWER_COUNT)
    {
        QString msg;

        msg.append(tr("%1 번째만에 맞혔습니다.\n").arg(_tryCount));
        msg.append(tr("정답: "));
        Q_FOREACH (QLineEdit *lineEdit, _numLineList)
            msg.append(lineEdit->text()).append(" ");
        msg.chop(1);

        QMessageBox::information(this, qApp->applicationName(), msg);

        newBaseball();
    }

    // 첫번째 숫자로 촛점 이동
    _numLineList.at(0)->setFocus();
    // 텍스트 전체 선택
    _numLineList.at(0)->selectAll();
}

void Baseball::newBaseball()
{
    // 입력 숫자 지우기
    Q_FOREACH (QLineEdit *lineEdit, _numLineList)
        lineEdit->clear();

    // 입력 목록 지우기
    _historyList->clear();

    // 촛점 이동
    _numLineList.at(0)->setFocus();

    // 시도 횟수 초기화
    _tryCount = 0;

    // 숫자 생성
    initAnswer();
}
