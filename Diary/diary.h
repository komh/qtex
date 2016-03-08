/****************************************************************************
**
** diary.h
**
** Copyright (C) 2016 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Diary.
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

/** @file diary.h */

#ifndef DIARY_H
#define DIARY_H

#include <QMainWindow>

#include <QtWidgets>
#include <QtSql>

/**
 * @brief 일기장 메인 클래스
 */
class Diary : public QMainWindow
{
    Q_OBJECT

public:
    Diary(QWidget *parent = 0);
    ~Diary();

    void initMenus();
    void initWidgets();

protected:
    void closeEvent(QCloseEvent *e);

private:
    static const int StartYear = 2010;  ///< 일기장 시작 년도
    static const int EndYear = 2020;    ///< 일기장 마지막 년도

    QComboBox *_yearCombo;      ///< 연도 콤보 박스
    QComboBox *_monthCombo;     ///< 월 콤보 박스
    QComboBox *_dayCombo;       ///< 일 콤보 박스
    QLineEdit *_titleLine;      ///< 제목
    QTextEdit *_contentText;    ///< 내용

    int _id;    ///< 일기 DB ID
    int _year;  ///< 연도
    int _month; ///< 월
    int _day;   ///< 일

    QSqlDatabase _db;   ///< 데이터베이스

    bool diaryOpenDb();
    bool diaryCreateTable();
    bool diaryFind(QSqlQuery *query);
    bool diaryInsert(QSqlQuery *query);
    bool diaryDelete(QSqlQuery *query);
    bool diaryUpdate(QSqlQuery *query);

    /**
     * @brief 창 제목을 돌려준다
     * @return 창 제목
     */
    inline QString title() const
    {
        return QDate(_year, _month, _day)
                .toString(Qt::SystemLocaleLongDate) + "[*]";
    }

    /**
     * @brief 경고창을 보여준다
     * @param msg 경고 메세지
     */
    inline void warning(const QString &msg)
    {
        QMessageBox::warning(this, qApp->applicationName(), msg);
    }

    bool askToSave();
    bool askToOverwrite();

private slots:
    void about();
    void aboutQt();

    void setDay();

    void setDiaryModified(bool modified = true);

    void newDiary();
    bool load();
    bool save();
};

#endif // DIARY_H
