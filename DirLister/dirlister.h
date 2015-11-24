/****************************************************************************
**
** dirlister.h
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Directory Lister.
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

/** @file dirlister.h
 */

#ifndef DIRLISTER_H
#define DIRLISTER_H

#include <QMainWindow>

#include <QtWidgets>
#include <QFileSystemModel>

/**
 * @brief 디렉토리 구조와 내용을 보여준다
 */
class DirLister : public QMainWindow
{
    Q_OBJECT

public:
    DirLister(QWidget *parent = 0);
    ~DirLister();

private:
    QLineEdit *_locationLine;       ///< 위치
    QFileSystemModel *_dirModel;    ///< 디렉토리 구조용 파일 시스템 모델
    QTreeView *_dirView;            ///< 디렉토리 구조용 트리뷰
    QFileSystemModel *_entryModel;  ///< 디렉토리 내용용 파일 시스템 모델
    QTreeView *_entryView;          ///< 디렉토리 내용용 트리뷰

    void initMenus();
    void initWidgets();

private slots:
    void  dirActivated(const QModelIndex &current);
};

#endif // DIRLISTER_H
