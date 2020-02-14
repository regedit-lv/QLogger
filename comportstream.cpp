#include "comportstream.h"

#include <QMessageBox>

ComPortStream::ComPortStream(const ComPortSettings &comPortSettings)
    : LogStream(comPortSettings.streamSettings)
{
    _comPortSettings = comPortSettings;
}

ComPortStream::~ComPortStream()
{

}

void ComPortStream::start()
{
    _port = new QSerialPort();
    _port->setPortName(_comPortSettings.port);
    _port->setBaudRate(_comPortSettings.baudRate);
    _port->setDataBits(_comPortSettings.dataBits);
    _port->setStopBits(_comPortSettings.stopBits);
    _port->setFlowControl(_comPortSettings.flowControl);
    _port->setParity(_comPortSettings.parity);
}

void ComPortStream::end()
{
    if(_port->isOpen())
    {
        _port->close();
    }
    delete _port;
    _port = nullptr;
}

QByteArray ComPortStream::getBytes()
{
    while (true)
    {
        if (!_port->isOpen())
        {
            _port->open(QIODevice::ReadWrite);
        }

        if (_port->isOpen())
        {
            _port->waitForReadyRead();
            return _port->readAll();
        }

    }

    return QByteArray();
}

