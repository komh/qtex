/****************************************************************************
**
** clock.cpp
**
** Copyright (C) 2015 by KO Myung-Hun
** All rights reserved.
** Contact: KO Myung-Hun (komh@chollian.net)
**
** This file is part of Clock.
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

/** @file clock.cpp
 */

#include "clock.h"

/**
 * @brief Clock 생성자
 * @param parent 부모 위젯
 */
Clock::Clock(QWidget *parent)
    : QMainWindow(parent)
    , _port(123)
{
    initMenus();    // 메뉴 초기화
    initWidgets();  // 위젯 초기화

    // 서버 찾기
    QHostInfo::lookupHost("time.nist.gov", this, SLOT(lookedUp(QHostInfo)));

    // 한 번만 타이머 실행
    _timer.setSingleShot(true);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(timeOver()));
    connect(this, SIGNAL(udpReadFinished()), &_timer, SLOT(stop()));

    connect(&_udp, SIGNAL(readyRead()), this, SLOT(udpRead()));
}

/**
 * @brief Clock 소멸자
 */
Clock::~Clock()
{
}

/**
 * @brief 메뉴를 초기화한다
 */
void Clock::initMenus()
{
    // '파일' 메뉴 생성
    QMenu *file = new QMenu(tr("파일(&F)"));

    // '끝내기' 항목 추가
    file->addAction(tr("끝내기(&x)"), this, SLOT(close()),
                    QKeySequence(tr("Ctrl+Q")));

    // '파일' 메뉴 추가
    menuBar()->addMenu(file);
}

/**
 * @brief 위젯을 초기화한다
 */
void Clock::initWidgets()
{
    // 표시 자리 19 자리
    _systemTimeLCD = new QLCDNumber(19);
    _systemTimeLCD->setSegmentStyle(QLCDNumber::Flat);

    // 표시 자리 19 자리
    _internetTimeLCD = new QLCDNumber(19);
    _internetTimeLCD->setSegmentStyle(QLCDNumber::Flat);

    // 표시 자리 19 자리
    _roundTripTimeLCD = new QLCDNumber(19);
    _roundTripTimeLCD->setSegmentStyle(QLCDNumber::Flat);

    _offsetTimeLabel = new QLabel;

    _timePush = new QPushButton(tr("시간확인(&T)"));
    _timePush->setDisabled(true);
    connect(_timePush, SIGNAL(clicked(bool)), this, SLOT(getTime()));

    QFormLayout *form = new QFormLayout;
    form->addRow(tr("시스템 시간:"), _systemTimeLCD);
    form->addRow(tr("인터넷 시간:"), _internetTimeLCD);
    form->addRow(tr("왕복 시간:"), _roundTripTimeLCD);
    form->addRow(tr("오차 시간:"), _offsetTimeLabel);
    form->addRow(_timePush);

    QWidget *w = new QWidget;
    w->setLayout(form);

    setCentralWidget(w);
}

/**
 * @brief SNTP 시간을 Qt 시간으로 바꾼다
 * @param secs SNTP 시간(초)
 * @return Qt 시간(초)
 * @remakr SNTP 는 1900년 1월 1일 UTC 를 기준으로 하고, Qt 는 1970년 1월 1일
 * UTC 를 기준으로 한다
 */
static inline quint32 fromSntp(quint32 secs)
{
    // 1900년 1월 1일 UTC 와 1970년 1월 1일 UTC 의 차이를 계산한다. 단, 1900년
    // 은 윤년이 아니지만, 2000년은 윤년이므로, 2000년 1월 2일 UTC 부터 계산한
    // 다
    QDateTime dt1(QDate(2000, 1, 2), QTime(0, 0), Qt::UTC);
    QDateTime dt2(QDate(2070, 1, 1), QTime(0, 0), Qt::UTC);

    return secs - dt1.secsTo(dt2);
}

/**
 * @brief Qt 시간을 SNTP 시간으로 바꾼다
 * @param secs Qt 시간(초)
 * @return SNTP 시간(초)
 */
static inline quint32 toSntp(quint32 secs)
{
    // 1900년 1월 1일 UTC 와 1970년 1월 1일 UTC 의 차이를 계산한다. 단, 1900년
    // 은 윤년이 아니지만, 2000년은 윤년이므로, 2000년 1월 2일 UTC 부터 계산한
    // 다
    QDateTime dt1(QDate(2000, 1, 2), QTime(0, 0), Qt::UTC);
    QDateTime dt2(QDate(2070, 1, 1), QTime(0, 0), Qt::UTC);

    return secs + dt1.secsTo(dt2);
}

/**
 * @brief 시간을 표시한다
 */
void Clock::displayTime()
{
    SntpPacket pkt;

    // 수신한 데이터그램의 크기가  패킷 크기 이상이어야 한다
    if (static_cast<unsigned>(_datagram.size()) < sizeof(pkt))
        return;

    memcpy(&pkt, _datagram.data(), sizeof(pkt));

    // 시간 계산
    QDateTime now(QDateTime::currentDateTime());
    quint32 orgTime = qFromBigEndian(pkt.orgTimeSec);
    quint32 recvTime = qFromBigEndian(pkt.recvTimeSec);
    quint32 transTime = qFromBigEndian(pkt.transTimeSec);
    quint32 destTime = toSntp(now.toTime_t());

    // 왕복 시간
    qint32 d = (destTime - orgTime) - (transTime - recvTime);
    // 오차 시간
    qint32 t = ((qint32)(recvTime - orgTime) +
                (qint32)(transTime - destTime)) / 2;

    QString format("yyyy-MM-dd HH:mm:ss");

    // 시간 표시
    _systemTimeLCD->display(now.toString(format));
    _internetTimeLCD->display(now.addSecs(t).toString(format));
    _roundTripTimeLCD->display(d);

    // 오차 시간 표시
    QString text;
    if (t > 0)
        text = tr("시스템 시간이 %1 초 느립니다.").arg(t);
    else if (t < 0)
        text = tr("시스템 시간이 %1 초 빠릅니다.").arg(-t);
    else
        text = tr("시스템 시간이 인터넷 시간과 같습니다.");

    _offsetTimeLabel->setText(text);
}

/**
 * @brief lookupHost() 결과 처리
 * @param hostInfo 호스트 정보
 */
void Clock::lookedUp(const QHostInfo &hostInfo)
{
    if (hostInfo.error() != QHostInfo::NoError) {
        QMessageBox::warning(this, windowTitle(),
                             tr("호스트를 찾지 못했습니다: %1")
                                .arg(hostInfo.errorString()));

        return;
    }

    // 호스트 정보 저장
    _hostInfo = hostInfo;

    // '시간확인' 버튼 활성화
    _timePush->setEnabled(true);
}

/**
 * @brief 인터넷 시간을 얻는다
 */
void Clock::getTime()
{
    _datagram.clear();

    // 진행 대화상자 생성
    QProgressDialog *progressDlg = new QProgressDialog(this);
    // 대화상자가 닫히면 메모리에서 해제함
    progressDlg->setAttribute(Qt::WA_DeleteOnClose);
    progressDlg->setLabelText(tr("시간을 확인하고 있습니다..."));
    progressDlg->setMinimumDuration(1 * 1000);
    // 진행중 표시
    progressDlg->setRange(0, 0);
    // 값 초기화
    progressDlg->setValue(0);
    connect(this, SIGNAL(udpReadFinished()), progressDlg, SLOT(close()));

    connect(&_timer, SIGNAL(timeout()), progressDlg, SLOT(close()));
    // 타이머 작동, 시간은 10초
    _timer.start(10 * 1000);

    // 패킷 설정
    SntpPacket pkt;
    memset(&pkt, 0, sizeof(pkt));
#if USE_BITFIELDS
    pkt.mode = 3;   // 클라이언트 모드
    pkt.vn = 4;     // 현재 버전 번호
#else
    pkt.li_vn_mode = 3/*클라이언트 모드*/ | (4/*현재 버전 번호*/ << 3);
#endif

    // 전송 시간 설정, 이후 생성 시간으로 돌아옴
    pkt.transTimeSec =
            qToBigEndian(
                toSntp(QDateTime::currentDateTime().toTime_t()));
    // 패킷 전송
    if (_udp.writeDatagram(reinterpret_cast<char *>(&pkt), sizeof(pkt),
                           _hostInfo.addresses().first(), _port) == -1)
        QMessageBox::warning(this, windowTitle(),
                             tr("데이터그램을 보내지 못했습니다: %1")
                                .arg(_udp.errorString()));
}

/**
 * @brief _udp.readyRead() 시그널이 발생하면 데이터그램을 읽는다
 */
void Clock::udpRead()
{
    // 데이터그램이 있으면
    while (_udp.hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(_udp.pendingDatagramSize());

        QHostAddress addr(_hostInfo.addresses().first());

        // 데이터그램을 읽음
        if (_udp.readDatagram(datagram.data(), datagram.size(), &addr, &_port)
                == -1)
        {
            QMessageBox::warning(this, windowTitle(),
                                 tr("데이터그램을 받지 못했습니다: %1")
                                    .arg(_udp.errorString()));

            break;
        }

        _datagram.append(datagram);
    }

    displayTime();

    // udpRead() 끝났음
    emit udpReadFinished();
}

/**
 * @brief _timer.timeout() 시그널을 처리한다
 */
void Clock::timeOver()
{
    QMessageBox::warning(this, windowTitle(),
                         tr("시간을 확인하지 못했습니다."));
}
