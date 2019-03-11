#ifndef COMPORTSTREAM_H
#define COMPORTSTREAM_H

#include <logstream.h>
#include <QtSerialPort/QSerialPort>

struct ComPortSettings
{
    LogStreamSettings streamSettings;
    QString port;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};

class ComPortStream : public LogStream
{
public:
    ComPortStream(const ComPortSettings &comPortSettings);
    virtual ~ComPortStream();

    virtual void start();
    virtual void end();
    virtual QByteArray getBytes();

protected:
    ComPortSettings _comPortSettings;
    QSerialPort *_port;
};

#endif // COMPORTSTREAM_H
