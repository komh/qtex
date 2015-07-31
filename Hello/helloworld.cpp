/****************************************************************************
**
** helloworld.cpp
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Hello.
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

#include "helloworld.h"
#include <QLabel>

HelloWorld::HelloWorld(QWidget *parent)
    : QMainWindow(parent)
{
    QLabel *helloLabel = new QLabel("Hello, world!!!");

    setCentralWidget(helloLabel);
}

HelloWorld::~HelloWorld()
{

}
