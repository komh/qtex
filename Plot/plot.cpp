/****************************************************************************
**
** plot.cpp
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

#include "plot.h"

/**
 * @brief 다항식 계산기
 */
class PolyCalc
{
public:
    /**
     * @brief PolyCalc 생성자
     * @param 다항식
     */
    explicit PolyCalc(const QString &poly)
        : _poly(poly)
        , _pos(0)
        , _x(0)
    {
    }

    /**
     * @brief x 값을 설정한다
     * @param x x 로 설정할 값
     */
    void setX(float x)
    {
        _x = x;
    }

    /**
     * @brief 다항식을 계산한다
     * @return 계산 결과를 돌려준다
     */
    float calc()
    {
        _pos = 0;

        return level1(getNumber());
    }

private:
    QString _poly;  ///< 다항식
    int _pos;       ///< 현재 파싱 위치
    float _x;       ///< x 값

    /**
     * @brief 토큰을 읽지만, 파싱 위치 바꾸지 않는다
     * @return 현재 토큰
     */
    inline QString peekToken()
    {
        return token(false);
    }

    /**
     * @brief 토큰을 읽고, 파싱 위치를 바꾼다
     * @return 현재 토큰
     */
    inline QString nextToken()
    {
        return token(true);
    }

    /**
     * @brief 수를 읽는다
     * @return 현재 토큰이 숫자이면, 해당 숫자를 돌려주고, 아니면 0 을 돌려준다.
     * @remark 숫자를 읽었으면 파싱 위치를 옮기고, 아니면 옮기지 않는다.
     */
    inline float getNumber()
    {
        bool ok;
        float num = peekToken().toFloat(&ok);
        if (ok)
            nextToken();
        else
            num = 0;

        return num;
    }

    /**
     * @brief 토큰을 읽는다
     * @param next true 이면 파싱 위치를 바꾸고, false 이면 바꾸지 않는다
     * @return 현재 토큰
     */
    QString token(bool next)
    {
        // 현재 위치 저장
        int savedPos = _pos;

        QString result;

        // 공백 문자 무시
        while (_pos < _poly.length() && _poly.at(_pos).isSpace())
            _pos++;

        if (_pos < _poly.length())
        {
            QChar ch = _poly.at(_pos++);

            if (ch.isNumber())
            {
                // 숫자 파싱
                do
                {
                    result.append(ch);

                    if (_pos >= _poly.length())
                        break;

                    ch = _poly.at(_pos++);
                } while (ch.isNumber());

                if (!ch.isNumber())
                    _pos--;
            }
            else if (ch.isLetter())
            {
                // 문자 파싱
                do
                {
                    result.append(ch);

                    if (_pos >= _poly.length())
                        break;

                    ch = _poly.at(_pos++);
                } while (ch.isLetter());

                if (!ch.isLetter())
                    _pos--;
            }
            else
            {
                // 나머지는 그대로 추가
                result.append(ch);
            }
        }

        // peek 모드이면 파싱 위치 복원
        if (!next)
            _pos = savedPos;

        // x 대체
        if (result == "x")
            result = QString::number(_x);

        return result;
    }

    /**
     * @brief 더하기/빼기를 계산한다
     * @param lv 왼쪽값
     * @return 계산 결과를 돌려준다
     */
    float level1(float lv)
    {
        QString op(peekToken());

        if (op == "+" || op == "-")
        {
            nextToken();

            // 오른쪽 값 계산
            float rv = level2(getNumber());

            // 더하기/빼기 수행
            if (op == "+")
                lv += rv;
            else /*if (op == "-") */
                lv -= rv;
        }
        else if (!op.isEmpty()) // 더하기/빼기가 아니면
            lv = level2(lv);    // 더 높은 우선순위로 넘김

        // 더하기/빼기가 이어지면 계속 계산
        op = peekToken();
        if (op == "+" || op == "-")
            return level1(lv);

        return lv;
    }

    /**
     * @brief 곱하기/나누기를 계산한다
     * @param lv 왼쪽값
     * @return 계산 결과를 돌려준다
     */
    float level2(float lv)
    {
        QString op(peekToken());

        if (op == "*" || op == "/")
        {
            nextToken();

            // 오른쪽 값 계산
            float rv = level3(getNumber());

            // 곱하기/나누기 수행
            if (op == "*")
                lv *= rv;
            else /*if (op == "/") */
                lv /= rv;
        }
        else if (!op.isEmpty()) // 곱하기/나누기가 아니면
            lv = level3(lv);    // 더 높은 우선 순위로 넘김

        // 곱하기/나누기가 이어지면 계속 계산
        op = peekToken();
        if (op == "*" || op == "/")
            return level2(lv);

        return lv;
    }

    /**
     * @brief 거듭제곱을 계산한다
     * @param lv
     * @return
     */
    float level3(float lv)
    {
        QString op(peekToken());

        if (op == "^")
        {
            nextToken();

            // 오른쪽 값 계산
            float rv = level4(getNumber());

            // 거듭제곱 수행
            lv = pow(lv, rv);
        }
        else if (!op.isEmpty()) // 거듭제곱이 아니면
            lv = level4(lv);    // 더 높은 우선 순위로 넘김

        // 거듭제곱이 이어지면 계속 계산
        op = peekToken();
        if (op == "^")
            return level3(lv);

        return lv;
    }

    /**
     * @brief 괄호를 계산한다
     * @param lv 왼쪽값
     * @return 계산 결과를 돌려준다
     */
    float level4(float lv)
    {
        QString op(peekToken());

        if (op == "(")
        {
            nextToken();

            // 괄호를 계산한다
            lv = level1(getNumber());

            // 괄호 대응여부 확인
            if (nextToken() != ")")
                qDebug() << "Missing ')'";
        }

        return lv;
    }
};


/**
 * @brief 그래프 위젯
 */
class GraphWidget : public QWidget
{
public:
    /**
     * @brief GraphWidget 생성자
     * @param parent 부모 위젯
     */
    GraphWidget(QWidget *parent = 0)
            : QWidget(parent)
            , _axisFixed(false)
            , _start(0)
            , _end(0)
    {
    }

    /**
     * @brief 좌표축 고정 상태 설정
     * @param fixed true 이면 고정되고, false 이면 고정되지 않음
     */
    void setAxisFixed(bool fixed)
    {
        _axisFixed = fixed;
    }

    /**
     * @brief 다항식을 설정한다
     * @param poly 다항식
     */
    void setPoly(const QString &poly)
    {
        _poly = poly;
    }

    /**
     * @brief 범위를 설정한다
     * @param start 시작값
     * @param end 끝값
     */
    void setRange(float start, float end)
    {
        _start = qMin(start, end);
        _end = qMax(start, end);
    }

protected:
    /**
     * @brief 위젯 내부를 그린다
     */
    void paintEvent(QPaintEvent */*e*/)
    {
        if (_poly.isEmpty())
            return;

        float xStart = _start;
        float xEnd = _end;
        float xDelta = (xEnd - xStart) / 1000; // 범위를 1,000 등분함

        QList<QPointF> ptfs;

        PolyCalc pc(_poly);

        // 최솟값과 최댓값 초기화
        pc.setX(xStart);
        float yMin = pc.calc();
        float yMax = yMin;

        // 점의 위치 계산
        for (float x = xStart; x <= xEnd; x += xDelta)
        {
            QPointF ptf;

            ptf.setX(x);

            pc.setX(x);
            ptf.setY(pc.calc());

            ptfs.append(ptf);

            // 최솟값 찾기
            if (ptf.y() < yMin)
                yMin = ptf.y();

            // 최댓값 찾기
            if (ptf.y() > yMax)
                yMax = ptf.y();
        }

        // 상수 함수에 대한 보정
        if (yMax == yMin)
        {
            if (yMax == 0)
            {
                yMax = 10;
                yMin = -10;
            }
            else
            {
                yMax = qAbs(yMax);
                yMin = -yMax;
            }
        }

        float xScale;   // 수평 배율
        float yScale;   // 수직 배율

        int xOrg;   // x 축 원점
        int yOrg;   // y 축 원점

        int w = width() - 1;    // 실제로 그릴 수 있는 폭
        int h = height() - 1;   // 실제로 그릴 수 있는 높이

        if (_axisFixed)         // 좌표축이 고정되어 있으면,
        {
            // 위젯의 중심을 기준으로 배율 계산
            xScale = (w / 2) / qMax(qAbs(xStart), qAbs(xEnd));
            yScale = (h / 2) / qMax(qAbs(yMin), qAbs(yMax));

            // 위젯의 중심이 원점
            xOrg = w / 2;
            yOrg = h / 2;
        }
        else                    // 좌표축이 고정되어 있지 않으면
        {
            // 수평 배율 계산
            if (xEnd * xStart < 0 )
                xScale = w * (xStart / (xEnd - xStart)) / xStart;
            else
                xScale = w / (xEnd - xStart);

            // 수직 배율 계산
            if (yMax * yMin < 0)
                yScale = h * (yMin / (yMax - yMin)) / yMin;
            else
                yScale = h / (yMax - yMin);

            // 실제 그래프에 따라 원점 설정
            xOrg = -xStart * xScale;
            yOrg = yMax * yScale;
        }

        QPainter painter(this);

        // 평행이동/원점 변경
        painter.translate(xOrg, yOrg);
        // 배율 설정, x 축 대칭.
        painter.scale(1, -1);

        // 좌표축의 색깔은 검은색
        painter.setPen(Qt::black);

        if (_axisFixed) // 좌표축이 고정되어 있으면
        {
            // 위젯 중심에 좌표축 그림
            painter.drawLine(-xOrg, 0, xOrg, 0);
            painter.drawLine(0, -yOrg, 0, yOrg);
        }
        else            // 좌표축이 고정되어 있지 않으면
        {
            // 실제 그래프에 따라 좌표축 그림
            painter.drawLine(xStart * xScale, 0, xEnd * xScale, 0);
            painter.drawLine(0, yMin * yScale, 0, yMax * yScale);
        }

        // 그래프의 색깔은 빨간색
        painter.setPen(Qt::red);

        // 각 점들을 선으로 이음
        for (int i = ptfs.count() - 1; i >= 1; --i)
            painter.drawLine(ptfs.at(i).x() * xScale,
                             ptfs.at(i).y() * yScale,
                             ptfs.at(i-1).x() * xScale,
                             ptfs.at(i-1).y() * yScale);
    }

private:
    bool _axisFixed;    ///< 좌표축 고정 상태
    QString _poly;      ///< 다항식
    float _start;       ///< 시작값
    float _end;         ///< 끝값
};

/**
 * @brief Plot 생성자
 * @param parent 부모 위젯
 */
Plot::Plot(QWidget *parent)
    : QMainWindow(parent)
{
    initMenus();    // 메뉴 초기화
    initWidgets();  // 위젯 초기화
}

/**
 * @brief Plot 소멸자
 */
Plot::~Plot()
{
}

/**
 * @brief 메뉴를 초기화한다
 */
void Plot::initMenus()
{
    // '파일' 메뉴 생성
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));
    // '끝내기' 항목 추가
    fileMenu->addAction(tr("끝내기(&x)"), this, SLOT(close()),
                        QKeySequence(tr("Ctrl+Q")));

    // '보기' 메뉴 생성
    QMenu *viewMenu = new QMenu(tr("보기(&V)"));
    // 체크 가능한 '좌표축 고정하기' 항목 추가
    viewMenu->addAction(tr("좌표축 고정하기(&A)"), this, SLOT(axisFixed(bool)),
                        QKeySequence(tr("Ctrl+F")))->setCheckable(true);

    // 메뉴바에 추가
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
}

/**
 * @brief 위젯을 초기화한다
 */
void Plot::initWidgets()
{
    _polyLine = new QLineEdit;
    _startLine = new QLineEdit;
    _endLine = new QLineEdit;

    _drawGraphPush = new QPushButton(tr("그래프 그리기(&G)"));
    connect(_drawGraphPush, SIGNAL(clicked(bool)),
            this, SLOT(drawGraph()));

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("f(x) ="), _polyLine);
    formLayout->addRow(tr("시작값"), _startLine);
    formLayout->addRow(tr("끝값"), _endLine);
    formLayout->addRow(_drawGraphPush);

    _graph = new GraphWidget;

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addLayout(formLayout);
    vboxLayout->addWidget(_graph, 1);

    QWidget *w = new QWidget;
    w->setLayout(vboxLayout);

    setCentralWidget(w);

    // 초기 크기 설정
    resize(640, 480);
}

/**
 * @brief '좌표축 고정하기' 항목이 선택될 때 호출된다
 * @param checked true 이면 체크 된 상태이고, false 이면 해제된 상태임
 */
void Plot::axisFixed(bool checked)
{
    // 그래프 위젯에 상태를 전달
    _graph->setAxisFixed(checked);
    // 그래프 다시 그림
    _graph->update();
}

/**
 * @brief 그래프를 그린다
 */
void Plot::drawGraph()
{
    bool ok;

    // 다항식 입력 여부 확인
    if (_polyLine->text().isEmpty())
    {
        QMessageBox::warning(this, qApp->applicationDisplayName(),
                             tr("다항식을 입력해 주세요"));

        _polyLine->setFocus();

        return;
    }

    // 시작값 입력 여부 확인
    if (_startLine->text().isEmpty())
    {
        QMessageBox::warning(this, qApp->applicationDisplayName(),
                             tr("시작값을 입력해 주세요"));

        _startLine->setFocus();

        return;
    }

    // 시작값 숫자 여부 확인
    _startLine->text().toFloat(&ok);
    if (!ok)
    {
        QMessageBox::warning(this, qApp->applicationDisplayName(),
                             tr("시작값에 숫자를 입력해 주세요."));

        _startLine->setFocus();

        return;
    }

    // 끝값 입력 여부 확인
    if (_endLine->text().isEmpty())
    {
        QMessageBox::warning(this, qApp->applicationDisplayName(),
                             tr("끝값을 입력해 주세요"));

        _endLine->setFocus();

        return;
    }

    // 끝값 숫자 여부 확인
    _endLine->text().toFloat(&ok);
    if (!ok)
    {
        QMessageBox::warning(this, qApp->applicationDisplayName(),
                             tr("끝값에 숫자를 입력해 주세요."));

        _endLine->setFocus();

        return;
    }

    // 시작값과 끝값이 같은지 확인
    if (_startLine->text().toFloat() == _endLine->text().toFloat())
    {
        QMessageBox::warning(this, qApp->applicationDisplayName(),
                             tr("시작값과 끝값에 다른 숫자를 입력해 주세요."));

        _startLine->setFocus();

        return;
    }

    // 입력 결과를 그래프 위젯에 전달
    _graph->setPoly(_polyLine->text());
    _graph->setRange(_startLine->text().toFloat(), _endLine->text().toFloat());
    // 그래프 다시 그림
    _graph->update();
}
