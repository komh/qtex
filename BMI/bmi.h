/****************************************************************************
**
** bmi.h
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of BMI.
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

#ifndef BMI_H
#define BMI_H

#include <QMainWindow>
#include <QtWidgets>

class BMI : public QMainWindow
{
    Q_OBJECT

public:
    BMI(QWidget *parent = 0);
    ~BMI();

private slots:
    void calcBMI();

private:
    QLineEdit *_heightLine;
    QLineEdit *_massLine;
    QPushButton *_calcPush;
    QLineEdit *_resultLine;

    QFormLayout *_formLayout;

    void initWidgets();
    void initActions();
};

#endif // BMI_H
