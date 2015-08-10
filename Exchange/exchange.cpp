/****************************************************************************
**
** exchange.cpp
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Exchange Currency.
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

/** @file exchange.cpp
 */

#include "exchange.h"

#include <QtWidgets>

/**
 * @brief 생성자
 * @param parent 부모 위젯
 */
Exchange::Exchange(QWidget *parent)
    : QMainWindow(parent)
{
    initMenus();        // 메뉴 초기화
    initWidgets();      // 위젯 초기화
    initConnections();  // 시그널, 슬롯 연결

    // 윕페이지를 읽는다
    _web->load(QUrl("http://search.daum.net/search?w=tot&q=%ED%99%98%EC%9C%A8"));
}

Exchange::~Exchange()
{
}

void Exchange::initMenus()
{
    // "파일" 메뉴 생성
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));
    // "끝내기" 액션 추가
    fileMenu->addAction(tr("끝내기(&x)"), qApp, SLOT(quit()),
                        QKeySequence(tr("Ctrl+Q")));

    // "웹페이지 보기" 액션 생성
    _webPageViewAction = new QAction(tr("웹페이지 보기(&W)"), this);
    // 체크 상태 표시
    _webPageViewAction->setCheckable(true);
    // 처음에는 체크하지 않는다
    _webPageViewAction->setChecked(false);

    // "보기" 메뉴 생생
    QMenu *viewMenu = new QMenu(tr("보기(&V)"));
    // "웹페이지 보기" 액션 추가
    viewMenu->addAction(_webPageViewAction);

    // 메뉴바에 추가
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
}

/**
 * @brief 위젯을 초기화한다
 */
void Exchange::initWidgets()
{
    // 바꿀 나라 콤보 박스 생성
    _fromCombo = new QComboBox;
    // 내용에 맞게 크기 조정
    _fromCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    // 가로는 최소 크기 보존
    _fromCombo->setSizePolicy(QSizePolicy::Minimum,
                              _fromCombo->sizePolicy().verticalPolicy());

    // 바꿀 금액 라인 에디터 생성
    _fromLine = new QLineEdit;

    // 바뀐 나라 콤보 박스 생성
    _toCombo = new QComboBox;
    // 내용에 맞게 크기 조정
    _toCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    // 가로는 최소 크기 보존
    _toCombo->setSizePolicy(QSizePolicy::Minimum,
                            _toCombo->sizePolicy().verticalPolicy());

    // 바뀐 금액 라인 에디터 생성
    _toLine = new QLineEdit;

    // 그리드 레이아웃 생성
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(_fromCombo, 0, 0);
    gridLayout->addWidget(_fromLine, 1, 0);
    gridLayout->addWidget(new QLabel(tr("=")), 0, 1, 2, 1);
    gridLayout->addWidget(_toCombo, 0, 2);
    gridLayout->addWidget(_toLine, 1, 2);

    // 센트럴 위젯 생성
    QWidget *w = new QWidget;
    w->setLayout(gridLayout);

    // 센트럴 위젯 설정
    setCentralWidget(w);

    // 웹 보기/편집 위젯 생성
    _web = new QWebView(this);
    // 가로/세로 최소 크기 보존
    _web->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    // 보이기 상태 설정
    _web->setVisible(_webPageViewAction->isChecked());

    // 진행 상태 대화 상자 생성
    _progressDlg = new QProgressDialog(this);
    // 부모 윈도우 입력 막음
    _progressDlg->setWindowModality(Qt::WindowModal);
    // 출력 메세지 설정
    _progressDlg->setLabelText(tr("준비중입니다..."));
    // 3초 동안 완료되지 않으면 대화 상자 표시
    _progressDlg->setMinimumDuration(3000);
}

/**
 * @brief 시그널, 슬롯 연결
 */
void Exchange::initConnections()
{
    // "윕페이지 보기" 상태가 달라지면 wbPageViewToggled() 호출
    connect(_webPageViewAction, SIGNAL(toggled(bool)),
            this, SLOT(webPageViewToggled(bool)));

    // 바꿀 나라가 바뀌면 exchange() 호출
    connect(_fromCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(exchange()));

    // 바꿀 금액이 바뀌면 exchange() 호출
    connect(_fromLine, SIGNAL(textChanged(QString)),
            this, SLOT(exchange()));

    // 바뀐 나라가 바뀌면 exchange() 호출
    connect(_toCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(exchange()));

    // 바뀐 금액이 바뀌면 exchange() 호출
    connect(_toLine, SIGNAL(textChanged(QString)),
            this, SLOT(exchange()));

    // 웹 페이지 읽기 진행 상태를 진행 상태 대화 상자에 연결
    connect(_web, SIGNAL(loadProgress(int)),
            _progressDlg, SLOT(setValue(int)));

    // 웹 페이지를 모두 읽으면 webLoadFinished() 호출
    connect(_web, SIGNAL(loadFinished(bool)),
            this, SLOT(webLoadFinished(bool)));
}

/**
 * @brief 웹 페이지를 모두 읽으면 호출된다. 위젯들의 내용물을 초기화
 * @param ok 웹페이지를 읽었으면 true, 아니면 false
 */
void Exchange::webLoadFinished(bool ok)
{
    // 웹 페이지를 읽지 못했으면
    if (!ok)
    {
        QMessageBox::critical(this, qApp->applicationName(),
                              tr("웹페이지를 읽지 못했습니다."));

        return;
    }

    // 현재 메인 프레임
    QWebFrame *frame = _web->page()->mainFrame();
    // 현재 문서 요소
    QWebElement document = frame->documentElement();

    // 나라 목록을 얻는다
    QVariant var = document.evaluateJavaScript("                    \
        var result = [];                                            \
        var list =  daum.$C(daum.$('exchangeColl'), 'select_list'); \
        for (var i = 0; i < list.length / 2; i++)                   \
            result.push(list[i].innerHTML);                         \
        result;"
    );

    // span 태그를 제거할 정규식 생성
    QRegExp rx("(.*)<span.*>(.*)</span>");

    // 나라 목록을 문자열 목록으로 변환
    QStringList countries(var.toStringList());
    Q_FOREACH (QString country, countries)
    {
        // 정규식에 대입
        rx.indexIn(country);

        // span 태그 제거
        QString item(rx.cap(1) + " " + rx.cap(2));

        // 나라 목록 추가
        _fromCombo->addItem(item);
        _toCombo->addItem(item);
    }


    // 바꿀 나라 선택(1 = 미국)
    _fromCombo->setCurrentIndex(1);
    // 바꿀 금액으로 입력 촛점 설정
    _fromLine->setFocus();
    // 바꿀 금액 설정
    _fromLine->setText("1");
    // 바뀐 나라 선택(0 = 대한민국)
    _toCombo->setCurrentIndex(0);

    // 환율 계산
    exchange();
}

/**
 * @brief "웹페이지 보기" 상태가 달라지면 호출된다. 상태에 따라 웹페이지 표지
 * @param checked 체크되었으면 true, 아니면 false
 */
void Exchange::webPageViewToggled(bool checked)
{
    // 웹페이지 표시 전 크기
    static QSize compactSize;

    // 센트럴 위젯의 레이아웃
    QGridLayout *layout =
            qobject_cast<QGridLayout *>(centralWidget()->layout());

    if (checked)                                // 체크되었으면
    {
        compactSize = size();                   // 현재 크기 저장
        layout->addWidget(_web, 2, 0, 1, 3);    // 웹 위젯 추가
    }
    else                                        // 아니면
    {
        layout->removeWidget(_web);             // 위젯 제거
        setMinimumSize(0, 0);                   // 창 최소 크기 (0, 0) 으로
        resize(compactSize);                    // 웹 표시 이전으로 크기 조정
    }

    // 웹 표시 상태 바꾸기
    _web->setVisible(checked);
}

/**
 * @brief 환율을 계산한다
 */
void Exchange::exchange()
{
    // 실제 바꿀 금액/바뀐 금액 초기화
    // 실제 바꿀 금액
    static QLineEdit *fromAmountLine = _fromLine;
    // 실제 바꿀 금액 JavaScript 요소 이름
    static QString fromAmountJS = "extFromMoney";
    // 실제 바뀐 금액
    static QLineEdit *toAmountLine = _toLine;
    // 실제 바뀐 금액 JavaScript 요소 이름
    static QString toAmountJS = "extToMoney";

    // 입력 촛점이 _fromLine 이라면
    if (QApplication::focusWidget() == _fromLine)
    {
        // 바꿀 금액/바뀐 금액 원래대로
        fromAmountLine = _fromLine;
        fromAmountJS = "extFromMoney";
        toAmountLine = _toLine;
        toAmountJS = "extToMoney";
    }
    else if (QApplication::focusWidget() == _toLine)
    {
        // 바꿀 금액/바뀐 금액 바꾸기
        fromAmountLine = _toLine;
        fromAmountJS = "extToMoney";
        toAmountLine = _fromLine;
        toAmountJS = "extFromMoney";
    }

    // 현재 프레임
    QWebFrame *frame = _web->page()->mainFrame();
    // 현재 문서
    QWebElement document = frame->documentElement();

    // 웹의 바뀔 나라 설정
    document.evaluateJavaScript(QString("ExfromExchange.selectIdx(%1)")
                                    .arg(_fromCombo->currentIndex()));
    // 웹의 바뀐 나라 설정
    document.evaluateJavaScript(QString("ExtoExchange.selectIdx(%1)")
                                    .arg(_toCombo->currentIndex()));

    // 웹의 바꿀 금액을 설정하고, 계산
    document.evaluateJavaScript(QString("           \
        %1.value = '%2';                            \
        var e = document.createEvent('HTMLEvents'); \
        e.initEvent('input', true, true);           \
        %1.dispatchEvent(e);"
    ).arg(fromAmountJS).arg(fromAmountLine->text()));

    // 웹의 바뀐 금액 저장
    QVariant var = document.evaluateJavaScript(toAmountJS + ".value");
    // 바뀐 금액 설정
    toAmountLine->setText(var.toString());
}
