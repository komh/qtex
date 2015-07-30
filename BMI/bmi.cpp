/****************************************************************************
**
** bmi.cpp
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of BMI.
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

#include "bmi.h"

#include <QtWidgets>

BMI::BMI(QWidget *parent)
    : QMainWindow(parent)
{
    initWidgets();
    initActions();
}

BMI::~BMI()
{
    // 생성된 위젯들은 자동으로 해제됨
}

// 체질량 계산
void BMI::calcBMI()
{
    float height;
    float mass;
    float bmi;
    QString result;

    // 체질량 계산
    height = _heightLine->text().toFloat() / 100;   // 미터로 환산
    mass = _massLine->text().toFloat();
    bmi = mass / height / height;   // 체질량 = 몸무게(kg) / 키(m)^2

    // 체질량에 따라 비만도 판정
    // 대한비만학회 기준,
    // https://ko.wikipedia.org/wiki/%EC%B2%B4%EC%A7%88%EB%9F%89_%EC%A7%80%EC%88%98
    if (bmi < 18.5)
        result = tr("저체중");
    else if (bmi < 23)
        result = tr("정상");
    else if (bmi < 30)
        result = tr("경도 비만");
    else if (bmi < 35)
        result = tr("중등도 비만");
    else
        result = tr("고도 비만");

    // 비만도에 BMI 추가. BMI 는 소숫점 아래 1 자리까지
    result.append(tr("(%1)").arg(bmi, 0, 'f', 1));  // 소숫점 아래 1 자리

    // 결과 출력
    _resultLine->setText(result);
}

// Widget 초기화
void BMI::initWidgets()
{
    // 키 입력창 생성
    _heightLine = new QLineEdit;

    // 몸무게 입력창 생성
    _massLine = new QLineEdit;

    // 계산 푸시 버튼 생성
    _calcPush = new QPushButton(tr("계산하기(&C)"));

    // 결과 입력창 생성
    _resultLine = new QLineEdit;
    _resultLine->setReadOnly(true); // 읽기 전용

    // 폼 레이아웃 생성
    _formLayout = new QFormLayout;
    _formLayout->addRow(tr("키(c&m)"), _heightLine);
    _formLayout->addRow(tr("몸무게(&kg):"), _massLine);
    _formLayout->addWidget(_calcPush);
    _formLayout->addRow(tr("결과:"), _resultLine);

    // 센트럴 위젯을 생성 및 설정
    setCentralWidget(new QWidget);

    // 센트럴 위젯의 레이아웃을 설정
    centralWidget()->setLayout(_formLayout);
}

// Action 초기화
void BMI::initActions()
{
    connect(_calcPush, SIGNAL(clicked(bool)), this, SLOT(calcBMI()));
}
