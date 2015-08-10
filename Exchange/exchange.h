/****************************************************************************
**
** exchange.h
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

/** @file exchange.h
 */

#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtWebKitWidgets>

/**
 * @brief 환율 계산기 클래스
 */
class Exchange : public QMainWindow
{
    Q_OBJECT

public:
    Exchange(QWidget *parent = 0);
    ~Exchange();

private:
    QAction *_webPageViewAction;    ///< 웹페이지 보기 액션

    QComboBox *_fromCombo;  ///< 바꿀 나라
    QLineEdit *_fromLine;   ///< 바꿀 금액

    QComboBox *_toCombo;    ///< 바뀐 나라
    QLineEdit *_toLine;     ///< 바뀐 금액

    QWebView *_web;                 ///< 웹 보기/편집 위젯
    QProgressDialog *_progressDlg;  ///< 진행 상태 대화상자

    void initMenus();
    void initWidgets();
    void initConnections();

private slots:
    void webLoadFinished(bool ok);
    void webPageViewToggled(bool checked);
    void exchange();
};

#endif // EXCHANGE_H
