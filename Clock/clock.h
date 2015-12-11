/****************************************************************************
**
** clock.h
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

/** @file check.h
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <QMainWindow>

#include <QtWidgets>
#include <QtNetwork>

// 비트 필드 사용 여부
#define USE_BITFIELDS   0

/**
 * @brief 인터넷 시계
 */
class Clock : public QMainWindow
{
    Q_OBJECT

public:
    explicit Clock(QWidget *parent = 0);
    ~Clock();

signals:
    void udpReadFinished(); // udpRead() 가 끝나면 발생

private:
    /**
     * @brief SNTP 패킷 구조체
     */
    struct __attribute__((packed)) SntpPacket
    {
#if USE_BITFIELDS
        quint8 mode : 3;            ///< 작동 모드
        quint8 vn   : 3;            ///< 버전 번호
        quint8 li   : 2;            ///< 윤초 정보
#else
        quint8 li_vn_mode;          ///< 작동 모드/버전번호/윤초정보
#endif
        quint8 stratum;             ///< 서버 층위
        qint8  poll;                ///< 대기 시간, 2의 거듭제곱(초)
        quint8 precision;           ///< 시스템 클럭의 정밀도, 2의 거듭제곱(초)
        qint32 rootDelay;           ///< 1차 표준 소스에 대한 왕복 지연(초)
        quint32 rootDispersion;     ///< 최대 오차(초)
        quint32 refId;              ///< 표준 소스의 ID
        quint32 refTimeSec;         ///< 시스템 클럭이 수정된 시각(초)
        quint32 refTimeFrac;        ///< 시스템 클럭이 수정된 시각
        quint32 orgTimeSec;         ///< 메시지 생성 시간(초)
        quint32 orgTimeFrac;        ///< 메시지 생성 시간(소수)
        quint32 recvTimeSec;        ///< 메시지 수신 시간(초)
        quint32 recvTimeFrac;       ///< 메시지 수신 시간(소수)
        quint32 transTimeSec;       ///< 메시지 전송 시간(초)
        quint32 transTimeFrac;      ///< 메시지 전송 시간(소수)
        /*quint32 keyId;*/          ///< Key Identifier(선택적)
        /*quint8 md[ 16 ];*/        ///< Message Digest(선택적)
    };

    QLCDNumber *_systemTimeLCD;     ///< '시스템 시간'
    QLCDNumber *_internetTimeLCD;   ///< '인터넷 시간'
    QLCDNumber *_roundTripTimeLCD;  ///< '왕복 시간'
    QLabel     *_offsetTimeLabel;   ///< '오차'
    QPushButton *_timePush;         ///< '시간확인'

    QUdpSocket _udp;        ///< UDP 소켓
    QHostInfo _hostInfo;    ///< 호스트 정보
    quint16 _port;          ///< 포토 번호
    QByteArray _datagram;   ///< UDP 수신 데이터

    QTimer _timer;  ///< 타이머

    void initMenus();
    void initWidgets();
    void displayTime();

private slots:
    void lookedUp(const QHostInfo &hostInfo);
    void getTime();
    void udpRead();
    void timeOver();
};

#endif // CLOCK_H
