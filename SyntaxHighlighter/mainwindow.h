/****************************************************************************
**
** mainwindow.h
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of SyntaxHighlighter.
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

/** @file mainwindow.h
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtWidgets>

/**
 * @brief SyntaxHighliter 클래스
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QTextEdit *_plainText;          /// 원본 텍스트
    QTextEdit *_syntaxText;         /// 문법 강조된 텍스트
    QPushButton *_highlightButton;  /// 문법 강조 실행 버튼

    void initMenus();
    void initWidgets();
    QString nextToken(const QString &s, const int start, int *next);

private slots:
    void plainTextChanged();
    void syntaxHighlight();
};

#endif // MAINWINDOW_H
