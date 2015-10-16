/****************************************************************************
**
** timetable.h
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

/** @file timetable.h
 */

#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <QMainWindow>
#include <QtWidgets>

/**
 * @brief 시간표 클래스
 */
class TimeTable : public QMainWindow
{
    Q_OBJECT

public:
    TimeTable(QWidget *parent = 0);
    ~TimeTable();

protected:
    void closeEvent(QCloseEvent *e) Q_DECL_OVERRIDE;

private:
    int _tableRows;             ///< 시간표 세로줄 수
    int _tableCols;             ///< 시간표 가로줄 수
    QTableWidget *_timeTable;   ///< 시간표를 위한 테이블 위젯
    QString _fileName;          ///< 현재 파일 이름
    bool _named;                ///< 이름이 정해졌으면 true, 아니면 false

    void initMenus();
    void initWidgets();
    void setFileName(const QString &name);
    bool saveModifiedTable();

private slots:
    void newTimeTable();
    void openTimeTable();
    void saveTimeTable();
    void modified();
    void headerContextMenuRequested(const QPoint &pos);
};

#endif // TIMETABLE_H
