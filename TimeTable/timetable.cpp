/****************************************************************************
**
** timetable.cpp
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Time Table.
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

/** @file timetable.cpp
 */

#include "timetable.h"

#include <QtWidgets>

/**
 * @brief TimeTable::TimeTable 생성자
 * @param parent 부모 위젯
 */
TimeTable::TimeTable(QWidget *parent)
    : QMainWindow(parent)
    , _tableRows(0)
    , _tableCols(0)
    , _timeTable(0)
    , _named(false)
{
    initMenus();    // 메뉴 초기화
    initWidgets();  // 위젯 초기화

    // newTimeTable() 을 나중에 이벤트 루프에서 호출
    QMetaObject::invokeMethod(this, "newTimeTable", Qt::QueuedConnection);
}

/**
 * @brief TimeTable::~TimeTable 소멸자
 */
TimeTable::~TimeTable()
{

}

/**
 * @brief TimeTable::closeEvent closeEvent() 를 처리한다
 * @param e 이벤트
 */
void TimeTable::closeEvent(QCloseEvent *e)
{
    if (saveModifiedTable())
        e->accept();
    else
        e->ignore();
}

/**
 * @brief TimeTable::initMenus 메뉴를 초기화한다
 */
void TimeTable::initMenus()
{
    // "파일" 메뉴 생성
    QMenu *fileMenu = new QMenu(tr("파일(&F)"));
    // "새 시간표" 액션 추가
    fileMenu->addAction(tr("새 시간표(&N)"), this, SLOT(newTimeTable()),
                        QKeySequence(QKeySequence::New));
    // "열기" 액션 추가
    fileMenu->addAction(tr("열기(&O)..."), this, SLOT(openTimeTable()),
                        QKeySequence(QKeySequence::Open));
    // "저장하기" 액션 추가
    fileMenu->addAction(tr("저장하기(&S)"), this, SLOT(saveTimeTable()),
                        QKeySequence(QKeySequence::Save));
    // "구분자" 추가
    fileMenu->addSeparator();
    // "끝내기" 액션 추가
    fileMenu->addAction(tr("끌내기(&x)"), this, SLOT(close()),
                        QKeySequence(tr("Ctrl+Q")));

    // "파일" 메뉴 추가
    menuBar()->addMenu(fileMenu);
}

/**
 * @brief TimeTable::initWidgets 위젯을 초기화한다
 */
void TimeTable::initWidgets()
{
    // 테이블 위젯 생성
    _timeTable = new QTableWidget;
    // 내용에 맞게 크기 조절
    _timeTable->setSizeAdjustPolicy(QTableWidget::AdjustToContents);

    // 센트럴 위젯 설정
    setCentralWidget(_timeTable);
}

/**
 * @brief TimeTable::setFileName 현재 파일이름을 설정한다
 * @param name 설정할 파일 이름
 */
void TimeTable::setFileName(const QString &name)
{
    _fileName = name;

    // 창 제목을 "파일 이름 - 프로그램이름" 형태로 표시
    // [*] 는 내용이 바뀌면 * 를 표시
    setWindowTitle(QFileInfo(_fileName).fileName() + " - " +
                   qApp->applicationDisplayName() + "[*]");
}

/**
 * @brief TimeTable::saveModifiedTable 변경된 시간표를 저장한다
 * @return Yes 또는 No 이면 true, Cancel 이면 false
 */
bool TimeTable::saveModifiedTable()
{
    // 변경되었으면
    if (isWindowModified())
    {
        // 저장할지 물어봄
        switch(QMessageBox::information(this, qApp->applicationDisplayName(),
                                        tr("시간표가 변경되었습니다. "
                                           "저장할까요?"),
                                        QMessageBox::Yes | QMessageBox::No |
                                        QMessageBox::Cancel))
        {
        case QMessageBox::Cancel:   // 취소
            return false;

        case QMessageBox::Yes:      // 저장
            saveTimeTable();
            break;

        case QMessageBox::No:       // 저장 안 함
        default:                    // 경고 제거용
            break;
        }
    }

    return true;
}

/**
 * @brief TimeTable::newTimeTable 새 시간표를 만든다
 */
void TimeTable::newTimeTable()
{
    static const QStringList weekDays(
                QStringList() << tr("월") << tr("화") << tr("수") << tr("목")
                              << tr("금") << tr("토"));

    // 변경된 시간표를 저장할지 물어봄
    if (!saveModifiedTable())
        return;

    _timeTable->clear();    // 테이블 초기화

    _tableCols = 6;         // 세로줄 수는 6
    _tableRows = 8;         // 가로줄 수는 8

    _timeTable->setColumnCount(_tableCols); // 세로줄 수 설정
    _timeTable->setRowCount(_tableRows);    // 가로줄 수 설정

    // 가로 헤더 아이템 설정
    for (int i = 0; i < _tableCols; ++i)
    {
        // 테이블 아이템 생성
        QTableWidgetItem *item = new QTableWidgetItem;
        // 아이템 텍스트 요일로 설정
        item->setText(weekDays.at(i));
        // 가로 헤더 아이템 설정
        _timeTable->setHorizontalHeaderItem(i, item);
    }
    // 가로 헤더 컨텍스트 메뉴 정책 설
    _timeTable->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    // 컨텍스트 메뉴 시그널 연결
    connect(_timeTable->horizontalHeader(),
            SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(headerContextMenuRequested(QPoint)));

    // 세로 헤더 아이템 설정
    for (int i = 0; i < _tableRows; ++i)
    {
        // 테이블 아이템 생성
        QTableWidgetItem *item = new QTableWidgetItem;
        // 아이템 텍스트 시간으로 설정
        item->setText(QString::number(i + 1));
        // 세로 헤더 아이템 설정
        _timeTable->setVerticalHeaderItem(i, item);
    }
    // 세로 헤더 컨텍스트 메뉴 정책 설정
    _timeTable->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    // 컨텍스트 메뉴 시그널 연결
    connect(_timeTable->verticalHeader(),
            SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(headerContextMenuRequested(QPoint)));

    // 테이블 셀 초기화
    for (int i = 0; i < _tableRows; ++i)
    {
        for (int j = 0; j < _tableCols; ++j)
        {
            // 빈 아이템으로 설정
            _timeTable->setItem(i, j, new QTableWidgetItem(QString()));
        }
    }

    // 테이블 크기에 맞추어서 창 크기 조절
    resize(_timeTable->sizeHint().width(),
           _timeTable->sizeHint().height());

    // 내부 위젯에 맞추어서 크기 조절
    adjustSize();

    // 셀 내용이 바뀌면 modified() 호출
    connect(_timeTable, SIGNAL(cellChanged(int,int)), this, SLOT(modified()));

    setFileName(tr("이름 없음"));   // 새 이름은 "이름 없음"
    setWindowModified(false);       // 변경되지 않았음
}

/**
 * @brief TimeTable::openTimeTable 시간표 파일을 읽는다
 */
void TimeTable::openTimeTable()
{
    // 변경된 시간표를 저장할지 물어봄
    if (!saveModifiedTable())
        return;

    QString name = QFileDialog::getOpenFileName(this, tr("시간표 열기"),
                                                QString(),
                                                tr("시간표 (*.tbl);;"
                                                   "모든 파일 (*)"));

    // 파일을 골랐으면
    if (!name.isEmpty())
    {
        QFile f(name);

        // 읽기 전용으로 파일 열기
        if (f.open(QIODevice::ReadOnly))
        {
            _timeTable->clear();    // 테이블 초기화


            QTextStream in(&f);     // 파일을 텍스트 스트림으로 처리

            in >> _tableRows >> _tableCols; // 세로줄 수와 가로줄 수 읽기
            in.readLine();                  // 줄바꿈 문자 읽기

            _timeTable->setColumnCount(_tableCols); // 가로줄 수 설정
            _timeTable->setRowCount(_tableRows);    // 세로줄 수 설정

            // 세로 헤더 아이템 읽어 설정
            for (int i = 0; i < _tableCols; ++i)
            {
                QTableWidgetItem *item = new QTableWidgetItem;
                item->setText(in.readLine());
                _timeTable->setHorizontalHeaderItem(i, item);
            }
            _timeTable->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(_timeTable->horizontalHeader(),
                    SIGNAL(customContextMenuRequested(QPoint)),
                    this, SLOT(headerContextMenuRequested(QPoint)));

            // 가로 헤더 아이템 읽어 설정
            for (int i = 0; i < _tableRows; ++i)
            {
                QTableWidgetItem *item = new QTableWidgetItem;
                item->setText(in.readLine());
                _timeTable->setVerticalHeaderItem(i, item);
            }
            _timeTable->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(_timeTable->verticalHeader(),
                    SIGNAL(customContextMenuRequested(QPoint)),
                    this, SLOT(headerContextMenuRequested(QPoint)));

            // 테이블 셀 설정
            for (int i = 0; i < _tableRows; ++i)
            {
                for (int j = 0; j < _tableCols; ++j)
                {
                    _timeTable->setItem(i, j,
                                        new QTableWidgetItem(in.readLine()));
                }
            }

            f.close();  // 파일 닫음

             // 테이블 크기에 따라 창크기 조절
            resize(_timeTable->sizeHint().width(),
                   _timeTable->sizeHint().height());


            adjustSize();   // 내부 위젯에 맞추어 크기 조절


            setFileName(name);          // 파일 이름 설정
            setWindowModified(false);   // 변경되지 않았
        }
    }
}

/**
 * @brief TimeTable::saveTimeTable 시간표를 저장한다
 */
void TimeTable::saveTimeTable()
{
    // 파일이름이 정해지지 않았거나 내용이 변경되었으면
    if (!_named || isWindowModified())
    {
        QString name;

        // 정해진 이름이 있으면 그 이름을 쓰고, 아니면 파일 이름을 물어봄
        name = _named ? _fileName :
                        QFileDialog::getSaveFileName(this, tr("시간표 저장"),
                                                     QString(),
                                                     tr("시간표 (*.tbl);;"
                                                        "모든 파일 (*)"));

        if (!name.isEmpty())
        {
            QFile f(name);

            // 쓰기 모드로 파일 열기
            if (f.open(QIODevice::WriteOnly))
            {
                QTextStream out(&f);    // 텍스트 스트림으로 처리


                // 세로줄 수 및 가로줄 수 저장
                out << _tableRows << " " << _tableCols << "\n";

                // 가로 헤더 아이템 저장
                for (int i = 0; i < _tableCols; ++i)
                    out << _timeTable->horizontalHeaderItem(i)->text() << "\n";

                // 세로 헤더 아이템 저장
                for (int i =  0; i < _tableRows; ++i)
                    out << _timeTable->verticalHeaderItem(i)->text() << "\n";

                // 테이블 셀 저장
                for (int i = 0; i < _tableRows; ++i)
                {
                    for (int j = 0; j < _tableCols; ++j)
                        out << _timeTable->item(i, j)->text() << "\n";
                }

                f.close();  // 파일 닫기

                setFileName(name);          // 파일 이름 설정
                setWindowModified(false);   // 변경되지 않았음

                _named = true;  // 이름 정해졌음
            }
        }
    }
}

/**
 * @brief TimeTable::modified 시간표 내용이 변경되면 변경 상태를 설정한다
 */
void TimeTable::modified()
{
    setWindowModified(true);    // 변경되었음
}

/**
 * @brief TimeTable::headerContextMenuRequested 헤더 컨텍스트 메뉴를 처리한다
 * @param pos 컨텍스트 메뉴가 호출된 위치
 */
void TimeTable::headerContextMenuRequested(const QPoint &pos)
{
    // 시그널을 보낸 헤더 뷰 위젯
    QHeaderView *headerView = qobject_cast<QHeaderView *>(sender());

    // 컨텍스트 메뉴가 호출된 위치에 있는 아이템의 인덱스
    int index = headerView->logicalIndexAt(pos);
    // 테이블 위젯 아이템 얻기
    QTableWidgetItem *item = headerView->orientation() == Qt::Horizontal
            ? _timeTable->horizontalHeaderItem(index)
            : _timeTable->verticalHeaderItem(index);

    // 새 이름을 물어봄
    QString newName =  QInputDialog::getText(this, qApp->applicationName(),
                                             tr("새 이름"), QLineEdit::Normal,
                                             item->text());

    if (!newName.isEmpty())
    {
        item->setText(newName); // 아이템 텍스트 설정

        // 테이블 크기에 따라 창 크기 조절
        resize(_timeTable->sizeHint().width(),
               _timeTable->sizeHint().height());

        adjustSize();   // 내부 위젯에 맞추어 창 크기 조절

        modified(); // 변경되었음
    }
}
