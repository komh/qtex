/****************************************************************************
**
** plot.h
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Plot.
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

#ifndef PLOT_H
#define PLOT_H

#include <QMainWindow>

#include <QtWidgets>

/**
 * @brief 그래프 위젯
 */
class GraphWidget : public QWidget
{
public:
    GraphWidget(QWidget *parent = 0);

    void setAxisFixed(bool fixed);
    void setPoly(const QString &poly);
    void setRange(float start, float end);

protected:
    void paintEvent(QPaintEvent *e);

private:
    bool _axisFixed;    ///< 좌표축 고정 상태
    QString _poly;      ///< 다항식
    float _start;       ///< 시작값
    float _end;         ///< 끝값
};

/**
 * @brief 그래프 그리기
 */
class Plot : public QMainWindow
{
    Q_OBJECT

public:
    Plot(QWidget *parent = 0);
    ~Plot();

private:
    QLineEdit *_polyLine;           ///< 다항식 편집기
    QLineEdit *_startLine;          ///< 시작값 편집기
    QLineEdit *_endLine;            ///< 끝값 편집기
    QPushButton *_drawGraphPush;    ///< 그래프 그리기 버튼
    GraphWidget *_graph;            ///< 그래프 위젯

    void initMenus();
    void initWidgets();

private slots:
    void axisFixed(bool checked);
    void drawGraph();
};

#endif // PLOT_H
