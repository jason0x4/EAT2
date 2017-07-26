﻿#include "mtrcrthread.h"
#include "Collect.h"
#include "formdisplay.h"

#include <QMutex>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QEvent>
#include <QTcpSocket>

mTRCRThread::mTRCRThread(QObject *parent)
    : QObject(parent)
    ,_data("")
    ,oldTc("")
    ,_trClient(new QTcpSocket(this))
    ,_tr2Client(new QTcpSocket(this))
{
    _trClient->connectToHost(ZQWL_IP, ZQWL_PORT + devInformation.at(1).com);
    _tr2Client->connectToHost(ZQWL_IP, ZQWL_PORT + devInformation.at(8).com);
    connect(_trClient, &QTcpSocket::readyRead, [this](){ _data = _trClient->readAll(); });
    connect(_tr2Client, &QTcpSocket::readyRead, [this](){ _data = _tr2Client->readAll(); });
}

mTRCRThread::~mTRCRThread()
{
}

QString mTRCRThread::getData(const QString &text) const
{
    _trClient->write(text.toLatin1().data());
    if (_trClient->waitForReadyRead(1000)) {
        return _data;
    }

    return NULL;
}

QString mTRCRThread::getData2(const QString &text) const
{
    _tr2Client->write(text.toLatin1().data());
    if (_tr2Client->waitForReadyRead(1000)) {
        return _data;
    }

    return NULL;
}

void mTRCRThread::updateTRCR()
{
    QMutexLocker locker(&mutex);
    QString tc;
    readList.clear();

    if (FormDisplay::mTRType == FormDisplay::YOKOGAWA_GP10) {
        tc = getData("FData,0,0001,0110");
        if (tc.size() != 723 || oldTc.isEmpty()) {
            tc = oldTc;
        }
        oldTc = tc;
        readList << tc.split("TT");
        Collect::getTemperatureRise(&readList);
    } else {
//        tc = getData2("FData,0,0001,0110");
//        if (tc.size() != 723 || oldTc.isEmpty()) {
//            tc = oldTc;
//        }
//        oldTc = tc;
//        readList << tc.split("TT");
//        Collect::getTemperatureRise(&readList);
    }
    emit tRCRData(readList);
}

