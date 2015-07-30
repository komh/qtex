/****************************************************************************
**
** baseball.h
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

#ifndef BASEBALL_H
#define BASEBALL_H

#include <QMainWindow>
#include <QtWidgets>

class Baseball : public QMainWindow
{
    Q_OBJECT

public:
    Baseball(QWidget *parent = 0);
    ~Baseball();

private:
    static const int MAX_ANSWER_COUNT = 3;

    QList<QLineEdit *> _numLineList;
    QPushButton *_enterPush;
    QPushButton *_newPush;

    QListWidget *_historyList;

    QList<int> _answerList;

    int _tryCount;

    void initWidgets();
    void initActions();
    void initAnswer();

private slots:
    void checkAnswer();
    void newBaseball();
};

#endif // BASEBALL_H
