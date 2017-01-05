/****************************************************************************
**
** mpgui.h
**
** Copyright (C) 2017 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of MPGui.
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

/** @file mpgui.h
 */

#ifndef MPGUI_H
#define MPGUI_H

#include <QMainWindow>

#include <QProcess>

/**
 * @brief Simple front-end class for MPlayer
 */
class MPGui : public QMainWindow
{
    Q_OBJECT

public:
    MPGui(QWidget *parent = 0);
    ~MPGui();

protected:
    bool eventFilter(QObject *o, QEvent *e) Q_DECL_OVERRIDE;

private:
    QProcess _mplayer;      ///< MPlayer 프로세스
    QString _mplayerPath;   ///< MPlayer 경로
    QString _movieFilePath; ///< 동영상 파일 경로

    void initMenus();
    void initWidgets();

    void play(const QString &fileName);

private slots:
    void fileOpen();
};

#endif // MPGUI_H
